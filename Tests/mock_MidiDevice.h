#pragma once

// provide a generic mock class that needs to be implemented per platform, because JUCE doesn't do dependency injection...

#include <JuceHeader.h>
#include <deque>
#include "gmock/gmock.h"

using ::testing::Return;
using ::testing::_;

class MockMidi;
class InformMockMidiOfSubsystem;

class MockMidi {
    public:
        MockMidi() {
            latestInstance = this;
        }
        virtual ~MockMidi() {
            latestInstance = nullptr;
        }
        static MockMidi* getInstance() { return latestInstance; }

        MOCK_METHOD(int, getNumDevices, ());
        MOCK_METHOD(int, getNumPorts, (int));

        MOCK_METHOD(void, openInput, (int, int));
        MOCK_METHOD(void, openOutput, (int, int));

        MOCK_METHOD(void, sendMidiEvent, (int, int, juce::MidiMessage));

        void setDefaultBehavior() {
            // ON_CALL(*this, getNumDevices()).WillByDefault(Return(1));
            // ON_CALL(*this, getNumPorts(_)).WillByDefault(Return(2));
            // expect, so that StrictMock doesn't fail on these:
            EXPECT_CALL(*this, getNumDevices()).WillRepeatedly(Return(1));
            EXPECT_CALL(*this, getNumPorts(_)).WillRepeatedly(Return(2));
        }
        bool hasSubsystemMock() { return (getInstance() != nullptr) && MockMidi::hasSubsystemMockRegistered; }

        std::deque<juce::MidiMessage> waitingMidiInput;
    protected:
        static MockMidi* latestInstance;
        static bool hasSubsystemMockRegistered;
        friend InformMockMidiOfSubsystem;
};

/**
 * Instantiating this class allows the 'fake' implementation of a midi subsystem to notify the MockMidi class that it is compiled in,
 * without the MockMidi class actually knowing about its clients.
 */
class InformMockMidiOfSubsystem {
    public:
        InformMockMidiOfSubsystem() {
            MockMidi::hasSubsystemMockRegistered = true;
        }
};
