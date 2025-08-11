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

#ifndef __JUCER_HEADER_CTRLRLUAMETHODCODEEDITORSETTINGS_CTRLRLUAMETHODCODEEDITORSETTINGS_FC2CDFB3__
#define __JUCER_HEADER_CTRLRLUAMETHODCODEEDITORSETTINGS_CTRLRLUAMETHODCODEEDITORSETTINGS_FC2CDFB3__

//[Headers]     -- You can add your own extra header files here --
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
    //[UserMethods]     -- You can add your own custom methods in this section.
    void changeListenerCallback(ChangeBroadcaster* source);
    const Font getFont();

    // Public getter methods for colors
    const Colour getBgColour();
    const Colour getFontColour();
    const Colour getLineNumbersBgColour();
    const Colour getLineNumbersColour();
    //[/UserMethods]

    void paint(Graphics& g);
    void resized();
    void comboBoxChanged(ComboBox* comboBoxThatHasChanged);
    void buttonClicked(Button* buttonThatWasClicked);
    void sliderValueChanged(Slider* sliderThatWasMoved);
    void CtrlrLuaMethodCodeEditorSettings::updateEditorColours();
    // Add this struct to store color information
    struct ColorItem {
        String name;
        Colour color;
    };

private:
    //[UserVariables]   -- You can add your own custom variables in this section.

    CtrlrLuaCodeTokeniser luaTokeniser;
    CodeEditorComponent::ColourScheme activeScheme;
    bool isSchemeInitialized = false;
    CodeDocument codeDocument;
    CtrlrLuaMethodEditor& owner;
    Font codeFont;
    int marginLeft; // Added v5.6.31
    int marginTop;
    int sampleWidth;
    int sampleHeight;

    // UI Components
    Label* label0; // Added v5.6.31
    Label* label1; // Added v5.6.31
    Label* label2; // Added v5.6.31
    Label* label3; // Added v5.6.31
    Label* label4; // Font color label

    ComboBox* fontTypeface;
    ComboBox* fontColour;       
    ComboBox* bgColour;          
    ComboBox* lineNumbersBgColour;
    ComboBox* lineNumbersColour;  
    ToggleButton* fontBold;
    ToggleButton* fontUnderline;
    ToggleButton* fontItalic;
    ToggleButton* openSearchTabs;
    TextButton* resetButton; // added JG
    Slider* fontSize;

    CodeEditorComponent* fontTest;

    /*
    Alternative to CtrlrColourEditorComponent -
    mostly because setting bgColour to any alpha below 1.0
    shows garbled editor text
    so it can never be used anyway except FFxxxxxx
    */
    static const ColorItem availableColors[];
    void populateColorCombo(ComboBox* combo);
    int findColorIndex(const Colour& color);
    Colour getColorFromCombo(ComboBox* combo);

    juce::Value& sharedSearchTabsValue;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CtrlrLuaMethodCodeEditorSettings);
};

#endif   // __JUCER_HEADER_CTRLRLUAMETHODCODEEDITORSETTINGS_CTRLRLUAMETHODCODEEDITORSETTINGS_FC2CDFB3__