#include "stdafx.h"
#include "CtrlrToggleButton.h"
#include "CtrlrValueMap.h"
#include "CtrlrModulator/CtrlrModulator.h"
#include "CtrlrPanel/CtrlrPanelEditor.h"
#include "CtrlrIDs.h"
#include "CtrlrPanel/CtrlrPanel.h"

CtrlrToggleButton::CtrlrToggleButton (CtrlrModulator &owner)
    : CtrlrComponent(owner),
      ctrlrButton (0)
{
	valueMap = std::make_unique<CtrlrValueMap>();
    addAndMakeVisible (ctrlrButton = new ToggleButton ("ctrlrButton"));
    ctrlrButton->setButtonText ("Button");
    ctrlrButton->addListener (this);

	ctrlrButton->setBufferedToImage (true);
	setProperty (Ids::uiToggleButtonText, "Button");
	setProperty (Ids::uiButtonTrueValue, 1);
	setProperty (Ids::uiButtonFalseValue, 0);
	owner.setProperty (Ids::modulatorMax, 1);
	owner.setProperty (Ids::modulatorMin, 0);
    
    setProperty (Ids::uiButtonLookAndFeel, "Default");

    if ( owner.getOwnerPanel().getEditor()->getProperty(Ids::uiPanelLookAndFeel) == "V3"
        || owner.getOwnerPanel().getEditor()->getProperty(Ids::uiPanelLookAndFeel) == "V2"
        || owner.getOwnerPanel().getEditor()->getProperty(Ids::uiPanelLookAndFeel) == "V1" )
    {
        setProperty (Ids::uiButtonTextColourOn, "0xff000000");
        setProperty (Ids::uiButtonColourOff, "0xff0000ff");
        setProperty (Ids::uiToggleButtonFocusOutline, "0x00000000");
        setProperty (Ids::uiToggleButtontickColour, "0x00000000");
    }
    else if (owner.getOwnerPanel().getEditor()->getProperty(Ids::uiPanelLookAndFeel) == "V4 Light") // Because V4 Light scheme returns white on white colours
    {
        setProperty (Ids::uiButtonTextColourOn, "0xff000000"); // Text colour
        setProperty (Ids::uiButtonColourOff, "0x80000000"); // V2 specific Colour
        setProperty (Ids::uiToggleButtonFocusOutline, "0x60000000"); // Tick box colour
        setProperty (Ids::uiToggleButtontickColour, "0xff000000"); // Tick colour
    }
    else
    {
        setProperty (Ids::uiButtonTextColourOn, (String)LookAndFeel::findColour(ToggleButton::textColourId).toString()); // Text colour
        setProperty (Ids::uiButtonColourOff, (String)LookAndFeel::findColour(ToggleButton::textColourId).withAlpha(0.7f).toString()); // V2 specific Colour
        setProperty (Ids::uiToggleButtonFocusOutline, (String)LookAndFeel::findColour(ToggleButton::tickDisabledColourId).withAlpha(0.5f).toString()); // Tick colour
        setProperty (Ids::uiToggleButtontickColour, (String)LookAndFeel::findColour(ToggleButton::tickColourId).toString()); // Tick colour
    }
    setSize (88, 48);
}

CtrlrToggleButton::~CtrlrToggleButton()
{
    deleteAndZero (ctrlrButton);
}

//==============================================================================
void CtrlrToggleButton::paint (Graphics& g)
{
}

void CtrlrToggleButton::resized()
{
    ctrlrButton->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
}

void CtrlrToggleButton::buttonClicked (Button* buttonThatWasClicked)
{
    if (isInternal())
	{
		owner.getOwnerPanel().performInternalComponentFunction(this);
		return;
	}

	if (!owner.getOwnerPanel().checkRadioGroup(this, buttonThatWasClicked->getToggleState()))
		return;

    if (buttonThatWasClicked == ctrlrButton)
    {
		setComponentValue (ctrlrButton->getToggleState(), true);
    }
}

void CtrlrToggleButton::mouseDown (const MouseEvent& e)
{
    CtrlrComponent::mouseDown(e);
}


void CtrlrToggleButton::setComponentValue (const double newValue, const bool sendChangeMessage)
{
	if (!owner.getOwnerPanel().checkRadioGroup(this, ctrlrButton->getToggleState()))
		return;

	if (ctrlrButton->getClickingTogglesState())
	{
		ctrlrButton->setToggleState (newValue ? true : false, dontSendNotification);
	}

	if (sendChangeMessage)
	{
		owner.getProcessor().setValueGeneric (CtrlrModulatorValue(newValue, CtrlrModulatorValue::changedByGUI), true);
	}
}

void CtrlrToggleButton::setComponentMidiValue (const int newValue, const bool sendChangeMessage)
{
	if (!owner.getOwnerPanel().checkRadioGroup(this, ctrlrButton->getToggleState()))
		return;

	if (ctrlrButton->getClickingTogglesState())
	{
		ctrlrButton->setToggleState (valueMap->getIndexForValue(newValue) ? true : false, dontSendNotification);
	}

	if (sendChangeMessage)
	{
		owner.getProcessor().setValueGeneric (CtrlrModulatorValue(newValue, CtrlrModulatorValue::changedByGUI), true);
	}
}

double CtrlrToggleButton::getComponentMaxValue()
{
	return (1);
}

bool CtrlrToggleButton::getToggleState()
{
	return (ctrlrButton->getToggleState());
}

double CtrlrToggleButton::getComponentValue()
{
	if (ctrlrButton->getToggleState())
	{
		return (1);
	}
	else
	{
		return (0);
	}
}

int CtrlrToggleButton::getComponentMidiValue()
{
	return (valueMap->getMappedValue(ctrlrButton->getToggleState()));
}

void CtrlrToggleButton::valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property)
{
	if (property == Ids::componentRadioGroupId)
	{
		ctrlrButton->setToggleState (false, dontSendNotification);
	}
    else if (property == Ids::uiButtonLookAndFeel)
    {
        String LookAndFeelType = getProperty(property);
        setLookAndFeel(CtrlrToggleButton::getLookAndFeelFromComponentProperty(LookAndFeelType)); // Updates the current component LookAndFeel
        
        updatingLookAndFeel = true; // Set guard flag

        CtrlrToggleButton::resetLookAndFeelOverrides(); // Retrieves LookAndFeel colours from selected ColourScheme
        
        updatingLookAndFeel = false; // Clear guard flag
    }
	if (property == Ids::uiButtonTextColourOn)
	{
		ctrlrButton->setColour (ToggleButton::textColourId, VAR2COLOUR(getProperty(Ids::uiButtonTextColourOn)));
	}
    if (property == Ids::uiButtonColourOff)
    {
        ctrlrButton->setColour (TextButton::buttonColourId, VAR2COLOUR(getProperty(Ids::uiButtonColourOff)));
    }
    else if (property == Ids::uiToggleButtontickColour)
    {
        ctrlrButton->setColour (ToggleButton::tickColourId, VAR2COLOUR(getProperty(Ids::uiToggleButtontickColour)));
    }
	else if (property == Ids::uiToggleButtonFocusOutline)
    {
        ctrlrButton->setColour (ToggleButton::tickDisabledColourId, VAR2COLOUR(getProperty(Ids::uiToggleButtonFocusOutline)));
        ctrlrButton->setColour (TextEditor::focusedOutlineColourId, VAR2COLOUR(getProperty(Ids::uiToggleButtonFocusOutline)));
    }
    else if (property == Ids::uiToggleButtonText)
    {
        ctrlrButton->setButtonText (getProperty(Ids::uiToggleButtonText));
    }
    else if (property == Ids::uiButtonTrueValue || property == Ids::uiButtonFalseValue)
    {
        valueMap->setPair (0, getProperty(Ids::uiButtonFalseValue), "");
        valueMap->setPair (1, getProperty(Ids::uiButtonTrueValue), "");
        owner.getProcessor().setValueMap (*valueMap);
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

void CtrlrToggleButton::click()
{
	ctrlrButton->triggerClick();
}

bool CtrlrToggleButton::isToggleButton()
{
	return (true);
}

void CtrlrToggleButton::setToggleState(const bool toggleState, const bool sendChangeMessage)
{
	ctrlrButton->setToggleState (toggleState, sendChangeMessage ? sendNotification : dontSendNotification);
}



LookAndFeel *CtrlrToggleButton::getLookAndFeelFromComponentProperty(const String &lookAndFeelComponentProperty) // Updated v5.6.34
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

void CtrlrToggleButton::resetLookAndFeelOverrides()
{
    if (restoreStateInProgress == false) // To prevent the props lines position stacking up to top and keep their original position
    {
        setProperty (Ids::componentLabelColour, (String)LookAndFeel::findColour(Label::textColourId).toString());

        if (getProperty(Ids::uiButtonLookAndFeel) == "V4 Light") // Because V4 Light scheme returns white on white colours
        {
            setProperty (Ids::uiButtonTextColourOn, "0xff000000"); // Text colour
            setProperty (Ids::uiButtonColourOff, "0x80000000"); // V2 specific Colour
            setProperty (Ids::uiToggleButtonFocusOutline, "0x60000000"); // Tick box colour
            setProperty (Ids::uiToggleButtontickColour, "0xff000000"); // Tick colour
        }
        else
        {
            setProperty (Ids::uiButtonTextColourOn, (String)LookAndFeel::findColour(ToggleButton::textColourId).toString()); // Text colour
            setProperty (Ids::uiButtonColourOff, (String)LookAndFeel::findColour(ToggleButton::textColourId).withAlpha(0.7f).toString()); // V2 specific Colour
            setProperty (Ids::uiToggleButtonFocusOutline, (String)LookAndFeel::findColour(ToggleButton::tickDisabledColourId).withAlpha(0.5f).toString()); // Tick colour
            setProperty (Ids::uiToggleButtontickColour, (String)LookAndFeel::findColour(ToggleButton::tickColourId).toString()); // Tick colour
        }
		
        updatePropertiesPanel(); // Refreshes property pane
    }
}

void CtrlrToggleButton::updatePropertiesPanel()
{
    CtrlrPanelProperties *props = owner.getCtrlrManagerOwner().getActivePanel()->getEditor(false)->getPropertiesPanel();
    if (props)
    {
        props->refreshAll(); // Needs extra code to prevent scrolling back to top on refresh
    }
}
