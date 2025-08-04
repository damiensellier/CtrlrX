#include "stdafx.h"
#include "CtrlrEditor.h"
#include "CtrlrAbout.h"
#include "CtrlrPanel/CtrlrPanel.h"
#include "CtrlrPanel/CtrlrPanelEditor.h"
#include "ctrlrEditorApplicationcommandsHandlers.cpp" // Added v5.6.34.


void CtrlrEditor::getAllCommands (Array< CommandID > &commands)
{
	const CommandID ids[] = {	doSaveState,
                                doOpenPanel,
                                doNewPanel,
                                showGlobalSettingsDialog,
                                showMidiMonitor,
                                showLogViewer,
                                showMidiCalculator,
                                showAboutDialog,
								showKeyboardMappingDialog,
								doViewPropertyDisplayIDs,
								doZoomIn,
								doZoomOut,
								doCopy,
								doCut,
								doPaste,
								doUndo,
								doRedo,
								doSave,
								doClose,
								doSaveAs,
								doSaveVersioned,
								doPanelMode,
								doPanelLock,
								doPanelDisableCombosOnEdit,
								doSendSnapshot,
								doRefreshDeviceList,
								showLuaEditor,
								showLuaConsole,
								showComparatorTables,
								showModulatorList,
								showBufferEditor,
								showLayers,
								doRefreshPropertyLists,
								doExportFileText,
								doExportFileZText,
								doExportFileBin,
								doExportFileZBin,
								doExportFileZBinRes,
								doExportFileInstance,
								doExportFileInstanceRestricted,
								doExportGenerateUID,
                                doSearchForProperty,
								doShowMidiSettingsDialog,
								optMidiInputFromHost,
								optMidiInputFromHostCompare,
								optMidiOutuptToHost,
								optMidiSnapshotOnLoad,
								optMidiSnapshotOnProgramChange,
								optMidiThruD2D,
								optMidiThruD2H,
								optMidiThruH2D,
								optMidiThruH2H,
								optMidiThruH2HChannelize,
								optMidiThruH2DChannelize,
								optMidiThruD2DChannelize,
								optMidiThruD2HChannelize,
								doSnapshotStore,
								doCrash,
								doDumpVstTables,
								doRegisterExtension,
								doKeyGenerator,
								doProgramWizard,
								doQuit
							};

    // This adds the first array of commands
    commands.addArray (ids, numElementsInArray (ids));

    // This is the correct, more efficient way to add your Lua commands
    const CommandID luaIds[] =
    {
        LuaMethodEditorCommandIDs::fileSave,
        LuaMethodEditorCommandIDs::fileSaveAndCompile,
        LuaMethodEditorCommandIDs::fileSaveAndCompileAll,
        LuaMethodEditorCommandIDs::fileCloseCurrentTab,
        LuaMethodEditorCommandIDs::fileCloseAllTabs,
        LuaMethodEditorCommandIDs::fileConvertToFiles,
        LuaMethodEditorCommandIDs::fileClose,
        LuaMethodEditorCommandIDs::editSearch,
        LuaMethodEditorCommandIDs::editFindAndReplace,
        LuaMethodEditorCommandIDs::editDebugger,
        LuaMethodEditorCommandIDs::editConsole,
        LuaMethodEditorCommandIDs::editClearOutput,
        LuaMethodEditorCommandIDs::editPreferences,
        LuaMethodEditorCommandIDs::editSingleLineComment,
        LuaMethodEditorCommandIDs::editMultiLineComment,
        LuaMethodEditorCommandIDs::editDuplicateLine,
        LuaMethodEditorCommandIDs::editGoToLine
    };

    // Now, add the array of Lua editor commands using addArray
    commands.addArray(luaIds, numElementsInArray(luaIds));
}

void CtrlrEditor::getCommandInfo (CommandID commandID, ApplicationCommandInfo &result)
{
	const String globalCategory ("Global");
	const String panelCategory ("Panel");
    const String luaCategory ("Lua Editor"); // defines the new Lua Editor category of commandIDs
    
	switch (commandID)
	{
		case doSaveState:
			result.setInfo ("Save CTRLR state", "Saves the CTRLR state to disk", globalCategory, 0);
            if (!isRestricted()) // Added v5.6.32. Disable shortcut on restricted instance
            {
                result.addDefaultKeypress ('s', ModifierKeys::commandModifier | ModifierKeys::altModifier);
            }
			break;

		case doOpenPanel:
			result.setInfo ("Open Panel", "Open a panel from a file", globalCategory, 0);
            if (!isRestricted()) // Added v5.6.32. Disable shortcut on restricted instance
            {
                result.addDefaultKeypress ('o', ModifierKeys::commandModifier);
            }
			break;

		case doNewPanel:
			result.setInfo ("New Panel", "Create a new empty panel", globalCategory, 0);
            if (!isRestricted()) // Added v5.6.32. Disable shortcut on restricted instance
            {
                result.addDefaultKeypress ('n', ModifierKeys::commandModifier);
            }
			break;

		case showGlobalSettingsDialog:
			result.setInfo ("Preferences", "Show global CTRLR preferences", globalCategory, 0);
            if (!isRestricted()) // Added v5.6.32. Disable shortcut on restricted instance
            {
                result.addDefaultKeypress ('p', ModifierKeys::commandModifier);
            }
			break;

		case showMidiMonitor:
			result.setInfo ("MIDI Monitor", "A small MIDI monitor that will display received and sent data", globalCategory, 0);
			result.addDefaultKeypress ('m', ModifierKeys::commandModifier);
			break;

		case showLogViewer:
			result.setInfo ("Log viewer", "You can view diagnostic messages here, useful when debugging problems", globalCategory, 0);
			result.addDefaultKeypress ('l', ModifierKeys::commandModifier);
			break;

		case showMidiCalculator:
			result.setInfo ("MIDI Calculator", "A useful tool to translate Heximal, Binary, Decimal values", globalCategory, 0);
			result.addDefaultKeypress ('j', ModifierKeys::commandModifier);
			break;

		case showAboutDialog:
			result.setInfo ("About", "About CTRLR", globalCategory, 0);
			result.addDefaultKeypress ('a', ModifierKeys::commandModifier);
			break;

		case showKeyboardMappingDialog:
			result.setInfo ("Keyboard mapping", "Change default keyboard mappings", globalCategory, 0);
            if (!isRestricted()) // Added v5.6.32. Disable shortcut on restricted instance
            {
                result.addDefaultKeypress ('k', ModifierKeys::commandModifier);
            }
			break;

		case doViewPropertyDisplayIDs:
			result.setInfo ("Property IDs/Names", "View property names or property IDs", panelCategory, 0);
			result.setTicked (isPanelActive() ? (bool)getActivePanel()->getProperty(Ids::panelPropertyDisplayIDs) : false);
			result.setActive (isPanelActive());
			break;

		case doZoomIn:
			result.setInfo ("Zoom In", "Zoom in the panel", panelCategory, 0);
			result.addDefaultKeypress ('+', ModifierKeys::commandModifier);
			result.setActive (isPanelActive());
			break;

		case doZoomOut:
			result.setInfo ("Zoom Out", "Zoom out the panel", panelCategory, 0);
			result.addDefaultKeypress ('-', ModifierKeys::commandModifier);
			result.setActive (isPanelActive());
			break;

		case doCopy:
			result.setInfo ("Copy", "Copy selected components to clipboard", panelCategory, 0);
			result.addDefaultKeypress ('c', ModifierKeys::commandModifier);
			result.setActive (isPanelActive(true));
			break;

		case doCut:
			result.setInfo ("Cut", "Cut selected components to clipboard", panelCategory, 0);
			result.addDefaultKeypress ('x', ModifierKeys::commandModifier);
			result.setActive (isPanelActive(true));
			break;

		case doPaste:
			result.setInfo ("Paste", "Paste components from clipboard", panelCategory, 0);
			result.addDefaultKeypress ('v', ModifierKeys::commandModifier);
			result.setActive (isPanelActive(true));
			break;

		case doUndo:
			result.setInfo ("Undo", "Undo last transaction", panelCategory, 0);
			result.addDefaultKeypress ('z', ModifierKeys::commandModifier);
			result.setActive (isPanelActive(true));
			break;

		case doRedo:
			result.setInfo ("Redo", "Redo last transaction", panelCategory, 0);
			result.addDefaultKeypress ('y', ModifierKeys::commandModifier);
			result.setActive (isPanelActive(true));
			break;
	    case doSearchForProperty:
            result.setInfo ("Search for property", "Search for any property in the current panel or selected component", panelCategory, 0);
            if (!isRestricted()) // Added v5.6.32. Disable shortcut on restricted instance
            {
                result.addDefaultKeypress ('f', ModifierKeys::commandModifier);
            }
            result.setActive (isPanelActive(true));
            break;

		case doSave:
			result.setInfo ("Save", "Save panel to a file", panelCategory, 0);
            if (!isRestricted()) // Added v5.6.32. Disable shortcut on restricted instance
            {
                 result.addDefaultKeypress ('s', ModifierKeys::commandModifier);
            }
			result.setActive (isPanelActive(true));
			break;

		case doClose:
			result.setInfo ("Close", "Close the current panel", panelCategory, 0);
			result.setActive (isPanelActive(true));
			break;

		case doSaveAs:
			result.setInfo ("Save As", "Save panel as a new file", panelCategory, 0);
            if (!isRestricted()) // Added v5.6.32. Disable shortcut on restricted instance
            {
                result.addDefaultKeypress ('s', ModifierKeys::commandModifier | ModifierKeys::shiftModifier);
            }
			result.setActive (isPanelActive(true));
			break;

		case doSaveVersioned:
			result.setInfo ("Save versioned", "Save panel to a new versioned file", panelCategory, 0);
            if (!isRestricted()) // Added v5.6.32. Disable shortcut on restricted instance
            {
                result.addDefaultKeypress ('s', ModifierKeys::altModifier | ModifierKeys::shiftModifier);
            }
			result.setActive (isPanelActive(true));
			break;

		case doPanelMode:
			result.setInfo ("Panel mode", "Switches panel from and to EDIT mode", panelCategory, 0);
            if (!isRestricted()) // Added v5.6.32. Disable shortcut on restricted instance
            {
                result.addDefaultKeypress ('e', ModifierKeys::commandModifier);
            }
			result.setActive (isPanelActive(true));
			break;

		case doPanelLock:
			result.setInfo ("Panel lock", "Locks components in edit mode", panelCategory, 0);
            if (!isRestricted()) // Added v5.6.32. Disable shortcut on restricted instance
            {
                result.addDefaultKeypress ('l', ModifierKeys::commandModifier);
            }
			result.setActive (isPanelActive(true));
			if (isPanelActive(true))
			{
				result.setTicked ((bool)getActivePanelEditor()->getProperty(Ids::uiPanelLock));
			}
			break;

		case doPanelDisableCombosOnEdit:
			result.setInfo ("Disable combos on edit", "Combo boxes will not open when editing panel", panelCategory, 0);
			result.setActive (isPanelActive(true));
			if (isPanelActive(true))
			{
				result.setTicked ((bool)getActivePanelEditor()->getProperty(Ids::uiPanelDisableCombosOnEdit));
			}
			break;

		case showLuaEditor:
			result.setInfo ("LUA Editor", "Show/hide the LUA editor window", panelCategory, 0);
			result.setActive (isPanelActive(true));
			break;

		case doRefreshDeviceList:
			result.setInfo ("Refresh devices", "Refresh the list of devices available in the OS", panelCategory, 0);
			result.setActive (true);
			break;

		case showLuaConsole:
			result.setInfo ("LUA Console", "Show/hide the LUA console", panelCategory, 0);
			result.setActive (isPanelActive(true));
			break;

		case showComparatorTables:
			result.setInfo ("Comparator tables", "Show/hide the dump for the comparator tables", panelCategory, 0);
			result.setActive (isPanelActive());
			break;

		case showMidiLibrary:
			result.setInfo ("MIDI Library", "Show/hide the MIDI LIbrary window", panelCategory, 0);
			result.setActive (isPanelActive());
            if (!isRestricted())
            {
                result.addDefaultKeypress ('l', ModifierKeys::altModifier | ModifierKeys::shiftModifier);
            }
			break;

		case showLayers:
			result.setInfo ("Layer editor", "Show/hide the layer editor", panelCategory, 0);
			result.setActive (isPanelActive(true));
			break;

		case showModulatorList:
			result.setInfo ("Modulator list", "Show/hide the modulator list window", panelCategory, 0);
			result.setActive (isPanelActive(true));
			break;

		case showBufferEditor:
			result.setInfo ("Buffer Editor", "Show/hide the buffer editor", panelCategory, 0);
            if (!isRestricted())
            {
                result.addDefaultKeypress ('b', ModifierKeys::commandModifier);
            }
			result.setActive (isPanelActive(true));
			break;

		case doSendSnapshot:
			result.setInfo ("Send Snapshot", "Send all values from the panel as their defines MIDI messages", panelCategory, 0);
			result.setActive (isPanelActive());
			break;

		case doSnapshotStore:
			result.setInfo ("Program snapshot", "Save the current panel state as a snapshot", panelCategory, 0);
			result.setActive (isPanelActive());
			break;

		case doRefreshPropertyLists:
			result.setInfo ("Refresh property lists", "Refreshes all dynamic lists that occur in the property pane", panelCategory, 0);
			result.setActive (isPanelActive());
			break;

		case doExportFileText:
			result.setInfo ("Export XML", "Export panel to a XML file with no compression, might be very large", panelCategory, 0);
			result.setActive (isPanelActive(true));
			break;

		case doExportFileZText:
			result.setInfo ("Export compressed XML", "Export panel to a zlib compressed XML file", panelCategory, 0);
			result.setActive (isPanelActive(true));
			break;

		case doExportFileBin:
			result.setInfo ("Export binary", "Export panel to a binary file, unlike XML is unreadable but loads faster", panelCategory, 0);
			result.setActive (isPanelActive(true));
			break;

		case doExportFileZBin:
			result.setInfo ("Export compressed binary", "Export panel to a zlib compressed binary file", panelCategory, 0);
			result.setActive (isPanelActive(true));
			break;

		case doExportFileZBinRes:
			result.setInfo ("Export compressed binary + resources", "Export panel to a zlib compressed binary file with resources included", panelCategory, 0);
			result.setActive (isPanelActive(true));
			break;

		case doExportFileInstance:
			result.setInfo ("Export instance", "Exports the panel as a standalone instance (executable/loadable file)", panelCategory, 0);
			result.setActive (isPanelActive(true));
			break;

		case doExportFileInstanceRestricted:
			result.setInfo ("Export restricted instance", "Exports the panel as a standalone restricted instance (executable/loadable file). It won't be editable.", panelCategory, 0);
			result.setActive (isPanelActive(true));
			break;

		case doExportGenerateUID:
			result.setInfo ("Re-generate UID ["+getPanelProperty(Ids::panelUID).toString()+"]", "Each panel has a Unique ID that can be re-generated", panelCategory, 0);
			result.setActive (isPanelActive(true));
			break;

		case optMidiInputFromHost:
			result.setInfo ("Input from plugin host", "Accept MIDI events from host and process them", panelCategory, 0);
			result.setActive (!JUCEApplication::isStandaloneApp());
			result.setTicked (isPanelActive() ? getActivePanel()->getMidiOptionBool((const CtrlrPanelMidiOption)optMidiInputFromHost) : false);
			break;

		case doShowMidiSettingsDialog:
			result.setInfo("Settings", "Show a more user friendly MIDI settings dialog", panelCategory, 0);
			result.setActive (isPanelActive(true));
			break;

		case optMidiInputFromHostCompare:
			result.setInfo ("Input from host to comparator", "This option tells Ctrlr to route all incomming MIDI events from the MIDI host to the Comparator engine, messages that will match modulators in the panel will be treated as comming from a MIDI device", panelCategory, 0);
			result.setActive (!JUCEApplication::isStandaloneApp());
			result.setTicked (isPanelActive() ? getActivePanel()->getMidiOptionBool((const CtrlrPanelMidiOption)optMidiInputFromHostCompare) : false);
			break;

		case optMidiOutuptToHost:
			result.setInfo ("Output to plugin host", "All MIDI output goes to the MIDI host (if supported by the plugin format and the host)", panelCategory, 0);
			result.setActive (!JUCEApplication::isStandaloneApp());
			result.setTicked (isPanelActive() ? getActivePanel()->getMidiOptionBool((const CtrlrPanelMidiOption)optMidiOutuptToHost) : false);
			break;

		case optMidiSnapshotOnLoad:
			result.setInfo ("Snapshot on load", "After the panel is loaded a Snapshot will be sent", panelCategory, 0);
			result.setActive (isPanelActive());
			result.setTicked (getPanelProperty(Ids::panelMidiSnapshotAfterLoad));
			break;

		case optMidiSnapshotOnProgramChange:
			result.setInfo ("Snapshot on program change", "After each program change event on the panel a Snapshot will be sent", panelCategory, 0);
			result.setActive (isPanelActive());
			result.setTicked (getPanelProperty(Ids::panelMidiSnapshotAfterProgramChange));
			break;

		case optMidiThruD2D:
			result.setInfo ("Input device -> Output device", "Send all MIDI messages comming from the MIDI input device via the MIDI output device", panelCategory, 0);
			result.setActive (isPanelActive());
			if (isPanelActive())
				result.setTicked (getActivePanel()->getMidiOptionBool (panelMidiThruD2D));
			break;

		case optMidiThruD2H:
			result.setInfo ("Input device -> Plugin host", "Send all MIDI messages from the MIDI input device to the plugin host (if supported by the plugin format and the host)", panelCategory, 0);
			result.setActive (isPanelActive() && (JUCEApplication::isStandaloneApp() == false));
			if (isPanelActive())
				result.setTicked (getActivePanel()->getMidiOptionBool (panelMidiThruD2H));

			break;

		case optMidiThruH2D:
			result.setInfo ("Plugin host -> Output device", "Send all MIDI messages comming from the plugin host the MIDI output device", panelCategory, 0);
			result.setActive (isPanelActive() && (JUCEApplication::isStandaloneApp() == false));
			if (isPanelActive())
				result.setTicked (getActivePanel()->getMidiOptionBool (panelMidiThruH2D));
			break;

		case optMidiThruH2H:
			result.setInfo ("Plugin host -> Plugin host", "Send all MIDI messages comming from the plugin host back to it (if supported by the plugin format and the host)", panelCategory, 0);
			result.setActive (isPanelActive() && (JUCEApplication::isStandaloneApp() == false));
			if (isPanelActive())
				result.setTicked (getActivePanel()->getMidiOptionBool (panelMidiThruH2H));
			break;

		case optMidiThruD2DChannelize:
			result.setInfo ("Change channel: Input device -> Output device", "Change the channel of MIDI messages comming in on the MIDI input device to the MIDI channel set for the output device", panelCategory, 0);
			result.setActive (isPanelActive() && (JUCEApplication::isStandaloneApp() == false));
			if (isPanelActive())
				result.setTicked (getActivePanel()->getMidiOptionBool (panelMidiThruD2DChannelize));
			break;

		case optMidiThruD2HChannelize:
			result.setInfo ("Change channel: Input device -> Plugin host", "Change the channel of MIDI messages comming in on the MIDI input device to the MIDI channel set for the output to plugin host", panelCategory, 0);
			result.setActive (isPanelActive() && (JUCEApplication::isStandaloneApp() == false));
			if (isPanelActive())
				result.setTicked (getActivePanel()->getMidiOptionBool (panelMidiThruD2HChannelize));
			break;

		case optMidiThruH2DChannelize:
			result.setInfo ("Change channel: Plugin host -> Output device", "Change the channel of MIDI messages comming in from the plugin host to the MIDI channel set for the output device", panelCategory, 0);
			result.setActive (isPanelActive() && (JUCEApplication::isStandaloneApp() == false));
			if (isPanelActive())
				result.setTicked (getActivePanel()->getMidiOptionBool (panelMidiThruH2DChannelize));
			break;

		case optMidiThruH2HChannelize:
			result.setInfo ("Change channel: Plugin host -> Plugin host", "Change the channel of MIDI messages comming in from the plugin host to the MIDI channel set for the output to plugin host", panelCategory, 0);
			result.setActive (isPanelActive() && (JUCEApplication::isStandaloneApp() == false));
			if (isPanelActive())
				result.setTicked (getActivePanel()->getMidiOptionBool (panelMidiThruH2HChannelize));
			break;

		case doCrash:
			result.setInfo ("Crash Ctrlr", "Simulate Ctrlr crashing to see what happens", globalCategory, 0);
			result.setActive (true);
			break;

		case doDumpVstTables:
			result.setInfo ("Dump Memory Info", "Dump information stored in memory in some readable format", globalCategory, 0);
			result.setActive (true);
			break;

		case doRegisterExtension:
			result.setInfo ("Register file extensions", "Register Ctrlr to hanle .panel file extension", globalCategory, 0);
			result.setActive (true);
			break;

        case doKeyGenerator:
            result.setInfo ("Key Generator", "Generate a private key for signing panels", globalCategory, 0);
            result.setActive (true);
            break;

        case doProgramWizard:
            result.setInfo ("Program wizard", "Wizard to generate program requests", globalCategory, 0);
            result.setActive (true);
            break;

		case doQuit:
			result.setInfo ("Quit", "Quit Ctrlr", globalCategory, 0);
			result.setActive (JUCEApplication::isStandaloneApp());
            result.addDefaultKeypress ('q', ModifierKeys::commandModifier); // Added v5.6.32
			break;
            
        // Added v5.6.34. CASES for LUA Method Editor menu items. This is for showing the shortcuts to the GUI.
        // Those combos are not picked up by the app and I don't know why.
        // So we handle them manually in CtrlrLuaMethodCodeEditor.cpp with CtrlrLuaMethodEditor::keyPressed

        case LuaMethodEditorCommandIDs::fileSave:
            result.setInfo("Save", "Saves the current Lua method", luaCategory, 0);
            result.addDefaultKeypress('s', ModifierKeys::commandModifier);
            result.setActive(isPanelActive());
            break;
            
        case LuaMethodEditorCommandIDs::fileSaveAndCompile:
            result.setInfo("Save and Compile", "Saves and compiles the current Lua method", luaCategory, 0);

            // Corrected syntax for Cmd+Shift+S
            result.addDefaultKeypress(CharacterFunctions::toUpperCase('s'), ModifierKeys::commandModifier | ModifierKeys::shiftModifier);
            
            // This was already correct
            result.addDefaultKeypress(KeyPress::F7Key, ModifierKeys::noModifiers);
            
            result.setActive(isPanelActive());
            break;
            
        case LuaMethodEditorCommandIDs::fileSaveAndCompileAll:
            result.setInfo("Save and Compile All", "Saves and compiles all Lua methods", luaCategory, 0);
            
            // This was already correct
            result.addDefaultKeypress(KeyPress::F8Key, ModifierKeys::noModifiers);
            
            result.setActive(isPanelActive());
            break;
            
        case LuaMethodEditorCommandIDs::fileCloseCurrentTab:
            result.setInfo("Close Current Tab", "Closes the current Lua method tab", luaCategory, 0);
            result.addDefaultKeypress('w', ModifierKeys::commandModifier);
            result.setActive(isPanelActive());
            break;
            
        case LuaMethodEditorCommandIDs::fileCloseAllTabs:
            result.setInfo("Close All Tabs", "Closes all Lua method tabs", luaCategory, 0);
            result.setActive(isPanelActive());
            break;
            
        case LuaMethodEditorCommandIDs::fileConvertToFiles:
            result.setInfo("Export Methods to Files", "Converts all methods to individual files", luaCategory, 0);
            result.setActive(isPanelActive());
            break;
            
        case LuaMethodEditorCommandIDs::fileClose:
            result.setInfo("Close Lua Editor", "Closes the Lua editor window", luaCategory, 0);
            // result.addDefaultKeypress (KeyPress::F12_KEY, ModifierKeys::noModifiers, 0); // OR whatever
            result.setActive(isPanelActive());
            break;
            
        case LuaMethodEditorCommandIDs::editSearch:
            result.setInfo("Search", "Finds text in the editor", luaCategory, 0);
            result.addDefaultKeypress('f', ModifierKeys::commandModifier);
            result.setActive(isPanelActive());
            break;
            
        case LuaMethodEditorCommandIDs::editFindAndReplace:
            result.setInfo("Find and Replace", "Finds and replaces text in the editor", luaCategory, 0);
            result.addDefaultKeypress('h', ModifierKeys::commandModifier); // Not a good idea since cmd+h is to hide the app for macOS cmd+r would be better
            result.setActive(isPanelActive());
            break;
            
        case LuaMethodEditorCommandIDs::editDebugger:
            result.setInfo("Show Debugger", "Shows the debugger panel", luaCategory, 0);
            result.setActive(isPanelActive());
            break;
            
        case LuaMethodEditorCommandIDs::editConsole:
            result.setInfo("Show Console", "Shows the console panel", luaCategory, 0);
            result.setActive(isPanelActive());
            break;
            
        case LuaMethodEditorCommandIDs::editClearOutput:
            result.setInfo("Clear Output", "Clears the output window", luaCategory, 0);
            result.setActive(isPanelActive());
            break;
            
        case LuaMethodEditorCommandIDs::editPreferences:
            result.setInfo("Preferences", "Opens the editor preferences", luaCategory, 0);
            result.setActive(isPanelActive());
            break;
            
        case LuaMethodEditorCommandIDs::editSingleLineComment:
            result.setInfo("Toggle single line comment", "Toggles single line comment", luaCategory, 0);
            result.addDefaultKeypress('/', ModifierKeys::commandModifier);
            result.setActive(isPanelActive());
            break;
            
        case LuaMethodEditorCommandIDs::editMultiLineComment:
            result.setInfo("Toggle Multi-Line Comment", "Toggles multi-line comments on the selection", luaCategory, 0);
            result.addDefaultKeypress('Q', ModifierKeys::commandModifier); // It's going to quit the app ???
            result.setActive(isPanelActive());
            break;
            
        case LuaMethodEditorCommandIDs::editDuplicateLine:
            result.setInfo("Duplicate Line", "Duplicates the current line or selection", luaCategory, 0);
            result.addDefaultKeypress(CharacterFunctions::toUpperCase('d'), ModifierKeys::commandModifier);
            result.setActive(isPanelActive());
            break;
            
        case LuaMethodEditorCommandIDs::editGoToLine:
            result.setInfo("Go to Line", "Jumps to a specific line number", luaCategory, 0);
            result.addDefaultKeypress('g', ModifierKeys::commandModifier);
            result.setActive(isPanelActive());
            break;
            
        default:
            break;
	}
}

ApplicationCommandTarget* CtrlrEditor::getNextCommandTarget() // Added v5.6.34. Required for the LUA method manager combo shortcuts
{
    // Check if this method is even being called.
    _DBG("getNextCommandTarget called");

    // Use getActivePanel() which returns a CtrlrPanel*.
    if (CtrlrPanel* activePanel = getActivePanel())
    {
        _DBG("Active panel exists.");

        juce::Component* luaEditorContent = activePanel->getPanelWindowManager().getContent(CtrlrPanelWindowManager::LuaMethodEditor);

        if (luaEditorContent != nullptr)
        {
            _DBG("Lua editor content exists.");

            if (luaEditorContent->hasKeyboardFocus(true))
            {
                _DBG("Lua editor has keyboard focus.");
                return dynamic_cast<ApplicationCommandTarget*>(luaEditorContent);
            }
            else
            {
                _DBG("Lua editor does NOT have keyboard focus.");
            }
        }
        else
        {
            _DBG("Lua editor content is nullptr.");
        }
    }
    else
    {
        _DBG("Active panel is nullptr.");
    }

    // If no specific component is active and focused, return the default.
    return findFirstTargetParentComponent();
}
