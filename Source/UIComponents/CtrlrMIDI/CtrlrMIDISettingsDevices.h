/*
  ==============================================================================

  This is an automatically generated GUI class created by the Introjucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Introjucer version: 4.0.2

  ------------------------------------------------------------------------------

  The Introjucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright (c) 2015 - ROLI Ltd.

  ==============================================================================
*/

#ifndef __JUCE_HEADER_76E483A2275D56C__
#define __JUCE_HEADER_76E483A2275D56C__

//[Headers]     -- You can add your own extra header files here --
#include "CtrlrMacros.h"
class CtrlrPanel;
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class CtrlrMIDISettingsDevices  : public Component,
									public ComboBox::Listener,
									public Label::Listener,
									public Button::Listener
{
public:
    //==============================================================================
    CtrlrMIDISettingsDevices (CtrlrPanel &_owner);
    ~CtrlrMIDISettingsDevices();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);
    void labelTextChanged (Label* labelThatHasChanged);
    void buttonClicked (Button* buttonThatWasClicked);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    CtrlrPanel &owner;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<ComboBox> inputDevices;
    std::unique_ptr<ComboBox> controllerDevices;
    std::unique_ptr<ComboBox> outputDevices;
    std::unique_ptr<Label> label;
    std::unique_ptr<Label> label2;
    std::unique_ptr<Label> label3;
    std::unique_ptr<ComboBox> oscProtocol;
    std::unique_ptr<Label> label4;
    std::unique_ptr<Label> oscPort;
    std::unique_ptr<Label> label5;
    std::unique_ptr<Label> label6;
    std::unique_ptr<ToggleButton> oscEnabled;
    std::unique_ptr<Label> label7;
    std::unique_ptr<ComboBox> inputChannel;
    std::unique_ptr<ComboBox> controllerChannel;
    std::unique_ptr<ComboBox> outputChannel;
    std::unique_ptr<Label> label8;
    std::unique_ptr<ComboBox> pluginOutputChannel;
    std::unique_ptr<ToggleButton> pluginOutput;
    std::unique_ptr<Label> label9;
    std::unique_ptr<ToggleButton> pluginInput;
    std::unique_ptr<ToggleButton> pluginInputToHostComp;
    std::unique_ptr<ComboBox> pluginInputChannel;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CtrlrMIDISettingsDevices)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_76E483A2275D56C__
