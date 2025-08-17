#pragma once    
#include <juce_gui_basics/juce_gui_basics.h>

using namespace juce;

// Custom LookAndFeel for the colour combo box
class ColourComboBoxLookAndFeel : public LookAndFeel_V4
{
public:
    ColourComboBoxLookAndFeel()
    {
        // Add some debug output
        DBG("ColourComboBoxLookAndFeel created");
    }

    void drawPopupMenuItem(Graphics& g, const Rectangle<int>& area,
        bool isSeparator, bool isActive, bool isHighlighted,
        bool isTicked, bool hasSubMenu, const String& text,
        const String& shortcutKeyText,
        const Drawable* icon, const Colour* textColour) override
    {
        DBG("drawPopupMenuItem called for: " + text);

        if (isSeparator)
        {
            LookAndFeel_V4::drawPopupMenuItem(g, area, isSeparator, isActive, isHighlighted,
                isTicked, hasSubMenu, text, shortcutKeyText, icon, textColour);
            return;
        }

        Rectangle<int> r = area;

        // Draw background first
        if (isHighlighted && isActive)
        {
            g.setColour(findColour(PopupMenu::highlightedBackgroundColourId));
            g.fillRect(r);
        }
        else
        {
            // Use a light background color instead of the default dark one
            g.setColour(Colours::white);
            g.fillRect(r);
        }

        // Draw color thumbnail centered and wider
        Rectangle<int> colorRect = r.withSizeKeepingCentre(r.getWidth() - 8, 16);
        colorRect = colorRect.removeFromLeft(50).withX(r.getX() + 4);

        // Find the actual color by name
        int itemIndex = colourNames.indexOf(text);

        if (itemIndex >= 0 && itemIndex < colours.size())
        {
            g.setColour(colours[itemIndex]);
            g.fillRect(colorRect);
            g.setColour(Colours::black);
            g.drawRect(colorRect, 2);
        }

        // Draw text with proper color for visibility
        Rectangle<int> textArea = r.withTrimmedLeft(58);

        if (isHighlighted && isActive)
        {
            g.setColour(Colours::white); // White text on highlighted background
        }
        else
        {
            g.setColour(Colours::black); // Black text on white background
        }

        g.setFont(getPopupMenuFont());
        g.drawFittedText(text, textArea, Justification::centredLeft, 1);

        // Draw checkmark for selected item
        if (isTicked)
        {
            Rectangle<int> tickArea = textArea.removeFromRight(20);
            g.setColour(findColour(PopupMenu::textColourId));

            // Draw a simple checkmark
            Path tickPath;
            tickPath.addRoundedRectangle(tickArea.reduced(6).toFloat(), 2.0f);
            g.fillPath(tickPath);
        }
    }

    void addColourMapping(const String& name, const Colour& colour)
    {
        colourNames.add(name);
        colours.add(colour);
        DBG("Added color mapping: " + name + " -> " + colour.toString());
    }

    void clearColours()
    {
        colourNames.clear();
        colours.clear();
        DBG("Cleared all color mappings");
    }

private:
    Array<Colour> colours;
    StringArray colourNames;
};

class ColourComboBox : public ComboBox
{
public:
    ColourComboBox(const String& name) : ComboBox(name)
    {
        customLookAndFeel = new ColourComboBoxLookAndFeel();
        setLookAndFeel(customLookAndFeel);

        // Try to force normal ComboBox appearance - use default LookAndFeel colors
        LookAndFeel& defaultLnF = LookAndFeel::getDefaultLookAndFeel();
        setColour(ComboBox::backgroundColourId, defaultLnF.findColour(ComboBox::backgroundColourId));
        setColour(ComboBox::textColourId, defaultLnF.findColour(ComboBox::textColourId));
        setColour(ComboBox::outlineColourId, defaultLnF.findColour(ComboBox::outlineColourId));
        setColour(ComboBox::buttonColourId, defaultLnF.findColour(ComboBox::buttonColourId));
        setColour(ComboBox::arrowColourId, defaultLnF.findColour(ComboBox::arrowColourId));

        DBG("ColourComboBox created with custom LookAndFeel");
    }

    ~ColourComboBox()
    {
        setLookAndFeel(nullptr);
        delete customLookAndFeel;
    }

    void addColourItem(const String& name, const Colour& colour, int itemId)
    {
        addItem(name, itemId);
        colours.add(colour);
        colourNames.add(name);

        // Update the LookAndFeel with the new color mapping
        customLookAndFeel->addColourMapping(name, colour);

        DBG("Added color item: " + name + " with ID: " + String(itemId));
    }

    void clearAllItems()
    {
        ComboBox::clear();
        colours.clear();
        colourNames.clear();
        customLookAndFeel->clearColours();
    }

    void paint(Graphics& g) override
    {
        // Call parent paint first
        ComboBox::paint(g);

        // Draw color thumbnail for selected item - made much wider
        if (getSelectedId() > 0)
        {
            Rectangle<int> bounds = getLocalBounds();
            // Make thumbnail much wider - takes up about 60 pixels from the right
            Rectangle<int> colorRect = bounds.removeFromRight(60).reduced(3);

            int selectedIndex = getSelectedItemIndex();

            if (selectedIndex >= 0 && selectedIndex < colours.size())
            {
                g.setColour(colours[selectedIndex]);
                g.fillRect(colorRect);
                g.setColour(Colours::black);
                g.drawRect(colorRect, 1);
            }
        }
    }

    // Debug method to check what's stored
    void debugColors()
    {
        DBG("=== ColourComboBox Debug Info ===");
        DBG("Number of items: " + String(getNumItems()));
        DBG("Number of colours: " + String(colours.size()));
        DBG("Number of colour names: " + String(colourNames.size()));

        for (int i = 0; i < getNumItems() && i < colours.size(); ++i)
        {
            DBG("Item " + String(i) + ": " + getItemText(i) + " -> " + colours[i].toString());
        }
        DBG("=== End Debug Info ===");
    }

private:
    Array<Colour> colours;
    StringArray colourNames;
    ColourComboBoxLookAndFeel* customLookAndFeel;
};