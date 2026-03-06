#pragma once
#include "JuceHeader.h"

class CtrlrDialogWindow
{
public:
    // -------------------------
    // Modal dialogs (all platforms)
    // -------------------------
    static int showModalDialog(const juce::String& title,
        juce::Component* content,
        const bool resizable = false,
        juce::Component* parent = nullptr);

    // -------------------------
    // Non-modal dialogs (all platforms)
    // Opens completely offscreen first on Linux to prevent flicker
    // Caller owns the returned pointer
    // -------------------------
    static juce::DialogWindow* showNonModalDialog(const juce::String& title,
        juce::Component* content,
        const juce::Colour& backgroundColour = juce::Colours::lightgrey,
        bool escapeKeyCloses = true,
        bool resizable = false);

private:
    JUCE_DECLARE_NON_COPYABLE(CtrlrDialogWindow)
};
