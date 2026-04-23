#include "mock_MidiDevice.h"

MockMidi* MockMidi::latestInstance = nullptr;
bool MockMidi::hasSubsystemMockRegistered = false;