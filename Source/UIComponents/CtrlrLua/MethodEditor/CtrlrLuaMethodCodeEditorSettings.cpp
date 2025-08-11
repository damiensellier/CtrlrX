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

#include "stdafx.h"
#include "CtrlrLuaMethodEditor.h"
#include "CtrlrPanel/CtrlrPanel.h"
#include "CtrlrManager/CtrlrManager.h"
#include "CtrlrLuaMethodCodeEditorSettings.h"

// Color array definition
const CtrlrLuaMethodCodeEditorSettings::ColorItem CtrlrLuaMethodCodeEditorSettings::availableColors[] = {
    {"White", Colours::white},
    {"Black", Colours::black},
    {"Light Grey", Colours::lightgrey},
    {"Dark Grey", Colours::darkgrey},
    {"Light Blue", Colours::lightblue},
    {"Blue", Colours::blue},
    {"Dark Blue", Colours::darkblue},
    {"Light Green", Colours::lightgreen},
    {"Green", Colours::green},
    {"Dark Green", Colours::darkgreen},
    {"Light Yellow", Colours::lightyellow},
    {"Yellow", Colours::yellow},
    {"Light Pink", Colours::lightpink},
    {"Pink", Colours::pink},
    {"Red", Colours::red},
    {"Orange", Colours::orange},
    {"Purple", Colours::purple},
    {"Cyan", Colours::cyan},
    {"Light Cyan", Colours::lightcyan},
    {"Cornflower Blue", Colours::cornflowerblue}
};

//==============================================================================
CtrlrLuaMethodCodeEditorSettings::CtrlrLuaMethodCodeEditorSettings(CtrlrLuaMethodEditor& _owner, juce::Value& sharedSearchTabsValue_)
    : owner(_owner), sharedSearchTabsValue(sharedSearchTabsValue_),
    fontTypeface(0),
    fontBold(0),
    fontItalic(0),
    fontSize(0),
    bgColour(0),
    lineNumbersBgColour(0),
    lineNumbersColour(0),
    fontColour(0),
    fontTest(0),
    resetButton(0),
    openSearchTabs(0)
    //activeScheme(luaTokeniser.getDefaultColourScheme())
{
    //if (!isSchemeInitialized)
    //{
    //    activeScheme = luaTokeniser.getDefaultColourScheme();
    //    isSchemeInitialized = true;
    //}

    //activeScheme.set("Identifier", getColorFromCombo(fontColour));
    // Add labels
    addAndMakeVisible(label0 = new Label("new label", TRANS("Font:")));
    label0->setFont(Font(14.00f));
    label0->setJustificationType(Justification::centredLeft);
    label0->setEditable(false, false, false);

    addAndMakeVisible(fontTypeface = new ComboBox(""));
    fontTypeface->setEditableText(false);
    fontTypeface->setJustificationType(Justification::centredLeft);
    fontTypeface->setTextWhenNothingSelected("");
    fontTypeface->setTextWhenNoChoicesAvailable(L"(no choices)");
    fontTypeface->addListener(this);

    addAndMakeVisible(fontBold = new ToggleButton(""));
    fontBold->setButtonText(L"Bold");
    fontBold->addListener(this);

    addAndMakeVisible(fontItalic = new ToggleButton(""));
    fontItalic->setButtonText(L"Italic");
    fontItalic->addListener(this);

    addAndMakeVisible(fontSize = new Slider(""));
    fontSize->setRange(0, 128, 1);
    fontSize->setSliderStyle(Slider::IncDecButtons);
    fontSize->setTextBoxStyle(Slider::TextBoxLeft, false, 32, 24);
    fontSize->addListener(this);

    addAndMakeVisible(label1 = new Label("new label", TRANS("Editor background:")));
    label1->setFont(Font(14.00f));
    label1->setJustificationType(Justification::centredLeft);
    label1->setEditable(false, false, false);

    // Create color combo boxes
    addAndMakeVisible(bgColour = new ComboBox("bgColour"));
    bgColour->setEditableText(false);
    bgColour->setJustificationType(Justification::centredLeft);
    bgColour->addListener(this);

    addAndMakeVisible(label2 = new Label("new label", TRANS("Line numbers background:")));
    label2->setFont(Font(14.00f));
    label2->setJustificationType(Justification::centredLeft);
    label2->setEditable(false, false, false);

    addAndMakeVisible(lineNumbersBgColour = new ComboBox("lineNumbersBgColour"));
    lineNumbersBgColour->setEditableText(false);
    lineNumbersBgColour->setJustificationType(Justification::centredLeft);
    lineNumbersBgColour->addListener(this);

    addAndMakeVisible(label3 = new Label("new label", TRANS("Line numbers:")));
    label3->setFont(Font(14.00f));
    label3->setJustificationType(Justification::centredLeft);
    label3->setEditable(false, false, false);

    addAndMakeVisible(lineNumbersColour = new ComboBox("lineNumbersColour"));
    lineNumbersColour->setEditableText(false);
    lineNumbersColour->setJustificationType(Justification::centredLeft);
    lineNumbersColour->addListener(this);

    addAndMakeVisible(label4 = new Label("new label", TRANS("Font colour:")));
    label4->setFont(Font(14.00f));
    label4->setJustificationType(Justification::centredLeft);
    label4->setEditable(false, false, false);

    addAndMakeVisible(fontColour = new ComboBox("fontColour"));
    fontColour->setEditableText(false);
    fontColour->setJustificationType(Justification::centredLeft);
    fontColour->addListener(this);


    addAndMakeVisible(fontTest = new CodeEditorComponent(codeDocument, &luaTokeniser));

    addAndMakeVisible(openSearchTabs = new ToggleButton(""));
    openSearchTabs->setButtonText("Open Search Tabs"); // You may want to use SharedValues::getSearchTabsLabel()
    // openSearchTabs->getToggleStateValue().referTo(SharedValues::getSearchTabsValue());

    addAndMakeVisible(resetButton = new TextButton("RESET"));
    resetButton->addListener(this);
    resetButton->setColour(TextButton::buttonColourId, Colours::cornflowerblue);

    //[UserPreSize]
    codeFont = owner.getOwner().getCtrlrManagerOwner().getFontManager().getFontFromString(
        owner.getComponentTree().getProperty(Ids::luaMethodEditorFont,
            owner.getOwner().getCtrlrManagerOwner().getFontManager().getStringFromFont(
                Font(owner.getOwner().getCtrlrManagerOwner().getFontManager().getDefaultMonoFontName(), 14.0f, Font::plain))));

    // Populate color combos
    populateColorCombo(bgColour);
    populateColorCombo(lineNumbersBgColour);
    populateColorCombo(lineNumbersColour);
    populateColorCombo(fontColour);

    // Set label colors
    label1->setColour(TextEditor::textColourId, findColour(TextEditor::textColourId));
    label1->setColour(TextEditor::backgroundColourId, Colour(0x00000000));
    label2->setColour(TextEditor::textColourId, findColour(TextEditor::textColourId));
    label2->setColour(TextEditor::backgroundColourId, Colour(0x00000000));
    label3->setColour(TextEditor::textColourId, findColour(TextEditor::textColourId));
    label3->setColour(TextEditor::backgroundColourId, Colour(0x00000000));
    label4->setColour(TextEditor::textColourId, findColour(TextEditor::textColourId));
    label4->setColour(TextEditor::backgroundColourId, Colour(0x00000000));

    // Set default color selections
    Colour defaultBgColor = VAR2COLOUR(owner.getComponentTree().getProperty(Ids::luaMethodEditorBgColour, Colours::white.toString()));
    bgColour->setSelectedId(findColorIndex(defaultBgColor), dontSendNotification);

    Colour defaultLineNumBgColor = VAR2COLOUR(owner.getComponentTree().getProperty(Ids::luaMethodEditorLineNumbersBgColour, Colour(0xffc5ddf1).toString()));
    lineNumbersBgColour->setSelectedId(findColorIndex(defaultLineNumBgColor), dontSendNotification);

    Colour defaultLineNumColor = VAR2COLOUR(owner.getComponentTree().getProperty(Ids::luaMethodEditorLineNumbersColour, Colours::black.toString()));
    lineNumbersColour->setSelectedId(findColorIndex(defaultLineNumColor), dontSendNotification);

    Colour defaultFontColor = VAR2COLOUR(owner.getComponentTree().getProperty(Ids::luaMethodEditorFontColour, Colours::black.toString()));
    fontColour->setSelectedId(findColorIndex(defaultFontColor), dontSendNotification);



    // Set font properties
    fontSize->setValue(codeFont.getHeight(), dontSendNotification);
    fontBold->setToggleState(codeFont.isBold(), dontSendNotification);
    fontItalic->setToggleState(codeFont.isItalic(), dontSendNotification);
    owner.getOwner().getCtrlrManagerOwner().getFontManager().fillCombo(*fontTypeface);
    fontTypeface->setText(codeFont.getTypefaceName(), sendNotification);
    codeDocument.replaceAllContent("-- This is a comment\nfunction myFunction(argument)\n\tcall(\"string\")\nend");
    


    updateEditorColours();

    setSize(334, 490);
}

CtrlrLuaMethodCodeEditorSettings::~CtrlrLuaMethodCodeEditorSettings()
{
    deleteAndZero(label0);
    deleteAndZero(fontTypeface);
    deleteAndZero(fontBold);
    deleteAndZero(fontItalic);
    deleteAndZero(fontSize);
    deleteAndZero(label1);
    deleteAndZero(bgColour);
    deleteAndZero(label2);
    deleteAndZero(lineNumbersBgColour);
    deleteAndZero(label3);
    deleteAndZero(lineNumbersColour);
    deleteAndZero(fontTest);
    deleteAndZero(openSearchTabs);
    deleteAndZero(resetButton);
    deleteAndZero(label4);
    deleteAndZero(fontColour);
}

void CtrlrLuaMethodCodeEditorSettings::paint(Graphics& g)
{
        // Draw color preview rectangles next to each combo box
        if (bgColour && lineNumbersBgColour && lineNumbersColour && fontColour)
        {
            // Background color preview
            Rectangle<int> bgColorRect = bgColour->getBounds().withX(bgColour->getRight() + 4).withWidth(32);
            g.setColour(getColorFromCombo(bgColour));
            g.fillRect(bgColorRect);
            g.setColour(Colours::darkgrey);
            g.drawRect(bgColorRect, 1);

            // Line numbers background color preview
            Rectangle<int> lineNumBgColorRect = lineNumbersBgColour->getBounds().withX(lineNumbersBgColour->getRight() + 4).withWidth(32);
            g.setColour(getColorFromCombo(lineNumbersBgColour));
            g.fillRect(lineNumBgColorRect);
            g.setColour(Colours::darkgrey);
            g.drawRect(lineNumBgColorRect, 1);

            // Line numbers color preview
            Rectangle<int> lineNumColorRect = lineNumbersColour->getBounds().withX(lineNumbersColour->getRight() + 4).withWidth(32);
            g.setColour(getColorFromCombo(lineNumbersColour));
            g.fillRect(lineNumColorRect);
            g.setColour(Colours::darkgrey);
            g.drawRect(lineNumColorRect, 1);

            // Font color preview
            Rectangle<int> fontColorRect = fontColour->getBounds().withX(fontColour->getRight() + 4).withWidth(32);
            g.setColour(getColorFromCombo(fontColour));
            g.fillRect(fontColorRect);
            g.setColour(Colours::darkgrey);
            g.drawRect(fontColorRect, 1);
        }
}

void CtrlrLuaMethodCodeEditorSettings::resized()
{
    marginLeft = 12;
    marginTop = 12;
    sampleWidth = 310;
    sampleHeight = 78;

    fontTest->setBounds(marginLeft, marginTop, sampleWidth, sampleHeight);
    label0->setBounds(marginLeft - 4, marginTop + sampleHeight + 8, sampleWidth, 24);
    fontTypeface->setBounds(marginLeft, marginTop + sampleHeight + 24 + 8, sampleWidth, 24);
    fontBold->setBounds(marginLeft, marginTop + sampleHeight + 24 + 40, 56, 24);
    fontItalic->setBounds(marginLeft + 64, marginTop + sampleHeight + 24 + 40, 64, 24);
    fontSize->setBounds(marginLeft + 224, marginTop + sampleHeight + 24 + 40, 78, 24);

    label1->setBounds(marginLeft - 4, marginTop + sampleHeight + 24 + 72, sampleWidth, 24);
    bgColour->setBounds(marginLeft, marginTop + sampleHeight + 24 + 72 + 24, sampleWidth - 40, 24);
    label2->setBounds(marginLeft - 4, marginTop + sampleHeight + 24 + 72 + 24 + 32, sampleWidth, 24);
    lineNumbersBgColour->setBounds(marginLeft, marginTop + sampleHeight + 24 + 72 + 2 * 24 + 32, sampleWidth - 40, 24);
    label3->setBounds(marginLeft - 4, marginTop + sampleHeight + 24 + 72 + 2 * 24 + 2 * 32, sampleWidth, 24);
    lineNumbersColour->setBounds(marginLeft, marginTop + sampleHeight + 24 + 72 + 3 * 24 + 2 * 32, sampleWidth - 40, 24);

    label4->setBounds(marginLeft - 4, marginTop + sampleHeight + 24 + 72 + 3 * 24 + 3 * 32, sampleWidth, 24);
    fontColour->setBounds(marginLeft, marginTop + sampleHeight + 24 + 72 + 4 * 24 + 3 * 32, sampleWidth - 40, 24);

    openSearchTabs->setBounds(marginLeft + 0, marginTop + (sampleHeight + 24 + 72 + 4 * 24 + 3 * 32) + 40, sampleWidth, 24);
    resetButton->setBounds(marginLeft + sampleWidth / 2 - (sampleWidth / 4 + marginLeft / 2), marginTop + (sampleHeight + 24 + 72 + 4 * 24 + 3 * 32) + 80, sampleWidth / 2, 24);
}

void CtrlrLuaMethodCodeEditorSettings::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == fontTypeface)
    {
        // Handle font typeface changes
    }
    else if (comboBoxThatHasChanged == bgColour)
    {
        // Save the new background color setting
        owner.getComponentTree().setProperty(Ids::luaMethodEditorBgColour, getColorFromCombo(bgColour).toString(), nullptr);
    }
    else if (comboBoxThatHasChanged == lineNumbersBgColour)
    {
        // Save the new line numbers background color setting
        owner.getComponentTree().setProperty(Ids::luaMethodEditorLineNumbersBgColour, getColorFromCombo(lineNumbersBgColour).toString(), nullptr);
    }
    else if (comboBoxThatHasChanged == lineNumbersColour)
    {
        // Save the new line numbers color setting
        owner.getComponentTree().setProperty(Ids::luaMethodEditorLineNumbersColour, getColorFromCombo(lineNumbersColour).toString(), nullptr);
    }
    else if (comboBoxThatHasChanged == fontColour)
    {
        // Save the new font color setting
        owner.getComponentTree().setProperty(Ids::luaMethodEditorFontColour, getColorFromCombo(fontColour).toString(), nullptr);
    }

    changeListenerCallback(nullptr);
}

void CtrlrLuaMethodCodeEditorSettings::buttonClicked(Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == fontBold)
    {
        // Handle bold button
    }
    else if (buttonThatWasClicked == fontItalic)
    {
        // Handle italic button
    }
    else if (buttonThatWasClicked == openSearchTabs)
    {
        bool currentState = openSearchTabs->getToggleState();
        owner.setOpenSearchTabsEnabled(currentState);
        owner.getComponentTree().setProperty(Ids::openSearchTabsState, currentState, nullptr);
    }
    else if (buttonThatWasClicked == resetButton)
    {
        int result = AlertWindow::showOkCancelBox(
            AlertWindow::QuestionIcon,
            "Reset Editor",
            "Reset Editor to default?",
            "OK",
            "Cancel"
        );

        if (result == 1)
        {
            fontTypeface->setText("Courier New", dontSendNotification);
            fontBold->setToggleState(false, dontSendNotification);
            fontItalic->setToggleState(false, dontSendNotification);
            openSearchTabs->setToggleState(false, dontSendNotification);
            fontSize->setValue(14.0f, dontSendNotification);

            // Reset color combos to defaults
            bgColour->setSelectedId(findColorIndex(Colours::white), dontSendNotification);
            lineNumbersBgColour->setSelectedId(findColorIndex(Colour(0xffc5ddf1)), dontSendNotification);
            lineNumbersColour->setSelectedId(findColorIndex(Colours::black), dontSendNotification);
            fontColour->setSelectedId(findColorIndex(Colours::black), dontSendNotification);
        }
    }

    changeListenerCallback(nullptr);
}

void CtrlrLuaMethodCodeEditorSettings::sliderValueChanged(Slider* sliderThatWasMoved)
{
    if (sliderThatWasMoved == fontSize)
    {
        // Handle font size changes
    }

    changeListenerCallback(nullptr);
}
void CtrlrLuaMethodCodeEditorSettings::changeListenerCallback(ChangeBroadcaster* source)
{
    // Modify the member variable in place.
    // Ensure "Identifier" is a valid token in your default scheme.
    activeScheme.set("Identifier", getColorFromCombo(fontColour));

    // Apply the single, stable scheme to all editors.
    if (fontTest)
    {
        fontTest->setColourScheme(activeScheme);
        fontTest->setColour(CodeEditorComponent::backgroundColourId, getColorFromCombo(bgColour));
        fontTest->setColour(CodeEditorComponent::lineNumberBackgroundId, getColorFromCombo(lineNumbersBgColour));
        fontTest->setColour(CodeEditorComponent::lineNumberTextId, getColorFromCombo(lineNumbersColour));
        fontTest->setFont(getFont());
    }

    if (auto* editor = owner.getEditorComponent())
    {
        editor->setColourScheme(activeScheme);
        editor->setColour(CodeEditorComponent::backgroundColourId, getColorFromCombo(bgColour));
        editor->setColour(CodeEditorComponent::lineNumberBackgroundId, getColorFromCombo(lineNumbersBgColour));
        editor->setColour(CodeEditorComponent::lineNumberTextId, getColorFromCombo(lineNumbersColour));
        editor->setFont(getFont());
    }

    repaint();
}
//void CtrlrLuaMethodCodeEditorSettings::changeListenerCallback(ChangeBroadcaster* source)
    // Replace this line:
    //owner.getEditorComponent().setColourScheme(editorScheme);
    //// With this:
    //if (auto* editor = owner.getEditorComponent())
    //    editor->setColourScheme(editorScheme);
//{
//    fontTest->setColour(CodeEditorComponent::backgroundColourId, getColorFromCombo(bgColour));
//    fontTest->setColour(CodeEditorComponent::lineNumberBackgroundId, getColorFromCombo(lineNumbersBgColour));
//    fontTest->setColour(CodeEditorComponent::lineNumberTextId, getColorFromCombo(lineNumbersColour));
//    fontTest->setColour(CodeEditorComponent::defaultTextColourId, getColorFromCombo(fontColour));
//    //fontTest->setColour(CodeEditorComponent::defaultTextColourId, Colours::red);
//    fontTest->setFont(getFont());
//    //DBG("=== Color Update Debug ===");
//    //DBG("Background: " + bgColour.toString());
//    //DBG("Line Num BG: " + lineNumBgColour.toString());
//    //DBG("Line Num Text: " + lineNumColour.toString());
//    //DBG("Font Color: " + fontColour.toString());
//    repaint();
//}

const Font CtrlrLuaMethodCodeEditorSettings::getFont()
{
    Font font = owner.getOwner().getCtrlrManagerOwner().getFontManager().getFont(fontTypeface->getSelectedItemIndex());

    if (fontTypeface)
        font.setTypefaceName(fontTypeface->getText());
    else
        return (font);

    font.setHeight(fontSize->getValue());
    font.setBold(fontBold->getToggleState());
    font.setItalic(fontItalic->getToggleState());
    return (font);
}

const Colour CtrlrLuaMethodCodeEditorSettings::getBgColour()
{
    return getColorFromCombo(bgColour);
}

const Colour CtrlrLuaMethodCodeEditorSettings::getLineNumbersBgColour()
{
    return getColorFromCombo(lineNumbersBgColour);
}

const Colour CtrlrLuaMethodCodeEditorSettings::getLineNumbersColour()
{
    return getColorFromCombo(lineNumbersColour);
}

// Helper functions
void CtrlrLuaMethodCodeEditorSettings::populateColorCombo(ComboBox* combo) {
    combo->clear();
    for (int i = 0; i < sizeof(availableColors) / sizeof(availableColors[0]); ++i) {
        combo->addItem(availableColors[i].name, i + 1);
    }
}

int CtrlrLuaMethodCodeEditorSettings::findColorIndex(const Colour& color) {
    for (int i = 0; i < sizeof(availableColors) / sizeof(availableColors[0]); ++i) {
        if (availableColors[i].color == color) {
            return i + 1; // ComboBox IDs start from 1
        }
    }
    return 1; // Default to first color (White) if not found
}

Colour CtrlrLuaMethodCodeEditorSettings::getColorFromCombo(ComboBox* combo) {
    int selectedId = combo->getSelectedId();
    if (selectedId > 0 && selectedId <= sizeof(availableColors) / sizeof(availableColors[0])) {
        return availableColors[selectedId - 1].color;
    }
    return Colours::white; // Default fallback
}
const Colour CtrlrLuaMethodCodeEditorSettings::getFontColour()
{
    return getColorFromCombo(fontColour);
}
void CtrlrLuaMethodCodeEditorSettings::updateEditorColours()
{
    // Now it's safe to call getColorFromCombo()
    activeScheme = luaTokeniser.getDefaultColourScheme();
    activeScheme.set("Identifier", getColorFromCombo(fontColour));

    if (fontTest)
    {
        fontTest->setColourScheme(activeScheme);
        fontTest->setColour(CodeEditorComponent::backgroundColourId, getColorFromCombo(bgColour));
        fontTest->setColour(CodeEditorComponent::lineNumberBackgroundId, getColorFromCombo(lineNumbersBgColour));
        fontTest->setColour(CodeEditorComponent::lineNumberTextId, getColorFromCombo(lineNumbersColour));
        fontTest->setFont(getFont());
    }

    if (auto* editor = owner.getEditorComponent())
    {
        editor->setColourScheme(activeScheme);
        editor->setColour(CodeEditorComponent::backgroundColourId, getColorFromCombo(bgColour));
        editor->setColour(CodeEditorComponent::lineNumberBackgroundId, getColorFromCombo(lineNumbersBgColour));
        editor->setColour(CodeEditorComponent::lineNumberTextId, getColorFromCombo(lineNumbersColour));
        editor->setFont(getFont());
    }
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
