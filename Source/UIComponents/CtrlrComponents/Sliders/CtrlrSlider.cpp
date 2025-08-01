#include "stdafx.h"
#include "CtrlrSlider.h"
#include "CtrlrLuaManager.h"
#include "CtrlrProcessor.h"
#include "../CtrlrComponentTypeManager.h"
#include "CtrlrPanel/CtrlrPanelEditor.h"
#include "CtrlrModulator/CtrlrModulator.h"
#include "Lua/JuceClasses/LLookAndFeel.h"

#include "CtrlrPanel/CtrlrPanelProperties.h"
#include "CtrlrPanel/CtrlrPanelComponentProperties.h"


CtrlrSlider::CtrlrSlider (CtrlrModulator &owner)
    :    CtrlrComponent(owner),
        lfV4(*this, componentTree), // For LookAndFeel_V4 CtrlrSliderLookAndFeel_V4
        lfV3(*this, componentTree), // For LookAndFeel_V3 CtrlrSliderLookAndFeel_V3
        lfV2(*this, componentTree), // For LookAndFeel_V2 CtrlrSliderLookAndFeel_V2
        lf(*this, componentTree), // For LookAndFeel_V2 CtrlrSliderLookAndFeel_V2 LEGACY
        ctrlrSlider (*this)
{

    /** Tooltip properties */
    setColour (TooltipWindow::textColourId, findColour(Label::textColourId));
    setColour (TooltipWindow::backgroundColourId, findColour(TooltipWindow::backgroundColourId));
    setColour (TooltipWindow::outlineColourId, findColour(TooltipWindow::outlineColourId));
    
    addAndMakeVisible (&ctrlrSlider);
    
    ctrlrSlider.setRange (0, 127, 1);
    ctrlrSlider.setSliderStyle (Slider::RotaryVerticalDrag);
    ctrlrSlider.setTextBoxStyle (Slider::TextBoxBelow, false, 64, 12);
    
    ctrlrSlider.addListener (this);
    //ctrlrSlider.setLookAndFeel (&lf); // V5.6.28 and before
    //ctrlrSlider.setLookAndFeel (&lfV4); // V5.6.29 and +
    componentTree.addListener (this);
        
    setProperty (Ids::uiSliderMin, 0);
    setProperty (Ids::uiSliderMax, 127);
    setProperty (Ids::uiSliderInterval, 1);
    setProperty (Ids::uiSliderDecimalPlaces, 0);
    setProperty (Ids::uiSliderValueSuffix, "");
    setProperty (Ids::uiSliderSetNotificationOnlyOnRelease, false);
    setProperty (Ids::uiSliderDoubleClickEnabled, true);
    setProperty (Ids::uiSliderDoubleClickValue, 0);
    
    setProperty (Ids::uiSliderVelocitySensitivity, 1.0);
    setProperty (Ids::uiSliderVelocityThreshold, 1);
    setProperty (Ids::uiSliderVelocityOffset, 0.0);
    setProperty (Ids::uiSliderVelocityMode, false);
    setProperty (Ids::uiSliderVelocityModeKeyTrigger, true);
    
    setProperty (Ids::uiSliderSpringMode, false);
    setProperty (Ids::uiSliderSpringValue, 0);
    
    setProperty (Ids::uiSliderMouseWheelInterval, 1);
    
    setProperty (Ids::uiSliderLookAndFeel, "Default");
    setProperty (Ids::uiSliderLookAndFeelIsCustom, false);
    
    setProperty (Ids::uiSliderPopupBubble, false);
    
    setProperty (Ids::uiSliderStyle, "RotaryVerticalDrag");
    
    bool LegacyMode = owner.getOwnerPanel().getEditor()->getProperty(Ids::uiPanelLegacyMode);  // Legacy mode flag for version before 5.6.29
    if (LegacyMode)
    {
        setLookAndFeel(new LookAndFeel_V3());
        setProperty(Ids::uiSliderLookAndFeel, "V3");
    }
    
    if ( owner.getOwnerPanel().getEditor()->getProperty(Ids::uiPanelLookAndFeel) == "V3"
        || owner.getOwnerPanel().getEditor()->getProperty(Ids::uiPanelLookAndFeel) == "V2"
        || owner.getOwnerPanel().getEditor()->getProperty(Ids::uiPanelLookAndFeel) == "V1" )
    {
        setSize (64, 64);
        setProperty (Ids::uiSliderRotaryOutlineColour, "0xff0000ff");  // 0xff0000ff
        setProperty (Ids::uiSliderRotaryFillColour, "0xff0000ff"); // 0xff0000ff
        setProperty (Ids::uiSliderThumbColour, "0xffff0000"); // 0xffff0000
        setProperty (Ids::uiSliderTrackColour, "0xff0f0f0f"); // 0xff0f0f0f
        setProperty (Ids::uiSliderLookAndFeelIsCustom, false);
    }
    else
    {
        setSize (72, 96); // requires Taller default footprint for V4 H90px min
        setProperty (Ids::uiSliderRotaryOutlineColour, (String)findColour(Slider::rotarySliderOutlineColourId).toString());
        setProperty (Ids::uiSliderRotaryFillColour, (String)findColour(Slider::rotarySliderFillColourId).toString());
        setProperty (Ids::uiSliderThumbColour, (String)findColour(Slider::thumbColourId).toString());
        setProperty (Ids::uiSliderTrackColour, (String)findColour(Slider::rotarySliderFillColourId).toString());
        setProperty (Ids::uiSliderLookAndFeelIsCustom, false);
    }
    
    setProperty (Ids::uiSliderIncDecButtonColour, (String)findColour(Slider::backgroundColourId).toString());
    setProperty (Ids::uiSliderIncDecTextColour, (String)findColour(Label::textColourId).toString());
    
    /**For LookAndFeel_V2 only*/
    setProperty (Ids::uiSliderTrackCornerSize, 5);
    setProperty (Ids::uiSliderThumbCornerSize, 3);
    setProperty (Ids::uiSliderThumbWidth, 0);
    setProperty (Ids::uiSliderThumbHeight, 0);
    setProperty (Ids::uiSliderThumbFlatOnLeft, false);
    setProperty (Ids::uiSliderThumbFlatOnRight, false);
    setProperty (Ids::uiSliderThumbFlatOnTop, false);
    setProperty (Ids::uiSliderThumbFlatOnBottom, false);
    
    setProperty (Ids::uiSliderValuePosition, (int)Slider::TextBoxBelow);
    setProperty (Ids::uiSliderValueWidth, 64);
    setProperty (Ids::uiSliderValueHeight, 10);
    setProperty (Ids::uiSliderValueTextJustification, "centred");
    setProperty (Ids::uiSliderValueFont, FONT2STR (Font(12)));
    setProperty (Ids::uiSliderValueTextColour, (String)findColour(Slider::textBoxTextColourId).toString());
    setProperty (Ids::uiSliderValueHighlightColour, (String)findColour(Slider::textBoxHighlightColourId).toString());
    setProperty (Ids::uiSliderValueBgColour, "0x00ffffff"); // (String)findColour(Slider::textBoxBackgroundColourId).toString());
    setProperty (Ids::uiSliderValueOutlineColour, "0x00ffffff"); //(String)findColour(Slider::textBoxOutlineColourId).toString());
    
    setProperty (Ids::uiSliderLookAndFeelIsCustom, false);
}

CtrlrSlider::~CtrlrSlider()
{
    componentTree.removeListener (this);
    ctrlrSlider.setLookAndFeel (nullptr);
}

void CtrlrSlider::resized()
{
    if (restoreStateInProgress)
        return;
    ctrlrSlider.setBounds (getUsableRect());
}

void CtrlrSlider::sliderValueChanged (Slider* sliderThatWasMoved)
{
    if (sliderThatWasMoved == &ctrlrSlider)
    {
        if ((bool)owner.getOwnerPanel().getEditor()->getProperty(Ids::uiPanelEditMode) == true)
            return;

        setComponentValue (ctrlrSlider.getValue(), true);
    }
}

void CtrlrSlider::mouseUp (const MouseEvent& e)
{
    if (mouseUpCbk && !mouseUpCbk.wasObjectDeleted())
    {
        if (mouseUpCbk->isValid())
        {
            owner.getOwnerPanel().getCtrlrLuaManager().getMethodManager().call (mouseUpCbk, this, e);
        }
    }
    if ((bool)getProperty(Ids::uiSliderSpringMode) == true)
    {
        ctrlrSlider.setValue ((double)getProperty(Ids::uiSliderSpringValue), sendNotificationSync);
    }
}

double CtrlrSlider::getComponentValue()
{
    return (ctrlrSlider.getValue());
}

int CtrlrSlider::getComponentMidiValue()
{
    return ((int)ctrlrSlider.getValue());
}

double CtrlrSlider::getComponentMaxValue()
{
    return (ctrlrSlider.getMaximum());
}

void CtrlrSlider::setComponentValue (const double newValue, const bool sendChangeMessage)
{
    ctrlrSlider.setValue (newValue, dontSendNotification);
    if (sendChangeMessage)
    {
        owner.getProcessor().setValueGeneric (CtrlrModulatorValue(newValue,CtrlrModulatorValue::changedByGUI));
    }
}

const Array<Font> CtrlrSlider::getFontList()
{
    Array <Font> ret;
    Font f = STR2FONT(getProperty(Ids::uiSliderValueFont));
    if (f.getTypefaceName() != Font::getDefaultSerifFontName()
        && f.getTypefaceName() != Font::getDefaultSansSerifFontName()
        && f.getTypefaceName() != Font::getDefaultMonospacedFontName()
        && f.getTypefaceName() != "<Sans-Serif>")
    {
        ret.add (f);
    }
    return (ret);
}

void CtrlrSlider::valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property)
{
    if (property == Ids::uiSliderStyle)
    {
        ctrlrSlider.setSliderStyle ((Slider::SliderStyle)CtrlrComponentTypeManager::sliderStringToStyle (getProperty (Ids::uiSliderStyle)));
    }
    else if (property == Ids::uiSliderLookAndFeel)
    {
        String LookAndFeelType = getProperty(property);
        setLookAndFeel(CtrlrSlider::getLookAndFeelFromComponentProperty(LookAndFeelType)); // Updates the current component LookAndFeel
        
        if (LookAndFeelType == "Default")
        {
            setProperty(Ids::uiSliderLookAndFeelIsCustom, false); // Resets the Customized Flag to False to allow Global L&F to apply
        }
        
        if (!getProperty(Ids::uiSliderLookAndFeelIsCustom))
        {
            CtrlrSlider::resetLookAndFeelOverrides(); // Retrieves LookAndFeel colours from selected ColourScheme
        }
    }
    else if (property == Ids::uiSliderRotaryFillColour)
    {
        ctrlrSlider.setColour (Slider::rotarySliderFillColourId, VAR2COLOUR(getProperty (Ids::uiSliderRotaryFillColour)) );
        setProperty(Ids::uiSliderLookAndFeelIsCustom, true); // Locks the component custom colourScheme
    }
    else if (property == Ids::uiSliderRotaryOutlineColour)
    {
        ctrlrSlider.setColour (Slider::rotarySliderOutlineColourId, VAR2COLOUR(getProperty (Ids::uiSliderRotaryOutlineColour)) );
        setProperty(Ids::uiSliderLookAndFeelIsCustom, true); // Locks the component custom colourScheme
    }
    else if (property == Ids::uiSliderTrackColour)
    {
        ctrlrSlider.setColour (Slider::trackColourId, VAR2COLOUR(getProperty (Ids::uiSliderTrackColour)) );
        setProperty(Ids::uiSliderLookAndFeelIsCustom, true); // Locks the component custom colourScheme
    }
    else if (property == Ids::uiSliderThumbColour)
    {
        ctrlrSlider.setColour (Slider::thumbColourId, VAR2COLOUR(getProperty (Ids::uiSliderThumbColour)) );
        setProperty(Ids::uiSliderLookAndFeelIsCustom, true); // Locks the component custom colourScheme
    }
    else if (property == Ids::uiSliderValueTextColour)
    {
        ctrlrSlider.setColour (Slider::textBoxTextColourId, VAR2COLOUR(getProperty (Ids::uiSliderValueTextColour)) );
        setProperty(Ids::uiSliderLookAndFeelIsCustom, true); // Locks the component custom colourScheme
    }
    else if (property == Ids::uiSliderValueHighlightColour)
    {
        ctrlrSlider.setColour (Slider::textBoxHighlightColourId, VAR2COLOUR(getProperty (Ids::uiSliderValueHighlightColour)) );
        setProperty(Ids::uiSliderLookAndFeelIsCustom, true); // Locks the component custom colourScheme
    }
    else if (property == Ids::uiSliderValueBgColour)
    {
        ctrlrSlider.setColour (Slider::textBoxBackgroundColourId, VAR2COLOUR(getProperty (Ids::uiSliderValueBgColour)) );
        setProperty(Ids::uiSliderLookAndFeelIsCustom, true); // Locks the component custom colourScheme
    }
    else if (property == Ids::uiSliderValueOutlineColour)
    {
        ctrlrSlider.setColour (Slider::textBoxOutlineColourId, VAR2COLOUR(getProperty (Ids::uiSliderValueOutlineColour)) );
        setProperty(Ids::uiSliderLookAndFeelIsCustom, true); // Locks the component custom colourScheme
    }
    else if (property == Ids::uiSliderInterval || property == Ids::uiSliderMax || property == Ids::uiSliderMin)
    {
        // This was the script in v5.2.198
        // ctrlrSlider.setRange ( (double) getProperty (Ids::uiSliderMin), (double) getProperty (Ids::uiSliderMax), (double) getProperty (Ids::uiSliderInterval) );
        //owner.setProperty (Ids::modulatorMax, ctrlrSlider.getMaximum());
        //owner.setProperty (Ids::modulatorMin, ctrlrSlider.getMinimum());
        
        // The following script is different since v5.6.26+
        // v5.6.32 since all values are set as double it needs to be dble too, not float, or we'll get strange equivalent values with dble->float
        double max = getProperty (Ids::uiSliderMax);  // Updated v5.6.32. double instead of float
        double min = getProperty (Ids::uiSliderMin);  // Updated v5.6.32. double instead of float
        double interval = getProperty (Ids::uiSliderInterval); // Updated v5.6.32. double instead of float
        if (interval == 0)
            interval = std::abs(max-min) + 1;
        // For JUCE MAX must be >= min
        if (max <= min) {
            // samething between 0.5 and 1 times the interval
            // to avoid rounding errors
            max = min + interval * 0.66;
        }
        ctrlrSlider.setRange ( min, max, interval );
        owner.setProperty (Ids::modulatorMin, ctrlrSlider.getMinimum());
        owner.setProperty (Ids::modulatorMax, ctrlrSlider.getMaximum());
        lookAndFeelChanged();
    }
    else if (property == Ids::uiSliderDecimalPlaces) // Added v5.6.32
    {
        ctrlrSlider.setNumDecimalPlacesToDisplay((int)getProperty(Ids::uiSliderDecimalPlaces));
        ctrlrSlider.lookAndFeelChanged();
        
    }
    else if (property == Ids::uiSliderValueSuffix) // Added v5.6.32
    {
        ctrlrSlider.setTextValueSuffix(getProperty(Ids::uiSliderValueSuffix).toString());
        ctrlrSlider.lookAndFeelChanged();
    }
    else if (property == Ids::uiSliderValuePosition || property == Ids::uiSliderValueHeight || property == Ids::uiSliderValueWidth)
    {
        ctrlrSlider.setTextBoxStyle (
            (Slider::TextEntryBoxPosition)(int)getProperty(Ids::uiSliderValuePosition),
            false,
            getProperty (Ids::uiSliderValueWidth, 64),
            getProperty (Ids::uiSliderValueHeight, 12));
        
        ctrlrSlider.lookAndFeelChanged();
    }
    else if (property == Ids::uiSliderSetNotificationOnlyOnRelease)
    {
        ctrlrSlider.setChangeNotificationOnlyOnRelease((bool)getProperty(Ids::uiSliderSetNotificationOnlyOnRelease));
    }
    else if (property == Ids::uiSliderIncDecButtonColour
             || property == Ids::uiSliderIncDecTextColour
             || property == Ids::uiSliderValueFont
             || property == Ids::uiSliderValueTextJustification)
    {
        
        if ( getProperty(Ids::uiSliderLookAndFeel) == "V3"
            || getProperty(Ids::uiSliderLookAndFeel) == "V2"
            || getProperty(Ids::uiSliderLookAndFeel) == "V1" )
        {
            ctrlrSlider.setLookAndFeel (nullptr); // Warning, it resets the LnF on loading
            ctrlrSlider.setLookAndFeel (&lf); // V5.6.28 and before
        }
        else
        {
            ctrlrSlider.setLookAndFeel (nullptr); // Warning, it resets the LnF on loading. Uncommented v5.6.32
            ctrlrSlider.setLookAndFeel (&lfV4); // V5.6.28+. Uncommented v5.6.32
        }
        setProperty(Ids::uiSliderLookAndFeelIsCustom, true); // Locks the component custom colourScheme
        ctrlrSlider.lookAndFeelChanged(); // Added v5.6.32
    }
    else if (property == Ids::uiSliderVelocityMode
        || property == Ids::uiSliderVelocityModeKeyTrigger
        || property == Ids::uiSliderVelocitySensitivity
        || property == Ids::uiSliderVelocityThreshold
        || property == Ids::uiSliderVelocityOffset
        )
    {
        ctrlrSlider.setVelocityBasedMode((bool)getProperty(Ids::uiSliderVelocityMode));
        ctrlrSlider.setVelocityModeParameters ((double)getProperty(Ids::uiSliderVelocitySensitivity),
                                                (int)getProperty(Ids::uiSliderVelocityThreshold),
                                                (double)getProperty(Ids::uiSliderVelocityOffset),
                                                (bool)getProperty(Ids::uiSliderVelocityModeKeyTrigger));
    }
    else if (property == Ids::uiSliderSpringValue)
    {
        ctrlrSlider.setValue (getProperty(property), dontSendNotification);
    }
    else if (property == Ids::uiSliderDoubleClickValue
            || property == Ids::uiSliderDoubleClickEnabled)
    {
        ctrlrSlider.setDoubleClickReturnValue ((bool)getProperty(Ids::uiSliderDoubleClickEnabled), getProperty(Ids::uiSliderDoubleClickValue));
    }
    else if (property == Ids::uiSliderSpringMode)
    {
        if ((bool)getProperty(property) == true)
        {
            ctrlrSlider.setValue (getProperty(Ids::uiSliderSpringValue), dontSendNotification);
        }
    }
    else if (property == Ids::uiSliderPopupBubble)
    {
        ctrlrSlider.setPopupDisplayEnabled ((bool)getProperty(property), (bool)getProperty(property), owner.getOwnerPanel().getEditor());
    }
    else
    {
        CtrlrComponent::valueTreePropertyChanged(treeWhosePropertyHasChanged, property);
    }
    if (restoreStateInProgress == false)
    {
        resized();
    }
}

const String CtrlrSlider::getComponentText()
{
    return (String(getComponentValue()));
}

void CtrlrSlider::customLookAndFeelChanged(LookAndFeelBase *customLookAndFeel) // For LUA script custom L&F
{
    if (customLookAndFeel == nullptr)
    {
        ctrlrSlider.setLookAndFeel (nullptr);
        
        if ( getProperty(Ids::uiSliderLookAndFeel) == "V3"
            || getProperty(Ids::uiSliderLookAndFeel) == "V2"
            || getProperty(Ids::uiSliderLookAndFeel) == "V1" )
        {
            ctrlrSlider.setLookAndFeel (&lf); // V5.6.28 and before
        }
        else
        {
            ctrlrSlider.setLookAndFeel (&lfV4); // V5.6.28+
        }
    }
    else
    {
        ctrlrSlider.setLookAndFeel (customLookAndFeel);
    }
}

LookAndFeel *CtrlrSlider::getLookAndFeelFromComponentProperty(const String &lookAndFeelComponentProperty) // Updated v5.6.34
{
    if (lookAndFeelComponentProperty == "Default")
    {
        // This case still means "use the default LookAndFeel (which might be the global one)"
        // so returning nullptr is appropriate if that's the desired behavior.
        return nullptr;
    }

    // Call your new generic factory function
    // We pass 'false' for the second argument here, as 'Default' is handled separately
    // and an unknown string should likely result in nullptr to fall back to the global L&F.
    return gui::createLookAndFeelFromDescription(lookAndFeelComponentProperty, false);
}

void CtrlrSlider::resetLookAndFeelOverrides()
{
    if (restoreStateInProgress == false) // To prevent the prop lines stacking up from top and keeping their original position
    {
        setProperty (Ids::componentLabelColour, (String)findColour(Label::textColourId).toString());
        
        setProperty (Ids::uiSliderRotaryOutlineColour, (String)findColour(Slider::rotarySliderOutlineColourId).toString());
        setProperty (Ids::uiSliderRotaryFillColour, (String)findColour(Slider::rotarySliderFillColourId).toString());
        setProperty (Ids::uiSliderThumbColour, (String)findColour(Slider::thumbColourId).toString());
        
        setProperty (Ids::uiSliderTrackColour, (String)findColour(Slider::rotarySliderFillColourId).toString());

        setProperty (Ids::uiSliderIncDecTextColour, (String)findColour(Slider::textBoxTextColourId).toString());
        setProperty (Ids::uiSliderIncDecButtonColour, (String)findColour(Slider::backgroundColourId).toString());
        
        setProperty (Ids::uiSliderValueTextColour, (String)findColour(Slider::textBoxTextColourId).toString());
        setProperty (Ids::uiSliderValueHighlightColour, (String)findColour(Slider::textBoxHighlightColourId).toString());
        setProperty (Ids::uiSliderValueBgColour, "0x00ffffff"); // (String)findColour(Slider::textBoxBackgroundColourId).toString());
        setProperty (Ids::uiSliderValueOutlineColour, "0x00ffffff"); //(String)findColour(Slider::textBoxOutlineColourId).toString());
        
        setProperty (Ids::uiSliderLookAndFeelIsCustom, false); // Resets the component colourScheme if a new default colourScheme is selected from the menu
        
        updatePropertiesPanel(); // Refreshes property pane
    }
}

void CtrlrSlider::updatePropertiesPanel()
{
    CtrlrPanelProperties *props = owner.getCtrlrManagerOwner().getActivePanel()->getEditor(false)->getPropertiesPanel();
    if (props)
    {
        props->refreshAll(); // Needs extra code to prevent scrolling back to top on refresh
    }
}
