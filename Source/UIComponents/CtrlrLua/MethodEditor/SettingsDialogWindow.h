/*
  ==============================================================================

    SettingsDialogWindow.h
    Created: 18 Aug 2025 9:59:10am
    Author:  zan64

  ==============================================================================
*/

#pragma once    
#include <juce_gui_basics/juce_gui_basics.h>
#include <CtrlrLog.h>

using namespace juce;

class SettingsDialogWindow : public DialogWindow
{
//public:
//    SettingsDialogWindow(CtrlrLuaMethodEditor& editor) 
//        : DialogWindow("Code Editor Settings", Colours::lightgrey, true)
//    {
//        settingsComponent = new CtrlrLuaMethodCodeEditorSettings(editor, SharedValues::getSearchTabsValue());
//        setContentOwned(settingsComponent, true);
//        setResizable(false, false);
//        centreAroundComponent(nullptr, getWidth(), getHeight());
//    }
//
//    void closeButtonPressed() override
//    {
//        if (settingsComponent && settingsComponent->promptToSaveChanges())
//        {
//            DialogWindow::closeButtonPressed();
//        }
//        // If promptToSaveChanges() returns false, don't close
//    }
//
//private:
//    CtrlrLuaMethodCodeEditorSettings* settingsComponent;
};