#include "test_ProcessorFixture.h"

#include "CtrlrMacros.h"
#include "CtrlrManager.h"
#include "CtrlrPanel.h"
#include "CtrlrModulator.h"

#include <algorithm>
#include <vector>

// The parameter count CtrlrX advertises to a VST3/AU host is read by the wrapper exactly once, at
// initialisation, before any panel exists. These tests pin that down:
//
//   T1 -- the count does not change when a panel is loaded.
//   T2 -- no parameter index beyond the advertised count ever reaches the host.
//   T3 -- in-range indices still do (so a guard that blocks everything cannot pass T1+T2).
//
// An AudioProcessorListener is exactly what the VST3/AU wrapper registers without needing a host.

namespace
{
    struct RecordingListener : public juce::AudioProcessorListener
    {
        std::vector<int> indices;

        void audioProcessorParameterChanged (juce::AudioProcessor*, int index, float) override
        {
            indices.push_back (index);
        }

        // pure virtual in JUCE 6; the panel load calls it via updateHostDisplay()
        void audioProcessorChanged (juce::AudioProcessor*, const ChangeDetails&) override {}

        bool sawIndex (int index) const
        {
            return std::find (indices.begin(), indices.end(), index) != indices.end();
        }

        int largestIndex() const
        {
            return indices.empty() ? -1 : *std::max_element (indices.begin(), indices.end());
        }
    };

    // Drive a modulator the way panel Lua would. mute=true suppresses the outgoing MIDI, so no
    // device-send expectations are needed; the host notification still happens.
    void driveModulator (CtrlrModulator* modulator, double value)
    {
        modulator->getProcessor().setValueGeneric (
            CtrlrModulatorValue (value, CtrlrModulatorValue::changedByLua), false, true);
    }
}

// T1 -- the invariant, plus the advertised value itself.
TEST_F(ProcessorInstance, test_num_parameters_is_constant_across_panel_load)
{
    allowAndRecordDeviceSends();

    const int before = processor->getNumParameters();
    EXPECT_EQ(before, CTRLR_MAX_EXPORTED_VST_PARAMETERS);

    ASSERT_NE(loadPanel("fixture_vst_param_overflow.panel"), nullptr);

    EXPECT_EQ(processor->getNumParameters(), before)
        << "VST3/AU hosts snapshot the parameter count at init; it must never change";
}

// T2 -- the actual bug: no index beyond the advertised count reaches the host.
TEST_F(ProcessorInstance, test_out_of_range_vst_index_never_reaches_host)
{
    allowAndRecordDeviceSends();

    const int advertised = processor->getNumParameters();

    CtrlrPanel* panel = loadPanel("fixture_vst_param_overflow.panel");
    ASSERT_NE(panel, nullptr);

    // The fixture's largest vstIndex is 200, so the panel wants far more parameters than the
    // instance advertises. Before the fix, getNumParameters() grew to match and the wrapper
    // indexed its init-time parameter-ID array out of bounds.
    ASSERT_GT(processor->getManager().getNumModulators(true), advertised);

    CtrlrModulator* outOfRange = panel->getModulator("out-of-range");
    ASSERT_NE(outOfRange, nullptr) << "fixture is missing the 'out-of-range' modulator";
    ASSERT_EQ(outOfRange->getVstIndex(), 200);

    RecordingListener listener;
    processor->addListener(&listener);

    driveModulator(outOfRange, 64.0);

    processor->removeListener(&listener);

    EXPECT_LT(listener.largestIndex(), advertised)
        << "a parameter index the host was never told about reached it; index "
        << listener.largestIndex() << " >= advertised " << advertised;
}

// T3 -- anti-regression for the guard: a guard that blocks EVERYTHING would pass T1 and T2.
TEST_F(ProcessorInstance, test_in_range_vst_index_still_reaches_host)
{
    allowAndRecordDeviceSends();

    CtrlrPanel* panel = loadPanel("fixture_vst_param_overflow.panel");
    ASSERT_NE(panel, nullptr);

    CtrlrModulator* inRange = panel->getModulator("in-range");
    ASSERT_NE(inRange, nullptr) << "fixture is missing the 'in-range' modulator";
    ASSERT_EQ(inRange->getVstIndex(), 1);
    ASSERT_LT(inRange->getVstIndex(), processor->getNumParameters());

    RecordingListener listener;
    processor->addListener(&listener);

    driveModulator(inRange, 64.0);

    processor->removeListener(&listener);

    EXPECT_TRUE(listener.sawIndex(1))
        << "an in-range parameter change was dropped; host automation would be dead";
}
