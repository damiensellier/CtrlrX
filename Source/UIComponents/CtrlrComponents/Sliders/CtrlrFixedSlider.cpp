#include "stdafx.h"
#include "CtrlrFixedSlider.h"
#include "CtrlrLuaManager.h"
#include "CtrlrPanel/CtrlrPanelEditor.h"

CtrlrFixedSlider::CtrlrFixedSlider (CtrlrModulator &owner)
    : CtrlrComponent(owner),
    lfV4(*this, componentTree), // For LookAndFeel_V4 CtrlrSliderLookAndFeel_V4
    lfV3(*this, componentTree), // For LookAndFeel_V3 CtrlrSliderLookAndFeel_V3
    lfV2(*this, componentTree), // For LookAndFeel_V2 CtrlrSliderLookAndFeel_V2
    lf(*this, componentTree), // For LookAndFeel_V2 CtrlrSliderLookAndFeel_V2 LEGACY
    ctrlrSlider (0)
{
    valueMap = new CtrlrValueMap();
    
    /** Tooltip properties */
    setColour (TooltipWindow::textColourId, findColour(Label::textColourId));
    setColour (TooltipWindow::backgroundColourId, findColour(TooltipWindow::backgroundColourId));
    setColour (TooltipWindow::outlineColourId, findColour(TooltipWindow::outlineColourId));
    
    addAndMakeVisible (ctrlrSlider = new CtrlrSliderInternal (*this));
    
    ctrlrSlider->setName ("ctrlrSlider");

    //[UserPreSize]
    ctrlrSlider->addListener (this);
    //ctrlrSlider->setLookAndFeel(lf);  // V5.6.28 and before
    //ctrlrSlider->setLookAndFeel(&lfV4); // V5.6.29 and +
    componentTree.addListener (this);
    
    setProperty (Ids::uiSliderMin, 0);
    setProperty (Ids::uiSliderMax, 1);
    // setProperty (Ids::uiSliderInterval, 1); // Removed v5.6.32. Useless since indexes are stepped 1 by 1
    setProperty (Ids::uiSliderValueSuffix, ""); // Added v5.6.32
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
    
    setProperty (Ids::uiSliderPopupBubble, false);
    
    setProperty (Ids::uiFixedSliderContent, "");
    
    setProperty (Ids::uiSliderLookAndFeel, "Default");
    setProperty (Ids::uiSliderStyle, "RotaryVerticalDrag");
    
    bool LegacyMode = owner.getOwnerPanel().getEditor()->getProperty(Ids::uiPanelLegacyMode);  // Legacy mode flag for version before 5.6.29
	String panelLnF = owner.getOwnerPanel().getEditor()->getProperty(Ids::uiPanelLookAndFeel);
	
	if (LegacyMode || panelLnF == "V3") // Added v5.6.34. Not really good because it will create a new LnF but won't destroy it so it will lead to memory leaks
	{
		setLookAndFeel(new LookAndFeel_V3());
		setProperty(Ids::uiSliderLookAndFeel, "V3");
	}
	
	else if (panelLnF == "V2") // Added v5.6.34. Not really good because it will create a new LnF but won't destroy it so it will lead to memory leaks
	{
		setLookAndFeel(new LookAndFeel_V2());
		setProperty(Ids::uiSliderLookAndFeel, "V2");
	}
	
	else if (panelLnF == "V1") // Added v5.6.34. Not really good because it will create a new LnF but won't destroy it so it will lead to memory leaks
	{
		setLookAndFeel(new LookAndFeel_V1());
		setProperty(Ids::uiSliderLookAndFeel, "V1");
	}
	
	if ( panelLnF == "V3"
		|| panelLnF == "V2"
		|| panelLnF == "V1" )
    {
        setSize (64, 64);
        setProperty (Ids::uiSliderRotaryOutlineColour, "0xff0000ff");  // 0xff0000ff
        setProperty (Ids::uiSliderRotaryFillColour, "0xff0000ff"); // 0xff0000ff
        setProperty (Ids::uiSliderTrackColour, "0xff0f0f0f"); // 0xff0f0f0f
        setProperty (Ids::uiSliderBackgroundColour, "0x00000000"); // Added v5.5.36 for linear bar slider
        setProperty (Ids::uiSliderThumbColour, "0xffff0000"); // 0xffff0000
    }
    else
    {
        setSize (72, 96); // requires Taller default footprint for V4 H90px
        setProperty (Ids::uiSliderRotaryOutlineColour, (String)findColour(Slider::rotarySliderOutlineColourId).toString());
        setProperty (Ids::uiSliderRotaryFillColour, (String)findColour(Slider::rotarySliderFillColourId).toString());
        setProperty (Ids::uiSliderTrackColour, (String)findColour(Slider::rotarySliderFillColourId).toString());
        setProperty (Ids::uiSliderThumbColour, (String)findColour(Slider::thumbColourId).toString());
        setProperty (Ids::uiSliderBackgroundColour, (String)findColour(Slider::backgroundColourId).toString()); // Added v5.5.36 for linear bar slider
    }
 
    setProperty (Ids::uiSliderIncDecButtonColour, (String)findColour (Slider::backgroundColourId).toString());
    setProperty (Ids::uiSliderIncDecTextColour, (String)findColour (Label::textColourId).toString());
    
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
    setProperty (Ids::uiSliderValueTextColour, (String)findColour (Slider::textBoxTextColourId).toString());
    setProperty (Ids::uiSliderValueBgColour, "0x00ffffff");
    setProperty (Ids::uiSliderValueHighlightColour, (String)findColour (Slider::textBoxHighlightColourId).toString());
    setProperty (Ids::uiSliderValueBgColour, "0x00ffffff"); // (String)findColour (Slider::textBoxBackgroundColourId).toString());
    setProperty (Ids::uiSliderValueOutlineColour, "0x00ffffff"); //(String)findColour (Slider::textBoxOutlineColourId).toString());
}

CtrlrFixedSlider::~CtrlrFixedSlider()
{
    deleteAndZero (ctrlrSlider);
}

//==============================================================================
void CtrlrFixedSlider::paint (Graphics& g)
{
}

void CtrlrFixedSlider::resized()
{
    if (restoreStateInProgress)
        return;
    ctrlrSlider->setBounds (getUsableRect());
}

void CtrlrFixedSlider::mouseUp (const MouseEvent& e)
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
        ctrlrSlider->setValue ((double)getProperty(Ids::uiSliderSpringValue), sendNotificationSync);
    }
}


double CtrlrFixedSlider::getComponentMaxValue()
{
    return (valueMap->getNonMappedMax());
}

double CtrlrFixedSlider::getComponentValue()
{
    return ((int)ctrlrSlider->getValue());
}

int CtrlrFixedSlider::getComponentMidiValue()
{
    return (valueMap->getMappedValue(ctrlrSlider->getValue()));
}

const String CtrlrFixedSlider::getComponentText()
{
    return (valueMap->getTextForIndex (ctrlrSlider->getValue()));
}

void CtrlrFixedSlider::setComponentValue (const double newValue, const bool sendChangeMessage)
{
    ctrlrSlider->setValue (newValue, dontSendNotification);
    if (sendChangeMessage)
    {
        owner.getProcessor().setValueGeneric (CtrlrModulatorValue(newValue,CtrlrModulatorValue::changedByGUI));
    }
}

void CtrlrFixedSlider::sliderContentChanged()
{
	valueMap->copyFrom (owner.getProcessor().setValueMap (getProperty(Ids::uiFixedSliderContent)));
	
	// ctrlrSlider->setRange (valueMap->getNonMappedMin(), valueMap->getNonMappedMax(), 1); // Removed v5.6.36
	
	// UPDATED v5.6.36. @Thanks to @dnaldoog
	// FIX for startup/panel-loading crash (juce_NormalisableRange.h:242 assertion failure)
	// that occurs when legacy or complex panels (like the Roland JD-990) initialize sliders with flat (min == max),
	// inverted, or empty ranges.
	// Get the limits from the value map
	const double minVal = valueMap->getNonMappedMin();
	const double maxVal = valueMap->getNonMappedMax();
	
	// Use the safe helper instead of calling ctrlrSlider->setRange directly
	if (ctrlrSlider != nullptr) {
		CtrlrComponent::applySafeSliderRange(*ctrlrSlider, minVal, maxVal, 1.0);
	}
}


void CtrlrFixedSlider::valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property)
{
    if (property == Ids::uiSliderStyle)
    {
        ctrlrSlider->setSliderStyle ((Slider::SliderStyle)CtrlrComponentTypeManager::sliderStringToStyle (getProperty (Ids::uiSliderStyle)));
    }
    else if (property == Ids::uiSliderLookAndFeel)
    {
        updatingLookAndFeel = true; // Set guard flag
        
        String LookAndFeelType = getProperty(property);
        setLookAndFeel(getLookAndFeelFromComponentProperty(LookAndFeelType)); // Updates the current component LookAndFeel
        
        resetLookAndFeelOverrides(); // Retrieves LookAndFeel colours from selected ColourScheme
		
        updatingLookAndFeel = false; // Clear guard flag
    }
    else if (property == Ids::uiSliderRotaryFillColour)
    {
        ctrlrSlider->setColour (Slider::rotarySliderFillColourId, VAR2COLOUR(getProperty (Ids::uiSliderRotaryFillColour)) );
    }
    else if (property == Ids::uiSliderRotaryOutlineColour)
    {
        ctrlrSlider->setColour (Slider::rotarySliderOutlineColourId, VAR2COLOUR(getProperty (Ids::uiSliderRotaryOutlineColour)) );
    }
    else if (property == Ids::uiSliderTrackColour)
    {
        ctrlrSlider->setColour (Slider::trackColourId, VAR2COLOUR(getProperty (Ids::uiSliderTrackColour)) );
    }
    else if (property == Ids::uiSliderBackgroundColour) // Added v5.5.36 for linear bar slider
    {
        ctrlrSlider->setColour (Slider::backgroundColourId, VAR2COLOUR(getProperty (Ids::uiSliderBackgroundColour)) );
    }
    else if (property == Ids::uiSliderThumbColour)
    {
        ctrlrSlider->setColour (Slider::thumbColourId, VAR2COLOUR(getProperty (Ids::uiSliderThumbColour)) );
    }
    else if (property == Ids::uiSliderValueTextColour)
    {
        ctrlrSlider->setColour (Slider::textBoxTextColourId, VAR2COLOUR(getProperty (Ids::uiSliderValueTextColour)) );
    }
    else if (property == Ids::uiSliderValueHighlightColour)
    {
        ctrlrSlider->setColour (Slider::textBoxHighlightColourId, VAR2COLOUR(getProperty (Ids::uiSliderValueHighlightColour)) );
    }
    else if (property == Ids::uiSliderValueBgColour)
    {
        ctrlrSlider->setColour (Slider::textBoxBackgroundColourId, VAR2COLOUR(getProperty (Ids::uiSliderValueBgColour)) );
    }
    else if (property == Ids::uiSliderValueOutlineColour)
    {
        ctrlrSlider->setColour (Slider::textBoxOutlineColourId, VAR2COLOUR(getProperty (Ids::uiSliderValueOutlineColour)) );
    }
    else if (property == Ids::uiFixedSliderContent)
    {
        sliderContentChanged();
    }
    else if (property == Ids::uiSliderValueSuffix) // Added v5.6.32
    {
        ctrlrSlider->setTextValueSuffix(getProperty(Ids::uiSliderValueSuffix).toString());
        ctrlrSlider->lookAndFeelChanged();
    }
    else if (property == Ids::uiSliderValuePosition || property == Ids::uiSliderValueHeight || property == Ids::uiSliderValueWidth)
    {
        ctrlrSlider->setTextBoxStyle (
            (Slider::TextEntryBoxPosition)(int)getProperty (Ids::uiSliderValuePosition),
            false,
            getProperty (Ids::uiSliderValueWidth, 64),
            getProperty (Ids::uiSliderValueHeight, 12));
    }
    else if (property == Ids::uiSliderSetNotificationOnlyOnRelease)
    {
    ctrlrSlider->setChangeNotificationOnlyOnRelease((bool)getProperty(Ids::uiSliderSetNotificationOnlyOnRelease));
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
            ctrlrSlider->setLookAndFeel (nullptr); // Warning, it resets the LnF on loading
            ctrlrSlider->setLookAndFeel (&lf); // V5.6.28 and before
        }
        else
        {
            //ctrlrSlider->setLookAndFeel (nullptr); // Warning, it resets the LnF on loading
            //ctrlrSlider->setLookAndFeel (&lfV4); // V5.6.28+
        }
    }
    else if (property == Ids::uiSliderVelocityMode
        || property == Ids::uiSliderVelocityModeKeyTrigger
        || property == Ids::uiSliderVelocitySensitivity
        || property == Ids::uiSliderVelocityThreshold
        || property == Ids::uiSliderVelocityOffset
        )
    {
        ctrlrSlider->setVelocityBasedMode((bool)getProperty(Ids::uiSliderVelocityMode));
        ctrlrSlider->setVelocityModeParameters ((double)getProperty(Ids::uiSliderVelocitySensitivity),
                                                (int)getProperty(Ids::uiSliderVelocityThreshold),
                                                (double)getProperty(Ids::uiSliderVelocityOffset),
                                                (bool)getProperty(Ids::uiSliderVelocityModeKeyTrigger));
    }
    else if (property == Ids::uiSliderSpringValue)
    {
        ctrlrSlider->setValue (getProperty(property), dontSendNotification);
    }
    else if (property == Ids::uiSliderSpringMode)
    {
        if ((bool)getProperty(property) == true)
        {
            ctrlrSlider->setValue (getProperty(Ids::uiSliderSpringValue), dontSendNotification);
        }
    }
    else if (property == Ids::uiSliderDoubleClickValue
            || property == Ids::uiSliderDoubleClickEnabled)
    {
        ctrlrSlider->setDoubleClickReturnValue ((bool)getProperty(Ids::uiSliderDoubleClickEnabled), getProperty(Ids::uiSliderDoubleClickValue));
    }
    else if (property == Ids::uiSliderPopupBubble)
    {
        ctrlrSlider->setPopupDisplayEnabled ((bool)getProperty(property), (bool)getProperty(property), owner.getOwnerPanel().getEditor());
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

const String CtrlrFixedSlider::getTextForValue(const double value)
{
    return (valueMap->getTextForIndex(value));
}

void CtrlrFixedSlider::sliderValueChanged (Slider* sliderThatWasMoved)
{
    setComponentValue(ctrlrSlider->getValue(), true);
}

LookAndFeel *CtrlrFixedSlider::getLookAndFeelFromComponentProperty(const String &lookAndFeelComponentProperty) // Updated v5.6.34
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

void CtrlrFixedSlider::resetLookAndFeelOverrides()
{
    if (restoreStateInProgress == false) // To prevent the prop lines stacking up from top and keeping their original position
    {
        setProperty (Ids::componentLabelColour, (String)findColour(Label::textColourId).toString());
        
        setProperty (Ids::uiSliderRotaryOutlineColour, (String)findColour(Slider::rotarySliderOutlineColourId).toString());
        setProperty (Ids::uiSliderRotaryFillColour, (String)findColour(Slider::rotarySliderFillColourId).toString());
        setProperty (Ids::uiSliderThumbColour, (String)findColour(Slider::thumbColourId).toString());

        setProperty (Ids::uiSliderTrackColour, (String)findColour(Slider::rotarySliderFillColourId).toString());
        setProperty (Ids::uiSliderBackgroundColour, (String)findColour(Slider::backgroundColourId).toString()); // Added v5.5.36 for linear bar slider

        setProperty (Ids::uiSliderIncDecTextColour, (String)findColour(Slider::textBoxTextColourId).toString());
        setProperty (Ids::uiSliderIncDecButtonColour, (String)findColour(Slider::backgroundColourId).toString());
        
        setProperty (Ids::uiSliderValueTextColour, (String)findColour(Slider::textBoxTextColourId).toString());
        setProperty (Ids::uiSliderValueHighlightColour, (String)findColour(Slider::textBoxHighlightColourId).toString());
        setProperty (Ids::uiSliderValueBgColour, "0x00ffffff"); // (String)findColour (Slider::textBoxBackgroundColourId).toString());
        setProperty (Ids::uiSliderValueOutlineColour, "0x00ffffff"); //(String)findColour (Slider::textBoxOutlineColourId).toString());
        
        updatePropertiesPanel(); // Refreshes property pane
    }
}

void CtrlrFixedSlider::updatePropertiesPanel()
{
    CtrlrPanelProperties *props = owner.getCtrlrManagerOwner().getActivePanel()->getEditor(false)->getPropertiesPanel();
    if (props)
    {
        props->refreshAll(); // Needs extra code to prevent scrolling back to top on refresh
    }
}

void CtrlrFixedSlider::showTextEditor()
{
    ctrlrSlider->showTextBox();
}
