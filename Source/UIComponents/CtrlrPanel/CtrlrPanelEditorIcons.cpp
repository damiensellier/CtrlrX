/*
  ==============================================================================

    CtrlrPanelEditorIcons.cpp
    Created: 23 Aug 2025 9:18:11pm
    Author:  zan64

  ==============================================================================
*/

#include "CtrlrPanelEditorIcons.h"


// Define all your SVG strings
const char* SvgIconManager::dragDropIcon = R"(
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 640 640">
<path d="M470.6 566.6L566.6 470.6C575.8 461.4 578.5 447.7 573.5 435.7C568.5 423.7 556.9 416 544 416L480 416L480 96C480 78.3 465.7 64 448 64C430.3 64 416 78.3 416 96L416 416L352 416C339.1 416 327.4 423.8 322.4 435.8C317.4 447.8 320.2 461.5 329.3 470.7L425.3 566.7C437.8 579.2 458.1 579.2 470.6 566.7zM214.6 73.4C202.1 60.9 181.8 60.9 169.3 73.4L73.3 169.4C64.1 178.6 61.4 192.3 66.4 204.3C71.4 216.3 83.1 224 96 224L160 224L160 544C160 561.7 174.3 576 192 576C209.7 576 224 561.7 224 544L224 224L288 224C300.9 224 312.6 216.2 317.6 204.2C322.6 192.2 319.8 178.5 310.7 169.3L214.7 73.3z"/></svg>
)";

const char* SvgIconManager::eyeDropperIcon = R"(
<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" fill="currentColor" viewBox="0 0 16 16">
  <path d="M13.354.646a1.207 1.207 0 0 0-1.708 0L8.5 3.793l-.646-.647a.5.5 0 1 0-.708.708L8.293 5l-7.147 7.146A.5.5 0 0 0 1 12.5v1.793l-.854.853a.5.5 0 1 0 .708.707L1.707 15H3.5a.5.5 0 0 0 .354-.146L11 7.707l1.146 1.147a.5.5 0 0 0 .708-.708l-.647-.646 3.147-3.146a1.207 1.207 0 0 0 0-1.708zM2 12.707l7-7L10.293 7l-7 7H2z"/>
</svg>
)";


const char* SvgIconManager::eyeSlash = R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 640 640"><!--!Font Awesome Free v7.0.0 by @fontawesome - https://fontawesome.com License - https://fontawesome.com/license/free Copyright 2025 Fonticons, Inc.--><path d="M73 39.1C63.6 29.7 48.4 29.7 39.1 39.1C29.8 48.5 29.7 63.7 39 73.1L567 601.1C576.4 610.5 591.6 610.5 600.9 601.1C610.2 591.7 610.3 576.5 600.9 567.2L504.5 470.8C507.2 468.4 509.9 466 512.5 463.6C559.3 420.1 590.6 368.2 605.5 332.5C608.8 324.6 608.8 315.8 605.5 307.9C590.6 272.2 559.3 220.2 512.5 176.8C465.4 133.1 400.7 96.2 319.9 96.2C263.1 96.2 214.3 114.4 173.9 140.4L73 39.1zM236.5 202.7C260 185.9 288.9 176 320 176C399.5 176 464 240.5 464 320C464 351.1 454.1 379.9 437.3 403.5L402.6 368.8C415.3 347.4 419.6 321.1 412.7 295.1C399 243.9 346.3 213.5 295.1 227.2C286.5 229.5 278.4 232.9 271.1 237.2L236.4 202.5zM357.3 459.1C345.4 462.3 332.9 464 320 464C240.5 464 176 399.5 176 320C176 307.1 177.7 294.6 180.9 282.7L101.4 203.2C68.8 240 46.4 279 34.5 307.7C31.2 315.6 31.2 324.4 34.5 332.3C49.4 368 80.7 420 127.5 463.4C174.6 507.1 239.3 544 320.1 544C357.4 544 391.3 536.1 421.6 523.4L357.4 459.2z"/></svg>)";
const char* SvgIconManager::eyeOpen = R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 576 512"> <!--!Font Awesome Free v7.0.0 by @fontawesome - https://fontawesome.com License - https://fontawesome.com/license/free Copyright 2025 Fonticons, Inc.--><path d="M572.52 241.4C518.29 135.59 410.93 64 288 64S57.68 135.64 3.48 241.46a48.35 48.35 0 0 0 0 49.19C57.71 376.41 165.07 448 288 448s230.32-71.64 284.52-177.46a48.35 48.35 0 0 0 .01-49.2zM288 400c-97.05 0-184.22-55-232-144c47.78-89 134.95-144 232-144s184.22 55 232 144c-47.78 89-134.95 144-232 144zm0-272a112 112 0 1 0 112 112a112.14 112.14 0 0 0 -112-112zm0 176a64 64 0 1 1 64-64a64.07 64.07 0 0 1 -64 64z"/></svg>)";
const char* SvgIconManager::getSvgString(IconType iconType)
{
    switch (iconType)
    {
    case IconType::DragDrop: return dragDropIcon;
    case IconType::EyeDropper: return eyeDropperIcon;
    case IconType::Eye: return eyeOpen;
    case IconType::EyeSlash: return eyeSlash;
    default:                 return nullptr;
    }
}

std::unique_ptr<juce::Drawable> SvgIconManager::getDrawable(IconType iconType)
{
    const char* svgString = getSvgString(iconType);
    if (svgString)
    {
        _DBG("Creating drawable for icon type");
        // This function already returns a std::unique_ptr
        return juce::Drawable::createFromImageData(svgString, strlen(svgString));
    }
    return nullptr;
}

void SvgIconManager::drawIcon(juce::Graphics& g, IconType iconType, juce::Rectangle<float> bounds)
{
    auto drawable = getDrawable(iconType);
    if (drawable)
    {
        drawable->drawWithin(g, bounds, juce::RectanglePlacement::centred, 1.0f);
    }
}
void SvgIconManager::initialise() {}
// Usage examples:

// In your DragIconComponent:
//class DragIconComponent : public Component
//{
//public:
//    void paint(Graphics& g) override
//    {
//        SvgIconManager::drawIcon(g, IconType::DragDrop, getLocalBounds().toFloat());
//    }
//};
//
//// Or if you need the raw string:
//const char* svgString = SvgIconManager::getSvgString(IconType::Play);
//
//// Or if you need a drawable object:
//auto playIcon = SvgIconManager::getDrawable(IconType::Play);

//=============================================================================
ToggleIconComponent::ToggleIconComponent(IconType offIconType, IconType onIconType)
    : juce::Button("ToggleIconComponent")
{
    // The getDrawable function must also return a std::unique_ptr<juce::Drawable>
    offIcon = SvgIconManager::getDrawable(offIconType);
    onIcon = SvgIconManager::getDrawable(onIconType);
    if (onIcon == nullptr)
    {
        _DBG("Failed to load 'eye' icon!");
    }
    setClickingTogglesState(true);
}

//void ToggleIconComponent::paint(juce::Graphics& g)
//{
//    juce::Drawable* iconToDraw = getToggleState() ? onIcon.get() : offIcon.get();
//
//    if (iconToDraw != nullptr)
//    {
//        iconToDraw->drawWithin(g, getLocalBounds().toFloat(), juce::RectanglePlacement::centred, 1.0f);
//    }
//}
void ToggleIconComponent::clicked()
{
    // This method is required by the juce::Button base class.
    // It is called after the button's state has been toggled.
    // You can add any custom logic here if needed, but a blank
    // implementation is sufficient to fix the error.
}

juce::String ToggleIconComponent::getButtonText() const
{
    // Return an empty string as your button uses an icon, not text.
    return {};
}

void ToggleIconComponent::paintButton(juce::Graphics& g, bool isMouseOverButton, bool isButtonDown)
{
    // Get the appropriate icon based on toggle state
    juce::Drawable* iconToDraw = getToggleState() ? onIcon.get() : offIcon.get();

    if (iconToDraw != nullptr)
    {
        // Get theme-appropriate color - same approach as DragIconComponent
        auto iconColour = getLookAndFeel().findColour(juce::Label::textColourId);

        // Create a copy of the drawable so we don't modify the original
        auto iconCopy = iconToDraw->createCopy();

        if (iconCopy)
        {
            // Replace the default color with theme color
            iconCopy->replaceColour(juce::Colours::black, iconColour);

            // Optional: Add visual feedback for mouse interactions
            if (isButtonDown)
            {
                iconCopy->replaceColour(iconColour, iconColour.darker(0.3f));
            }
            else if (isMouseOverButton)
            {
                iconCopy->replaceColour(iconColour, iconColour.brighter(0.2f));
            }

            iconCopy->drawWithin(g, getLocalBounds().toFloat(), juce::RectanglePlacement::centred, 1.0f);
        }
    }
}