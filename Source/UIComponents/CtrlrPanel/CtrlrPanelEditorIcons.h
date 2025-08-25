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
    EyeDropper,
    Eye,
    EyeSlash
};

class SvgIconManager
{
public:
    static void initialise();
    // Get SVG string by enum
    static const char* getSvgString(IconType iconType);

    // Get a drawable object ready to use
    static std::unique_ptr<juce::Drawable> getDrawable(IconType iconType);

    // Convenience method to draw directly
    static void drawIcon(juce::Graphics& g, IconType iconType, juce::Rectangle<float> bounds);

private:
    // Private storage - all your SVG strings
    static const char* dragDropIcon;
    static const char* eyeDropperIcon;
    static const char* eyeOpen;
    static const char* eyeSlash;



};


class ToggleIconComponent : public juce::Button
{
public:
    ToggleIconComponent(IconType offIconType, IconType onIconType);

    void paintButton(juce::Graphics& g, bool isMouseOverButton, bool isButtonDown) override; // Correct method to override
    void clicked() override;
    juce::String getButtonText() const;

private:
    std::unique_ptr<juce::Drawable> offIcon;
    std::unique_ptr<juce::Drawable> onIcon;
};