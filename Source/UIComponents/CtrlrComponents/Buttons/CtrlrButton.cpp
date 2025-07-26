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
	valueMap = new CtrlrValueMap();
    addAndMakeVisible (ctrlrButton = new TextButton ("ctrlrButton"));
    ctrlrButton->addListener (this);
    
    setProperty (Ids::uiButtonLookAndFeel, "Default");
    setProperty (Ids::uiButtonLookAndFeelIsCustom, false);
    
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
    if (LegacyMode)
    {
        setLookAndFeel(new LookAndFeel_V3());
        setProperty(Ids::uiButtonLookAndFeel, "V3");
    }
    
    if ( owner.getOwnerPanel().getEditor()->getProperty(Ids::uiPanelLookAndFeel) == "V3"
        || owner.getOwnerPanel().getEditor()->getProperty(Ids::uiPanelLookAndFeel) == "V2"
        || owner.getOwnerPanel().getEditor()->getProperty(Ids::uiPanelLookAndFeel) == "V1" )
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
    
    setProperty (Ids::uiButtonLookAndFeelIsCustom, false); // Resets the component colourScheme if a new default colourScheme is selected from the menu

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
        String LookAndFeelType = getProperty(property);
        setLookAndFeel(CtrlrButton::getLookAndFeelFromComponentProperty(LookAndFeelType)); // Updates the current component LookAndFeel
        
        if (LookAndFeelType == "Default")
        {
            setProperty(Ids::uiButtonLookAndFeelIsCustom, false); // Resets the Customized Flag to False to allow Global L&F to apply
        }
        
        if (!getProperty(Ids::uiButtonLookAndFeelIsCustom))
        {
            CtrlrButton::resetLookAndFeelOverrides(); // Retrieves LookAndFeel colours from selected ColourScheme
        }
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
        setProperty(Ids::uiButtonLookAndFeelIsCustom, true); // Locks the component custom colourScheme
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


LookAndFeel *CtrlrButton::getLookAndFeelFromComponentProperty(const String &lookAndFeelComponentProperty)
{
    if (lookAndFeelComponentProperty == "Default")
    // Leave empty to get L&F from Global Property
    if (lookAndFeelComponentProperty == "V4 Light")
        return new LookAndFeel_V4(LookAndFeel_V4::getLightColourScheme());
    if (lookAndFeelComponentProperty == "V4 Grey")
        return new LookAndFeel_V4(LookAndFeel_V4::getGreyColourScheme());
    if (lookAndFeelComponentProperty == "V4 Dark")
        return new LookAndFeel_V4(LookAndFeel_V4::getDarkColourScheme());
    if (lookAndFeelComponentProperty == "V4 Midnight")
        return new LookAndFeel_V4(LookAndFeel_V4::getMidnightColourScheme());
    if (lookAndFeelComponentProperty == "V4 JetBlack")
        return new LookAndFeel_V4(LookAndFeel_V4::getJetBlackColourScheme());
    if (lookAndFeelComponentProperty == "V4 YamDX")
        return new LookAndFeel_V4(LookAndFeel_V4::getYamDxColourScheme());
    if (lookAndFeelComponentProperty == "V4 AkAPC")
        return new LookAndFeel_V4(LookAndFeel_V4::getAkApcColourScheme());
    if (lookAndFeelComponentProperty == "V4 AkMPC")
        return new LookAndFeel_V4(LookAndFeel_V4::getAkMpcColourScheme());
    if (lookAndFeelComponentProperty == "V4 LexiBlue")
        return new LookAndFeel_V4(LookAndFeel_V4::getLexiBlueColourScheme());
    if (lookAndFeelComponentProperty == "V4 KurzGreen")
        return new LookAndFeel_V4(LookAndFeel_V4::getKurzGreenColourScheme());
    if (lookAndFeelComponentProperty == "V4 KorGrey")
        return new LookAndFeel_V4(LookAndFeel_V4::getKorGreyColourScheme());
    if (lookAndFeelComponentProperty == "V4 KorGold")
        return new LookAndFeel_V4(LookAndFeel_V4::getKorGoldColourScheme());
    if (lookAndFeelComponentProperty == "V4 ArturOrange")
        return new LookAndFeel_V4(LookAndFeel_V4::getArturOrangeColourScheme());
    if (lookAndFeelComponentProperty == "V4 AiraGreen")
        return new LookAndFeel_V4(LookAndFeel_V4::getAiraGreenColourScheme());
    if (lookAndFeelComponentProperty == "V3")
        return new LookAndFeel_V3();
    if (lookAndFeelComponentProperty == "V2")
        return new LookAndFeel_V2();
    if (lookAndFeelComponentProperty == "V1")
        return new LookAndFeel_V1();
    
    return (nullptr);
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
        
        setProperty (Ids::uiButtonLookAndFeelIsCustom, false); // Resets the component colourScheme if a new default colourScheme is selected from the menu
        
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



//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="CtrlrButton" componentName=""
                 parentClasses="public CtrlrComponent" constructorParams="CtrlrModulator &amp;owner"
                 variableInitialisers="CtrlrComponent(owner)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330000013" fixedSize="1" initialWidth="88"
                 initialHeight="32">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="ffffff"/>
  <TEXTBUTTON name="ctrlrButton" id="d906fca95b2d6ff7" memberName="ctrlrButton"
              virtualName="" explicitFocusOrder="0" pos="0 0 0M 0M" buttonText="Button"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
