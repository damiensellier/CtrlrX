#include "mock_MidiDevice.h"
#if JUCE_LINUX && JUCE_ALSA

InformMockMidiOfSubsystem mockMidiSubsystem;

#include <alsa/asoundlib.h>
// #include <alsa/seq.h>
// #include <alsa/seq_event.h>
// #include <alsa/seqmid.h>
// #include <alsa/seq_midi_event.h>

#include <poll.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include <cstring>
#include <map>
#include <mutex>
#include <tuple>

/**
 * This mock re-implements the ALSA methods that JUCE seems to be using (see juce_Midi_linux.cpp).
 * This works because libasound is linked dynamically, which means that if the (ALSA) symbol
 * was already defined in the binary, it'll use that i.s.o. the one in the dynamic library.
 *
 * In ALSA each application is a 'client', which can open a 'handle' to the sequencer subsystem.
 * with that handle, you can open one or more 'ports'.
 * you can also subscribe to 'ports' of other clients (or hardware) for both reading and writing.
 *
 * JUCE 6 -> 8 notes (this mock targets JUCE 8 while staying backward compatible with JUCE 6):
 *  - Device connections now go through snd_seq_subscribe_port (JUCE 6 used snd_seq_connect_to/from).
 *    snd_seq_connect_from is now used only for the system 'announce' port.
 *  - Device enumeration is asynchronous: JUCE's input (Sequencer) thread must receive a system
 *    event (e.g. PORT_START) for it to (re)scan ports via findEndpoints(). We deliver one such
 *    event at startup. The thread is gated on poll(), so we hand it a real, signalable eventfd.
 *  - The input thread reads MIDI events through snd_seq_event_input + snd_midi_event_decode, and
 *    derives timestamps from a sequencer queue; we provide a minimal fake queue with a fixed
 *    real-time base so injected input gets sensible (wall-clock) timestamps.
 */
struct FakeSndSeqState {
		static constexpr int applicationClientId = 1;

		int device_number = -1;
		int device_port_number = -1;
		// JUCE 6's input router dispatches by ports[event->dest.port], where the index is the portId
		// returned by snd_seq_create_simple_port and JUCE's OwnedArray::set() *appends* when the id is
		// past the end. So the app port ids must start at 0 and increment in lockstep with JUCE's port
		// array, or incoming events get an out-of-range dest.port and are dropped. (Real ALSA hands an
		// app its first port id 0 as well.)
		unsigned char application_port_number = 0;
		std::map<unsigned char, std::tuple<int, int>> application_to_device_port_map{};

		// Async-enumeration / input plumbing:
		int eventfd = -1;						  // real fd handed to JUCE's poll() so we can wake its input thread
		bool pendingSystemEvent = false;		  // deliver one PORT_START at startup to trigger enumeration
		int queueId = 0;						  // fake sequencer queue id
		snd_seq_real_time_t queueStartReal{0, 0}; // queue real-time base (startTimeNative for JUCE)
		juce::uint32 queueStartMillis = 0;		  // wall clock captured when the queue starts (~JUCE's startTimeMillis)
		unsigned char inputAppPort = 0;			  // app port id last used to subscribe an input (device->host)
};

static FakeSndSeqState fakeHandle;

// Wakes JUCE's input thread by making its poll() fd readable.
static void signalFakeEventFd() {
	if (fakeHandle.eventfd >= 0) {
		uint64_t one = 1;
		[[maybe_unused]] auto ignored = ::write(fakeHandle.eventfd, &one, sizeof(one));
	}
}

// Resets the poll() fd so JUCE's next poll() blocks again (call when there's nothing left to deliver).
static void drainFakeEventFd() {
	if (fakeHandle.eventfd >= 0) {
		uint64_t value = 0;
		[[maybe_unused]] auto ignored = ::read(fakeHandle.eventfd, &value, sizeof(value));
	}
}

// Register, once at load, how MockMidi::injectMidiInput should wake our (ALSA) input thread.
static struct RegisterAlsaInputNotifier {
		RegisterAlsaInputNotifier() {
			MockMidi::setSubsystemInputNotifier(signalFakeEventFd);
		}
} registerAlsaInputNotifier;

/*
 * The following functions are declared extern "C" in the header file.
 */
/**************************************************************
 * Midi output to device
 **************************************************************/
std::shared_ptr<juce::MidiMessage> lastEncodedMidiMessage;
long snd_midi_event_encode(snd_midi_event_t *dev, const unsigned char *buf, long count, snd_seq_event_t *ev) {
	lastEncodedMidiMessage = std::make_shared<juce::MidiMessage>(buf, count);
	return count;
}

int snd_seq_event_output_direct(snd_seq_t *handle, snd_seq_event_t *ev) {
	auto mapIter = fakeHandle.application_to_device_port_map.find(ev->source.port);
	if (mapIter == fakeHandle.application_to_device_port_map.end())
		return 0; // not a routed application port (e.g. the announce port), ignore

	// JUCE's MidiOutput flushes queued messages on a background thread that can outlive the test
	// (and its per-test StrictMock). If there's no current mock, there's nothing to record -- drop
	// the late send rather than dereferencing a null instance (a source of teardown segfaults).
	MockMidi *mock = MockMidi::getInstance();
	if (mock == nullptr)
		return 0;

	int event_device_number = std::get<0>(mapIter->second);
	int event_device_port_number = std::get<1>(mapIter->second);
	mock->sendMidiEvent(event_device_number, event_device_port_number, *lastEncodedMidiMessage);
	return lastEncodedMidiMessage->getRawDataSize();
}

/**************************************************************
 * UMP shim
 *
 * On modern libasound (>= 1.2.10, e.g. 1.2.11 here) the UMP symbols are *present*, so JUCE's
 * weak-linked `!= nullptr` checks are true and it takes the UMP code path. Those functions would
 * be called with our fake handle (segfault), so we shadow the few JUCE actually uses:
 *  - decline UMP for client setup / input / endpoint enumeration (so JUCE uses the MIDI 1.0
 *    bytestream path for enumeration and input), and
 *  - translate UMP output back to a MidiMessage and route it like the bytestream output would.
 *
 * NOTE (JUCE 6 -> 8): JUCE 8's ALSA output sends *everything* as UMP when
 * snd_seq_ump_event_output_direct is non-null (juce_Midi_linux.cpp, OutputImplNative::send), which
 * it always is here because this file defines it.
 * Reassembly is delegated to JUCE's own ump::ToBytestreamConverter.
 *
 * The whole section is compiled only where it can be needed: JUCE 6 has no UMP support at all (and
 * never calls these symbols), and on libasound < 1.2.10 the UMP symbols do not exist, so JUCE's
 * weak references are null and it uses the snd_midi_event_encode bytestream path above.
 **************************************************************/
#if JUCE_MAJOR_VERSION >= 8 && defined(SND_LIB_VERSION) && SND_LIB_VERSION >= SND_LIB_VER(1, 2, 10)

// Must outlive individual calls: a SysEx7 stream arrives as several packets (start / continue /
// end) across successive snd_seq_ump_event_output_direct() calls and the translator holds the
// partial message between them. Only JUCE's (single) MidiOutput background thread reaches this.
static juce::ump::ToBytestreamConverter umpToBytestream{4096};

int snd_seq_set_client_midi_version(snd_seq_t *seq, int midi_version) {
	return 0;
}

int snd_seq_ump_event_input(snd_seq_t *seq, snd_seq_ump_event_t **ev) {
	return -1; // decline: make JUCE fall back to snd_seq_event_input (bytestream)
}

int snd_seq_get_ump_endpoint_info(snd_seq_t *seq, int client, void *info) {
	return -1; // decline: our fake devices aren't UMP, so JUCE enumerates them as MIDI 1.0 proxies
}

int snd_seq_ump_event_output_direct(snd_seq_t *seq, snd_seq_ump_event_t *ev) {
	auto mapIter = fakeHandle.application_to_device_port_map.find(ev->source.port);
	if (mapIter == fakeHandle.application_to_device_port_map.end())
		return 0; // not a routed application port (e.g. the announce port), ignore

	MockMidi *mock = MockMidi::getInstance();
	if (mock == nullptr)
		return 0; // background-thread flush after the test's mock was destroyed; drop it (see above)

	const int event_device_number = std::get<0>(mapIter->second);
	const int event_device_port_number = std::get<1>(mapIter->second);

	// Emits nothing for the intermediate packets of a multi-packet (SysEx7) message, and once for
	// each complete bytestream message.
	umpToBytestream.convert(juce::ump::View{ev->ump}, 0.0, [&](juce::ump::BytesOnGroup message, double) {
		mock->sendMidiEvent(event_device_number, event_device_port_number,
							juce::MidiMessage(message.bytes.data(), (int)message.bytes.size()));
	});
	return 1;
}

#endif // JUCE 8 + libasound >= 1.2.10

long snd_midi_event_decode(snd_midi_event_t *dev, unsigned char *buf, long count, const snd_seq_event_t *ev) {
	MockMidi *mock = MockMidi::getInstance();
	if (mock == nullptr)
		return 0;

	std::lock_guard<std::mutex> lock(mock->inputMutex);
	if (!mock->waitingMidiInput.empty()) {
		juce::MidiMessage message = mock->waitingMidiInput.front();
		std::memcpy(buf, message.getRawData(), message.getRawDataSize());
		mock->waitingMidiInput.pop_front();
		return message.getRawDataSize();
	}
	return 0;
}
void snd_midi_event_reset_decode(snd_midi_event_t *dev) {}

/**************************************************************
 * Midi input from device
 **************************************************************/
void snd_midi_event_reset_encode(snd_midi_event_t *dev) {}

// JUCE 8's input thread does: snd_midi_event_new -> ... -> snd_midi_event_free.
// We don't use a real parser; we just hand back a non-null token so JUCE's null/error checks pass.
static char fakeMidiEventParser = 0;
int snd_midi_event_new(size_t bufsize, snd_midi_event_t **rdev) {
	if (rdev != nullptr)
		*rdev = reinterpret_cast<snd_midi_event_t *>(&fakeMidiEventParser);
	return 0;
}
void snd_midi_event_free(snd_midi_event_t *dev) {}
void snd_midi_event_no_status(snd_midi_event_t *dev, int on) {}

int snd_seq_event_input(snd_seq_t *handle, snd_seq_event_t **ev) {
	static snd_seq_event_t fakeEvent; // only the (single) Sequencer input thread calls this
	std::memset(&fakeEvent, 0, sizeof(fakeEvent));

	MockMidi *mock = MockMidi::getInstance();

	std::unique_lock<std::mutex> lock;
	if (mock != nullptr)
		lock = std::unique_lock<std::mutex>(mock->inputMutex);

	// 1) Startup system event: makes JUCE (re)scan ports via findEndpoints().
	if (fakeHandle.pendingSystemEvent) {
		fakeHandle.pendingSystemEvent = false;
		fakeEvent.type = SND_SEQ_EVENT_PORT_START;
		fakeEvent.source.client = SND_SEQ_CLIENT_SYSTEM;
		fakeEvent.source.port = SND_SEQ_PORT_SYSTEM_ANNOUNCE;
		*ev = &fakeEvent;
		return 0;
	}

	// 2) Pending injected MIDI input from a (fake) device.
	if (mock != nullptr && !mock->waitingMidiInput.empty()) {
		// Build a real-time, absolute, queued (non-direct) event so JUCE computes a wall-clock
		// timestamp relative to the queue's real-time base (which we report as 0).
		const juce::uint32 elapsedMillis = juce::Time::getMillisecondCounter() - fakeHandle.queueStartMillis;
		fakeEvent.type = SND_SEQ_EVENT_NOTEON; // any non-system type; the bytes come from decode()
		fakeEvent.flags = SND_SEQ_TIME_STAMP_REAL | SND_SEQ_TIME_MODE_ABS;
		fakeEvent.queue = (unsigned char)fakeHandle.queueId; // not SND_SEQ_QUEUE_DIRECT
		fakeEvent.time.time.tv_sec = elapsedMillis / 1000;
		fakeEvent.time.time.tv_nsec = (elapsedMillis % 1000) * 1000000u;
		fakeEvent.source.client =
			(unsigned char)std::get<0>(fakeHandle.application_to_device_port_map.count(fakeHandle.inputAppPort)
										   ? fakeHandle.application_to_device_port_map[fakeHandle.inputAppPort]
										   : std::make_tuple(0, 0));
		fakeEvent.dest.client = FakeSndSeqState::applicationClientId;
		fakeEvent.dest.port = fakeHandle.inputAppPort; // route to the subscribed input Port
		*ev = &fakeEvent;
		return 0;
	}

	// 3) Nothing to deliver: reset the poll() fd so JUCE's next poll() blocks.
	drainFakeEventFd();
	return -EAGAIN;
}

int snd_seq_free_event(snd_seq_event_t *ev) {
	// snd_seq_event_input is supposed to allocate the pointer to 'ev'.
	// Since we hand back a static event, there's nothing to free.
	return 0;
}

int snd_seq_event_input_pending(snd_seq_t *seq, int fetch_sequencer) {
	// should return the byte-size of pending events
	MockMidi *mock = MockMidi::getInstance();
	if (mock == nullptr)
		return 0;

	std::lock_guard<std::mutex> lock(mock->inputMutex);
	int numBytes = 0;
	for (const auto &message : mock->waitingMidiInput)
		numBytes += message.getRawDataSize();
	return numBytes;
}

/**************************************************************
 * Sequencer queue (used by JUCE 8 for input timestamps)
 **************************************************************/
int snd_seq_alloc_queue(snd_seq_t *handle) {
	return fakeHandle.queueId;
}
int snd_seq_free_queue(snd_seq_t *handle, int q) {
	return 0;
}
// snd_seq_start_queue / snd_seq_stop_queue are macros over snd_seq_control_queue, so mock that.
int snd_seq_control_queue(snd_seq_t *seq, int q, int type, int value, snd_seq_event_t *ev) {
	if (type == SND_SEQ_EVENT_START) {
		// Capture a wall-clock reference ~when JUCE captures its own startTimeMillis.
		fakeHandle.queueStartMillis = juce::Time::getMillisecondCounter();
		fakeHandle.queueStartReal = snd_seq_real_time_t{0, 0};
	}
	return 0;
}
int snd_seq_drain_output(snd_seq_t *handle) {
	return 0;
}
int snd_seq_get_queue_status(snd_seq_t *handle, int q, snd_seq_queue_status_t *status) {
	return 0;
}
const snd_seq_real_time_t *snd_seq_queue_status_get_real_time(const snd_seq_queue_status_t *info) {
	// We shadow the real getter, so the (opaque) status struct doesn't need to be populated.
	return &fakeHandle.queueStartReal;
}

/**************************************************************
 * poll() descriptors -- hand JUCE a real, signalable fd
 **************************************************************/
int snd_seq_poll_descriptors_count(snd_seq_t *handle, short events) {
	return 1;
}
int snd_seq_poll_descriptors(snd_seq_t *handle, struct pollfd *pfds, unsigned int space, short events) {
	if (pfds == nullptr || space < 1)
		return 0;
	pfds[0].fd = fakeHandle.eventfd;
	pfds[0].events = POLLIN;
	pfds[0].revents = 0;
	return 1;
}

/**************************************************************
 * Port iteration and opening stuff
 **************************************************************/
int snd_seq_open(snd_seq_t **handle, const char *name, int streams, int mode) {
	*handle = (snd_seq_t *)&fakeHandle;
	// assert name == "default"
	// assert streams = SND_SEQ_OPEN_DUPLEX
	// assert mode = 0

	// A fresh client (JUCE's AlsaClient singleton is destroyed/recreated between tests) starts its
	// port numbering at 0 and has an empty port table. Reset here so the app port ids the mock hands
	// out stay aligned with JUCE's freshly-rebuilt ports[] array (it dispatches input by portId, see
	// application_port_number above); otherwise incoming events get an out-of-range dest.port.
	fakeHandle.application_port_number = 0;
	fakeHandle.inputAppPort = 0;
	fakeHandle.application_to_device_port_map.clear();

	if (fakeHandle.eventfd < 0)
		fakeHandle.eventfd = ::eventfd(0, EFD_NONBLOCK);
	// Trigger one initial port-change so JUCE performs its (async) device enumeration.
	fakeHandle.pendingSystemEvent = true;
	signalFakeEventFd();
	return 0;
}
int snd_seq_nonblock(snd_seq_t *handle, int nonblock) {
	return 0;
}
int snd_seq_set_client_name(snd_seq_t *seq, const char *name) {
	return 0;
}
int snd_seq_client_id(snd_seq_t *handle) {
	return FakeSndSeqState::applicationClientId;
}
int snd_seq_close(snd_seq_t *handle) {
	fakeHandle.device_number = -1;
	fakeHandle.device_port_number = -1;
	if (fakeHandle.eventfd >= 0) {
		::close(fakeHandle.eventfd);
		fakeHandle.eventfd = -1;
	}
	return 0;
}

// Client / port info reads on our fake handle (JUCE 8 uses these during enumeration / port setup).
int snd_seq_get_client_info(snd_seq_t *handle, snd_seq_client_info_t *info) {
	return 0;
}
int snd_seq_get_any_client_info(snd_seq_t *handle, int client, snd_seq_client_info_t *info) {
	return 0;
}
// Return non-zero so JUCE skips the (UMP-only) per-port detail loop, which would otherwise call
// weak UMP getters that are intentionally left undefined (nullptr) in this mock.
int snd_seq_get_any_port_info(snd_seq_t *handle, int client, int port, snd_seq_port_info_t *info) {
	return -1;
}
const char *snd_seq_client_info_get_name(snd_seq_client_info_t *info) {
	static const char *name = "CtrlrX Mock";
	return name;
}

void snd_seq_port_info_set_client(snd_seq_port_info_t *info, int client) {}
void snd_seq_port_info_set_port(snd_seq_port_info_t *info, int port) {}

int snd_seq_system_info(snd_seq_t *handle, snd_seq_system_info_t *info) {
	// (Legacy / JUCE 6) kept for backward compatibility.
	if (handle != NULL && info != NULL) {
		fakeHandle.device_number = -1; // start of iteration
		return 0;
	}
	return 1;
}

int snd_seq_system_info_get_cur_clients(const snd_seq_system_info_t *info) {
	// (Legacy / JUCE 6) kept for backward compatibility.
	fakeHandle.device_number = -1; // start of iteration
	return MockMidi::getInstance()->getNumDevices();
}

int snd_seq_query_next_client(snd_seq_t *handle, snd_seq_client_info_t *info) {
	fakeHandle.device_number++;
	fakeHandle.device_port_number = -1;
	return (fakeHandle.device_number < MockMidi::getInstance()->getNumDevices() ? 0 : 1);
}

int snd_seq_client_info_get_num_ports(const snd_seq_client_info_t *info) {
	fakeHandle.device_port_number = -1;
	if (0 <= fakeHandle.device_number && fakeHandle.device_number < MockMidi::getInstance()->getNumDevices())
		return MockMidi::getInstance()->getNumPorts(fakeHandle.device_number);
	return 0;
}

int snd_seq_client_info_get_client(const snd_seq_client_info_t *info) {
	return fakeHandle.device_number;
}

// for iterating over ports of devices
int snd_seq_query_next_port(snd_seq_t *handle, snd_seq_port_info_t *info) {
	fakeHandle.device_port_number++;
	return (fakeHandle.device_port_number < MockMidi::getInstance()->getNumPorts(fakeHandle.device_number) ? 0 : 1);
}
unsigned int snd_seq_port_info_get_capability(const snd_seq_port_info_t *info) {
	// all ports can read/write in this fake
	return SND_SEQ_PORT_CAP_SUBS_READ | SND_SEQ_PORT_CAP_SUBS_WRITE;
}

// These are present in modern libasound and JUCE reads them from the port/client info structs
// during enumeration. We don't populate those (opaque) structs, so define the getters to return
// deterministic, legacy-MIDI values: every mock port is a bidirectional MIDI 1.0 (non-UMP) port.
int snd_seq_port_info_get_direction(const snd_seq_port_info_t *info) {
	return SND_SEQ_PORT_DIR_BIDIRECTION;
}
int snd_seq_port_info_get_ump_group(const snd_seq_port_info_t *info) {
	return 0;
}
int snd_seq_client_info_get_midi_version(const snd_seq_client_info_t *info) {
	return SND_SEQ_CLIENT_LEGACY_MIDI;
}

const char *snd_seq_port_info_get_name(const snd_seq_port_info_t *info) {
	static char buffer[32];
	if (0 <= fakeHandle.device_number && fakeHandle.device_number < MockMidi::getInstance()->getNumDevices() &&
		0 <= fakeHandle.device_port_number &&
		fakeHandle.device_port_number < MockMidi::getInstance()->getNumPorts(fakeHandle.device_number))
		snprintf(buffer, sizeof(buffer), "Device %02d, port %02d", fakeHandle.device_number,
				 fakeHandle.device_port_number);
	else
		snprintf(buffer, sizeof(buffer), "??? %02d, %02d ???", fakeHandle.device_number, fakeHandle.device_port_number);
	return buffer;
}

int snd_seq_port_info_get_port(const snd_seq_port_info_t *info) {
	return fakeHandle.device_port_number;
}

/**************************************************************
 * Connecting / subscribing to device ports
 **************************************************************/
// JUCE 8 connects to real devices via snd_seq_subscribe_port. The subscription carries the
// 'sender' and 'dest' addresses (filled by the real libasound setters into a real, stack-allocated
// snd_seq_port_subscribe_t), which we read back via the real getters.
int snd_seq_subscribe_port(snd_seq_t *seq, snd_seq_port_subscribe_t *sub) {
	const snd_seq_addr_t *sender = snd_seq_port_subscribe_get_sender(sub);
	const snd_seq_addr_t *dest = snd_seq_port_subscribe_get_dest(sub);
	if (sender == nullptr || dest == nullptr)
		return 0;

	const int myClient = FakeSndSeqState::applicationClientId;

	if (dest->client != myClient) {
		// dest is the device -> this is an OUTPUT (host -> device); sender is our application port.
		fakeHandle.application_to_device_port_map[sender->port] = std::make_tuple(dest->client, dest->port);
		MockMidi::getInstance()->openOutput(dest->client, dest->port);
	} else if (sender->client != myClient) {
		// sender is the device -> this is an INPUT (device -> host); dest is our application port.
		fakeHandle.inputAppPort = dest->port;
		fakeHandle.application_to_device_port_map[dest->port] = std::make_tuple(sender->client, sender->port);
		MockMidi::getInstance()->openInput(sender->client, sender->port);
	}
	return 0;
}
int snd_seq_unsubscribe_port(snd_seq_t *seq, snd_seq_port_subscribe_t *sub) {
	return 0;
}

// from seqmid.h
int snd_seq_connect_from(snd_seq_t *seq, int my_port, int src_client, int src_port) {
	// JUCE 8 subscribes the system 'announce' port (client 0, port 1) this way; ignore only that.
	// NB: don't guard on src_client alone -- JUCE 6's MidiInput::openDevice subscribes real device
	// input through snd_seq_connect_from, and the mock enumerates its (only) device as client 0,
	// which would otherwise collide with SND_SEQ_CLIENT_SYSTEM and never open the input.
	if (src_client == SND_SEQ_CLIENT_SYSTEM && src_port == SND_SEQ_PORT_SYSTEM_ANNOUNCE)
		return 0;

	// (Legacy / JUCE 6) the 'src' is the input device.
	fakeHandle.inputAppPort = (unsigned char)my_port;
	fakeHandle.application_to_device_port_map[my_port] = std::make_tuple(src_client, src_port);
	MockMidi::getInstance()->openInput(src_client, src_port);
	return 0;
}
int snd_seq_connect_to(snd_seq_t *seq, int my_port, int dest_client, int dest_port) {
	// (Legacy / JUCE 6) the 'dest' is the output device.
	fakeHandle.application_to_device_port_map[my_port] = std::make_tuple(dest_client, dest_port);
	MockMidi::getInstance()->openOutput(dest_client, dest_port);
	return 0;
}
int snd_seq_disconnect_from(snd_seq_t *seq, int my_port, int src_client, int src_port) {
	return 0;
}
int snd_seq_disconnect_to(snd_seq_t *seq, int my_port, int dest_client, int dest_port) {
	return 0;
}

int snd_seq_create_simple_port(snd_seq_t *seq, const char *name, unsigned int caps, unsigned int type) {
	return fakeHandle.application_port_number++;
}
int snd_seq_delete_simple_port(snd_seq_t *seq, int port) {
	return 0;
}

#endif
