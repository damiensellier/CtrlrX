#include "stdafx.h"
#include "CtrlrButton.h"
#include "CtrlrValueMap.h"
#include "CtrlrIDs.h"
#include "CtrlrPanel/CtrlrPanelEditor.h"
#include "CtrlrModulator/CtrlrModulator.h"
#include "CtrlrPanel/CtrlrPanel.h"

CtrlrButton::CtrlrButton (CtrlrModulator &owner)
    : CtrlrComponent(owner),
      ctrlrButton (0)
{
	valueMap = std::make_unique<CtrlrValueMap>();
    addAndMakeVisible (ctrlrButton = new TextButton ("ctrlrButton"));
    ctrlrButton->addListener (this);
    
    setProperty (Ids::uiButtonLookAndFeel, "Default");
    
    ctrlrButton->addMouseListener(this, true);
    ctrlrButton->setBufferedToImage (true);
    setProperty (Ids::uiButtonIsToggle, true);
	setProperty (Ids::uiButtonTrueValue, 1);
	setProperty (Ids::uiButtonFalseValue, 0);
    setProperty (Ids::uiButtonContent, "False\nTrue");
    
	setProperty (Ids::uiButtonRepeat, false);
	setProperty (Ids::uiButtonRepeatRate, 100);
	setProperty (Ids::uiButtonTriggerOnMouseDown, false);
	setProperty (Ids::componentInternalFunction, COMBO_ITEM_NONE);
    
    bool LegacyMode = owner.getOwnerPanel().getEditor()->getProperty(Ids::uiPanelLegacyMode); // Legacy mode flag for version before 5.6.29
    String panelLnF = owner.getOwnerPanel().getEditor()->getProperty(Ids::uiPanelLookAndFeel);
    
    if (LegacyMode || panelLnF == "V3") // Added v5.6.34. Not really good because it will create a new LnF but won't destroy it so it will lead to memory leaks
    {
        setLookAndFeel(new LookAndFeel_V3());
        setProperty(Ids::uiButtonLookAndFeel, "V3");
    }
    
    else if (panelLnF == "V2") // Added v5.6.34. Not really good because it will create a new LnF but won't destroy it so it will lead to memory leaks
    {
        setLookAndFeel(new LookAndFeel_V2());
        setProperty(Ids::uiButtonLookAndFeel, "V2");
    }
    
    else if (panelLnF == "V1") // Added v5.6.34. Not really good because it will create a new LnF but won't destroy it so it will lead to memory leaks
    {
        setLookAndFeel(new LookAndFeel_V1());
        setProperty(Ids::uiButtonLookAndFeel, "V1");
    }
    
    if ( panelLnF == "V3"
        || panelLnF == "V2"
        || panelLnF == "V1" )
    {
        setSize (88, 32);
        setProperty (Ids::uiButtonColourOn, "0xff0000ff");
        setProperty (Ids::uiButtonColourOff, "0xff4364ff");
        setProperty (Ids::uiButtonTextColourOn, "0xff000000");
        setProperty (Ids::uiButtonTextColourOff, "0xff454545");
    }
    else
    {
        setSize (88, 64);
        setProperty (Ids::uiButtonColourOn,  (String)findColour(TextButton::buttonOnColourId).toString());
        setProperty (Ids::uiButtonColourOff, (String)findColour(TextButton::buttonColourId).toString());
        setProperty (Ids::uiButtonTextColourOn, (String)findColour(TextButton::textColourOnId).toString());
        setProperty (Ids::uiButtonTextColourOff, (String)findColour(TextButton::textColourOffId).toString());
    }
    
    setProperty (Ids::uiButtonConnectedLeft, false); // Hints about which edges of the button might be connected to adjoining buttons.
    setProperty (Ids::uiButtonConnectedRight, false);
    setProperty (Ids::uiButtonConnectedTop, false);
    setProperty (Ids::uiButtonConnectedBottom, false);
}

CtrlrButton::~CtrlrButton()
{
    deleteAndZero (ctrlrButton);
}

//==============================================================================
void CtrlrButton::paint (Graphics& g)
{
}

void CtrlrButton::resized()
{
	ctrlrButton->setBounds (getUsableRect());
}

void CtrlrButton::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    if (isInternal())
	{
		owner.getOwnerPanel().performInternalComponentFunction(this);
		return;
	}

	if (!owner.getOwnerPanel().checkRadioGroup(this, buttonThatWasClicked->getToggleState()))
		return;
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == ctrlrButton)
    {
        //[UserButtonCode_ctrlrButton] -- add your button handler code here..
		valueMap->increment();
		ctrlrButton->setButtonText (valueMap->getCurrentText());
		setComponentValue (valueMap->getCurrentNonMappedValue(), true);
        //[/UserButtonCode_ctrlrButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void CtrlrButton::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    if ((bool)getProperty(Ids::uiButtonTriggerOnMouseDown) == true)
    {
        if (e.eventComponent == ctrlrButton)
        {
            if (!isTimerRunning() && (bool)getProperty(Ids::uiButtonRepeat))
            {
                startTimer ((int)getProperty(Ids::uiButtonRepeatRate));
            }

            if (getProperty(Ids::uiButtonTriggerOnMouseDown))
            {
                ctrlrButton->triggerClick();
            }
        }
    }
	CtrlrComponent::mouseDown(e);
    //[/UserCode_mouseDown]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void CtrlrButton::timerCallback()
{
	if (ctrlrButton->isMouseButtonDown())
	{
		ctrlrButton->triggerClick();
	}
	else
	{
		stopTimer();
	}
}

void CtrlrButton::setComponentValue (const double newValue, const bool sendChangeMessage)
{
	valueMap->setCurrentNonMappedValue (newValue);
	ctrlrButton->setButtonText (valueMap->getTextForIndex (newValue));

	if (ctrlrButton->getClickingTogglesState())
	{
		if ((double)getProperty(Ids::uiButtonTrueValue) == newValue)
		{
			ctrlrButton->setToggleState (true, dontSendNotification);
			valueMap->setCurrentNonMappedValue (1);
		}
		else
		{
			ctrlrButton->setToggleState (false, dontSendNotification);
			valueMap->setCurrentNonMappedValue (0);
		}
	}

	if (sendChangeMessage)
	{
		owner.getProcessor().setValueGeneric (CtrlrModulatorValue(newValue,CtrlrModulatorValue::changedByGUI), sendChangeMessage);
	}
}

double CtrlrButton::getComponentMaxValue()
{
	return (valueMap->getNonMappedMax());
}

bool CtrlrButton::getToggleState()
{
	return (ctrlrButton->getToggleState());
}

const String CtrlrButton::getComponentText()
{
	return (ctrlrButton->getButtonText());
}

void CtrlrButton::setComponentText (const String &componentText)
{
	setComponentValue (valueMap->getNonMappedValue(componentText));
}

double CtrlrButton::getComponentValue()
{
	return (valueMap->getCurrentNonMappedValue());
}

int CtrlrButton::getComponentMidiValue()
{
	return (valueMap->getCurrentMappedValue());
}

void CtrlrButton::buttonContentChanged()
{
	valueMap->copyFrom (owner.getProcessor().setValueMap (getProperty (Ids::uiButtonContent)));
	setComponentValue (0, false);
}

void CtrlrButton::valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property)
{
	if (property == Ids::uiButtonContent)
	{
		buttonContentChanged();
	}
    else if (property == Ids::uiButtonLookAndFeel)
    {
        updatingLookAndFeel = true; // Set guard flag
        
        String LookAndFeelType = getProperty(property);
        setLookAndFeel(CtrlrButton::getLookAndFeelFromComponentProperty(LookAndFeelType)); // Updates the current component LookAndFeel
        
        CtrlrButton::resetLookAndFeelOverrides(); // Retrieves LookAndFeel colours from selected ColourScheme
        
        updatingLookAndFeel = false; // Clear guard flag
    }
	
	else if (property == Ids::uiButtonColourOff
		|| property == Ids::uiButtonColourOn
		|| property == Ids::uiButtonTextColourOff
		|| property == Ids::uiButtonTextColourOn)
	{
		ctrlrButton->setColour (TextButton::buttonColourId, VAR2COLOUR(getProperty(Ids::uiButtonColourOff)));
		ctrlrButton->setColour (TextButton::buttonOnColourId, VAR2COLOUR(getProperty(Ids::uiButtonColourOn)));
		ctrlrButton->setColour (TextButton::textColourOffId, VAR2COLOUR(getProperty(Ids::uiButtonTextColourOff)));
		ctrlrButton->setColour (TextButton::textColourOnId, VAR2COLOUR(getProperty(Ids::uiButtonTextColourOn)));
	}

	else if (property == Ids::uiButtonIsToggle)
	{
		ctrlrButton->setClickingTogglesState((bool)getProperty(property));
	}


	else if (property == Ids::uiButtonConnectedLeft
		|| property == Ids::uiButtonConnectedRight
		|| property == Ids::uiButtonConnectedTop
		|| property == Ids::uiButtonConnectedBottom
		)
	{
		const int leftFlag = (bool)getProperty(Ids::uiButtonConnectedLeft) ? Button::ConnectedOnLeft : 0;
		const int rightFlag = (bool)getProperty(Ids::uiButtonConnectedRight) ? Button::ConnectedOnRight : 0;
		const int topFlag = (bool)getProperty(Ids::uiButtonConnectedTop) ? Button::ConnectedOnTop : 0;
		const int bottomFlag = (bool)getProperty(Ids::uiButtonConnectedBottom) ? Button::ConnectedOnBottom : 0;
		ctrlrButton->setConnectedEdges (leftFlag | rightFlag | topFlag | bottomFlag);
	}
	else if (property == Ids::uiButtonTrueValue)
	{
		owner.setProperty (Ids::modulatorMax, getProperty(property));
	}
	else if (property == Ids::uiButtonFalseValue)
	{
		owner.setProperty (Ids::modulatorMin, getProperty(property));
	}
	else if (property == Ids::uiButtonRepeat)
	{
		if ((bool)getProperty(property) == false)
        {
			stopTimer();
		}
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

void CtrlrButton::click()
{
	ctrlrButton->triggerClick();
}

bool CtrlrButton::isToggleButton()
{
	return (ctrlrButton->getClickingTogglesState());
}

void CtrlrButton::setToggleState(const bool toggleState, const bool sendChangeMessage)
{
	ctrlrButton->setToggleState (toggleState, sendChangeMessage ? sendNotification : dontSendNotification);
}


LookAndFeel *CtrlrButton::getLookAndFeelFromComponentProperty(const String &lookAndFeelComponentProperty) // Updated v5.6.34
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

void CtrlrButton::resetLookAndFeelOverrides()
{
    if (restoreStateInProgress == false) // To prevent the props lines position stacking up to top and keep their original position
    {
        setProperty (Ids::componentLabelColour, (String)findColour(Label::textColourId).toString());
        
        setProperty (Ids::uiButtonColourOn,  (String)findColour(TextButton::buttonOnColourId).toString());
        setProperty (Ids::uiButtonColourOff, (String)findColour(TextButton::buttonColourId).toString());
        setProperty (Ids::uiButtonTextColourOn, (String)findColour(TextButton::textColourOnId).toString());
        setProperty (Ids::uiButtonTextColourOff, (String)findColour(TextButton::textColourOffId).toString());
        
        updatePropertiesPanel(); // Refreshes property pane
    }
}

void CtrlrButton::updatePropertiesPanel()
{
    CtrlrPanelProperties *props = owner.getCtrlrManagerOwner().getActivePanel()->getEditor(false)->getPropertiesPanel();
    if (props)
    {
        props->refreshAll(); // Needs extra code to prevent scrolling back to top on refresh
    }
}
