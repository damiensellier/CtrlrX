/*
  ==============================================================================

  This is an automatically generated GUI class created by the Introjucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Introjucer version: 3.2.0

  ------------------------------------------------------------------------------

  The Introjucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright (c) 2015 - ROLI Ltd.

  ==============================================================================
*/

#ifndef __JUCE_HEADER_38C6DBC920B693D4__
#define __JUCE_HEADER_38C6DBC920B693D4__

//[Headers]     -- You can add your own extra header files here --
#include "CtrlrManager/CtrlrManager.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class CtrlrAbout  : public Component,
					public Button::Listener
{
public:
    //==============================================================================
    CtrlrAbout (CtrlrManager &_owner);
    ~CtrlrAbout();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void addVersionInfo (const String &componentName, const String &componentVersion);
	void updateVersionLabel();
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);


private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	CtrlrManager &owner;
	StringPairArray versionInformationArray;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<Label> ctrlrName;
    std::unique_ptr<Label> ctrlrxVersionLabel;
    std::unique_ptr<Label> ctrlrxReleaseDateLabel;
    std::unique_ptr<Label> ctrlrxLibsVersionLabel;
    std::unique_ptr<Label> label;
    std::unique_ptr<Label> label2;
    std::unique_ptr<Label> labelDonate;
    std::unique_ptr<Label> labelAuthorEmail;
    std::unique_ptr<Label> label3;
    std::unique_ptr<Label> label4;
    std::unique_ptr<Label> instanceVersion;
    std::unique_ptr<Label> instanceAuthor;
    std::unique_ptr<Label> instanceName;
    
    std::unique_ptr<DrawableButton> ctrlrLogo;
    std::unique_ptr<DrawableButton> vst3AuJuceLogo;
    std::unique_ptr<DrawableButton> githubLogo;
    std::unique_ptr<DrawableButton> donateLogo;
        
    std::unique_ptr<HyperlinkButton> ctrlrxUrl;
    std::unique_ptr<HyperlinkButton> ctrlrxDonateUrl;
    std::unique_ptr<HyperlinkButton> instanceUrl;
    std::unique_ptr<HyperlinkButton> instanceAuthorDonateUrl;
    std::unique_ptr<HyperlinkButton> instanceAuthorEmail;
    
    std::unique_ptr<TextEditor> versionInfoLabel;
    std::unique_ptr<TextEditor> creditsLabel;
    std::unique_ptr<TextEditor> descriptionLabel;
    std::unique_ptr<TextEditor> copyrightLabel;
    std::unique_ptr<TextEditor> instanceDescription;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CtrlrAbout)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_38C6DBC920B693D4__
