#include "stdafx.h"
#include "CtrlrManager/CtrlrManager.h"
#include "CtrlrPanel/CtrlrPanel.h"
#include "CtrlrLog.h"
#include "CtrlrProcessor.h"
#include "CtrlrPanelMIDIInputThread.h"

CtrlrPanelMIDIInputThread::CtrlrPanelMIDIInputThread(CtrlrPanel &_owner, const CtrlrMIDIDeviceType _source)
	:	owner(_owner),
		Thread("PANEL MIDI INPUT THREAD"),
		inputDevicePtr(nullptr),
		inputComparator(nullptr),
		source(_source)
{
	inputComparator             = new CtrlrMidiInputComparator (owner, source);
	junkBuffer.ensureSize(8192);
	hostInputBuffer.ensureSize(8192);
	deviceInputBuffer.ensureSize(8192);
}

CtrlrPanelMIDIInputThread::~CtrlrPanelMIDIInputThread()
{
	if (inputDevicePtr)
	{
		inputDevicePtr->removeDeviceListener(this);
	}

	inputDevicePtr  = nullptr;
}

void CtrlrPanelMIDIInputThread::run()
{
	while (! threadShouldExit())
    {
		uint32 timeToWait = 500;

		{
			process();
		}

		wait ((int) timeToWait);
	}
	_WRN("CtrlrPanelMIDIInputThread::run stopping");
}

void CtrlrPanelMIDIInputThread::process()
{
    // Take the oversized messages out under the lock, then work on them outside it
    Array<MidiMessage> big;
    {
        const ScopedWriteLock sl(lock);
        big.swapWith(oversizedInput);
    }

    {
        const ScopedWriteLock sl(lock);

        if (owner.getMidiOptionBool(panelMidiThruD2D)) {
            owner.sendMidi(deviceInputBuffer);
        }

        if (inputComparator) {
            inputComparator->match(deviceInputBuffer);
        }

        if (owner.getMidiOptionBool(panelMidiInputFromHostCompare) && hostInputBuffer.getNumEvents() > 0) {
            if (inputComparator)
                inputComparator->match(hostInputBuffer);
        }

        if (owner.getMidiOptionBool(panelMidiThruD2H)) {
            channelizeBuffer(deviceInputBuffer, junkBuffer, owner.getMidiChannel(panelMidiOutputChannelHost),
                             owner.getMidiOptionBool(panelMidiThruD2HChannelize));
            owner.getCtrlrManagerOwner().getProcessorOwner()->addMidiToOutputQueue(deviceInputBuffer);
        }

        hostInputBuffer.clear();
        deviceInputBuffer.clear();
        devicePartialBuffer.clear();
    }

    // Messages too large for a MidiBuffer, handled one at a time, outside the lock
    for (auto &m : big) {
		DBG("process: oversized loop start, size=" + String(m.getRawDataSize()));
        if (owner.getMidiOptionBool(panelMidiThruD2D))
            owner.sendMidi(m);

        if (inputComparator)
            inputComparator->match(m);
			DBG("process: oversized loop, calling panelReceivedMidi size=" + String(m.getRawDataSize()));
owner.panelReceivedMidi (m, source);
	DBG("process: after match, calling panelReceivedMidi");
        if (owner.getMidiOptionBool(panelMidiThruD2H))
            owner.getCtrlrManagerOwner().getProcessorOwner()->addMidiToOutputQueue(m); // dropped by its size guard
    }
}

int CtrlrPanelMIDIInputThread::getListenerInputMidiChannel()
{
	return (owner.getMidiChannel(panelMidiInputChannelDevice));
}

void CtrlrPanelMIDIInputThread::handlePartialMIDIFromDevice (const uint8* messageData, const int numBytesSoFar, const double timestamp)
{
	const ScopedWriteLock sl (lock);

	devicePartialBuffer.add (MemoryBlock (messageData, numBytesSoFar));
}

void CtrlrPanelMIDIInputThread::handleMIDIFromDevice (const MidiMessage &message)
{
    const ScopedWriteLock sl (lock);

	if (owner.getMidiOptionBool(panelMidiRealtimeIgnore) && message.getRawDataSize() <= 1)
		return;

    if (message.getRawDataSize() > 65535) {
        oversizedInput.add(message); // whole message, no 16-bit limit
        notify();
        return;
    }

    if (message.isNoteOnOrOff() || message.isMidiClock())
	{
		deviceInputBuffer.addEvent (message, deviceInputBuffer.getNumEvents()+1);
	}
	else
	{
		deviceInputBuffer.addEvent (message, deviceInputBuffer.getNumEvents()+2);
	}

	notify();
}

void CtrlrPanelMIDIInputThread::handleMIDIFromHost(MidiBuffer &buffer)
{
    // PROBLEM 2 FIX: Ensure ScopedWriteLock is applied to ALL paths that modify hostInputBuffer
    const ScopedWriteLock sl(lock);

//    #ifdef JucePlugin_Build_AAX
//        // This code runs ONLY when building for AAX.
//        // PROBLEM 1 FIX: Use the actual samplePosition from the MidiBuffer::Event
//        for (const auto& msg : buffer)
//        {
//            hostInputBuffer.addEvent(msg.getMessage(), msg.samplePosition);
//        }
//    #else
//        // This code runs for ALL other formats (VST, AU, Standalone).
//        // This line generally works well for copying the entire buffer with correct relative timings.
//        // If buffer.getLastEventTime() + 1 effectively represents the block size, this is fine.
//        // Alternatively, for a full copy of all events in the buffer up to its last sample:
//        // hostInputBuffer.addEvents (buffer, 0, buffer.getNumSamples(), 0);
//        hostInputBuffer.addEvents (buffer, 0, buffer.getLastEventTime() + 1, 1);
//    #endif

     hostInputBuffer.addEvents (buffer, 0, buffer.getLastEventTime() + 1, 1);

    // The notify() call is correctly outside the #ifdef/#else, so it's always called.
    notify();
}

void CtrlrPanelMIDIInputThread::closeInputDevice()
{
	if (inputDevicePtr)
	{
		inputDevicePtr->closeDevice();
	}
}

bool CtrlrPanelMIDIInputThread::openInputDevice (const String &inputDeviceName)
{
	const ScopedWriteLock sl(lock);

	inputDevicePtr = owner.getCtrlrManagerOwner().getCtrlrMidiDeviceManager().getDeviceByName (inputDeviceName, inputDevice, true);

	if (inputDevicePtr != nullptr)
	{
		inputDevicePtr->addDeviceListener (this);

		return (true);
	}

	return (false);
}

CtrlrMidiInputComparator &CtrlrPanelMIDIInputThread::getInputComparator()
{
	if (inputComparator == nullptr)
	{
		inputComparator = new CtrlrMidiInputComparator (owner);
	}

	return (*inputComparator);
}

void CtrlrPanelMIDIInputThread::panelEditModeChanged (const bool _panelEditMode)
{
	if (inputComparator)
	{
		inputComparator->panelEditModeChanged(_panelEditMode);
	}
}

void CtrlrPanelMIDIInputThread::midiChannelChaned(const CtrlrPanelMidiChannel channelThatChanged)
{
	if (channelThatChanged == panelMidiOutputChannelDevice)
	{
		if (inputComparator)
		{
			const ScopedWriteLock sl(lock);
			inputComparator->rehashComparator();
		}
	}
}
