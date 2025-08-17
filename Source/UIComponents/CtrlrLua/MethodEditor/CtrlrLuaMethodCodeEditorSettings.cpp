#include "stdafx.h"
#include "CtrlrLuaMethodEditor.h"
#include "CtrlrPanel/CtrlrPanel.h"
#include "CtrlrManager/CtrlrManager.h"
#include "CtrlrLuaMethodCodeEditorSettings.h"

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


CtrlrLuaMethodCodeEditorSettings::CtrlrLuaMethodCodeEditorSettings (CtrlrLuaMethodEditor &_owner, juce::Value& sharedSearchTabsValue_)
    : owner(_owner), sharedSearchTabsValue(sharedSearchTabsValue_),
      fontTypeface (0),
      fontBold (0),
      // fontUnderline (0),
      fontItalic (0),
      fontSize (0),
      bgColour (0), // Added v5.6.31
      lineNumbersBgColour(0), // Added v5.6.31
      lineNumbersColour(0), // Added v5.6.31
      fontTest (0),
      resetButton (0), // added JG
      openSearchTabs (0) // added JG
{
    addAndMakeVisible(label0 = new Label("new label", TRANS("Font:"))); // Added v.5.6.31
    label0->setFont(Font(14.00f)); // Added v.5.6.31
    label0->setJustificationType(Justification::centredLeft); // Added v.5.6.31
    label0->setEditable(false, false, false); // Added v.5.6.31
    
    addAndMakeVisible (fontTypeface = new ComboBox (""));
    fontTypeface->setEditableText (false);
    fontTypeface->setJustificationType (Justification::centredLeft);
	//fontTypeface->setTextWhenNothingSelected ("");
	fontTypeface->setTextWhenNothingSelected ("<Monospaced>"); // Will set default type from Font class
    fontTypeface->setTextWhenNoChoicesAvailable (L"(no choices)");
    fontTypeface->addListener (this);

    addAndMakeVisible (fontBold = new ToggleButton (""));
    fontBold->setButtonText (L"Bold");
    fontBold->addListener (this);
	
    addAndMakeVisible (fontItalic = new ToggleButton (""));
    fontItalic->setButtonText (L"Italic");
    fontItalic->addListener (this);

    addAndMakeVisible (fontSize = new Slider (""));
    fontSize->setRange (0, 128, 1);
    fontSize->setSliderStyle (Slider::IncDecButtons);
    fontSize->setTextBoxStyle (Slider::TextBoxLeft, false, 32, 24);
    fontSize->addListener (this);
    
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

    addAndMakeVisible (fontTest = new CodeEditorComponent (codeDocument, &luaTokeniser));

    addAndMakeVisible(openSearchTabs = new ToggleButton(""));
    openSearchTabs->setButtonText(SharedValues::getSearchTabsLabel());
	// openSearchTabs->getToggleStateValue().referTo(SharedValues::getSearchTabsValue()); // This line is likely the source of the crash, as SharedValues might not be ready yet. Let's comment it out.
	
    addAndMakeVisible(resetButton = new TextButton("RESET")); // Added JG
    resetButton->addListener(this);
    resetButton->setColour(TextButton::buttonColourId, findColour(TextButton::buttonColourId)); // Will follow the main LnF
	resetButton->setColour(TextButton::buttonOnColourId, findColour(TextButton::buttonOnColourId)); // Will follow the main LnF
	resetButton->setColour(TextButton::textColourOffId, findColour(TextButton::textColourOffId)); // Will follow the main LnF
	resetButton->setColour(TextButton::textColourOnId, findColour(TextButton::textColourOnId)); // Will follow the main LnF

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


    // codeFont = owner.getOwner().getCtrlrManagerOwner().getFontManager().getFontFromString(owner.getComponentTree().getProperty(Ids::luaMethodEditorFont, owner.getOwner().getCtrlrManagerOwner().getFontManager().getStringFromFont(Font(owner.getOwner().getCtrlrManagerOwner().getFontManager().getDefaultMonoFontName(), 14.0f, Font::plain))));
	
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
    lineNumbersBgColour->setSelectedId(findColourIndex(defaultLineNumBgColour), dontSendNotification);
	
    Colour defaultLineNumColour = VAR2COLOUR(owner.getComponentTree().getProperty(Ids::luaMethodEditorLineNumbersColour, Colours::black.toString()));
    lineNumbersColour->setSelectedId(findColourIndex(defaultLineNumColour), dontSendNotification);
	
    fontSize->setValue(codeFont.getHeight(), dontSendNotification);
    fontBold->setToggleState(codeFont.isBold(), dontSendNotification);
    fontItalic->setToggleState(codeFont.isItalic(), dontSendNotification);
	
    owner.getOwner().getCtrlrManagerOwner().getFontManager().fillCombo(*fontTypeface);
    fontTypeface->setText(codeFont.getTypefaceName(), sendNotification);
    codeDocument.replaceAllContent("-- This is a comment\nfunction myFunction(argument)\n\tcall(\"string\")\nend");
    previousFont = getFont(); // This captures the initial loaded font
    resetToPreviousButton->setEnabled(false); // Start disabled

    setSize(334, 464);
}

CtrlrLuaMethodCodeEditorSettings::~CtrlrLuaMethodCodeEditorSettings()
{
    deleteAndZero (label0);
	deleteAndZero (label1);
	deleteAndZero (label2);
	deleteAndZero (label3);
    deleteAndZero (fontTypeface);
    deleteAndZero (fontBold);
    deleteAndZero (fontItalic);
    deleteAndZero (fontSize);
    deleteAndZero (bgColour);
    deleteAndZero (lineNumbersBgColour);
    deleteAndZero (lineNumbersColour);
    deleteAndZero (fontTest);
    deleteAndZero (openSearchTabs);
    deleteAndZero (resetButton);
}

void CtrlrLuaMethodCodeEditorSettings::paint (Graphics& g)
{
	// Update the main window's background colour based on the current Look and Feel
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

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
    resetToPreviousButton->setBounds(marginLeft + 136, marginTop + sampleHeight + 24 + 40, 80, 24);
    label1->setBounds(marginLeft - 4, marginTop + sampleHeight + 24 + 72, sampleWidth, 24);
    bgColour->setBounds(marginLeft, marginTop + sampleHeight + 24 + 72 + 24, sampleWidth - 40, 24);
	
    label2->setBounds(marginLeft - 4, marginTop + sampleHeight + 24 + 72 + 24 + 32, sampleWidth, 24);
    lineNumbersBgColour->setBounds(marginLeft, marginTop + sampleHeight + 24 + 72 + 2 * 24 + 32, sampleWidth - 40, 24);
	
    label3->setBounds(marginLeft - 4, marginTop + sampleHeight + 24 + 72 + 2 * 24 + 2 * 32, sampleWidth, 24);
    lineNumbersColour->setBounds(marginLeft, marginTop + sampleHeight + 24 + 72 + 3 * 24 + 2 * 32, sampleWidth - 40, 24);
    openSearchTabs->setBounds(marginLeft + 0, marginTop + (sampleHeight + 24 + 72 + 3 * 24 + 2 * 32) + 40, sampleWidth, 24);
    
	resetButton->setBounds(marginLeft + sampleWidth / 2 - (sampleWidth / 4 + marginLeft / 2),
						   marginTop + (sampleHeight + 24 + 72 + 3 * 24 + 2 * 32) + 80,
						   sampleWidth / 2,
						   40);
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
        else if (buttonThatWasClicked == fontBold || buttonThatWasClicked == fontItalic)
        {
            // For style changes, store previous state if not already stored
            if (!resetToPreviousButton->isEnabled())
            {
                resetToPreviousButton->setEnabled(true);
            }
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
            // Store current font before resetting everything

            fontTypeface->setText("<Monospaced>", dontSendNotification);
            fontBold->setToggleState(false, dontSendNotification);
            fontItalic->setToggleState(false, dontSendNotification);
            openSearchTabs->setToggleState(false, dontSendNotification);
            fontSize->setValue(14.0f, dontSendNotification);
            bgColour->setSelectedId(findColourIndex(Colours::white), dontSendNotification);
            lineNumbersBgColour->setSelectedId(findColourIndex(Colours::cornflowerblue), dontSendNotification);
            lineNumbersColour->setSelectedId(findColourIndex(Colours::black), dontSendNotification);
            previousFont = getFont();
            // Enable reset button so user can undo the reset
            resetToPreviousButton->setEnabled(true);

            changeListenerCallback(nullptr);
        }
    }

    changeListenerCallback(nullptr);
}

void CtrlrLuaMethodCodeEditorSettings::sliderValueChanged(Slider* sliderThatWasMoved)
{
    if (sliderThatWasMoved == fontSize)
    {
        // For font size changes, enable reset if not already enabled
        if (!resetToPreviousButton->isEnabled())
        {
            // Store font before size change (but with old size)
            previousFont = Font(fontTypeface->getText(),
                previousFont.getHeight(),  // Keep the old size
                (fontBold->getToggleState() ? Font::bold : 0) |
                (fontItalic->getToggleState() ? Font::italic : 0));
            resetToPreviousButton->setEnabled(true);
        }
    }
    changeListenerCallback(nullptr);
}

void CtrlrLuaMethodCodeEditorSettings::changeListenerCallback (ChangeBroadcaster* source)
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
    Font font = owner.getOwner().getCtrlrManagerOwner().getFontManager().getFont (fontTypeface->getSelectedItemIndex());

    if (fontTypeface)
        font.setTypefaceName (fontTypeface->getText());
    else
        return (font);

    font.setHeight (fontSize->getValue());
    font.setBold (fontBold->getToggleState());
    font.setItalic (fontItalic->getToggleState());
    // font.setUnderline (fontUnderline->getToggleState());
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
