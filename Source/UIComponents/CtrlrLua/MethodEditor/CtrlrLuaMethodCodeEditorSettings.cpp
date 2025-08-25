#include "stdafx.h"
#include "CtrlrLuaMethodEditor.h"
#include "CtrlrPanel/CtrlrPanel.h"
#include "CtrlrManager/CtrlrManager.h"
#include "CtrlrLuaMethodCodeEditorSettings.h"

const CtrlrLuaMethodCodeEditorSettings::ColourItem CtrlrLuaMethodCodeEditorSettings::availableColours[] = {
    // Common colors first
    {"Black", Colours::black},
    {"White", Colours::white},
    {"Grey", Colours::grey},
    {"Dark Grey", Colours::darkgrey},
    {"Light Grey", Colours::lightgrey},

    // Primary colors
    {"Red", Colours::red},
    {"Green", Colours::green},
    {"Blue", Colours::blue},
    {"Yellow", Colours::yellow},
    {"Cyan", Colours::cyan},
    {"Magenta", Colours::magenta},

    // Dark variants
    {"Dark Red", Colours::darkred},
    {"Dark Green", Colours::darkgreen},
    {"Dark Blue", Colours::darkblue},
    {"Dark Cyan", Colours::darkcyan},
    {"Dark Magenta", Colours::darkmagenta},

    // Light variants
    {"Light Blue", Colours::lightblue},
    {"Light Green", Colours::lightgreen},
    {"Light Coral", Colours::lightcoral},
    {"Light Pink", Colours::lightpink},
    {"Light Yellow", Colours::lightyellow},

    // Popular web colors
    {"Orange", Colours::orange},
    {"Purple", Colours::purple},
    {"Brown", Colours::brown},
    {"Pink", Colours::pink},
    {"Gold", Colours::gold},
    {"Silver", Colours::silver},

    // Nature colors
    {"Forest Green", Colours::forestgreen},
    {"Sea Green", Colours::seagreen},
    {"Sky Blue", Colours::skyblue},
    {"Royal Blue", Colours::royalblue},
    {"Coral", Colours::coral},
    {"Salmon", Colours::salmon},

    // Distinctive colors
    {"Crimson", Colours::crimson},
    {"Indigo", Colours::indigo},
    {"Violet", Colours::violet},
    {"Turquoise", Colours::turquoise},
    {"Teal", Colours::teal},
    {"Maroon", Colours::maroon},
    {"Navy", Colours::navy},
    {"Olive", Colours::olive},

    // Warm colors
    {"Chocolate", Colours::chocolate},
    {"Sienna", Colours::sienna},
    {"Peru", Colours::peru},
    {"Tan", Colours::tan},
    {"Wheat", Colours::wheat},

    // Cool colors
    {"Steel Blue", Colours::steelblue},
    {"Slate Blue", Colours::slateblue},
    {"Cornflower Blue", Colours::cornflowerblue},
    {"Dodger Blue", Colours::dodgerblue},
    {"Deep Sky Blue", Colours::deepskyblue},

    // Specialty colors
    {"Firebrick", Colours::firebrick},
    {"Hot Pink", Colours::hotpink},
    {"Deep Pink", Colours::deeppink},
    {"Lime Green", Colours::limegreen},
    {"Yellow Green", Colours::yellowgreen},
    {"Orange Red", Colours::orangered},

    // Muted colors
    {"Dim Grey", Colours::dimgrey},
    {"Slate Grey", Colours::slategrey},
    {"Gainsboro", Colours::gainsboro},
    {"Beige", Colours::beige},
    {"Khaki", Colours::khaki},
    {"Lavender", Colours::lavender},
};


CtrlrLuaMethodCodeEditorSettings::CtrlrLuaMethodCodeEditorSettings(CtrlrLuaMethodEditor& _owner, juce::Value& sharedSearchTabsValue_)
    : owner(_owner), sharedSearchTabsValue(sharedSearchTabsValue_),
    fontTypeface(0),
    fontBold(0),
    // fontUnderline (0),
    fontItalic(0),
    fontSize(0),
    bgColour(0), // Added v5.6.31
    lineNumbersBgColour(0), // Added v5.6.31
    lineNumbersColour(0), // Added v5.6.31
    fontTest(0),
    resetButton(0), // added JG
    openSearchTabs(0) // added JG
{
    addAndMakeVisible(label0 = new Label("new label", TRANS("Font:"))); // Added v.5.6.31
    label0->setFont(Font(14.00f)); // Added v.5.6.31
    label0->setJustificationType(Justification::centredLeft); // Added v.5.6.31
    label0->setEditable(false, false, false); // Added v.5.6.31

    addAndMakeVisible(fontTypeface = new ComboBox(""));
    fontTypeface->setEditableText(false);
    fontTypeface->setJustificationType(Justification::centredLeft);
    //fontTypeface->setTextWhenNothingSelected ("");
    fontTypeface->setTextWhenNothingSelected("<Monospaced>"); // Will set default type from Font class
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

    addAndMakeVisible(resetToPreviousButton = new TextButton("Reset Font"));
    resetToPreviousButton->addListener(this);
    resetToPreviousButton->setColour(TextButton::buttonColourId, findColour(TextButton::buttonColourId));

    addAndMakeVisible(label1 = new Label("new label", TRANS("Editor background:"))); // Added v.5.6.31
    label1->setFont(Font(14.00f)); // Added v.5.6.31
    label1->setJustificationType(Justification::centredLeft); // Added v.5.6.31
    label1->setEditable(false, false, false); // Added v.5.6.31

    addAndMakeVisible(label2 = new Label("new label", TRANS("Line numbers background:"))); // Added v.5.6.31
    label2->setFont(Font(14.00f)); // Added v.5.6.31
    label2->setJustificationType(Justification::centredLeft); // Added v.5.6.31
    label2->setEditable(false, false, false); // Added v.5.6.31

    addAndMakeVisible(label3 = new Label("new label", TRANS("Line numbers:"))); // Added v.5.6.31
    label3->setFont(Font(14.00f)); // Added v.5.6.31
    label3->setJustificationType(Justification::centredLeft); // Added v.5.6.31
    label3->setEditable(false, false, false); // Added v.5.6.31

    addAndMakeVisible(fontTest = new CodeEditorComponent(codeDocument, &luaTokeniser));

    addAndMakeVisible(openSearchTabs = new ToggleButton(""));
    //openSearchTabs->setButtonText("Open Search Tabs"); // Corrected to use a string literal
    openSearchTabs->getToggleStateValue().referTo(SharedValues::getSearchTabsValue());
    openSearchTabs->setButtonText(SharedValues::getSearchTabsLabel());
    openSearchTabs->getToggleStateValue().referTo(SharedValues::getSearchTabsValue());

    addAndMakeVisible(bgColour = new ColourComboBox("bgColour"));
    bgColour->setEditableText(false);
    bgColour->setJustificationType(Justification::centredLeft);
    bgColour->addListener(this);

    addAndMakeVisible(lineNumbersBgColour = new ColourComboBox("lineNumbersBgColour"));
    lineNumbersBgColour->setEditableText(false);
    lineNumbersBgColour->setJustificationType(Justification::centredLeft);
    lineNumbersBgColour->addListener(this);

    addAndMakeVisible(lineNumbersColour = new ColourComboBox("lineNumbersColour"));
    lineNumbersColour->setEditableText(false);
    lineNumbersColour->setJustificationType(Justification::centredLeft);
    lineNumbersColour->addListener(this);

    // Now that the combo boxes exist, populate them
    populateColourComboWithThumbnails(static_cast<ColourComboBox*>(bgColour));
    populateColourComboWithThumbnails(static_cast<ColourComboBox*>(lineNumbersBgColour));
    populateColourComboWithThumbnails(static_cast<ColourComboBox*>(lineNumbersColour));

    addAndMakeVisible(syntaxLabel = new Label("syntaxLabel", TRANS("Syntax Highlighting:")));
    syntaxLabel->setFont(Font(14.00f));
    syntaxLabel->setJustificationType(Justification::centredLeft);
    syntaxLabel->setEditable(false, false, false);

    addAndMakeVisible(syntaxTokenType = new ComboBox("syntaxTokenType"));
    syntaxTokenType->setEditableText(false);
    syntaxTokenType->setJustificationType(Justification::centredLeft);
    syntaxTokenType->addListener(this);

    addAndMakeVisible(syntaxTokenColor = new ColourComboBox("syntaxTokenColor"));
    syntaxTokenColor->setEditableText(false);
    syntaxTokenColor->setJustificationType(Justification::centredLeft);
    syntaxTokenColor->addListener(this);

    addAndMakeVisible(resetButton = new TextButton("Reset")); // Added JG
    resetButton->addListener(this);
    resetButton->setColour(TextButton::buttonColourId, findColour(TextButton::buttonColourId)); // Will follow the main LnF
    resetButton->setColour(TextButton::buttonOnColourId, findColour(TextButton::buttonOnColourId)); // Will follow the main LnF
    resetButton->setColour(TextButton::textColourOffId, findColour(TextButton::textColourOffId)); // Will follow the main LnF
    resetButton->setColour(TextButton::textColourOnId, findColour(TextButton::textColourOnId)); // Will follow the main LnF

    //    addAndMakeVisible(cancelButton = new TextButton("CANCEL"));
    //    cancelButton->addListener(this);
    //    cancelButton->setColour(TextButton::buttonColourId, findColour(TextButton::buttonColourId));

    addAndMakeVisible(applyButton = new TextButton("Apply"));
    applyButton->addListener(this);
    applyButton->setColour(TextButton::buttonColourId, findColour(TextButton::buttonColourId));

    populateSyntaxTokenCombo();
    populateColourCombo(syntaxTokenColor);

    // Set a default/fallback colour for the syntax token combo box.
    // We'll use your existing findColourIndex() function to find the ID of the 'Red' colour.
    const int defaultColourID = findColourIndex(Colours::red);
    syntaxTokenColor->setSelectedId(defaultColourID, dontSendNotification);

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

    codeFont = owner.getOwner().getCtrlrManagerOwner().getFontManager().getFontFromString(owner.getComponentTree().getProperty(Ids::luaMethodEditorFont, owner.getOwner().getCtrlrManagerOwner().getFontManager().getStringFromFont(Font("<Monospaced>", 14.0f, Font::plain))));

    label1->setColour(TextEditor::textColourId, findColour(TextEditor::textColourId));
    label1->setColour(TextEditor::backgroundColourId, Colour(0x00000000));
    label2->setColour(TextEditor::textColourId, findColour(TextEditor::textColourId));
    label2->setColour(TextEditor::backgroundColourId, Colour(0x00000000));
    label3->setColour(TextEditor::textColourId, findColour(TextEditor::textColourId));
    label3->setColour(TextEditor::backgroundColourId, Colour(0x00000000));

    Colour defaultBgColour = VAR2COLOUR(owner.getComponentTree().getProperty(Ids::luaMethodEditorBgColour, Colours::white.toString()));
    bgColour->setSelectedId(findColourIndex(defaultBgColour), dontSendNotification);

    Colour defaultLineNumBgColour = VAR2COLOUR(owner.getComponentTree().getProperty(Ids::luaMethodEditorLineNumbersBgColour, Colours::cornflowerblue.toString()));

    Colour defaultLineNumColour = VAR2COLOUR(owner.getComponentTree().getProperty(Ids::luaMethodEditorLineNumbersColour, Colours::black.toString()));

    lineNumbersColour->setSelectedId(findColourIndex(defaultLineNumColour), dontSendNotification);
    lineNumbersBgColour->setSelectedId(findColourIndex(defaultLineNumBgColour), dontSendNotification);

    fontSize->setValue(codeFont.getHeight(), dontSendNotification);
    fontBold->setToggleState(codeFont.isBold(), dontSendNotification);
    fontItalic->setToggleState(codeFont.isItalic(), dontSendNotification);

    owner.getOwner().getCtrlrManagerOwner().getFontManager().fillCombo(*fontTypeface);
    fontTypeface->setText(codeFont.getTypefaceName(), sendNotification);
    codeDocument.replaceAllContent("-- This is a comment\nfunction myFunction(argument)\n\tcall(\"string\")\nend");

    previousFont = getFont(); // This captures the initial loaded font
    resetToPreviousButton->setEnabled(false); // Start disabled

    originalFont = getFont();
    originalBgColour = getBgColour();
    originalLineNumbersBgColour = getLineNumbersBgColour();
    originalLineNumbersColour = getLineNumbersColour();
    originalOpenSearchTabs = openSearchTabs->getToggleState();

    setSize(334, 520);
    updateSyntaxColors();
}

CtrlrLuaMethodCodeEditorSettings::~CtrlrLuaMethodCodeEditorSettings()
{
    deleteAndZero(fontTest);

    //deleteAndZero(label0);
    deleteAndZero(fontTypeface);
    deleteAndZero(fontBold);
    deleteAndZero(fontItalic);
    deleteAndZero(resetToPreviousButton);
    deleteAndZero(fontSize);

    //deleteAndZero(label1);
    deleteAndZero(bgColour);
    //deleteAndZero(label2);
    deleteAndZero(lineNumbersBgColour);
    //deleteAndZero(label3);
    deleteAndZero(lineNumbersColour);

    //deleteAndZero(syntaxLabel);
    deleteAndZero(syntaxTokenType);
    deleteAndZero(syntaxTokenColor);

    deleteAndZero(openSearchTabs);

    deleteAndZero(resetButton);
    // deleteAndZero(cancelButton);
    deleteAndZero(applyButton);
}

void CtrlrLuaMethodCodeEditorSettings::paint(Graphics& g)
{
    // Update the main window's background colour based on the current Look and Feel
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    // Draw horizontal separator line
    int syntaxY = marginTop + (sampleHeight + 24 + 72 + 3 * 24 + 2 * 32) + 40;
    int lineY = syntaxY + 96; // Just above the buttons

    // g.setColour(findColour(GroupComponent::outlineColourId));
    // g.drawHorizontalLine(lineY, marginLeft, marginLeft + sampleWidth);
}

void CtrlrLuaMethodCodeEditorSettings::resized()
{
    marginLeft = 12;
    marginTop = 12;
    sampleWidth = 310;
    sampleHeight = 78;

    if (fontTest != nullptr)
        fontTest->setBounds(marginLeft, marginTop, sampleWidth, sampleHeight);

    // Font type
    label0->setBounds(marginLeft - 4, marginTop + sampleHeight + 8, sampleWidth, 24);
    fontTypeface->setBounds(marginLeft, marginTop + sampleHeight + 24 + 8, sampleWidth, 24);

    //Font style
    fontBold->setBounds(marginLeft, marginTop + sampleHeight + 24 + 40, 56, 24);
    fontItalic->setBounds(marginLeft + 64, marginTop + sampleHeight + 24 + 40, 64, 24);

    // Font reset
    resetToPreviousButton->setBounds(marginLeft + 136, marginTop + sampleHeight + 24 + 40, 80, 24);

    // Font size
    fontSize->setBounds(marginLeft + 224, marginTop + sampleHeight + 24 + 40, 88, 24);

    // Editor background
    label1->setBounds(marginLeft - 4, marginTop + sampleHeight + 24 + 72, sampleWidth, 24);
    bgColour->setBounds(marginLeft, marginTop + sampleHeight + 24 + 72 + 24, sampleWidth, 24);

    // Line numbers background
    label2->setBounds(marginLeft - 4, marginTop + sampleHeight + 24 + 72 + 24 + 32, sampleWidth, 24);
    lineNumbersBgColour->setBounds(marginLeft, marginTop + sampleHeight + 24 + 72 + 2 * 24 + 32, sampleWidth, 24);

    // Line numbers
    label3->setBounds(marginLeft - 4, marginTop + sampleHeight + 24 + 72 + 2 * 24 + 2 * 32, sampleWidth, 24);
    lineNumbersColour->setBounds(marginLeft, marginTop + sampleHeight + 24 + 72 + 3 * 24 + 2 * 32, sampleWidth, 24);

    // Highlights
    int syntaxY = marginTop + (sampleHeight + 24 + 72 + 3 * 24 + 2 * 32) + 40;
    syntaxLabel->setBounds(marginLeft - 4, syntaxY, sampleWidth, 24);
    syntaxTokenType->setBounds(marginLeft, syntaxY + 24, (sampleWidth - 8) / 2, 24);
    syntaxTokenColor->setBounds(marginLeft + (sampleWidth - 8) / 2 + 8, syntaxY + 24, (sampleWidth - 8) / 2, 24);

    // Open search tab  check box
    openSearchTabs->setBounds(marginLeft + 0, syntaxY + 64, sampleWidth, 24);

    // Add horizontal line above buttons
    int buttonY = syntaxY + 104;

    // Position the three buttons in a row: RESET  APPLY  CANCEL
    int buttonWidth = (sampleWidth - 16) / 2; // Account for spacing between buttons
    resetButton->setBounds(marginLeft, buttonY, (sampleWidth - 8) / 2, 42); // 42px high is the default JUCE buttons height
    // cancelButton->setBounds(marginLeft + buttonWidth + 8, buttonY, buttonWidth, 24);
    applyButton->setBounds(marginLeft + (sampleWidth - 8) / 2 + 8, buttonY, (sampleWidth - 8) / 2, 42);
}

void CtrlrLuaMethodCodeEditorSettings::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == fontTypeface)
    {
        String newTypefaceName = fontTypeface->getText();
        String currentTypefaceName = previousFont.getTypefaceName();

        _DBG("Font combo changed from: " + currentTypefaceName + " to: " + newTypefaceName);

        // Only store previous font if this is a real user change (not initial setup)
        if (newTypefaceName != currentTypefaceName && resetToPreviousButton != nullptr)
        {
            // This is a real change - the previous font is what we had before
            resetToPreviousButton->setEnabled(true);
            _DBG("Real font change detected. Previous font stored: " + previousFont.getTypefaceName());
        }

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
    if (buttonThatWasClicked == resetToPreviousButton)
    {
        _DBG("Resetting to previous font settings");
        _DBG(String("Current font: ") + fontTypeface->getText());
        _DBG(String("Previous font to restore: ") + previousFont.getTypefaceName());

        if (previousFont.getTypefaceName().isNotEmpty())
        {
            // Create font object from current UI state BEFORE changing it
            Font currentUIFont = Font(fontTypeface->getText(),
                fontSize->getValue(),
                (fontBold->getToggleState() ? Font::bold : 0) |
                (fontItalic->getToggleState() ? Font::italic : 0));

            // Apply the previous font settings
            fontTypeface->setText(previousFont.getTypefaceName(), dontSendNotification);
            fontSize->setValue(previousFont.getHeight(), dontSendNotification);
            fontBold->setToggleState(previousFont.isBold(), dontSendNotification);
            fontItalic->setToggleState(previousFont.isItalic(), dontSendNotification);

            // Now swap: current becomes previous for next reset
            previousFont = currentUIFont;

            _DBG(String("Font restored. New previous font: ") + previousFont.getTypefaceName());

            changeListenerCallback(nullptr);
        }
    }
    else if (buttonThatWasClicked == applyButton)
    {
        applySettings();
        closeWindow(); // Added to apply and close settings window
    }
    //	else if (buttonThatWasClicked == cancelButton)
    //	{
    //		closeWindow();
    //	}
    else if (buttonThatWasClicked == resetButton)
    {
        int result = AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon,
            "Reset Editor",
            "Reset Editor to default?"
        );

        if (result == 1)
        {
            // Reset to defaults
            // fontTypeface->setText(getDefaultFont(), dontSendNotification);
            fontTypeface->setText("<Monospaced>", dontSendNotification); // "Courrier New" ???
            fontBold->setToggleState(false, dontSendNotification);
            fontItalic->setToggleState(false, dontSendNotification);
            openSearchTabs->setToggleState(false, dontSendNotification);
            fontSize->setValue(14.0f, dontSendNotification);
            bgColour->setSelectedId(findColourIndex(Colours::white), dontSendNotification);
            lineNumbersBgColour->setSelectedId(findColourIndex(Colours::cornflowerblue), dontSendNotification);
            lineNumbersColour->setSelectedId(findColourIndex(Colours::black), dontSendNotification);

            customSyntaxColors.clear();
            clearSyntaxColorSettings();
            String currentToken = getCurrentSelectedTokenType();
            updateTokenColorDisplay(currentToken);
            updateSyntaxColors();

            previousFont = getFont();
            resetToPreviousButton->setEnabled(true);
            changeListenerCallback(nullptr);
            closeWindow(); // Added to apply and close settings window
        }
        else if (buttonThatWasClicked == fontBold || buttonThatWasClicked == fontItalic)
        {
            // For style changes, also enable reset and store previous
            if (!resetToPreviousButton->isEnabled())
            {
                previousFont = getFont(); // Store current before style change
                resetToPreviousButton->setEnabled(true);
            }
        }
        else if (buttonThatWasClicked == openSearchTabs)
        {
            bool currentState = openSearchTabs->getToggleState();
            owner.setOpenSearchTabsEnabled(currentState);
            owner.getComponentTree().setProperty(Ids::openSearchTabsState, currentState, nullptr);
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

void CtrlrLuaMethodCodeEditorSettings::changeListenerCallback(ChangeBroadcaster* source)
{
    if (fontTest)
    {
        // Only update the preview - don't save anything
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

void CtrlrLuaMethodCodeEditorSettings::populateColourCombo(ColourComboBox* combo) {
    combo->clear();
    for (int i = 0; i < sizeof(availableColours) / sizeof(availableColours[0]); ++i) {
        combo->addColourItem(availableColours[i].name, availableColours[i].colour, i + 1);
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
    static CodeEditorComponent::ColourScheme sharedScheme = CtrlrLuaCodeTokeniser().getDefaultColourScheme(); // FIXED
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
void CtrlrLuaMethodCodeEditorSettings::clearSyntaxColorSettings()
{
    // Remove all saved syntax color settings
    StringArray tokenTypes = CtrlrLuaCodeTokeniser::getTokenTypeNames();

    for (int i = 0; i < tokenTypes.size(); ++i)
    {
        const String& tokenType = tokenTypes[i];
        String settingKey = "syntaxColor_" + tokenType;
        owner.getComponentTree().removeProperty(settingKey, nullptr);
        DBG("Cleared setting: " + settingKey);
    }
}

void CtrlrLuaMethodCodeEditorSettings::updateTokenColorDisplay(const String& tokenType)
{
    // Show default color for the token type
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

    Colour defaultColor = Colours::black; // fallback
    for (int i = 0; i < 10; ++i)
    {
        if (tokenType == defaultTypes[i].name)
        {
            defaultColor = Colour(defaultTypes[i].colour);
            break;
        }
    }
    syntaxTokenColor->setSelectedId(findColourIndex(defaultColor), dontSendNotification);
}
void CtrlrLuaMethodCodeEditorSettings::populateColourComboWithThumbnails(ColourComboBox* combo)
{
    combo->clear();
    for (int i = 0; i < sizeof(availableColours) / sizeof(availableColours[0]); ++i)
    {
        combo->addColourItem(availableColours[i].name, availableColours[i].colour, i + 1);
    }
}
bool CtrlrLuaMethodCodeEditorSettings::hasUnsavedChanges() const
{
    // Font comparison
    Font currentFont = const_cast<CtrlrLuaMethodCodeEditorSettings*>(this)->getFont();
    if (currentFont.getTypefaceName() != originalFont.getTypefaceName()) return true;
    if (currentFont.getHeight() != originalFont.getHeight()) return true;
    if (currentFont.isBold() != originalFont.isBold()) return true;
    if (currentFont.isItalic() != originalFont.isItalic()) return true;

    // Color comparisons
    Colour currentBg = const_cast<CtrlrLuaMethodCodeEditorSettings*>(this)->getBgColour();
    if (currentBg != originalBgColour) return true;

    Colour currentLineNumBg = const_cast<CtrlrLuaMethodCodeEditorSettings*>(this)->getLineNumbersBgColour();
    if (currentLineNumBg != originalLineNumbersBgColour) return true;

    Colour currentLineNum = const_cast<CtrlrLuaMethodCodeEditorSettings*>(this)->getLineNumbersColour();
    if (currentLineNum != originalLineNumbersColour) return true;

    // Toggle comparison
    if (openSearchTabs->getToggleState() != originalOpenSearchTabs) return true;

    return false;
}

void CtrlrLuaMethodCodeEditorSettings::markAsChanged()
{
    hasChanges = true;
}


void CtrlrLuaMethodCodeEditorSettings::markAsSaved()
{
    hasChanges = false;
    // Update original values
    originalFont = getFont();
    originalBgColour = getBgColour();
    originalLineNumbersBgColour = getLineNumbersBgColour();
    originalLineNumbersColour = getLineNumbersColour();
    originalOpenSearchTabs = openSearchTabs->getToggleState();
}

bool CtrlrLuaMethodCodeEditorSettings::promptToSaveChanges()
{
    if (!hasUnsavedChanges())
        return true; // No changes, safe to close

    int result = AlertWindow::showYesNoCancelBox(
        AlertWindow::QuestionIcon,
        "Unsaved Changes",
        "You have unsaved changes. Do you want to apply them before closing?",
        "Apply & Close",
        "Close Without Saving",
        "Cancel"
    );

    switch (result)
    {
    case 1: // Apply & Close
        applySettings();
        return true;
    case 2: // Close Without Saving
        return true;
    case 0: // Cancel
    default:
        return false;
    }
}

void CtrlrLuaMethodCodeEditorSettings::applySettings()
{
    // Save all settings (existing save logic from the close callback)
    owner.getComponentTree().setProperty(Ids::luaMethodEditorFont,
        owner.getOwner().getCtrlrManagerOwner().getFontManager().getStringFromFont(getFont()), nullptr);

    owner.getComponentTree().setProperty(Ids::luaMethodEditorBgColour,
        getBgColour().toString(), nullptr);

    owner.getComponentTree().setProperty(Ids::luaMethodEditorLineNumbersBgColour,
        getLineNumbersBgColour().toString(), nullptr);

    owner.getComponentTree().setProperty(Ids::luaMethodEditorLineNumbersColour,
        getLineNumbersColour().toString(), nullptr);

    owner.getComponentTree().setProperty(Ids::openSearchTabsState,
        openSearchTabs->getToggleState(), nullptr);

    // Save syntax colors
    saveSyntaxColorsToSettings();

    // Apply to current editor
    updateSyntaxColors();

    // Mark as saved
    markAsSaved();

    // Trigger a repaint/refresh of the editor
    if (CtrlrLuaMethodCodeEditor* currentEditor = owner.getCurrentEditor())
    {
        if (currentEditor->getCodeComponent())
        {
            currentEditor->getCodeComponent()->repaint();
        }
    }
}
void CtrlrLuaMethodCodeEditorSettings::closeWindow()
{
    // Find the parent window and close it
    if (DialogWindow* parentWindow = findParentComponentOfClass<DialogWindow>())
    {
        parentWindow->closeButtonPressed();
    }
    else if (DocumentWindow* parentWindow = findParentComponentOfClass<DocumentWindow>())
    {
        parentWindow->closeButtonPressed();
    }
}