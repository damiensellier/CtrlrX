#include "mock_MidiDevice.h"
#if JUCE_ALSA

InformMockMidiOfSubsystem mockMidiSubsystem;

#include <alsa/asoundlib.h>
// #include <alsa/seq.h>
// #include <alsa/seq_event.h>
// #include <alsa/seqmid.h>
// #include <alsa/seq_midi_event.h>

#include <map>
#include <tuple>

/**
 * This mock re-implements the ALSA methods that JUCE seems to be using (see juce_linux_Midi.cpp).
 * This works because the libasound is linked dynamically, which means that if the (ALSA) symbol
 * was already defined in the binary, it'll use that i.s.o. the one in the dynamic library.
 * 
 * In ALSA each application is a 'client', which can open a 'handle' to the sequencer subsystem.
 * with that handle, you can open one or more 'ports'.
 * you can also subscribe to 'ports' of other clients (or hardware) for both reading and writing
 */
struct FakeSndSeqState
{
    int device_number = -1;
    int device_port_number = -1;
    unsigned char application_port_number = 16;
    std::map<unsigned char, std::tuple<int, int>> application_to_device_port_map{};
};

static FakeSndSeqState fakeHandle;

/*
 * The following functions are declared extern "C" in the header file.
 */
/**************************************************************
 * Midi output to device
 **************************************************************/
std::shared_ptr<juce::MidiMessage> lastEncodedMidiMessage;
long snd_midi_event_encode(snd_midi_event_t *dev, const unsigned char *buf, long count, snd_seq_event_t *ev)
{
    lastEncodedMidiMessage = std::make_shared<juce::MidiMessage>(buf, count);
    return count;
}

int snd_seq_event_output_direct(snd_seq_t *handle, snd_seq_event_t *ev)
{
    int event_device_number = std::get<0>(fakeHandle.application_to_device_port_map.at(ev->source.port));
    int event_device_port_number = std::get<1>(fakeHandle.application_to_device_port_map.at(ev->source.port));
    MockMidi::getInstance()->sendMidiEvent(
        event_device_number,
        event_device_port_number,
        *lastEncodedMidiMessage);
    return lastEncodedMidiMessage->getRawDataSize();
}

long snd_midi_event_decode(snd_midi_event_t *dev, unsigned char *buf, long count, const snd_seq_event_t *ev)
{
    if (!MockMidi::getInstance()->waitingMidiInput.empty())
    {
        juce::MidiMessage message = MockMidi::getInstance()->waitingMidiInput.front();
        std::memcpy(buf, message.getRawData(), message.getRawDataSize());
        MockMidi::getInstance()->waitingMidiInput.pop_front();
        return message.getRawDataSize();
    }
    return 0;
}
void snd_midi_event_reset_decode(snd_midi_event_t *dev) {}

/**************************************************************
 * Midi input from device
 **************************************************************/
void snd_midi_event_reset_encode(snd_midi_event_t *dev) {}

int snd_seq_event_input(snd_seq_t *handle, snd_seq_event_t **ev)
{

    return 0; // always say we have no errors
}

int snd_seq_free_event(snd_seq_event_t *ev)
{
    // snd_seq_event_input is supposed to allocate the pointer to 'ev'
    // since we don't, we should also not free it
    return 0;
}

int snd_seq_event_input_pending(snd_seq_t *seq, int fetch_sequencer)
{
    // should return the byte-size of pending events
    int numBytes = 0;
    for (const auto &message : MockMidi::getInstance()->waitingMidiInput)
    {
        numBytes += message.getRawDataSize();
    }
    return numBytes;
}

// we need poll() to return sth non-zero... That's difficult as it's a glibc function, not alsa specific.
// Thus, we let poll be, and it will time out as the JUCE code doesn't distinguish between a time-out and an actual event present....
// int snd_seq_poll_descriptors(snd_seq_t *handle, struct pollfd *pfds, unsigned int space, short events) {
//     // pfds is an array of pollfd . This function initializes them
// }
// int snd_seq_poll_descriptors_count(snd_seq_t *handle, short events) {
//     // return maximum number of events
//     return 32;
// }

/**************************************************************
 * Port iteration and opening stuff
 **************************************************************/
int snd_seq_open(snd_seq_t **handle, const char *name, int streams, int mode)
{
    // std::cout << "Size of snd_seq_t = " << sizeof(snd_seq_t) << std::endl;
    *handle = (snd_seq_t *)&fakeHandle;
    // assert name == "default"
    // assert streams = SND_SEQ_OPEN_DUPLEX
    // assert mode = 0
    return 0;
}
int snd_seq_nonblock(snd_seq_t *handle, int nonblock)
{
    return 0;
}
int snd_seq_set_client_name(snd_seq_t *seq, const char *name) { return 0; }
int snd_seq_client_id(snd_seq_t *handle)
{
    return 1;
}
int snd_seq_close(snd_seq_t *handle)
{
    fakeHandle.device_number = -1;
    fakeHandle.device_port_number = -1;
    return 0;
}

void snd_seq_port_info_set_client(snd_seq_port_info_t *info, int client) {}
void snd_seq_port_info_set_port(snd_seq_port_info_t *info, int port) {}

int snd_seq_system_info(snd_seq_t *handle, snd_seq_system_info_t *info)
{
    if (handle != NULL && info != NULL)
    {
        fakeHandle.device_number = -1; // start of iteration
        return 0;
    }
    return 1;
}

int snd_seq_system_info_get_cur_clients(const snd_seq_system_info_t *info)
{
    fakeHandle.device_number = -1; // start of iteration
    return MockMidi::getInstance()->getNumDevices();
}

int snd_seq_query_next_client(snd_seq_t *handle, snd_seq_client_info_t *info)
{
    fakeHandle.device_number++;
    fakeHandle.device_port_number = -1;
    return (fakeHandle.device_number < MockMidi::getInstance()->getNumDevices() ? 0 : 1);
}

int snd_seq_client_info_get_num_ports(const snd_seq_client_info_t *info)
{
    fakeHandle.device_port_number = -1;
    if (0 <= fakeHandle.device_number && fakeHandle.device_number < MockMidi::getInstance()->getNumDevices())
        return MockMidi::getInstance()->getNumPorts(fakeHandle.device_number);
    return 0;
}

int snd_seq_client_info_get_client(const snd_seq_client_info_t *info)
{
    return fakeHandle.device_number;
}

// for iterating over ports of devices
int snd_seq_query_next_port(snd_seq_t *handle, snd_seq_port_info_t *info)
{
    fakeHandle.device_port_number++;
    return (fakeHandle.device_port_number < MockMidi::getInstance()->getNumPorts(fakeHandle.device_number) ? 0 : 1);
}
unsigned int snd_seq_port_info_get_capability(const snd_seq_port_info_t *info)
{
    // all ports can read/write in this fake
    return SND_SEQ_PORT_CAP_SUBS_READ | SND_SEQ_PORT_CAP_SUBS_WRITE;
}

const char *snd_seq_port_info_get_name(const snd_seq_port_info_t *info)
{
    static char buffer[32];
    if (0 <= fakeHandle.device_number && fakeHandle.device_number < MockMidi::getInstance()->getNumDevices() &&
        0 <= fakeHandle.device_port_number && fakeHandle.device_port_number < MockMidi::getInstance()->getNumPorts(fakeHandle.device_number))
        snprintf(buffer, sizeof(buffer), "Device %02d, port %02d", fakeHandle.device_number, fakeHandle.device_port_number);
    else
        snprintf(buffer, sizeof(buffer), "??? %02d, %02d ???", fakeHandle.device_number, fakeHandle.device_port_number);
    return buffer;
}

int snd_seq_port_info_get_port(const snd_seq_port_info_t *info)
{
    return fakeHandle.device_port_number;
}

// from seqmid.h
int snd_seq_connect_from(snd_seq_t *seq, int my_port, int src_client, int src_port)
{
    // my_port is given by snd_seq_create_simple_port
    // the 'src' is the input
    fakeHandle.application_to_device_port_map[my_port] = std::make_tuple(src_client, src_port);
    MockMidi::getInstance()->openInput(src_client, src_port);
    return 0;
}
int snd_seq_connect_to(snd_seq_t *seq, int my_port, int dest_client, int dest_port)
{
    // my_port is given by snd_seq_create_simple_port
    // the 'dest' is the output
    fakeHandle.application_to_device_port_map[my_port] = std::make_tuple(dest_client, dest_port);
    MockMidi::getInstance()->openOutput(dest_client, dest_port);
    return 0;
}
int snd_seq_create_simple_port(snd_seq_t *seq, const char *name, unsigned int caps, unsigned int type)
{
    return fakeHandle.application_port_number++;
}
int snd_seq_delete_simple_port(snd_seq_t *seq, int port) { return 0; }

#endif
