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
	inputComparator             = std::make_unique<CtrlrMidiInputComparator>(owner, source);
	junkBuffer.ensureSize(8192);
	hostInputBuffer.ensureSize(8192);
	deviceInputBuffer.ensureSize(8192);
	batchBuffer.ensureSize(8192);
	segmentBuffer.ensureSize(8192);
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
    // Updated v5.6.37 take the device input and the oversized messages in one swap, then handle them in
    // input order: the short messages before each oversized one, that one, and so on. Before, all short
    // messages went first, so thru D2D sent an oversized message after short ones that came after it.
    Array<OversizedInput> big;
    {
        const ScopedWriteLock sl(lock);
        big.swapWith(oversizedInput);
        batchBuffer.clear();
        batchBuffer.swapWith(deviceInputBuffer);
        devicePartialBuffer.clear();
    }

    if (big.isEmpty()) {
        const ScopedWriteLock sl(lock);
        processDeviceInput(batchBuffer);
    }
    else {
        int first = 0;
        for (int i = 0; i <= big.size(); i++) {
            const int end = i < big.size() ? big.getReference(i).position : batchBuffer.getNumEvents();

            if (end > first) {
                const ScopedWriteLock sl(lock);
                segmentBuffer.clear();
                int index = 0;
                for (const auto meta : batchBuffer) {
                    if (index >= first && index < end)
                        segmentBuffer.addEvent(meta.data, meta.numBytes, meta.samplePosition);
                    index++;
                }
                processDeviceInput(segmentBuffer);
                first = end;
            }

            // Messages too large for a MidiBuffer, handled one at a time, outside the lock
            if (i < big.size())
                processOversized(big.getReference(i).message);
        }
    }

    {
        const ScopedWriteLock sl(lock);

        if (owner.getMidiOptionBool(panelMidiInputFromHostCompare) && hostInputBuffer.getNumEvents() > 0) {
            if (inputComparator)
                inputComparator->match(hostInputBuffer);
        }

        hostInputBuffer.clear();
    }
}

void CtrlrPanelMIDIInputThread::processDeviceInput(MidiBuffer &messages)
{
    if (owner.getMidiOptionBool(panelMidiThruD2D)) {
        owner.sendMidi(messages);
    }

    if (inputComparator) {
        inputComparator->match(messages);
    }

    if (owner.getMidiOptionBool(panelMidiThruD2H)) {
        channelizeBuffer(messages, junkBuffer, owner.getMidiChannel(panelMidiOutputChannelHost),
                         owner.getMidiOptionBool(panelMidiThruD2HChannelize));
        owner.getCtrlrManagerOwner().getProcessorOwner()->addMidiToOutputQueue(messages);
    }
}

void CtrlrPanelMIDIInputThread::processOversized(const MidiMessage &m)
{
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

	if (message.getRawDataSize() > kMaxMidiBufferEventBytes) {
		oversizedInput.add({message, deviceInputBuffer.getNumEvents()}); // whole message, no 16-bit limit; position keeps the input order
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
		inputComparator = std::make_unique<CtrlrMidiInputComparator>(owner);
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
