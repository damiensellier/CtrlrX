#include "stdafx.h"
#include "CtrlrLuaCodeTokeniser.h"

/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

This is an automatically generated file created by the Jucer!

Creation date : 27 Oct 2012 8 : 17 : 44pm

Be careful when adding custom code to these files, as only the code within
the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
and re - saved.

Jucer version : 1.12

------------------------------------------------------------------------------

The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
Copyright 2004 - 6 by Raw Material Software ltd.

==============================================================================
*/

#ifndef __JUCER_HEADER_CTRLRLUAMETHODCODEEDITORSETTINGS_CTRLRLUAMETHODCODEEDITORSETTINGS_FC2CDFB3__
#define __JUCER_HEADER_CTRLRLUAMETHODCODEEDITORSETTINGS_CTRLRLUAMETHODCODEEDITORSETTINGS_FC2CDFB3__

//[Headers]       -- You can add your own extra header files here --
#include "Methods/CtrlrLuaMethod.h"
#include "CtrlrTextEditor.h"
#include "CtrlrWindowManagers/CtrlrChildWindowContent.h"
#include "CtrlrWindowManagers/CtrlrPanelWindowManager.h"
#include "CtrlrLuaCodeTokeniser.h"
#include "CtrlrPropertyEditors/CtrlrPropertyComponent.h"
class CtrlrLuaMethodEditor;
//[/Headers]



//==============================================================================
/**
                                                                              //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                              //[/Comments]
*/
class CtrlrLuaMethodCodeEditorSettings : public Component,
    public ChangeListener,
    public ComboBox::Listener,
    public Button::Listener,
    public Slider::Listener
{
public:
    //==============================================================================
    CtrlrLuaMethodCodeEditorSettings(CtrlrLuaMethodEditor& _owner, juce::Value& sharedSearchTabsValue_);
    ~CtrlrLuaMethodCodeEditorSettings();

    //==============================================================================
    //[UserMethods]   -- You can add your own custom methods in this section.
    void changeListenerCallback(ChangeBroadcaster* source);
    const Font getFont();
    const Colour getBgColour();
    const Colour getLineNumbersBgColour();
    const Colour getLineNumbersColour();

    // Moved these from local functions inside the constructor
    void populateColourCombo(ComboBox* combo);
    int findColourIndex(const Colour& colour);
    Colour getColourFromCombo(ComboBox* combo);
    //[/UserMethods]
    void clearSyntaxColorSettings();
    void updateTokenColorDisplay(const String& tokenType);
    void paint(Graphics& g);
    void resized();
    void comboBoxChanged(ComboBox* comboBoxThatHasChanged);
    void buttonClicked(Button* buttonThatWasClicked);
    void sliderValueChanged(Slider* sliderThatWasMoved);
    void loadSyntaxColorsFromSettings();
    void saveSyntaxColorsToSettings();
    void populateSyntaxTokenCombo();
    void updateSyntaxColors();
    String getCurrentSelectedTokenType();
    
private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    struct ColourItem {
        String name;
        Colour colour;
    };
    static const ColourItem availableColours[];

    CtrlrLuaCodeTokeniser luaTokeniser;
    CodeDocument codeDocument;
    CtrlrLuaMethodEditor& owner;
    Font codeFont;
    int marginLeft;
    int marginTop;
    int sampleWidth;
    int sampleHeight;
    //[/UserVariables]

    //==============================================================================
    Label* label0;
    ComboBox* fontTypeface;
    ComboBox* bgColour;
    ComboBox* lineNumbersBgColour;
    ComboBox* lineNumbersColour;
    ComboBox* syntaxTokenType;
    ComboBox* syntaxTokenColor;

    ToggleButton* fontBold;
    ToggleButton* fontItalic;
    ToggleButton* openSearchTabs;
    TextButton* resetButton;
    Slider* fontSize;
    Label* label1;
    Label* label2;
    Label* label3;
    Label* syntaxLabel;
    CodeEditorComponent* fontTest;


    static CodeEditorComponent::ColourScheme& getSharedScheme();


    //std::unique_ptr<ComboBox> syntaxTokenType;
    //std::unique_ptr<ComboBox> syntaxTokenColor;
    //std::unique_ptr<Label> syntaxLabel;
    // Storage for custom syntax colors
    HashMap<String, Colour> customSyntaxColors;
    juce::Value& sharedSearchTabsValue;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CtrlrLuaMethodCodeEditorSettings);
};
//void CtrlrLuaMethodCodeEditorSettings::updateSyntaxColors() {
//    // Update the shared scheme
//    SharedColorScheme::updateScheme(customSyntaxColors);
//
//    // Get reference to the shared scheme (no copying!)
//    CodeEditorComponent::ColourScheme& sharedScheme = SharedColorScheme::getCurrentScheme();
//
//    // Update preview
//    fontTest->setColourScheme(sharedScheme);
//
//    // Update main editor with the same shared reference
//    GenericCodeEditorComponent* mainEditor = owner.getEditorComponent();
//    if (mainEditor) {
//        mainEditor->setColourScheme(sharedScheme);
//        mainEditor->repaint();
//    }
//}

#endif    // __JUCER_HEADER_CTRLRLUAMETHODCODEEDITORSETTINGS_CTRLRLUAMETHODCODEEDITORSETTINGS_FC2CDFB3__