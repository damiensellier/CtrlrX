#include "stdafx.h"
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

    void paint(Graphics& g);
    void resized();
    void comboBoxChanged(ComboBox* comboBoxThatHasChanged);
    void buttonClicked(Button* buttonThatWasClicked);
    void sliderValueChanged(Slider* sliderThatWasMoved);


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
    ToggleButton* fontBold;
    ToggleButton* fontItalic;
    ToggleButton* openSearchTabs;
    TextButton* resetButton;
    Slider* fontSize;
    Label* label1;
    Label* label2;
    Label* label3;
    CodeEditorComponent* fontTest;
    juce::Value& sharedSearchTabsValue;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CtrlrLuaMethodCodeEditorSettings);
};


#endif    // __JUCER_HEADER_CTRLRLUAMETHODCODEEDITORSETTINGS_CTRLRLUAMETHODCODEEDITORSETTINGS_FC2CDFB3__