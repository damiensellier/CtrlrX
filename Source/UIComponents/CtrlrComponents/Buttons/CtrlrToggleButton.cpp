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
	valueMap = new CtrlrValueMap();
    addAndMakeVisible (ctrlrButton = new ToggleButton ("ctrlrButton"));
    ctrlrButton->setButtonText ("Button");
    ctrlrButton->addListener (this);


    //[UserPreSize]
	ctrlrButton->setBufferedToImage (true);
	setProperty (Ids::uiToggleButtonText, "Button");
	setProperty (Ids::uiButtonTrueValue, 1);
	setProperty (Ids::uiButtonFalseValue, 0);
	owner.setProperty (Ids::modulatorMax, 1);
	owner.setProperty (Ids::modulatorMin, 0);
    
    setProperty (Ids::uiButtonLookAndFeel, "Default");
    setProperty (Ids::uiButtonLookAndFeelIsCustom, false);

    //[/UserPreSize]

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
    
    setProperty (Ids::uiButtonLookAndFeelIsCustom, false); // Resets the component colourScheme if a new default colourScheme is selected from the menu

    //[Constructor] You can add your own custom stuff here..
	//owner.getProcessor().setValueFromGUI (0, true);
    //[/Constructor]
}

CtrlrToggleButton::~CtrlrToggleButton()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    deleteAndZero (ctrlrButton);

    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void CtrlrToggleButton::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void CtrlrToggleButton::resized()
{
    ctrlrButton->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void CtrlrToggleButton::buttonClicked (Button* buttonThatWasClicked)
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
		setComponentValue (ctrlrButton->getToggleState(), true);
        //[/UserButtonCode_ctrlrButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void CtrlrToggleButton::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    CtrlrComponent::mouseDown(e);
    //[/UserCode_mouseDown]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
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
        
        if (LookAndFeelType == "Default")
        {
            setProperty(Ids::uiButtonLookAndFeelIsCustom, false); // Resets the Customized Flag to False to allow Global L&F to apply
        }
        
        if (!getProperty(Ids::uiButtonLookAndFeelIsCustom))
        {
            CtrlrToggleButton::resetLookAndFeelOverrides(); // Retrieves LookAndFeel colours from selected ColourScheme
        }
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



LookAndFeel *CtrlrToggleButton::getLookAndFeelFromComponentProperty(const String &lookAndFeelComponentProperty)
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
        
        setProperty (Ids::uiButtonLookAndFeelIsCustom, false); // Resets the component colourScheme if a new default colourScheme is selected from the menu
        
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





//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="CtrlrToggleButton" componentName=""
                 parentClasses="public CtrlrComponent" constructorParams="CtrlrModulator &amp;owner"
                 variableInitialisers="CtrlrComponent(owner)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330000013" fixedSize="1" initialWidth="88"
                 initialHeight="32">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="ffffff"/>
  <TOGGLEBUTTON name="ctrlrButton" id="ece5e33c201d706e" memberName="ctrlrButton"
                virtualName="" explicitFocusOrder="0" pos="0 0 0M 0M" buttonText="Button"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
