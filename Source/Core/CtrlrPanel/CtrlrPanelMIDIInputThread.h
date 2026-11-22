#pragma once

#include "CtrlrMacros.h"
#include "CtrlrMIDIDevice.h"
#include "CtrlrMidiInputComparator.h"

class CtrlrPanel;

class CtrlrPanelMIDIInputThread : public Thread, public CtrlrMIDIDevice::Listener, public CtrlrPanelMidiProcessor
{
	public:
		CtrlrPanelMIDIInputThread(CtrlrPanel &_owner, CtrlrMIDIDeviceType _source);
		~CtrlrPanelMIDIInputThread();
		void run();
		int getListenerInputMidiChannel();
		void handleMIDIFromDevice(const MidiMessage &message);
		void handleMIDIFromHost(MidiBuffer &buffer);
		void processMessage(MidiMessage &/*midiMessage*/) {}
		void handlePartialMIDIFromDevice (const uint8* messageData, const int numBytesSoFar, const double timestamp);
		bool openInputDevice (const String &inputDeviceName);
        bool hasInputDevice() const
        {
            return inputDevicePtr != nullptr;
        }
		void panelEditModeChanged (const bool _panelEditMode);
		CtrlrMidiInputComparator &getInputComparator();
		void process();
		void midiOptionChanged(const CtrlrPanelMidiOption /*optionThatChanged*/){}
		void midiChannelChaned(const CtrlrPanelMidiChannel channelThatChanged);
		void closeInputDevice();
		JUCE_LEAK_DETECTOR(CtrlrPanelMIDIInputThread)

	private:
		// A message too large for a MidiBuffer, and the number of deviceInputBuffer events before it
		struct OversizedInput
		{
			MidiMessage message;
			int position;
		};

		void processDeviceInput (MidiBuffer &messages);
		void processOversized (const MidiMessage &m);

		MidiBuffer deviceInputBuffer, hostInputBuffer, thruToDeviceBuffer, junkBuffer;
		MidiBuffer batchBuffer, segmentBuffer;
		Array<OversizedInput> oversizedInput;
		Array <MemoryBlock, CriticalSection> devicePartialBuffer;
		ReadWriteLock lock;
		CtrlrPanel &owner;
		CtrlrMIDIDevice *inputDevicePtr;
		CtrlrMIDIDeviceType source;
		std::unique_ptr<CtrlrMidiInputComparator> inputComparator;
};
