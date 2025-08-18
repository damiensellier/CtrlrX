#include "stdafx.h"
#include "CtrlrEditor.h"
#include "CtrlrLuaManager.h" // Added v5.6.34.
#include "CtrlrAbout.h"
#include "CtrlrPanel/CtrlrPanel.h"
#include "CtrlrPanel/CtrlrPanelEditor.h"
#include "CtrlrLua/MethodEditor/CtrlrLuaMethodEditor.h" // Added v5.6.34.
#include "CtrlrLua/MethodEditor/CtrlrLuaMethodCodeEditorSettings.h" // Added v5.6.34.
#include "CtrlrWindowManagers/CtrlrDialogWindow.h"
#include "CtrlrMIDI/CtrlrMIDISettingsDialog.h"
#include "CtrlrMIDI/CtrlrMIDISettingsDialog.h"
#include "CtrlrLua/MethodEditor/CtrlrLuaMethodEditorCommandIDs.h" // Added v5.6.34.

void CtrlrEditor::performLuaEditorCommand(const int commandID) // Added v5.6.34. Declare the functions to call depending on the LUA method editor menu item selected from the GUI.
{
    _DBG("performLuaEditorCommand called with ID: " + String(commandID));

    if (auto* activePanel = getActivePanel())
    {
        _DBG("getActivePanel() is valid.");
        auto& winManager = activePanel->getPanelWindowManager();

        if (auto* luaMethodEditor = (CtrlrLuaMethodEditor*)winManager.getContent(CtrlrPanelWindowManager::LuaMethodEditor))
        {
            _DBG("luaMethodEditor is valid.");

            if (commandID == LuaMethodEditorCommandIDs::fileSave)
            {
                if (auto* editor = luaMethodEditor->getCurrentEditor())
                    editor->saveDocument();
            }
            else if (commandID == LuaMethodEditorCommandIDs::fileSaveAndCompile)
            {
                if (auto* editor = luaMethodEditor->getCurrentEditor())
                    editor->saveAndCompileDocument();
            }
            else if (commandID == LuaMethodEditorCommandIDs::fileSaveAndCompileAll)
            {
                luaMethodEditor->saveAndCompilAllMethods();
            }
            else if (commandID == LuaMethodEditorCommandIDs::fileCloseCurrentTab)
            {
                luaMethodEditor->closeCurrentTab();
            }
            else if (commandID == LuaMethodEditorCommandIDs::fileCloseAllTabs)
            {
                luaMethodEditor->closeAllTabs();
            }
            else if (commandID == LuaMethodEditorCommandIDs::fileConvertToFiles)
            {
                luaMethodEditor->convertToFiles();
            }
            else if (commandID == LuaMethodEditorCommandIDs::fileClose)
            {
                winManager.toggle(CtrlrPanelWindowManager::LuaMethodEditor, false);
            }
            else if (commandID == LuaMethodEditorCommandIDs::editSearch)
            {
                if (auto* editor = luaMethodEditor->getCurrentEditor())
                {
                    if (auto* codeComponent = editor->getCodeComponent())
                    {
                        codeComponent->showFindPanel(false);
                    }
                }
            }
            else if (commandID == LuaMethodEditorCommandIDs::editFindAndReplace)
            {
                if (auto* editor = luaMethodEditor->getCurrentEditor())
                {
                    if (auto* codeComponent = editor->getCodeComponent())
                    {
                        codeComponent->showFindPanel(true);
                    }
                }
            }
            else if (commandID == LuaMethodEditorCommandIDs::editDebugger)
            {
                luaMethodEditor->getMethodEditArea()->showDebuggerTab();
            }
            else if (commandID == LuaMethodEditorCommandIDs::editConsole)
            {
                luaMethodEditor->getMethodEditArea()->showConsoleTab();
            }
            else if (commandID == LuaMethodEditorCommandIDs::editClearOutput)
            {
                luaMethodEditor->getMethodEditArea()->clearOutputText();
            }
            else if (commandID == LuaMethodEditorCommandIDs::editPreferences)
            {
                CtrlrLuaMethodCodeEditorSettings s(*luaMethodEditor, SharedValues::getSearchTabsValue());
                
                _DBG("Attempting to show modal dialog.");

                CtrlrDialogWindow::showModalDialog ("Code editor preferences", &s, false, luaMethodEditor);
                
                _DBG("Modal dialog returned.");

                if (activePanel)
                {
                    auto& manager = activePanel->getCtrlrManagerOwner();

                    luaMethodEditor->getComponentTree().setProperty (Ids::luaMethodEditorFont, manager.getFontManager().getStringFromFont (s.getFont()), nullptr);
                    luaMethodEditor->getComponentTree().setProperty (Ids::luaMethodEditorBgColour, COLOUR2STR (s.getBgColour()), nullptr);
                    luaMethodEditor->getComponentTree().setProperty (Ids::luaMethodEditorLineNumbersBgColour, COLOUR2STR(s.getLineNumbersBgColour()), nullptr);
                    luaMethodEditor->getComponentTree().setProperty (Ids::luaMethodEditorLineNumbersColour, COLOUR2STR(s.getLineNumbersColour()), nullptr);
                    luaMethodEditor->updateTabs();
                }
            }
            else if (commandID == LuaMethodEditorCommandIDs::editSingleLineComment)
            {
                if (auto* editor = luaMethodEditor->getCurrentEditor())
                    editor->toggleLineComment();
            }
            else if (commandID == LuaMethodEditorCommandIDs::editMultiLineComment)
            {
                if (auto* editor = luaMethodEditor->getCurrentEditor())
                    editor->toggleLongLineComment();
            }
            else if (commandID == LuaMethodEditorCommandIDs::editDuplicateLine)
            {
                if (auto* editor = luaMethodEditor->getCurrentEditor())
                    editor->duplicateCurrentLine();
            }
            else if (commandID == LuaMethodEditorCommandIDs::editGoToLine)
            {
                if (auto* editor = luaMethodEditor->getCurrentEditor())
                    if (auto* codeComponent = editor->getCodeComponent())
                        codeComponent->showGoTOPanel();
            }
        }
        else
        {
            _DBG("luaMethodEditor is a nullptr.");
        }
    }
    else
    {
        _DBG("activePanel is a nullptr.");
    }
}

bool CtrlrEditor::perform (const InvocationInfo &info) // Updated v5.6.34. Will now perform the different switch case function for all menu including the LUA method editor menu items.
{
    _DBG("perform called with commandID: " + juce::String(info.commandID));
    
    if (Time::getCurrentTime().toMilliseconds() - lastCommandInvocationMillis < 20)
    {
        return (true);
    }

    lastCommandInvocationMillis = Time::getCurrentTime().toMilliseconds();

    // Check for the new Lua editor commands using the corrected range.
    // This is the dispatch point for all the shortcuts we added.
    // Correct range check for the LuaMethodEditorCommandIDs enum
    if (info.commandID >= LuaMethodEditorCommandIDs::fileClose && info.commandID <= LuaMethodEditorCommandIDs::editPreferences)
    {
        performLuaEditorCommand(info.commandID);
        return true;
    }

    // Existing command dispatch logic.
    switch (info.commandID)
    {
        case CtrlrEditor::showKeyboardMappingDialog:
            performShowKeyboardMappingDialog(info.commandID);
            break;

        case CtrlrEditor::showMidiMonitor:
            owner.getWindowManager().toggle(CtrlrManagerWindowManager::MidiMonWindow, true);
            break;

        case CtrlrEditor::showLogViewer:
            owner.getWindowManager().toggle(CtrlrManagerWindowManager::LogViewer, true);
            break;

        case CtrlrEditor::showMidiCalculator:
            owner.getWindowManager().toggle(CtrlrManagerWindowManager::MIDICalculator, true);
            break;

        case CtrlrEditor::doSaveState:
            owner.saveStateToDisk();
            break;

        case CtrlrEditor::doOpenPanel:
            owner.openPanelFromFile(nullptr);
            break;

        case CtrlrEditor::doNewPanel:
            owner.addPanel(ValueTree());
            break;

        case CtrlrEditor::doRefreshPropertyLists:
            if (isPanelActive())
            {
                if (getActivePanel()->getEditor(false))
                {
                    CtrlrPanelProperties *props = getActivePanel()->getEditor(false)->getPropertiesPanel();
                    if (props)
                    {
                        props->refreshAll();
                    }
                }
            }
            break;

        case CtrlrEditor::showGlobalSettingsDialog:
            owner.getWindowManager().showModalDialog ("Ctrlr/Settings", ScopedPointer <CtrlrSettings> (new CtrlrSettings(owner)), true, this);
            break;

        case CtrlrEditor::showAboutDialog:
        {
            CtrlrAbout* aboutWindow = new CtrlrAbout(owner);
            owner.getWindowManager().showModalDialog ("Ctrlr/About", aboutWindow, false, this);
        }
        break;

        case CtrlrEditor::doZoomIn:
            if (getActivePanelEditor())
            {
                double newZoomFactor = (double)getActivePanelEditor()->getProperty(Ids::uiPanelZoom) + 0.1;
                if (newZoomFactor < MINZOOM || newZoomFactor > MAXZOOM)
                    return true;
                getActivePanelEditor()->setProperty(Ids::uiPanelZoom, newZoomFactor);
            }
            break;

        case CtrlrEditor::doZoomOut:
            if (getActivePanelEditor())
            {
                double newZoomFactor = (double)getActivePanelEditor()->getProperty(Ids::uiPanelZoom) - 0.1;
                if (newZoomFactor < MINZOOM || newZoomFactor >MAXZOOM)
                    return true;
                getActivePanelEditor()->setProperty(Ids::uiPanelZoom, newZoomFactor);
            }
            break;

        case CtrlrEditor::doCopy:
            getActivePanel()->getCanvas()->copy();
            break;

        case CtrlrEditor::doCut:
            getActivePanel()->getCanvas()->cut();
            break;

        case CtrlrEditor::doPaste:
            getActivePanel()->getCanvas()->paste();
            break;

        case CtrlrEditor::doUndo:
            getActivePanel()->undo();
            break;

        case CtrlrEditor::doRedo:
            getActivePanel()->redo();
            break;

        case CtrlrEditor::doSearchForProperty:
            getActivePanelEditor()->searchForProperty();
            break;

        case CtrlrEditor::doSave:
            getActivePanel()->savePanel();
            break;

        case CtrlrEditor::doClose:
            if (getActivePanel()->canClose(true))
            {
                owner.removePanel(getActivePanelEditor());
            }
            break;

        case CtrlrEditor::doSaveAs:
            getActivePanel()->savePanelAs(doExportFileText);
            break;

        case CtrlrEditor::doSaveVersioned:
            getActivePanel()->savePanelVersioned();
            break;

        case CtrlrEditor::doExportFileText:
        case CtrlrEditor::doExportFileZText:
        case CtrlrEditor::doExportFileBin:
        case CtrlrEditor::doExportFileZBin:
        case CtrlrEditor::doExportFileZBinRes:
        case CtrlrEditor::doExportFileInstance:
        case CtrlrEditor::doExportGenerateUID:
        case CtrlrEditor::doExportFileInstanceRestricted:
            getActivePanel()->savePanelAs (info.commandID);
            break;

        case CtrlrEditor::doViewPropertyDisplayIDs:
            if (getActivePanel()) getActivePanel()->setProperty (Ids::panelPropertyDisplayIDs, !getActivePanel()->getProperty(Ids::panelPropertyDisplayIDs));
            break;

        case CtrlrEditor::doPanelMode:
            DBG("doPanelMode");
            if (getActivePanelEditor()) getActivePanelEditor()->setProperty (Ids::uiPanelEditMode, !getActivePanelEditor()->getProperty(Ids::uiPanelEditMode));
            break;

        case CtrlrEditor::doPanelLock:
            if (getActivePanelEditor()) getActivePanelEditor()->setProperty (Ids::uiPanelLock, !(bool)getActivePanelEditor()->getProperty(Ids::uiPanelLock));
            break;

        case CtrlrEditor::doPanelDisableCombosOnEdit:
            if (getActivePanelEditor()) getActivePanelEditor()->setProperty (Ids::uiPanelDisableCombosOnEdit, !(bool)getActivePanelEditor()->getProperty(Ids::uiPanelDisableCombosOnEdit));
            break;

        case CtrlrEditor::showLuaEditor:
            if (getActivePanel()) getActivePanel()->getPanelWindowManager().toggle (CtrlrPanelWindowManager::LuaMethodEditor, true);
            break;

        case CtrlrEditor::doRefreshDeviceList:
            performMidiDeviceRefresh();
            break;

        case CtrlrEditor::showLuaConsole:
            if (getActivePanel()) getActivePanel()->getPanelWindowManager().toggle (CtrlrPanelWindowManager::LuaConsole, true);
            break;

        case CtrlrEditor::showComparatorTables:
            if (getActivePanel()) getActivePanel()->dumpComparatorTables();
            break;

        case CtrlrEditor::showModulatorList:
            if (getActivePanel()) getActivePanel()->getPanelWindowManager().toggle (CtrlrPanelWindowManager::ModulatorList, true);
            break;

        case CtrlrEditor::showLayers:
            if (getActivePanel()) getActivePanel()->getPanelWindowManager().toggle (CtrlrPanelWindowManager::LayerEditor, true);
            break;

        case CtrlrEditor::doSendSnapshot:
            if (getActivePanel()) getActivePanel()->sendSnapshot();
            break;

        case CtrlrEditor::doShowMidiSettingsDialog:
            if (getActivePanel()) getActivePanel()->getPanelWindowManager().toggle (CtrlrPanelWindowManager::MIDISettings, true);
            break;

        case CtrlrEditor::optMidiInputFromHost:
        case CtrlrEditor::optMidiInputFromHostCompare:
        case CtrlrEditor::optMidiOutuptToHost:
            performMidiHostOptionChange(info.commandID);
            break;

        case CtrlrEditor::optMidiSnapshotOnLoad:
        case CtrlrEditor::optMidiSnapshotOnProgramChange:
            performMidiOptionChange(info.commandID);
            break;

        case CtrlrEditor::optMidiThruD2D:
        case CtrlrEditor::optMidiThruD2H:
        case CtrlrEditor::optMidiThruH2D:
        case CtrlrEditor::optMidiThruH2H:
        case CtrlrEditor::optMidiThruD2DChannelize:
        case CtrlrEditor::optMidiThruD2HChannelize:
        case CtrlrEditor::optMidiThruH2DChannelize:
        case CtrlrEditor::optMidiThruH2HChannelize:
            performMidiThruChange(info.commandID);
            break;

        case CtrlrEditor::doCrash:
            invalidCtrlrPtr->cancelPendingUpdate();
            break;

        case CtrlrEditor::doDumpVstTables:
            owner.getVstManager().dumpDebugData();
            if (isPanelActive())
            {
                getActivePanel()->dumpDebugData();
            }
            break;

        case CtrlrEditor::doQuit:
            if (owner.canCloseWindow())
            {
                JUCEApplication::quit();
            }
            break;

        case CtrlrEditor::doRegisterExtension:
            tempResult = owner.getNativeObject().registerFileHandler();
            if (tempResult.wasOk())
            {
                INFO("Register file handler", "Registration successful");
            }
            else
            {
                WARN("Registration failed");
            }
            break;

        default:
            break;
    }

    return (true);
}

void CtrlrEditor::performRecentFileOpen(const int menuItemID)
{
	File f = getRecentOpenedFilesList() [menuItemID - 0x9000];

	if (f.existsAsFile())
	{
		owner.addPanel (CtrlrPanel::openPanel (f), true);
	}
}

void CtrlrEditor::performShowKeyboardMappingDialog(const int menuItemID)
{
	ScopedPointer <KeyMappingEditorComponent> keys (new KeyMappingEditorComponent (*owner.getCommandManager().getKeyMappings(), true));
	owner.getWindowManager().showModalDialog ("Keyboard mapping", keys, true, this);

	ScopedPointer <XmlElement> keysXml (owner.getCommandManager().getKeyMappings()->createXml (true).release());

	if (keysXml)
	{
		owner.setProperty (Ids::ctrlrKeyboardMapping, keysXml->createDocument(""));
	}
}

void CtrlrEditor::performMidiChannelChange(const int menuItemID)
{
	if (isPanelActive())
	{
		if (menuItemID >= 0x6100 && menuItemID <= 0x6111)
		{
			getActivePanel()->setProperty (Ids::panelMidiInputChannelDevice, menuItemID - 0x6100);
		}
		else if (menuItemID >= 0x6200 && menuItemID <= 0x6211)
		{
			getActivePanel()->setProperty (Ids::panelMidiOutputChannelDevice, menuItemID - 0x6200);
		}
		else if (menuItemID >= 0x6300 && menuItemID <= 0x6311)
		{
			getActivePanel()->setProperty (Ids::panelMidiControllerChannelDevice, menuItemID - 0x6300);
		}
		else if (menuItemID >= 0x6400 && menuItemID <= 0x6411)
		{
			getActivePanel()->setProperty (Ids::panelMidiInputChannelHost, menuItemID - 0x6400);
		}
		else if (menuItemID >= 0x6500 && menuItemID <= 0x6511)
		{
			getActivePanel()->setProperty (Ids::panelMidiOutputChannelHost, menuItemID - 0x6500);
		}
	}
}

void CtrlrEditor::performMidiDeviceChange(const int menuItemID)
{
	if (isPanelActive())
	{
		if (menuItemID > MENU_OFFSET_MIDI_DEV_IN && menuItemID < MENU_OFFSET_MIDI_DEV_OUT)
		{
			getActivePanel()->setProperty(Ids::panelMidiInputDevice, owner.getCtrlrMidiDeviceManager().getDeviceName(menuItemID - MENU_OFFSET_MIDI_DEV_IN - 2, inputDevice));
		}
		else if (menuItemID > MENU_OFFSET_MIDI_DEV_OUT && menuItemID < MENU_OFFSET_MIDI_DEV_CTRLR)
		{
			getActivePanel()->setProperty(Ids::panelMidiOutputDevice, owner.getCtrlrMidiDeviceManager().getDeviceName(menuItemID - MENU_OFFSET_MIDI_DEV_OUT - 2, outputDevice));
		}
		else if (menuItemID > MENU_OFFSET_MIDI_DEV_CTRLR && menuItemID < MENU_OFFSET_MIDI_HOST_IN)
		{
			getActivePanel()->setProperty(Ids::panelMidiControllerDevice, owner.getCtrlrMidiDeviceManager().getDeviceName(menuItemID - MENU_OFFSET_MIDI_DEV_CTRLR - 2, controllerDevice));
		}
	}
}

void CtrlrEditor::performMidiThruChange(const int menuItemID)
{
	const Identifier optionId = CtrlrPanel::getMidiOptionIdentifier((const CtrlrPanelMidiOption)(menuItemID - MENU_OFFSET_MIDI));

	if (isPanelActive() && optionId != Ids::null)
	{
		getActivePanel()->setProperty (optionId, !getActivePanel()->getProperty(optionId));
	}
}

void CtrlrEditor::performMidiOptionChange(const int menuItemID)
{
	if (menuItemID == optMidiSnapshotOnLoad)
	{
		if (isPanelActive())
		{
			getActivePanel()->setProperty (Ids::panelMidiSnapshotAfterLoad, !getPanelProperty(Ids::panelMidiSnapshotAfterLoad));
		}
	}
	else if (menuItemID == optMidiSnapshotOnProgramChange)
	{
		if (isPanelActive())
		{
			getActivePanel()->setProperty (Ids::panelMidiSnapshotAfterProgramChange, !getPanelProperty(Ids::panelMidiSnapshotAfterProgramChange));
		}
	}
}

void CtrlrEditor::performProgramChange(const int menuItemID)
{
	if (isPanelActive())
	{
		// getActivePanel()->getCtrlrMIDILibrary().setProgram (menuItemID);
	}
}

void CtrlrEditor::performMidiHostOptionChange(const int menuItemID)
{
	if (isPanelActive())
	{
		CtrlrPanel *p = getActivePanel();

		switch (menuItemID)
		{
			case optMidiInputFromHost:
				p->setProperty(Ids::panelMidiInputFromHost, !(bool)p->getProperty(Ids::panelMidiInputFromHost));
				break;
			case optMidiInputFromHostCompare:
				p->setProperty(Ids::panelMidiInputFromHostCompare, !(bool)p->getProperty(Ids::panelMidiInputFromHostCompare));
				break;
			case optMidiOutuptToHost:
				p->setProperty(Ids::panelMidiOutputToHost, !(bool)p->getProperty(Ids::panelMidiOutputToHost));
				break;
		}

		//getActivePanel()->setMidiOptionBool ((const CtrlrPanelMidiOption)(menuItemID - MENU_OFFSET_MIDI), !getActivePanel()->getMidiOptionBool((const CtrlrPanelMidiOption)(menuItemID - MENU_OFFSET_MIDI)));
	}
}

void CtrlrEditor::sliderValueChanged (Slider* slider)
{
	if (slider->getName() == "Snapshot delay")
	{
		if (isPanelActive())
		{
			getActivePanel()->setProperty(Ids::panelMidiSnapshotDelay, slider->getValue());
		}
	}
}

void CtrlrEditor::performMidiDeviceRefresh()
{
	owner.getCtrlrMidiDeviceManager().refreshDevices();
}
