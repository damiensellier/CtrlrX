/*
  ==============================================================================

    ColorComboBoxLookAndFeel.cpp
    Created: 17 Aug 2025 6:13:44pm
    Author:  zan64

  ==============================================================================
*/

#pragma once

# include "ColorComboBoxLookAndFeel.h"

//==============================================================================
ColorComboBoxLookAndFeel::ColorComboBoxLookAndFeel(const CtrlrLuaMethodCodeEditorSettings::ColourItem* colors)
    : availableColors(colors)
{
    // The constructor's body is here.
    // In this case, it's empty because the member variable is initialized
    // in the initialization list above.
}

void ColorComboBoxLookAndFeel::drawPopupMenuItem(Graphics& g, const Rectangle<int>& area,
    bool isSeparator, bool isActive, bool isHighlighted, bool isTicked,
    bool hasSubMenu, const String& text, const String& shortcutKeyText,
    const Drawable* icon, const Colour* textColourToUse)
{
    if (isSeparator)
    {
        // Draw a separator line
        g.setColour(findColour(PopupMenu::textColourId).withAlpha(0.2f));
        g.drawLine(area.getX(), area.getCentreY(), area.getRight(), area.getCentreY(), 1.0f);
    }
    else
    {
        // Highlight the background if it's the selected item
        Colour background = isHighlighted ? findColour(PopupMenu::highlightedBackgroundColourId)
            : findColour(PopupMenu::backgroundColourId);

        g.setColour(background);
        g.fillRect(area);

        // Find the color from your custom list based on the item's text
        for (int i = 0; availableColors[i].name.isNotEmpty(); ++i)
        {
            if (availableColors[i].name == text)
            {
                // Draw a small color swatch
                g.setColour(availableColors[i].colour);
                g.fillRect(area.getX() + 4, area.getCentreY() - 4, 8, 8); // Adjust position as needed
                break;
            }
        }

        // Draw the text of the menu item
        g.setColour(textColourToUse ? *textColourToUse : findColour(PopupMenu::textColourId));
        g.drawText(text, area.getX() + 20, area.getY(), area.getWidth() - 20, area.getHeight(), Justification::centredLeft);
    }
}
