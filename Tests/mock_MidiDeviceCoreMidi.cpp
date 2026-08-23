// The CoreMIDI / CoreFoundation system headers must be included BEFORE JuceHeader.h: they pull in
// MacTypes.h, whose global `Point` type would otherwise clash with juce::Point ("reference to
// 'Point' is ambiguous"). JUCE's own juce_mac_CoreMidi.mm relies on the same ordering. Guard with
// the compiler-predefined __APPLE__ (JUCE_MAC isn't known until JuceHeader.h is included).
#ifdef __APPLE__
 #include <CoreMIDI/CoreMIDI.h>
 #include <CoreFoundation/CoreFoundation.h>
 #include <Block.h>
#endif

#include "mock_MidiDevice.h"
// This mock shadows the MIDIEventList / protocol-aware CoreMIDI API
// (MIDIInputPortCreateWithProtocol, MIDISendEventList, ...) and leans on JUCE's
// universal_midi_packets converters. It supports JUCE 6 and JUCE 8 from one source, so that the
// file can be merged between those branches without being rewritten each time.
//
// Both JUCE versions take the same road at runtime: juce_mac_CoreMidi.mm / juce_CoreMidi_mac.mm
// select the EventList/UMP API under `if (@available (macOS 11, iOS 14, *))` and only fall back to
// the older MIDIPacketList API below macOS 11. Every CI runner is well above that, so the
// MIDIPacketList path is left to the framework (referenced but never called) on both versions and
// there is nothing version-specific about the CoreMIDI surface itself.
//
// What does differ is JUCE's UMP layer -- see the three JUCE_MAJOR_VERSION guards below:
//   1. header location and visibility (here)
//   2. GenericUMPConverter::convert's input type (deliverToBlocks)
//   3. the ToBytestreamConverter callback signature (MIDISendEventList)
#if JUCE_MAC

InformMockMidiOfSubsystem mockMidiSubsystem;

#if JUCE_MAJOR_VERSION >= 8
// JUCE 8 exposes the converter chain publicly: juce_audio_basics.h includes midi/ump/juce_UMP.h and
// declares `namespace juce { namespace ump = universal_midi_packets; }`, so <JuceHeader.h> (pulled
// in by mock_MidiDevice.h above) is all this file needs. As of 8.0.12 these headers live in
// juce_audio_basics/midi/ump/, and the old juce_audio_devices/midi_io/ump/ directory holds
// unrelated files (juce_UMPEndpoint.h, juce_UMPSession.h, ...).
#else
// JUCE 6 keeps the chain private to the module -- juce_audio_devices.h does not include it and the
// `ump` alias exists only inside juce_audio_devices.cpp -- so include it by hand, in dependency
// order, exactly as juce_audio_devices.cpp does.
#include <juce_audio_devices/midi_io/ump/juce_UMPProtocols.h>
#include <juce_audio_devices/midi_io/ump/juce_UMPUtils.h>
#include <juce_audio_devices/midi_io/ump/juce_UMPacket.h>
#include <juce_audio_devices/midi_io/ump/juce_UMPSysEx7.h>
#include <juce_audio_devices/midi_io/ump/juce_UMPView.h>
#include <juce_audio_devices/midi_io/ump/juce_UMPIterator.h>
#include <juce_audio_devices/midi_io/ump/juce_UMPackets.h>
#include <juce_audio_devices/midi_io/ump/juce_UMPFactory.h>
#include <juce_audio_devices/midi_io/ump/juce_UMPConversion.h>
#include <juce_audio_devices/midi_io/ump/juce_UMPMidi1ToBytestreamTranslator.h>
#include <juce_audio_devices/midi_io/ump/juce_UMPMidi1ToMidi2DefaultTranslator.h>
#include <juce_audio_devices/midi_io/ump/juce_UMPConverters.h>
#endif

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <map>
#include <mutex>
#include <vector>

/**
 * macOS (CoreMIDI) MIDI subsystem mock for TestsProcessor, sibling of mock_MidiDeviceAlsa.cpp.
 *
 * Same philosophy as the ALSA mock but via link-time symbol shadowing: this TU defines the
 * CoreMIDI C functions that JUCE's juce_mac_CoreMidi.mm calls, and ld64 resolves references from
 * the executable's own objects before the CoreMIDI.framework dylib. Unmocked functions fall
 * through to the real framework (fine for never-executed paths).
 *
 * On a macos-14 runner JUCE's `@available(macOS 11)` always selects the new EventList/UMP API, so
 * we implement that path fully; the old (MIDIPacketList) API is left to the framework (referenced
 * but never called at runtime).
 *
 * Endpoint numbering matches the ALSA mock exactly: a flattened list of
 * getNumDevices()*getNumPorts() endpoints named "Device %02d, port %02d", flat index
 * <-> (idx / numPorts, idx % numPorts). Fake MIDIEndpointRefs encode direction + flat index.
 *
 * JUCE 8 (juce_CoreMidi_mac.mm, upstream 8.0.0-Maintenance) coverage, reviewed best-effort:
 *  - Its legacy device provider still enumerates via MIDIGetNumberOfSources/Destinations +
 *    MIDIGetSource/Destination, and getConnectedEndpointInfo still declines the connection-data
 *    property and falls back to getEndpointInfo -> (entity == 0) virtual -> getMidiObjectInfo.
 *    That is exactly the path mocked here, and returning entity == 0 from MIDIEndpointGetEntity
 *    short-circuits all the JUCE 8 entity/device getters (MIDIDeviceGetEntity, MIDIEntityGetSource/
 *    Destination, MIDIEntityGetNumberOf*, MIDIDeviceGetNumberOfEntities), so they never need mocks.
 *  - Send/receive use the same EventList/UMP calls mocked here.
 *  - GAP: JUCE 8 also has an Obj-C MIDIUMPEndpointManager enumeration provider gated behind
 *    @available(macOS 15). This mock shadows the C CoreMIDI API, not that manager, so on a
 *    macOS 15+ runner JUCE 8 would enumerate through it and bypass this mock. Out of scope for the
 *    macos-14 CI target; revisit when the Tests/ suite is ported to the JUCE 8 branch.
 */

namespace ump = juce::universal_midi_packets;


// The EventList/UMP CoreMIDI entry points and MIDIReceiveBlock are API_AVAILABLE(macos(11.0)).
// JUCE gates them behind @available; this mock calls them directly, and the CI runner is always
// macOS 14, so silence the (default-error) unguarded-availability diagnostic for the whole TU.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunguarded-availability-new"

namespace
{
    // Fake ref encodings. MIDIObjectRef is a UInt32.
    constexpr MIDIEndpointRef kSourceBase = 0x00010000; // device -> host (inputs)
    constexpr MIDIEndpointRef kDestBase   = 0x00020000; // host -> device (outputs)
    constexpr MIDIEndpointRef kRangeSize  = 0x00010000;
    constexpr MIDIPortRef     kPortBase   = 0x00030000;

    int mockNumDevices()
    {
        MockMidi* m = MockMidi::getInstance();
        return m != nullptr ? m->getNumDevices() : 0;
    }
    int mockNumPorts()
    {
        MockMidi* m = MockMidi::getInstance();
        return m != nullptr ? m->getNumPorts(0) : 0;
    }
    int mockNumEndpoints() { return mockNumDevices() * mockNumPorts(); }

    bool decodeEndpoint (MIDIObjectRef ref, int& dev, int& port, bool& isSource)
    {
        const int np = mockNumPorts();
        if (np <= 0) return false;

        int flat;
        if (ref >= kSourceBase && ref < kSourceBase + kRangeSize)      { flat = (int) (ref - kSourceBase); isSource = true; }
        else if (ref >= kDestBase && ref < kDestBase + kRangeSize)     { flat = (int) (ref - kDestBase);   isSource = false; }
        else return false;

        if (flat >= mockNumEndpoints()) return false;
        dev = flat / np; port = flat % np;
        return true;
    }

    void nameForRef (MIDIObjectRef ref, char* out, size_t n)
    {
        int dev, port; bool isSource;
        if (decodeEndpoint (ref, dev, port, isSource))
            std::snprintf (out, n, "Device %02d, port %02d", dev, port);
        else
            std::snprintf (out, n, "??? %u ???", (unsigned) ref);
    }

    std::mutex g_mutex;
    std::map<MIDIPortRef, MIDIReceiveBlock> g_inputBlocks; // created input ports -> receive block
    std::vector<MIDIPortRef> g_connectedInputs;            // input ports with a connected source
    MIDIPortRef  g_nextPort = kPortBase;
    juce::uint32 g_startMillis = 0;

    // Build a MIDIEventList (MIDI 1.0 UMP) from one MidiMessage and invoke each connected input
    // block. GenericUMPConverter splits sysex into sysex7 UMP packets; JUCE's U32ToBytestreamHandler
    // reassembles + timestamps on its side. Runs on the injecting (test) thread.
    void deliverToBlocks (const std::vector<MIDIReceiveBlock>& blocks, const juce::MidiMessage& msg)
    {
        if (blocks.empty())
            return;

        alignas (8) juce::uint8 storage[65536];
        auto* list = reinterpret_cast<MIDIEventList*> (storage);
        MIDIEventPacket* packet = MIDIEventListInit (list, kMIDIProtocol_1_0);

        ump::GenericUMPConverter converter (ump::PacketProtocol::MIDI_1_0);

        // GUARD 2: as of JUCE 8.0.12 convert() takes a BytesOnGroup, View, Packets or an iterator
        // pair -- the MidiMessage overload JUCE 6 has was removed. Either way the callback receives
        // a View per converted packet, so only the input differs.
        const auto emitPacket = [&] (const ump::View& view)
        {
            packet = MIDIEventListAdd (list, sizeof (storage), packet, /*timeStamp*/ 0,
                                       view.size(), view.data());
        };

#if JUCE_MAJOR_VERSION >= 8
        converter.convert (ump::BytesOnGroup { 0,
                               juce::Span<const std::byte> (
                                   reinterpret_cast<const std::byte*> (msg.getRawData()),
                                   (size_t) msg.getRawDataSize()) },
                           emitPacket);
#else
        converter.convert (msg, emitPacket);
#endif

        for (auto block : blocks)
            if (block != nullptr)
                block (list, nullptr);
    }

    void dispatchInjectedInput()
    {
        MockMidi* mock = MockMidi::getInstance();
        if (mock == nullptr)
            return;

        for (;;)
        {
            juce::MidiMessage msg;
            {
                std::lock_guard<std::mutex> lock (mock->inputMutex);
                if (mock->waitingMidiInput.empty())
                    break;
                msg = mock->waitingMidiInput.front();
                mock->waitingMidiInput.pop_front();
            }

            std::vector<MIDIReceiveBlock> blocks;
            {
                std::lock_guard<std::mutex> lock (g_mutex);
                for (auto p : g_connectedInputs)
                {
                    auto it = g_inputBlocks.find (p);
                    if (it != g_inputBlocks.end())
                        blocks.push_back (it->second);
                }
            }
            deliverToBlocks (blocks, msg);
        }
    }

    struct RegisterCoreMidiMock
    {
        RegisterCoreMidiMock()
        {
            g_startMillis = juce::Time::getMillisecondCounter();
            MockMidi::setSubsystemInputNotifier (dispatchInjectedInput);
        }
    } registerCoreMidiMock;
} // namespace

// ============================================================================================
// CoreMIDI C API shadows (extern "C", global scope so ld64 binds JUCE's references to these).
// ============================================================================================
extern "C" {

OSStatus MIDIClientCreate (CFStringRef, MIDINotifyProc, void*, MIDIClientRef* outClient)
{
    if (outClient != nullptr) *outClient = 1;
    return noErr;
}

ItemCount MIDIGetNumberOfSources (void)      { return (ItemCount) mockNumEndpoints(); }
ItemCount MIDIGetNumberOfDestinations (void) { return (ItemCount) mockNumEndpoints(); }

MIDIEndpointRef MIDIGetSource (ItemCount i)
{
    return (i < (ItemCount) mockNumEndpoints()) ? (MIDIEndpointRef) (kSourceBase + i) : 0;
}
MIDIEndpointRef MIDIGetDestination (ItemCount i)
{
    return (i < (ItemCount) mockNumEndpoints()) ? (MIDIEndpointRef) (kDestBase + i) : 0;
}

OSStatus MIDIEndpointGetEntity (MIDIEndpointRef, MIDIEntityRef* outEntity)
{
    if (outEntity != nullptr) *outEntity = 0; // 0 -> JUCE takes the "virtual endpoint" branch
    return noErr;
}

// IMPORTANT: we deliberately do NOT CFStringCompare against the kMIDIProperty* constants.
// In this executable those constants are bad/unresolved pointers -- JUCE only ever passes them
// through to CoreMIDI functions (which we mock) and never dereferences them, so JUCE is unaffected,
// but a CFStringCompare here segfaults inside CFStringGetLength. Since JUCE queries only a single
// string property (kMIDIPropertyName) and two integer properties (kMIDIPropertyUniqueID and
// kMIDIPropertyProtocolID), we can answer by the object ref alone and ignore propertyID entirely.
OSStatus MIDIObjectGetStringProperty (MIDIObjectRef obj, CFStringRef /*propertyID*/, CFStringRef* str)
{
    if (str == nullptr)
        return kMIDIUnknownProperty;

    // Only kMIDIPropertyName is ever requested here -> return the endpoint name.
    char name[64];
    nameForRef (obj, name, sizeof (name));
    *str = CFStringCreateWithCString (kCFAllocatorDefault, name, kCFStringEncodingUTF8);
    return noErr;
}

OSStatus MIDIObjectGetIntegerProperty (MIDIObjectRef obj, CFStringRef /*propertyID*/, SInt32* outValue)
{
    if (outValue == nullptr)
        return kMIDIUnknownProperty;

    // Returning the (unique) object ref satisfies kMIDIPropertyUniqueID (round-trips as the
    // identifier) and, being != kMIDIProtocol_2_0, makes getProtocolForEndpoint() report MIDI 1.0
    // for the kMIDIPropertyProtocolID query -- exactly what we want.
    *outValue = (SInt32) obj;
    return noErr;
}

OSStatus MIDIObjectGetDataProperty (MIDIObjectRef, CFStringRef, CFDataRef*)
{
    // Decline -> JUCE leaves 'connections' null and falls back to getEndpointInfo("no connections").
    return kMIDIUnknownProperty;
}

OSStatus MIDIObjectSetIntegerProperty (MIDIObjectRef, CFStringRef, SInt32) { return noErr; }

OSStatus MIDIInputPortCreateWithProtocol (MIDIClientRef, CFStringRef, MIDIProtocolID,
                                          MIDIPortRef* outPort, MIDIReceiveBlock receiveBlock)
{
    std::lock_guard<std::mutex> lock (g_mutex);
    const MIDIPortRef port = g_nextPort++;
    g_inputBlocks[port] = (MIDIReceiveBlock) Block_copy (receiveBlock);
    if (outPort != nullptr) *outPort = port;
    return noErr;
}

OSStatus MIDIPortConnectSource (MIDIPortRef port, MIDIEndpointRef source, void*)
{
    int dev, p; bool isSource;
    if (decodeEndpoint (source, dev, p, isSource))
        if (MockMidi* m = MockMidi::getInstance())
            m->openInput (dev, p);

    std::lock_guard<std::mutex> lock (g_mutex);
    g_connectedInputs.push_back (port);
    return noErr;
}

OSStatus MIDIPortDisconnectSource (MIDIPortRef port, MIDIEndpointRef)
{
    std::lock_guard<std::mutex> lock (g_mutex);
    g_connectedInputs.erase (std::remove (g_connectedInputs.begin(), g_connectedInputs.end(), port),
                             g_connectedInputs.end());
    return noErr;
}

OSStatus MIDIOutputPortCreate (MIDIClientRef, CFStringRef portName, MIDIPortRef* outPort)
{
    // The port name JUCE passes is the target endpoint's name ("Device %02d, port %02d"): parse it.
    char name[64] = {};
    if (portName != nullptr)
        CFStringGetCString (portName, name, sizeof (name), kCFStringEncodingUTF8);

    int dev = -1, port = -1;
    if (std::sscanf (name, "Device %d, port %d", &dev, &port) == 2)
        if (MockMidi* m = MockMidi::getInstance())
            m->openOutput (dev, port);

    std::lock_guard<std::mutex> lock (g_mutex);
    if (outPort != nullptr) *outPort = g_nextPort++;
    return noErr;
}

OSStatus MIDISendEventList (MIDIPortRef, MIDIEndpointRef dest, const MIDIEventList* evtList)
{
    int dev, port; bool isSource;
    if (! decodeEndpoint (dest, dev, port, isSource) || evtList == nullptr)
        return noErr;

    // JUCE's MidiOutput background thread can outlive the per-test StrictMock; drop late sends.
    MockMidi* m = MockMidi::getInstance();
    if (m == nullptr)
        return noErr;

    // A persistent translator would be needed to reassemble a sysex split across multiple
    // MIDISendEventList calls; the fixture sends each sysex in one list, so a per-call converter
    // suffices (and avoids cross-device interleave).
    ump::ToBytestreamConverter converter (4096);

    const MIDIEventPacket* packet = &evtList->packet[0];
    for (UInt32 i = 0; i < evtList->numPackets; ++i)
    {
        ump::Iterator it   (packet->words, packet->wordCount * sizeof (juce::uint32));
        ump::Iterator end  (packet->words + packet->wordCount, 0);
        for (; it != end; ++it)
            // GUARD 3: JUCE 6's translator hands the callback a ready-made MidiMessage. As of JUCE
            // 8.0.12 SingleGroupMidi1ToBytestreamTranslator::dispatch instead calls
            // `callback (BytesOnGroup { 0, bytes }, time)`, so the message must be rebuilt from the
            // bytes. The body is the same either way.
#if JUCE_MAJOR_VERSION >= 8
            converter.convert (*it, 0.0, [&] (const ump::BytesOnGroup& groupBytes, double)
            {
                if (groupBytes.bytes.empty())
                    return;

                m->sendMidiEvent (dev, port,
                                  juce::MidiMessage (groupBytes.bytes.data(),
                                                     (int) groupBytes.bytes.size()));
            });
#else
            converter.convert (*it, 0.0, [&] (const juce::MidiMessage& msg)
            {
                m->sendMidiEvent (dev, port, msg);
            });
#endif

        packet = MIDIEventPacketNext (packet);
    }
    return noErr;
}

OSStatus MIDIPortDispose (MIDIPortRef port)
{
    std::lock_guard<std::mutex> lock (g_mutex);
    auto it = g_inputBlocks.find (port);
    if (it != g_inputBlocks.end())
    {
        if (it->second != nullptr) Block_release (it->second);
        g_inputBlocks.erase (it);
    }
    g_connectedInputs.erase (std::remove (g_connectedInputs.begin(), g_connectedInputs.end(), port),
                             g_connectedInputs.end());
    return noErr;
}

OSStatus MIDIEndpointDispose (MIDIEndpointRef) { return noErr; }

// Virtual-port creation: never exercised by the fixture (CtrlrX opens hardware ports), but define
// as no-op-success so an accidental call can't reach the real framework with a fake client.
OSStatus MIDIDestinationCreateWithProtocol (MIDIClientRef, CFStringRef, MIDIProtocolID,
                                            MIDIEndpointRef* outDest, MIDIReceiveBlock)
{
    if (outDest != nullptr) *outDest = kDestBase; // arbitrary non-zero
    return noErr;
}
OSStatus MIDISourceCreateWithProtocol (MIDIClientRef, CFStringRef, MIDIProtocolID,
                                       MIDIEndpointRef* outSrc)
{
    if (outSrc != nullptr) *outSrc = kSourceBase;
    return noErr;
}

} // extern "C"

#pragma clang diagnostic pop

#endif // JUCE_MAC
