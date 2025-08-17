#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "CtrlrLuaMethodCodeEditorSettings.h" .

using namespace juce;

// Existing class definition
class ColorComboBoxLookAndFeel : public LookAndFeel_V4
{
public:
    ColorComboBoxLookAndFeel(const CtrlrLuaMethodCodeEditorSettings::ColourItem* colors);

    void drawPopupMenuItem(Graphics& g, const Rectangle<int>& area,
        bool isSeparator, bool isActive, bool isHighlighted, bool isTicked,
        bool hasSubMenu, const String& text, const String& shortcutKeyText,
        const Drawable* icon, const Colour* textColourToUse) override;

private:
    const CtrlrLuaMethodCodeEditorSettings::ColourItem* availableColors;
};