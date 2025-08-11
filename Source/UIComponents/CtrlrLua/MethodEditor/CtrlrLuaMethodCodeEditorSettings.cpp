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
const CtrlrLuaMethodCodeEditorSettings::ColourItem CtrlrLuaMethodCodeEditorSettings::availableColours[] = {
    {"Alice Blue", Colours::aliceblue},
    {"Antique White", Colours::antiquewhite},
    {"Aqua", Colours::aqua},
    {"Aquamarine", Colours::aquamarine},
    {"Azure", Colours::azure},
    {"Beige", Colours::beige},
    {"Bisque", Colours::bisque},
    {"Black", Colours::black},
    {"Blanched Almond", Colours::blanchedalmond},
    {"Blue", Colours::blue},
    {"Blue Violet", Colours::blueviolet},
    {"Brown", Colours::brown},
    {"Burly Wood", Colours::burlywood},
    {"Cadet Blue", Colours::cadetblue},
    {"Chartreuse", Colours::chartreuse},
    {"Chocolate", Colours::chocolate},
    {"Coral", Colours::coral},
    {"Cornflower Blue", Colours::cornflowerblue},
    {"Cornsilk", Colours::cornsilk},
    {"Crimson", Colours::crimson},
    {"Cyan", Colours::cyan},
    {"Dark Blue", Colours::darkblue},
    {"Dark Cyan", Colours::darkcyan},
    {"Dark Goldenrod", Colours::darkgoldenrod},
    {"Dark Green", Colours::darkgreen},
    {"Dark Grey", Colours::darkgrey},
    {"Dark Khaki", Colours::darkkhaki},
    {"Dark Magenta", Colours::darkmagenta},
    {"Dark Olive Green", Colours::darkolivegreen},
    {"Dark Orange", Colours::darkorange},
    {"Dark Orchid", Colours::darkorchid},
    {"Dark Red", Colours::darkred},
    {"Dark Salmon", Colours::darksalmon},
    {"Dark Sea Green", Colours::darkseagreen},
    {"Dark Slate Blue", Colours::darkslateblue},
    {"Dark Slate Grey", Colours::darkslategrey},
    {"Dark Turquoise", Colours::darkturquoise},
    {"Dark Violet", Colours::darkviolet},
    {"Deep Pink", Colours::deeppink},
    {"Deep Sky Blue", Colours::deepskyblue},
    {"Dim Grey", Colours::dimgrey},
    {"Dodger Blue", Colours::dodgerblue},
    {"Firebrick", Colours::firebrick},
    {"Floral White", Colours::floralwhite},
    {"Forest Green", Colours::forestgreen},
    {"Fuchsia", Colours::fuchsia},
    {"Gainsboro", Colours::gainsboro},
    {"Gold", Colours::gold},
    {"Goldenrod", Colours::goldenrod},
    {"Green", Colours::green},
    {"Green Yellow", Colours::greenyellow},
    {"Grey", Colours::grey},
    {"Honeydew", Colours::honeydew},
    {"Hot Pink", Colours::hotpink},
    {"Indian Red", Colours::indianred},
    {"Indigo", Colours::indigo},
    {"Ivory", Colours::ivory},
    {"Khaki", Colours::khaki},
    {"Lavender", Colours::lavender},
    {"Lavender Blush", Colours::lavenderblush},
    {"Lawn Green", Colours::lawngreen},
    {"Lemon Chiffon", Colours::lemonchiffon},
    {"Light Blue", Colours::lightblue},
    {"Light Coral", Colours::lightcoral},
    {"Light Cyan", Colours::lightcyan},
    {"Light Goldenrod Yellow", Colours::lightgoldenrodyellow},
    {"Light Green", Colours::lightgreen},
    {"Light Grey", Colours::lightgrey},
    {"Light Pink", Colours::lightpink},
    {"Light Salmon", Colours::lightsalmon},
    {"Light Sea Green", Colours::lightseagreen},
    {"Light Sky Blue", Colours::lightskyblue},
    {"Light Slate Grey", Colours::lightslategrey},
    {"Light Steel Blue", Colours::lightsteelblue},
    {"Light Yellow", Colours::lightyellow},
    {"Lime", Colours::lime},
    {"Lime Green", Colours::limegreen},
    {"Linen", Colours::linen},
    {"Magenta", Colours::magenta},
    {"Maroon", Colours::maroon},
    {"Medium Aquamarine", Colours::mediumaquamarine},
    {"Medium Blue", Colours::mediumblue},
    {"Medium Orchid", Colours::mediumorchid},
    {"Medium Purple", Colours::mediumpurple},
    {"Medium Sea Green", Colours::mediumseagreen},
    {"Medium Slate Blue", Colours::mediumslateblue},
    {"Medium Spring Green", Colours::mediumspringgreen},
    {"Medium Turquoise", Colours::mediumturquoise},
    {"Medium Violet Red", Colours::mediumvioletred},
    {"Midnight Blue", Colours::midnightblue},
    {"Mint Cream", Colours::mintcream},
    {"Misty Rose", Colours::mistyrose},
    {"Moccasin", Colours::moccasin},
    {"Navajo White", Colours::navajowhite},
    {"Navy", Colours::navy},
    {"Old Lace", Colours::oldlace},
    {"Olive", Colours::olive},
    {"Olive Drab", Colours::olivedrab},
    {"Orange", Colours::orange},
    {"Orange Red", Colours::orangered},
    {"Orchid", Colours::orchid},
    {"Pale Goldenrod", Colours::palegoldenrod},
    {"Pale Green", Colours::palegreen},
    {"Pale Turquoise", Colours::paleturquoise},
    {"Pale Violet Red", Colours::palevioletred},
    {"Papaya Whip", Colours::papayawhip},
    {"Peach Puff", Colours::peachpuff},
    {"Peru", Colours::peru},
    {"Pink", Colours::pink},
    {"Plum", Colours::plum},
    {"Powder Blue", Colours::powderblue},
    {"Purple", Colours::purple},
    {"Rebecca Purple", Colours::rebeccapurple},
    {"Red", Colours::red},
    {"Rosy Brown", Colours::rosybrown},
    {"Royal Blue", Colours::royalblue},
    {"Saddle Brown", Colours::saddlebrown},
    {"Salmon", Colours::salmon},
    {"Sandy Brown", Colours::sandybrown},
    {"Sea Green", Colours::seagreen},
    {"Seashell", Colours::seashell},
    {"Sienna", Colours::sienna},
    {"Silver", Colours::silver},
    {"Sky Blue", Colours::skyblue},
    {"Slate Blue", Colours::slateblue},
    {"Slate Grey", Colours::slategrey},
    {"Snow", Colours::snow},
    {"Spring Green", Colours::springgreen},
    {"Steel Blue", Colours::steelblue},
    {"Tan", Colours::tan},
    {"Teal", Colours::teal},
    {"Thistle", Colours::thistle},
    {"Tomato", Colours::tomato},
    {"Turquoise", Colours::turquoise},
    {"Violet", Colours::violet},
    {"Wheat", Colours::wheat},
    {"White", Colours::white},
    {"White Smoke", Colours::whitesmoke},
    {"Yellow", Colours::yellow},
    {"Yellow Green", Colours::yellowgreen},
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
    fontTest(0),
    resetButton(0),
    openSearchTabs(0)
{
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

    addAndMakeVisible(label2 = new Label("new label", TRANS("Line numbers background:")));
    label2->setFont(Font(14.00f));
    label2->setJustificationType(Justification::centredLeft);
    label2->setEditable(false, false, false);

    addAndMakeVisible(label3 = new Label("new label", TRANS("Line numbers:")));
    label3->setFont(Font(14.00f));
    label3->setJustificationType(Justification::centredLeft);
    label3->setEditable(false, false, false);

    addAndMakeVisible(fontTest = new CodeEditorComponent(codeDocument, &luaTokeniser));

    addAndMakeVisible(openSearchTabs = new ToggleButton(""));
    //openSearchTabs->setButtonText("Open Search Tabs"); // Corrected to use a string literal
     openSearchTabs->getToggleStateValue().referTo(SharedValues::getSearchTabsValue()); 

    addAndMakeVisible(resetButton = new TextButton("RESET"));
    resetButton->addListener(this);
    resetButton->setColour(TextButton::buttonColourId, Colours::cornflowerblue);


    addAndMakeVisible(bgColour = new ComboBox("bgColour"));
    bgColour->setEditableText(false);
    bgColour->setJustificationType(Justification::centredLeft);
    bgColour->addListener(this);

    addAndMakeVisible(lineNumbersBgColour = new ComboBox("lineNumbersBgColour"));
    lineNumbersBgColour->setEditableText(false);
    lineNumbersBgColour->setJustificationType(Justification::centredLeft);
    lineNumbersBgColour->addListener(this);

    addAndMakeVisible(lineNumbersColour = new ComboBox("lineNumbersColour"));
    lineNumbersColour->setEditableText(false);
    lineNumbersColour->setJustificationType(Justification::centredLeft);
    lineNumbersColour->addListener(this);

    // Now that the combo boxes exist, populate them
    populateColourCombo(bgColour);
    populateColourCombo(lineNumbersBgColour);
    populateColourCombo(lineNumbersColour);

    addAndMakeVisible(syntaxLabel = new Label("syntaxLabel", TRANS("Syntax Highlighting:")));
    syntaxLabel->setFont(Font(14.00f));
    syntaxLabel->setJustificationType(Justification::centredLeft);
    syntaxLabel->setEditable(false, false, false);

    addAndMakeVisible(syntaxTokenType = new ComboBox("syntaxTokenType"));
    syntaxTokenType->setEditableText(false);
    syntaxTokenType->setJustificationType(Justification::centredLeft);
    syntaxTokenType->addListener(this);

    addAndMakeVisible(syntaxTokenColor = new ComboBox("syntaxTokenColor"));
    syntaxTokenColor->setEditableText(false);
    syntaxTokenColor->setJustificationType(Justification::centredLeft);
    syntaxTokenColor->addListener(this);

    populateSyntaxTokenCombo();
    populateColourCombo(syntaxTokenColor);

    // Load saved settings BEFORE setting initial selections
    loadSyntaxColorsFromSettings();

    // Set initial selection for token type
    syntaxTokenType->setSelectedId(1, dontSendNotification);

    // Update the color combo to show the color for the initially selected token
    String initialToken = getCurrentSelectedTokenType();
    if (customSyntaxColors.contains(initialToken))
    {
        Colour currentColor = customSyntaxColors[initialToken];
        syntaxTokenColor->setSelectedId(findColourIndex(currentColor), dontSendNotification);
    }

    codeFont = owner.getOwner().getCtrlrManagerOwner().getFontManager().getFontFromString(owner.getComponentTree().getProperty(Ids::luaMethodEditorFont, owner.getOwner().getCtrlrManagerOwner().getFontManager().getStringFromFont(Font(owner.getOwner().getCtrlrManagerOwner().getFontManager().getDefaultMonoFontName(), 14.0f, Font::plain))));
    label1->setColour(TextEditor::textColourId, findColour(TextEditor::textColourId));
    label1->setColour(TextEditor::backgroundColourId, Colour(0x00000000));
    label2->setColour(TextEditor::textColourId, findColour(TextEditor::textColourId));
    label2->setColour(TextEditor::backgroundColourId, Colour(0x00000000));
    label3->setColour(TextEditor::textColourId, findColour(TextEditor::textColourId));
    label3->setColour(TextEditor::backgroundColourId, Colour(0x00000000));
    Colour defaultBgColour = VAR2COLOUR(owner.getComponentTree().getProperty(Ids::luaMethodEditorBgColour, Colours::white.toString()));
    bgColour->setSelectedId(findColourIndex(defaultBgColour), dontSendNotification);
    Colour defaultLineNumBgColour = VAR2COLOUR(owner.getComponentTree().getProperty(Ids::luaMethodEditorLineNumbersBgColour, Colours::cornflowerblue.toString()));
    lineNumbersBgColour->setSelectedId(findColourIndex(defaultLineNumBgColour), dontSendNotification);
    Colour defaultLineNumColour = VAR2COLOUR(owner.getComponentTree().getProperty(Ids::luaMethodEditorLineNumbersColour, Colours::black.toString()));
    lineNumbersColour->setSelectedId(findColourIndex(defaultLineNumColour), dontSendNotification);
    fontSize->setValue(codeFont.getHeight(), dontSendNotification);
    fontBold->setToggleState(codeFont.isBold(), dontSendNotification);
    fontItalic->setToggleState(codeFont.isItalic(), dontSendNotification);
    owner.getOwner().getCtrlrManagerOwner().getFontManager().fillCombo(*fontTypeface);
    fontTypeface->setText(codeFont.getTypefaceName(), sendNotification);
    codeDocument.replaceAllContent("-- This is a comment\nfunction myFunction(argument)\n\tcall(\"string\")\nend");

    setSize(334, 600);
    updateSyntaxColors();
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
    deleteAndZero(syntaxLabel);
    deleteAndZero(syntaxTokenType);
    deleteAndZero(syntaxTokenColor);
}

//==============================================================================
void CtrlrLuaMethodCodeEditorSettings::paint(Graphics& g)
{
    Rectangle<int> bgColourRect = bgColour->getBounds().withX(bgColour->getRight() + 4).withWidth(32);
    g.setColour(getColourFromCombo(bgColour));
    g.fillRect(bgColourRect);
    g.setColour(Colours::darkgrey);
    g.drawRect(bgColourRect, 1);

    // Line numbers background colour preview
    Rectangle<int> lineNumBgColourRect = lineNumbersBgColour->getBounds().withX(lineNumbersBgColour->getRight() + 4).withWidth(32);
    g.setColour(getColourFromCombo(lineNumbersBgColour));
    g.fillRect(lineNumBgColourRect);
    g.setColour(Colours::darkgrey);
    g.drawRect(lineNumBgColourRect, 1);

    // Line numbers colour preview
    Rectangle<int> lineNumColourRect = lineNumbersColour->getBounds().withX(lineNumbersColour->getRight() + 4).withWidth(32);
    g.setColour(getColourFromCombo(lineNumbersColour));
    g.fillRect(lineNumColourRect);
    g.setColour(Colours::darkgrey);
    g.drawRect(lineNumColourRect, 1);

    Rectangle<int> syntaxColorRect = syntaxTokenColor->getBounds().withX(syntaxTokenColor->getRight() + 4).withWidth(32);
    g.setColour(getColourFromCombo(syntaxTokenColor));
    g.fillRect(syntaxColorRect);
    g.setColour(Colours::darkgrey);
    g.drawRect(syntaxColorRect, 1);

}

void CtrlrLuaMethodCodeEditorSettings::resized()
{
    marginLeft = 12;
    marginTop = 12;
    sampleWidth = 310;
    sampleHeight = 78;

    if (fontTest != nullptr)
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
    openSearchTabs->setBounds(marginLeft + 0, marginTop + (sampleHeight + 24 + 72 + 3 * 24 + 2 * 32) + 40, sampleWidth, 24);
    resetButton->setBounds(marginLeft + sampleWidth / 2 - (sampleWidth / 4 + marginLeft / 2), marginTop + (sampleHeight + 24 + 72 + 3 * 24 + 2 * 32) + 80, sampleWidth / 2, 24);

    int syntaxY = marginTop + (sampleHeight + 24 + 72 + 3 * 24 + 2 * 32) + 120;

    syntaxLabel->setBounds(marginLeft - 4, syntaxY, sampleWidth, 24);
    syntaxTokenType->setBounds(marginLeft, syntaxY + 24, (sampleWidth - 8) / 2, 24);
    syntaxTokenColor->setBounds(marginLeft + (sampleWidth - 8) / 2 + 8, syntaxY + 24, (sampleWidth - 8) / 2 - 40, 24);

    // Adjust reset button position
    resetButton->setBounds(marginLeft + sampleWidth / 2 - (sampleWidth / 4 + marginLeft / 2), syntaxY + 60, sampleWidth / 2, 24);
}


void CtrlrLuaMethodCodeEditorSettings::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    DBG("ComboBox changed: " + comboBoxThatHasChanged->getName());
    if (comboBoxThatHasChanged == fontTypeface)
    {
        changeListenerCallback(nullptr);
    }
    else if (comboBoxThatHasChanged == bgColour ||
        comboBoxThatHasChanged == lineNumbersBgColour ||
        comboBoxThatHasChanged == lineNumbersColour)
    {
        changeListenerCallback(nullptr);
    }
    else if (comboBoxThatHasChanged == syntaxTokenType)
    {
        // When token type changes, update the color combo to show current color for that token
        String selectedToken = getCurrentSelectedTokenType();
          
if (customSyntaxColors.contains(selectedToken))
        {
            Colour currentColor = customSyntaxColors[selectedToken];
            syntaxTokenColor->setSelectedId(findColourIndex(currentColor), dontSendNotification);
        }
        else
        {
            // Show default color from tokenizer
            CodeEditorComponent::ColourScheme defaultScheme = luaTokeniser.getDefaultColourScheme();
            Colour defaultColor = Colours::black; // fallback

            struct DefaultType { const char* name; uint32 colour; };
            const DefaultType defaultTypes[] = {
                { "Error",      0xffcc0000 },
                { "Comment",    0xff008000 },
                { "Keyword",    0xff0000cc },
                { "Operator",   0xff225500 },
                { "Identifier", 0xff000000 },
                { "Integer",    0xff880000 },
                { "Float",      0xff885500 },
                { "String",     0xff990099 },
                { "Bracket",    0xff000055 },
                { "Punctuation", 0xff004400 }
            };

            for (int i = 0; i < 10; ++i)
            {
                if (selectedToken == defaultTypes[i].name)
                {
                    defaultColor = Colour(defaultTypes[i].colour);
                    break;
                }
            }
            syntaxTokenColor->setSelectedId(findColourIndex(defaultColor), dontSendNotification);
        }
        repaint();
    }
    else if (comboBoxThatHasChanged == syntaxTokenColor)
    {
        // When color changes, update the custom color for the selected token type
        String selectedToken = getCurrentSelectedTokenType();
        Colour selectedColor = getColourFromCombo(syntaxTokenColor);

        customSyntaxColors.set(selectedToken, selectedColor);
        saveSyntaxColorsToSettings();
        updateSyntaxColors();
        repaint();
    }
}

void CtrlrLuaMethodCodeEditorSettings::buttonClicked(Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == fontBold)
    {
    }
    else if (buttonThatWasClicked == openSearchTabs)
    {
        bool currentState = openSearchTabs->getToggleState();
        owner.setOpenSearchTabsEnabled(currentState);
        owner.getComponentTree().setProperty(Ids::openSearchTabsState, currentState, nullptr);
    }
    else if (buttonThatWasClicked == fontItalic)
    {
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
            bgColour->setSelectedId(findColourIndex(Colours::white), dontSendNotification);
            lineNumbersBgColour->setSelectedId(findColourIndex(Colours::cornflowerblue), dontSendNotification);
            lineNumbersColour->setSelectedId(findColourIndex(Colours::black), dontSendNotification);

            // This is needed to update the UI after a reset
            changeListenerCallback(nullptr);
        }
    }

    changeListenerCallback(nullptr);
}

void CtrlrLuaMethodCodeEditorSettings::sliderValueChanged(Slider* sliderThatWasMoved)
{
    if (sliderThatWasMoved == fontSize)
    {
    }
    changeListenerCallback(nullptr);
}

//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void CtrlrLuaMethodCodeEditorSettings::changeListenerCallback(ChangeBroadcaster* source)
{
    if (fontTest)
    {
        fontTest->setColour(CodeEditorComponent::backgroundColourId, getColourFromCombo(bgColour));
        fontTest->setColour(CodeEditorComponent::lineNumberBackgroundId, getColourFromCombo(lineNumbersBgColour));
        fontTest->setColour(CodeEditorComponent::lineNumberTextId, getColourFromCombo(lineNumbersColour));
        fontTest->setFont(getFont());
    }
    repaint();
}

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
    return getColourFromCombo(bgColour);
}

const Colour CtrlrLuaMethodCodeEditorSettings::getLineNumbersBgColour()
{
    return getColourFromCombo(lineNumbersBgColour);
}

const Colour CtrlrLuaMethodCodeEditorSettings::getLineNumbersColour()
{
    return getColourFromCombo(lineNumbersColour);
}

void CtrlrLuaMethodCodeEditorSettings::populateColourCombo(ComboBox* combo) {
    combo->clear();
    for (int i = 0; i < sizeof(availableColours) / sizeof(availableColours[0]); ++i) {
        combo->addItem(availableColours[i].name, i + 1);
    }
}

int CtrlrLuaMethodCodeEditorSettings::findColourIndex(const Colour& colour) {
    for (int i = 0; i < sizeof(availableColours) / sizeof(availableColours[0]); ++i) {
        if (availableColours[i].colour == colour) {
            return i + 1; // ComboBox IDs start from 1
        }
    }
    return 1; // Default to first colour (White) if not found
}

Colour CtrlrLuaMethodCodeEditorSettings::getColourFromCombo(ComboBox* combo) {
    int selectedId = combo->getSelectedId();
    if (selectedId > 0 && selectedId <= sizeof(availableColours) / sizeof(availableColours[0])) {
        return availableColours[selectedId - 1].colour;
    }
    return Colours::white; // Default fallback
}
void CtrlrLuaMethodCodeEditorSettings::populateSyntaxTokenCombo()
{
    syntaxTokenType->clear();
    StringArray tokenTypes = CtrlrLuaCodeTokeniser::getTokenTypeNames();

    for (int i = 0; i < tokenTypes.size(); ++i)
    {
        syntaxTokenType->addItem(tokenTypes[i], i + 1);
    }

    // Select first item by default
    syntaxTokenType->setSelectedId(1, dontSendNotification);
}

void CtrlrLuaMethodCodeEditorSettings::updateSyntaxColors() {
    // Create the custom scheme
    CodeEditorComponent::ColourScheme scheme = CtrlrLuaCodeTokeniser::getCustomColourScheme(customSyntaxColors);

    // Update the static shared scheme so new editors use it
    getSharedScheme() = scheme;

    // Update preview
    fontTest->setColourScheme(scheme);

    // Update current editor
    CtrlrLuaMethodCodeEditor* currentEditor = owner.getCurrentEditor();
    if (currentEditor && currentEditor->getCodeComponent()) {
        try {
            currentEditor->getCodeComponent()->setColourScheme(scheme);
            currentEditor->getCodeComponent()->repaint();
            DBG("Successfully updated current editor");
        }
        catch (...) {
            DBG("Failed to update current editor - trying content refresh");
            currentEditor->getCodeComponent()->repaint();
        }
    }
    else {
        DBG("No current editor available");
    }
}

CodeEditorComponent::ColourScheme& CtrlrLuaMethodCodeEditorSettings::getSharedScheme()
{
    static CodeEditorComponent::ColourScheme sharedScheme =
        CtrlrLuaCodeTokeniser::getDefaultColourScheme();
    return sharedScheme;
}

String CtrlrLuaMethodCodeEditorSettings::getCurrentSelectedTokenType()
{
    if (syntaxTokenType->getSelectedId() > 0)
    {
        return syntaxTokenType->getText();
    }
    return "Error"; // Default fallback
}

void CtrlrLuaMethodCodeEditorSettings::loadSyntaxColorsFromSettings()
{
    StringArray tokenTypes = CtrlrLuaCodeTokeniser::getTokenTypeNames();

    for (int i = 0; i < tokenTypes.size(); ++i)
    {
        const String& tokenType = tokenTypes[i];
        String settingKey = "syntaxColor_" + tokenType;
        var colorVar = owner.getComponentTree().getProperty(settingKey);
        DBG("Loading: " + settingKey + " = " + colorVar.toString());

        if (!colorVar.isVoid() && colorVar.toString().isNotEmpty())
        {
            Colour savedColor = VAR2COLOUR(colorVar);
            customSyntaxColors.set(tokenType, savedColor);
        }
    }
}

void CtrlrLuaMethodCodeEditorSettings::saveSyntaxColorsToSettings()
{
    HashMap<String, Colour>::Iterator it(customSyntaxColors);
    while (it.next())
    {
        String settingKey = "syntaxColor_" + it.getKey();
        String colorValue = it.getValue().toString();
        DBG("Saving: " + settingKey + " = " + colorValue);
        owner.getComponentTree().setProperty(settingKey, colorValue, nullptr);
    }
}
//[/MiscUserCode]


//==============================================================================
#if 0
/* -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="CtrlrLuaMethodCodeEditorSettings"
                 componentName="" parentClasses="public Component, public ChangeListener"
                 constructorParams="CtrlrLuaMethodEditor &amp;_owner, juce::Value&amp; sharedSearchTabsValue_"
                 variableInitialisers="owner(_owner), sharedSearchTabsValue(sharedSearchTabsValue_)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330000013"
                 fixedSize="1" initialWidth="334" initialHeight="450">
  <BACKGROUND backgroundColour="0"/>
  <LABEL name="new label" id="9d980d9b43d262a" memberName="label0" virtualName=""
         explicitFocusOrder="0" pos="8 102 310 24" edTextCol="ff000000"
         edBkgCol="0" sourceText="Font:" is(false)" has(false)" editable="0"/>
  <COMBOBOX name="" id="aa72a7d14fe94324" memberName="fontTypeface" virtualName=""
            explicitFocusOrder="0" pos="12 134 310 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <TOGGLEBUTTON name="" id="9d9f05c9c5dd0d7d" memberName="fontBold" virtualName=""
                explicitFocusOrder="0" pos="12 166 56 24" buttonText="Bold"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="" id="b8a95fd341c23fa0" memberName="fontItalic" virtualName=""
                explicitFocusOrder="0" pos="76 166 64 24" buttonText="Italic"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <SLIDER name="" id="d6eb265ed035d08a" memberName="fontSize" virtualName=""
          explicitFocusOrder="0" pos="236 166 78 24" min="0" max="128" int="1"
          style="IncDecButtons" textBoxPos="TextBoxLeft" textBoxEditable="1"
          textBoxWidth="32" textBoxHeight="24" skewFactor="1"/>
  <LABEL name="new label" id="d8b1e4284157b8fe" memberName="label1" virtualName=""
         explicitFocusOrder="0" pos="8 198 310 24" edTextCol="ff000000"
         edBkgCol="0" sourceText="Editor background:" is(false)" has(false)"
         editable="0"/>
  <LABEL name="new label" id="245084931a2ac537" memberName="label2" virtualName=""
         explicitFocusOrder="0" pos="8 254 310 24" edTextCol="ff000000"
         edBkgCol="0" sourceText="Line numbers background:" is(false)" has(false)"
         editable="0"/>
  <LABEL name="new label" id="e947b19818b76dfb" memberName="label3" virtualName=""
         explicitFocusOrder="0" pos="8 310 310 24" edTextCol="ff000000"
         edBkgCol="0" sourceText="Line numbers:" is(false)" has(false)" editable="0"/>
  <GENERICCOMPONENT name="" id="f7d42a6b93ff05ed" memberName="fontTest" virtualName=""
                    explicitFocusOrder="0" pos="12 12 310 78" class="CodeEditorComponent"
                    params="codeDocument, &amp;luaTokeniser"/>
  <TOGGLEBUTTON name="" id="c74d618d3639e4a3" memberName="openSearchTabs"
                virtualName="" explicitFocusOrder="0" pos="12 376 310 24"
                buttonText="Open Search Tabs" connectedEdges="0" needsCallback="1"
                radioGroupId="0" state="0"/>
  <TEXTBUTTON name="RESET" id="3ab346c8b0051829" memberName="resetButton"
              virtualName="" explicitFocusOrder="0" pos="82 416 160 24"
              bgColOff="ff6495ed" buttonText="RESET" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <COMBOBOX name="bgColour" id="705a63901b0f0237" memberName="bgColour" virtualName=""
            explicitFocusOrder="0" pos="12 230 310 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="lineNumbersBgColour" id="45300d8f6d2b38f8" memberName="lineNumbersBgColour"
            virtualName="" explicitFocusOrder="0" pos="12 286 310 24" editable="0"
            layout="33" items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="lineNumbersColour" id="24f11e3b6a948e3e" memberName="lineNumbersColour"
            virtualName="" explicitFocusOrder="0" pos="12 342 310 24" editable="0"
            layout="33" items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

/*
  -- End of Jucer information section --
*/