#include "stdafx.h"
/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  27 Oct 2012 8:17:44pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
#include "CtrlrLuaMethodEditor.h"
#include "CtrlrPanel/CtrlrPanel.h"
#include "CtrlrManager/CtrlrManager.h"
//[/Headers]

#include "CtrlrLuaMethodCodeEditorSettings.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
CtrlrLuaMethodCodeEditorSettings::CtrlrLuaMethodCodeEditorSettings (CtrlrLuaMethodEditor &_owner)
    : owner(_owner),
      fontTypeface (0),
      fontBold (0),
      fontUnderline (0),
      fontItalic (0),
      fontSize (0),
      bgColour (0), // Added v5.6.31
      lineNumbersBgColour(0), // Added v5.6.31
      lineNumbersColour(0), // Added v5.6.31
      fontTest (0)
{
    addAndMakeVisible(label0 = new Label("new label", TRANS("Font:"))); // Added v.5.6.31
    label0->setFont(Font(14.00f)); // Added v.5.6.31
    label0->setJustificationType(Justification::centredLeft); // Added v.5.6.31
    label0->setEditable(false, false, false); // Added v.5.6.31
    
    addAndMakeVisible (fontTypeface = new ComboBox (""));
    fontTypeface->setEditableText (false);
    fontTypeface->setJustificationType (Justification::centredLeft);
    fontTypeface->setTextWhenNothingSelected ("");
    fontTypeface->setTextWhenNoChoicesAvailable (L"(no choices)");
    fontTypeface->addListener (this);

    addAndMakeVisible (fontBold = new ToggleButton (""));
    fontBold->setButtonText (L"Bold");
    fontBold->addListener (this);

    addAndMakeVisible (fontUnderline = new ToggleButton (""));
    fontUnderline->setButtonText (L"Underline");
    fontUnderline->addListener (this);

    addAndMakeVisible (fontItalic = new ToggleButton (""));
    fontItalic->setButtonText (L"Italic");
    fontItalic->addListener (this);

    addAndMakeVisible (fontSize = new Slider (""));
    fontSize->setRange (0, 128, 1);
    fontSize->setSliderStyle (Slider::IncDecButtons);
    fontSize->setTextBoxStyle (Slider::TextBoxLeft, false, 32, 24);
    fontSize->addListener (this);
    
    addAndMakeVisible(label1 = new Label("new label", TRANS("Editor background:"))); // Added v.5.6.31
    label1->setFont(Font(14.00f)); // Added v.5.6.31
    label1->setJustificationType(Justification::centredLeft); // Added v.5.6.31
    label1->setEditable(false, false, false); // Added v.5.6.31

    addAndMakeVisible (bgColour = new CtrlrColourEditorComponent (this));

    addAndMakeVisible(label2 = new Label("new label", TRANS("Line numbers background:"))); // Added v.5.6.31
    label2->setFont(Font(14.00f)); // Added v.5.6.31
    label2->setJustificationType(Justification::centredLeft); // Added v.5.6.31
    label2->setEditable(false, false, false); // Added v.5.6.31

    addAndMakeVisible(lineNumbersBgColour = new CtrlrColourEditorComponent(this)); // Added v.5.6.31
 
    addAndMakeVisible(label3 = new Label("new label", TRANS("Line numbers:"))); // Added v.5.6.31
    label3->setFont(Font(14.00f)); // Added v.5.6.31
    label3->setJustificationType(Justification::centredLeft); // Added v.5.6.31
    label3->setEditable(false, false, false); // Added v.5.6.31
    
    addAndMakeVisible(lineNumbersColour = new CtrlrColourEditorComponent(this)); // Added v.5.6.31

    addAndMakeVisible (fontTest = new CodeEditorComponent (codeDocument, &luaTokeniser));


    //[UserPreSize]
    codeFont = owner.getOwner().getCtrlrManagerOwner().getFontManager().getFontFromString(owner.getComponentTree().getProperty(Ids::luaMethodEditorFont, owner.getOwner().getCtrlrManagerOwner().getFontManager().getStringFromFont (Font(owner.getOwner().getCtrlrManagerOwner().getFontManager().getDefaultMonoFontName(), 14.0f, Font::plain))));
    label1->setColour(TextEditor::textColourId, findColour(TextEditor::textColourId)); // Colours::black);
    label1->setColour(TextEditor::backgroundColourId, Colour(0x00000000));
    bgColour->setColour(VAR2COLOUR(owner.getComponentTree().getProperty(Ids::luaMethodEditorBgColour, Colours::white.toString())));
    label2->setColour(TextEditor::textColourId, findColour(TextEditor::textColourId)); // Colours::black);
    label2->setColour(TextEditor::backgroundColourId, Colour(0x00000000));
    lineNumbersBgColour->setColour(VAR2COLOUR(owner.getComponentTree().getProperty(Ids::luaMethodEditorLineNumbersBgColour, Colour(0xffc5ddf1).toString())));
    label3->setColour(TextEditor::textColourId, findColour(TextEditor::textColourId)); // Colours::black);
    label3->setColour(TextEditor::backgroundColourId, Colour(0x00000000));
    lineNumbersColour->setColour(VAR2COLOUR(owner.getComponentTree().getProperty(Ids::luaMethodEditorLineNumbersColour, Colours::black.toString())));

    fontSize->setValue (codeFont.getHeight(), dontSendNotification);
    fontUnderline->setToggleState (codeFont.isUnderlined(), dontSendNotification);
    fontBold->setToggleState (codeFont.isBold(), dontSendNotification);
    fontItalic->setToggleState (codeFont.isItalic(), dontSendNotification);
    owner.getOwner().getCtrlrManagerOwner().getFontManager().fillCombo (*fontTypeface);
    fontTypeface->setText (codeFont.getTypefaceName(), sendNotification);
    codeDocument.replaceAllContent ("-- This is a comment\nfunction myFunction(argument)\n\tcall(\"string\")\nend");
    //[/UserPreSize]

    setSize (334, 360);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

CtrlrLuaMethodCodeEditorSettings::~CtrlrLuaMethodCodeEditorSettings()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    deleteAndZero (label0);
    deleteAndZero (fontTypeface);
    deleteAndZero (fontBold);
    deleteAndZero (fontUnderline);
    deleteAndZero (fontItalic);
    deleteAndZero (fontSize);
    deleteAndZero (label1);
    deleteAndZero (bgColour);
    deleteAndZero (label2);
    deleteAndZero (lineNumbersBgColour);
    deleteAndZero (label3);
    deleteAndZero (lineNumbersColour);
    deleteAndZero (fontTest);


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void CtrlrLuaMethodCodeEditorSettings::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void CtrlrLuaMethodCodeEditorSettings::resized()
{
    marginLeft = 12;
    marginTop = 12;
    sampleWidth = 310;
    sampleHeight = 78;

    fontTest->setBounds(marginLeft, marginTop, sampleWidth, sampleHeight);
    label0->setBounds(marginLeft - 4, marginTop+ sampleHeight +8, sampleWidth, 24);
    fontTypeface->setBounds(marginLeft, marginTop + sampleHeight + 24 + 8, sampleWidth, 24);
    fontBold->setBounds(marginLeft, marginTop + sampleHeight + 24 + 40, 56, 24);
    fontUnderline->setBounds(marginLeft + 128, marginTop + sampleHeight + 24 + 40, 88, 24);
    fontItalic->setBounds(marginLeft + 64, marginTop + sampleHeight + 24 + 40, 64, 24);
    fontSize->setBounds(marginLeft + 224, marginTop + sampleHeight + 24 + 40, 78, 24);
    //[UserResized] Add your own custom resize handling here..
    label1->setBounds(marginLeft - 4, marginTop + sampleHeight + 24 + 72, sampleWidth, 24);
    bgColour->setBounds(marginLeft, marginTop + sampleHeight + 24 + 72 + 24, sampleWidth, 24);
    label2->setBounds(marginLeft - 4, marginTop + sampleHeight + 24 + 72 + 24 + 32, sampleWidth, 24);
    lineNumbersBgColour->setBounds(marginLeft, marginTop + sampleHeight + 24 + 72 + 2 * 24 + 32, sampleWidth, 24);
    label3->setBounds(marginLeft - 4, marginTop + sampleHeight + 24 + 72 + 2 * 24 + 2 * 32, sampleWidth, 24);
    lineNumbersColour->setBounds(marginLeft, marginTop + sampleHeight + 24 + 72 + 3 * 24 + 2 * 32, sampleWidth, 24);
    //[/UserResized]
}

void CtrlrLuaMethodCodeEditorSettings::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == fontTypeface)
    {
        //[UserComboBoxCode_fontTypeface] -- add your combo box handling code here..
        //[/UserComboBoxCode_fontTypeface]
    }

    //[UsercomboBoxChanged_Post]
    changeListenerCallback(nullptr);
    //[/UsercomboBoxChanged_Post]
}

void CtrlrLuaMethodCodeEditorSettings::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == fontBold)
    {
        //[UserButtonCode_fontBold] -- add your button handler code here..
        //[/UserButtonCode_fontBold]
    }
    else if (buttonThatWasClicked == fontUnderline)
    {
        //[UserButtonCode_fontUnderline] -- add your button handler code here..
        //[/UserButtonCode_fontUnderline]
    }
    else if (buttonThatWasClicked == fontItalic)
    {
        //[UserButtonCode_fontItalic] -- add your button handler code here..
        //[/UserButtonCode_fontItalic]
    }

    //[UserbuttonClicked_Post]
    changeListenerCallback(nullptr);
    //[/UserbuttonClicked_Post]
}

void CtrlrLuaMethodCodeEditorSettings::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == fontSize)
    {
        //[UserSliderCode_fontSize] -- add your slider handling code here..
        //[/UserSliderCode_fontSize]
    }

    //[UsersliderValueChanged_Post]
    changeListenerCallback(nullptr);
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void CtrlrLuaMethodCodeEditorSettings::changeListenerCallback (ChangeBroadcaster* source)
{
    fontTest->setColour(CodeEditorComponent::backgroundColourId, bgColour->getColour());
    fontTest->setColour(CodeEditorComponent::lineNumberBackgroundId, lineNumbersBgColour->getColour());
    fontTest->setColour(CodeEditorComponent::lineNumberTextId, lineNumbersColour->getColour());
    fontTest->setFont (getFont());
    repaint();
}

const Font CtrlrLuaMethodCodeEditorSettings::getFont()
{
    Font font = owner.getOwner().getCtrlrManagerOwner().getFontManager().getFont (fontTypeface->getSelectedItemIndex());

    if (fontTypeface)
        font.setTypefaceName (fontTypeface->getText());
    else
        return (font);

    font.setHeight (fontSize->getValue());
    font.setBold (fontBold->getToggleState());
    font.setItalic (fontItalic->getToggleState());
    font.setUnderline (fontUnderline->getToggleState());
    return (font);
}

const Colour CtrlrLuaMethodCodeEditorSettings::getBgColour()
{
    return (bgColour->getColour());
}

const Colour CtrlrLuaMethodCodeEditorSettings::getLineNumbersBgColour()
{
    return (lineNumbersBgColour->getColour());
}

const Colour CtrlrLuaMethodCodeEditorSettings::getLineNumbersColour()
{
    return (lineNumbersColour->getColour());
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="CtrlrLuaMethodCodeEditorSettings"
                 componentName="" parentClasses="public Component, public ChangeListener"
                 constructorParams="CtrlrLuaMethodEditor &amp;_owner" variableInitialisers="owner(_owner)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330000013"
                 fixedSize="1" initialWidth="320" initialHeight="200">
  <BACKGROUND backgroundColour="0"/>
  <COMBOBOX name="" id="aa72a7d14fe94324" memberName="fontTypeface" virtualName=""
            explicitFocusOrder="0" pos="8 8 304 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <TOGGLEBUTTON name="" id="9d9f05c9c5dd0d7d" memberName="fontBold" virtualName=""
                explicitFocusOrder="0" pos="8 40 56 24" buttonText="Bold" connectedEdges="0"
                needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="" id="58285682010860f2" memberName="fontUnderline" virtualName=""
                explicitFocusOrder="0" pos="136 40 88 24" buttonText="Underline"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="" id="b8a95fd341c23fa0" memberName="fontItalic" virtualName=""
                explicitFocusOrder="0" pos="72 40 64 24" buttonText="Italic"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <SLIDER name="" id="d6eb265ed035d08a" memberName="fontSize" virtualName=""
          explicitFocusOrder="0" pos="232 40 78 24" min="0" max="128" int="1"
          style="IncDecButtons" textBoxPos="TextBoxLeft" textBoxEditable="1"
          textBoxWidth="32" textBoxHeight="24" skewFactor="1"/>
  <GENERICCOMPONENT name="" id="b41e05286450619c" memberName="bgColour" virtualName=""
                    explicitFocusOrder="0" pos="8 72 304 24" class="CtrlrColourEditorComponent"
                    params="this"/>
  <GENERICCOMPONENT name="" id="f7d42a6b93ff05ed" memberName="fontTest" virtualName=""
                    explicitFocusOrder="0" pos="8 104 304 88" class="CodeEditorComponent"
                    params="codeDocument, &amp;luaTokeniser"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
