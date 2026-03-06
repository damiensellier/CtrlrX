#include "stdafx.h"
#include "CtrlrPanelWindowManager.h"
#include "CtrlrChildWindowContent.h"
#include "CtrlrInlineUtilitiesGUI.h"
#include "CtrlrChildWindow.h"

#if JUCE_LINUX
// Linux fade-in helper for any Component
class LinuxFadeInWindow : public Timer
{
public:
    LinuxFadeInWindow(Component* c)
        : comp(c), alpha(0.0f)
    {
        comp->setAlpha(alpha);
        startTimer(10); // 10ms interval
    }

    void timerCallback() override
    {
        if (!comp) { stopTimer(); return; }

        alpha += 0.1f; // ~100ms fade-in
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
#endif

CtrlrChildWindow::CtrlrChildWindow(CtrlrWindowManager &_owner)
#if JUCE_LINUX
    : owner(_owner), DocumentWindow("Child window", Colours::lightgrey, DocumentWindow::allButtons, false),
      contentComponent(nullptr)
#else
    : owner(_owner), DocumentWindow("Child window", Colours::lightgrey, DocumentWindow::allButtons, true),
      contentComponent(nullptr)
#endif
{
    setUsingNativeTitleBar(true);
    setResizable(true, false);

    containerComponent = new CtrlrChildWindowContainer(owner);

#if JUCE_LINUX
    // Offscreen + transparent
    setBounds(-10000, -10000, 800, 500);
    setAlpha(0.0f);

    setContentOwned(containerComponent, true);
    addToDesktop(ComponentPeer::windowHasTitleBar
                 | ComponentPeer::windowAppearsOnTaskbar);

    centreWithSize(getWidth(), getHeight());

    // fade-in
    new LinuxFadeInWindow(this);

#else
    setContentOwned(containerComponent, true);
    setSize(800, 500);
    if (!JUCEApplication::isStandaloneApp())
        setAlwaysOnTop(true);
#endif
}

CtrlrChildWindow::~CtrlrChildWindow()
{
    deleteAndZero(containerComponent);
    deleteAndZero(contentComponent);
}

void CtrlrChildWindow::resized()        { DocumentWindow::resized(); owner.windowChanged(this); }
void CtrlrChildWindow::enablementChanged() {}
void CtrlrChildWindow::moved()          { owner.windowChanged(this); }
CtrlrChildWindowContent* CtrlrChildWindow::getContent() { return contentComponent; }
uint8 CtrlrChildWindow::getType()       { return contentComponent ? contentComponent->getType() : 0; }

void CtrlrChildWindow::setContent(CtrlrChildWindowContent *c)
{
    contentComponent = c;
    if (contentComponent != nullptr)
    {
        setName(contentComponent->getName());
        containerComponent->setContent(contentComponent);
    }
}

void CtrlrChildWindow::closeButtonPressed()
{
    if (!contentComponent || contentComponent->canCloseWindow())
        owner.windowClosedButtonPressed(this);
}

bool CtrlrChildWindow::keyPressed(const KeyPress &key, Component*)
{
    return contentComponent && contentComponent->keyPressed(key, nullptr);
}
