#include "stdafx.h"
#include "CtrlrDialogWindow.h"
#include "CtrlrInlineUtilitiesGUI.h"

#if JUCE_LINUX

// Fade-in helper for any Component
class LinuxFadeInWindow : public Timer
{
public:
    LinuxFadeInWindow(Component* c)
        : comp(c), alpha(0.0f)
    {
        comp->setAlpha(alpha);
        startTimer(5); // 5ms interval for smoother/faster update
    }

    void timerCallback() override
    {
        if (!comp) { stopTimer(); return; }

        alpha += 0.25f; // faster increment (0.25 per tick)
        if (alpha >= 1.0f)
        {
            alpha = 1.0f;
            stopTimer();
        }

        comp->setAlpha(alpha);
    }

private:
    Component* comp;
    float alpha;
};

class CtrlrTempDialogWindow : public DialogWindow
{
public:
    CtrlrTempDialogWindow(const String& title,
        Component* contentComponent_,
        Component* /*unused*/,
        const Colour& colour,
        const bool escapeKeyTriggersCloseButton_,
        const bool shouldBeResizable,
        const bool useBottomRightCornerResizer)
        : DialogWindow(title, colour, escapeKeyTriggersCloseButton_, false)
    {
        setUsingNativeTitleBar(true);

        setBounds(-10000, -10000, 400, 300);
        setAlpha(0.0f);

        if (contentComponent_ != nullptr)
        {
            int w = contentComponent_->getWidth();  if (w <= 0) w = 400;
            int h = contentComponent_->getHeight(); if (h <= 0) h = 300;
            contentComponent_->setSize(w, h);
            setSize(w, h);
        }

        setContentNonOwned(contentComponent_, true);

        addToDesktop(ComponentPeer::windowHasTitleBar
            | ComponentPeer::windowAppearsOnTaskbar);

        centreWithSize(getWidth(), getHeight());
        setResizable(shouldBeResizable, useBottomRightCornerResizer);

        new LinuxFadeInWindow(this);
    }

    void closeButtonPressed() override { setVisible(false); }

private:
    TooltipWindow tooltip;
    JUCE_DECLARE_NON_COPYABLE(CtrlrTempDialogWindow);
};

int CtrlrDialogWindow::showModalDialog(const String& title,
    Component* content,
    const bool resizable,
    Component* /*parent*/)
{
    CtrlrTempDialogWindow dw(title, content, nullptr,
        Colours::lightgrey, true, resizable, false);
    return dw.runModalLoop();
}

DialogWindow* CtrlrDialogWindow::showNonModalDialog(const String& title,
    Component* content,
    const Colour& backgroundColour,
    bool escapeKeyCloses,
    bool resizable)
{
    return new CtrlrTempDialogWindow(title, content, nullptr,
        backgroundColour, escapeKeyCloses,
        resizable, false);
}

#else

// Original macOS/Windows code
class CtrlrTempDialogWindow : public DialogWindow
{
public:
    CtrlrTempDialogWindow(const String& title,
        Component* contentComponent_,
        Component* componentToCentreAround,
        const Colour& colour,
        const bool escapeKeyTriggersCloseButton_,
        const bool shouldBeResizable,
        const bool useBottomRightCornerResizer)
        : DialogWindow(title, colour, escapeKeyTriggersCloseButton_, true)
    {
        setUsingNativeTitleBar(true);
        setContentNonOwned(contentComponent_, true);
        centreAroundComponent(componentToCentreAround, getWidth(), getHeight());
        setResizable(shouldBeResizable, useBottomRightCornerResizer);
    }

    void closeButtonPressed() override { setVisible(false); }

private:
    TooltipWindow tooltip;
    JUCE_DECLARE_NON_COPYABLE(CtrlrTempDialogWindow);
};

int CtrlrDialogWindow::showModalDialog(const String& title,
    Component* content,
    const bool resizable,
    Component* parent)
{
    CtrlrTempDialogWindow dw(title, content, parent,
        Colours::lightgrey, true, resizable, false);
    return dw.runModalLoop();
}

DialogWindow* CtrlrDialogWindow::showNonModalDialog(const String& title,
    Component* content,
    const Colour& backgroundColour,
    bool escapeKeyCloses,
    bool resizable)
{
    return new CtrlrTempDialogWindow(title, content, nullptr,
        backgroundColour, escapeKeyCloses,
        resizable, false);
}

#endif // JUCE_LINUX
