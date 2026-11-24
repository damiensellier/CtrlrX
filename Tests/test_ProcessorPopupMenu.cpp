#include "test_ProcessorFixture.h"

#include "CtrlrInlineUtilitiesGUI.h"

/**
 * The asynchronous popup-menu helpers in CtrlrInlineUtilitiesGUI.h (namespace PU), through their
 * test hook: no menu is shown, the hook chooses the item. Each helper must pass the options of
 * the blocking PopupMenu call it replaces (so the menu opens where it did), call back only later
 * from the message loop (never inside the call), and give 0 when the menu was dismissed.
 */
class PopupMenuHelpers : public ::testing::Test
{
protected:
    int hookCalls = 0;
    int answer = 0;
    juce::PopupMenu askedMenu;
    juce::Component *askedTarget = nullptr;
    juce::Rectangle<int> askedArea;

    void SetUp() override
    {
        PU::testAnswer() = [this] (const juce::PopupMenu &menu, const juce::PopupMenu::Options &options) {
            ++hookCalls;
            askedMenu = menu;
            askedTarget = options.getTargetComponent();
            askedArea = options.getTargetScreenArea();
            return answer;
        };
    }

    void TearDown() override { PU::testAnswer() = nullptr; }

    static void pump() { juce::MessageManager::getInstance()->runDispatchLoopUntil (50); }

    static juce::PopupMenu twoItems()
    {
        juce::PopupMenu m;
        m.addItem (1, "First");
        m.addItem (2, "Second");
        return m;
    }
};

TEST_F(PopupMenuHelpers, show_opens_at_the_mouse_and_calls_back_later)
{
    answer = 2;
    int calls = 0;
    int got = -1;
    PU::show (twoItems(), [&] (int result) {
        ++calls;
        got = result;
    });

    EXPECT_EQ(hookCalls, 1);
    EXPECT_EQ(askedMenu.getNumItems(), 2);
    // PopupMenu::show(): no target, so JUCE opens the menu at the mouse position
    EXPECT_EQ(askedTarget, nullptr);
    EXPECT_TRUE(askedArea.isEmpty());
    EXPECT_EQ(calls, 0) << "the callback must not run inside the call";

    pump();
    EXPECT_EQ(calls, 1);
    EXPECT_EQ(got, 2);
}

TEST_F(PopupMenuHelpers, show_at_targets_the_component)
{
    answer = 1;
    juce::Component button;
    int got = -1;
    PU::showAt (twoItems(), &button, [&] (int result) { got = result; });
    // PopupMenu::showAt (component)
    EXPECT_EQ(askedTarget, &button);
    pump();
    EXPECT_EQ(got, 1);
}

TEST_F(PopupMenuHelpers, show_at_without_a_component_is_show)
{
    PU::showAt (twoItems(), nullptr, [] (int) {});
    EXPECT_EQ(hookCalls, 1);
    EXPECT_EQ(askedTarget, nullptr);
    EXPECT_TRUE(askedArea.isEmpty());
    pump();
}

TEST_F(PopupMenuHelpers, dismissed_gives_0_and_still_calls_back)
{
    answer = 0;
    int calls = 0;
    int got = -1;
    PU::show (twoItems(), [&] (int result) {
        ++calls;
        got = result;
    });
    pump();
    EXPECT_EQ(calls, 1) << "a dismissed menu still calls back";
    EXPECT_EQ(got, 0);
}

// JUCE shows nothing for an empty menu and never calls back; the blocking show() returned 0.
TEST_F(PopupMenuHelpers, empty_menu_calls_back_with_0_without_asking)
{
    answer = 5;
    int calls = 0;
    int got = -1;
    PU::show (juce::PopupMenu(), [&] (int result) {
        ++calls;
        got = result;
    });
    EXPECT_EQ(hookCalls, 0) << "no menu is shown";
    EXPECT_EQ(calls, 0) << "the callback must not run inside the call";
    pump();
    EXPECT_EQ(calls, 1);
    EXPECT_EQ(got, 0);
}
