/*
  ==============================================================================

    CtrlrPanelEditorIcons.h
    Created: 23 Aug 2025 9:18:11pm
    Author:  zan64

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <CtrlrLog.h>

using namespace juce;

enum class IconType
{
    DragDrop,
    DragDropThin,
    EyeDropper,
    Eye,
    EyeSlash
};

class SvgIconManager
{
public:
    static void initialise();
    static juce::String getSvgString(IconType iconType);
    static std::unique_ptr<juce::Drawable> getDrawable(IconType iconType, const juce::Component& component);

private:
    static const juce::String dragDropIcon;
    static const juce::String dragDropIconThin;
    static const juce::String eyeDropperIcon;
    static const juce::String eyeOpen;
    static const juce::String eyeSlash;
};

class ToggleIconComponent : public juce::Button
{
public:
    ToggleIconComponent(IconType offIconType, IconType onIconType);
    void paintButton(juce::Graphics& g, bool isMouseOverButton, bool isButtonDown) override;
    void clicked() override;
    juce::String getButtonText() const;

private:
    IconType offIconType;
    IconType onIconType;
};
