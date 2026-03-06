#ifndef CTRLR_INLINE_UTILITIES_GUI
#define CTRLR_INLINE_UTILITIES_GUI

#include "CtrlrMacros.h"
#include <juce_gui_basics/juce_gui_basics.h> // Make sure this is included for LookAndFeel_V4

namespace gui {

    static inline
    DrawableButton *createDrawableButton(const String &buttonName, const String &svgData,
										 const String &svgDataDown="",
										 const MouseCursor cursor = MouseCursor::PointingHandCursor) {
        std::unique_ptr<XmlElement> svgXml (XmlDocument::parse(svgData));
        std::unique_ptr<Drawable> drawable (Drawable::createFromSVG(*svgXml));

        std::unique_ptr<XmlElement> svgXmlDown (XmlDocument::parse(svgDataDown));
        std::unique_ptr<Drawable> drawableDown (svgXmlDown ? Drawable::createFromSVG(*svgXmlDown) : nullptr);

        auto btn = new DrawableButton(buttonName, DrawableButton::ImageFitted);
        btn->setImages(drawable.get(),
                       nullptr,
                       drawableDown.get(),
                       nullptr,
                       nullptr,
                       nullptr,
                       nullptr,
                       nullptr);
        btn->setMouseCursor(cursor);
        return btn;
    }

    static inline
    Drawable *createDrawable(const String &svgData) {
        std::unique_ptr<XmlElement> svgXml (XmlDocument::parse(svgData));
        return Drawable::createFromSVG(*svgXml).release();
    }

    static const inline void
    drawSelectionRectangle(Graphics &g, int width, int height, Colour base = Colour(HIGHLIGHT_COLOUR),
                           const float baseSaturation = 0.9f, const float baseAlpha = 0.9f,
                           const float gradientMin = 0.2f, const float gradientMax = 0.25f) {
        Colour baseColour(base.withMultipliedSaturation(baseSaturation).withMultipliedAlpha(baseAlpha));
        const float mainBrightness = baseColour.getBrightness();
        const float mainAlpha = baseColour.getFloatAlpha();
        Path outline;
        outline.addRoundedRectangle(0, 0, width, height, 4.0f, 4.0f, false, false, false, false);
        g.setGradientFill(ColourGradient(baseColour.brighter(gradientMin), 0.0f, 0.0f,
                                         baseColour.darker(gradientMax), 0.0f, height, false));
        g.fillPath(outline);

        g.setColour(Colours::white.withAlpha(0.4f * mainAlpha * mainBrightness * mainBrightness));
        g.strokePath(outline, PathStrokeType(1.0f),
                     AffineTransform::translation(0.0f, 1.0f).scaled(1.0f, (height - 1.6f) / height));
        g.setColour(Colours::black.withAlpha(0.4f * mainAlpha));
        g.strokePath(outline, PathStrokeType(1.0f));
    }

    static const inline void
    drawSelectionRectangle(Graphics &g, int x, int y, int width, int height, Colour base = Colour(HIGHLIGHT_COLOUR),
                           const float baseSaturation = 0.9f, const float baseAlpha = 0.9f) {
        Colour baseColour(base.withMultipliedSaturation(baseSaturation).withMultipliedAlpha(baseAlpha));
        const float mainBrightness = baseColour.getBrightness();
        const float mainAlpha = baseColour.getFloatAlpha();
        Path outline;

        outline.addRoundedRectangle(x, y, width, height, 4.0f, 4.0f, false, false, false, false);

        g.setGradientFill(ColourGradient(baseColour.brighter(0.2f), x, y, baseColour.darker(0.25f), x, height, false));
        g.fillPath(outline);

        g.setColour(Colours::white.withAlpha(0.4f * mainAlpha * mainBrightness * mainBrightness));
        g.strokePath(outline, PathStrokeType(1.0f),
                     AffineTransform::translation(0.0f, 1.0f).scaled(1.0f, (height - 1.6f) / height));
        g.setColour(Colours::black.withAlpha(0.4f * mainAlpha));
        g.strokePath(outline, PathStrokeType(1.0f));
    }

    // static LookAndFeel_V4::ColourScheme colourSchemeFromProperty(const var &property) {} // Updated v5.6.34. Moved to CtrlrInlineUtilitiesGUI.cpp

    // Declare your custom ColourScheme getter functions
    juce::LookAndFeel_V4::ColourScheme getJetBlackColourScheme();
    juce::LookAndFeel_V4::ColourScheme getYamDxColourScheme();
    juce::LookAndFeel_V4::ColourScheme getAkApcColourScheme();
    juce::LookAndFeel_V4::ColourScheme getAkMpcColourScheme();
    juce::LookAndFeel_V4::ColourScheme getLexiBlueColourScheme();
    juce::LookAndFeel_V4::ColourScheme getKurzGreenColourScheme();
    juce::LookAndFeel_V4::ColourScheme getKorGreyColourScheme();
    juce::LookAndFeel_V4::ColourScheme getKorGoldColourScheme();
    juce::LookAndFeel_V4::ColourScheme getArturOrangeColourScheme();
    juce::LookAndFeel_V4::ColourScheme getAiraGreenColourScheme();

    // Your existing colourSchemeFromProperty function
    juce::LookAndFeel_V4::ColourScheme colourSchemeFromProperty(const juce::var &property);

    // The central LookAndFeel factory function
    // Keep the optional 'colourSchemeProperty' parameter, it's not directly used here but the signature is fine.
    juce::LookAndFeel* createLookAndFeelFromDescription(const juce::String& description,
                                                        const juce::var& colourSchemeProperty = juce::var(),
                                                        bool returnDefaultV4ForUnknown = true);

    // Your existing colourSchemeFromProperty function (can now call createLookAndFeelFromDescription if it uses properties
    // that map directly to the names used in createLookAndFeelFromDescription)
    juce::LookAndFeel_V4::ColourScheme colourSchemeFromProperty(const juce::var &property);
}
#endif
