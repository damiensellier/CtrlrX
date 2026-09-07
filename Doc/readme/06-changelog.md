[← 05 — Exporting Instances](05-exporting-instances.md) | [Main Index](../../README.md)

---

# 06 — Roadmap & Changelog

> **TL;DR**
> - Active development tracks are focused on upgrading the underlying core engine to modern framework architectures.
> - Comprehensive structural changes, bug fixes, and feature additions are detailed chronologically below.

## Contents
- [Roadmap Task Tracker](#roadmap-task-tracker)
- [Changelog](#changelog)

---

## Roadmap Task Tracker

- [ ] Complete platform updates targeting core JUCE 8 frame releases.
- [ ] Transition binding scripts from legacy Luabind layouts to streamlined modern wrappers (LuaBridge, Sol, etc.).
- [ ] Expose missing structural JUCE underlying object classes into editable runtime Lua scripts.

---

## Changelog

#### Version 5.6.36 | 2026.08.30

* **FIXED** CtrlrTabsComponents.cpp and CtrlrGroup.cpp keep component owner group on uiTabs. `CtrlrTabsComponent.cpp`, `CtrlrGroup.cpp`
* **FIXED** Doubling of menu item causing potential crash. `CtrlrEditor.cpp`. Thanks to [John Goodland](https://github.com/dnaldoog) `CtrlrEditorApplicationCommandsHandlers.cpp`
* **FIXED** Slider double click interval definition (uiSliderDoubleClickValue). `CtrlrIDs.xml`
* **FIXED** Custom Keyboard mappings not being saved on panel close due to the Wayland fix options. Thanks to [John Goodland](https://github.com/dnaldoog) `CtrlrEditorApplicationCommandsHandlers.cpp`
* **FIXED** WIN doRegisterExtension menu item removed from unrelated OS. Thanks to [John Goodland](https://github.com/dnaldoog) `CtrlrEditorApplicationCommandsMenus.cpp`
* **FIXED** FIXED Slider not reacting on Midi channel different than 1. Thanks to [John Goodland](https://github.com/dnaldoog) `CtrlrMIDIDevice.cpp`
* **FIXED** MIDI IN leakage between multiple panels. Thanks to [John Goodland](https://github.com/dnaldoog) and [Holger Dehnhardt](https://github.com/dehnhardt)
* **FIXED** Label component leak on exit. `CtrlrPropertyComponent.cpp`. Thanks to [John Goodland](https://github.com/dnaldoog)
* **FIXED** Some modulator duplication when loading a panel. `CtrlrPropertyComponent.cpp`. Thanks to [John Goodland](https://github.com/dnaldoog)
* **ADDED** .clang-format file to the root to enforce code style across IDEs. `.clang-format`
* **UPGRADED** LUA 5.1 to LuaJIT 2.1. Thanks to [John Goodland](https://github.com/dnaldoog)
* **ADDED** refresh MIDI devices shortcut (cmd/ctrl + D). `CtrlrEditorApplicationCommands.cpp`, `CtrlrEditorApplicationCommandsHandlers.cpp`. Thanks to [John Goodland](https://github.com/dnaldoog)
* **UPDATED** Zoom Shortcuts & Keypad Minus zoom-out. Thanks to [John Goodland](https://github.com/dnaldoog)
* **FIXED** Zoom Out for Linux Wayland. Thanks to [John Goodland](https://github.com/dnaldoog)
* **FIXED** Resources not updating on load after panel update. RSRCs now compared by hash. `CtrlrPanelResourceManager.cpp`, `CtrlrPanelResource.cpp` & `.h`. Thanks to [John Goodland](https://github.com/dnaldoog)
* **FIXED** Modulator List configurations being lost or duplicated incorrectly. `CtrlrPanelModulatorList.cpp` & `.h`. Thanks to [John Goodland](https://github.com/dnaldoog)
* **FIXED** Modulator List selected row background color giving a gradient blue part at the end of the line. `CtrlrPanelModulatorList.cpp`
* **ADDED** Bubble Help for component triggered from mouse events. `CtrlrIDs.h`, `CtrlrIDs.xml`, `CtrlrComponent.cpp` &`.h`, `CtrlrPropertyComponent.cpp` & `.h`. Thanks to [John Goodland](https://github.com/dnaldoog)
* **ADDED** uiSliderBackgroundColour property to uiSlider & uiFixedSlider for linear bar background colour. `CtrlrIDs.h`, `CtrlrIDs.xml`, `CtrlrSlider.cpp`, `CtrlrFixedSlider.cpp` , `CtrlSliderInternal.cpp` & `.h`
* **ADDED** Centralized documentation as .md file. Thanks to [Sgorpi](https://github.com/sgorpi)
* **ADDED** Align/Space/Resize components on canvas. Thanks to [John Goodland](https://github.com/dnaldoog).`CtrlrPanelCanvas.cpp` & `.h`, `CtrlrPanelCanvasHandlers.cpp`
* **FIXED** ComboBox fuzzy search. Thanks to [John Goodland](https://github.com/dnaldoog). `CtrlrCombo.cpp` & `.h`
* **ADDED** new templates for LUA table and class on right-click menu in LUA editor. Thanks to [John Goodland](https://github.com/dnaldoog). `CtrlrLuaMethodEditor.cpp` & `.h`
* **UPDATED** Custom colours LnF logic to remove ui_Component_LookAndFeelIsCustom for sliders, buttons, group and combo.
* **UPDATED** play, stop, clear missing icons added to CtrlrLuaMethodDebuggerPrompt.cpp. Thanks to [John Goodland](https://github.com/dnaldoog). `CtrlrLuaMethodDebuggerPrompt.cpp`
* **FIXED** layer drag/drop in Layer Editor. Thanks to [John Goodland](https://github.com/dnaldoog). `CtrlrPanel/CtrlrPanelLayerList.cpp` & `.h`
* **FIXED** leak in CtrlrPropertyComponent. Thanks to [Sgorpi](https://github.com/sgorpi) `CtrlrPropertyComponent.cpp` & `.h`
* **UPDATED** CI/CD test files for JUCE 8 compatibility. Thanks to [Sgorpi](https://github.com/sgorpi)
* **ADDED** uiLabel showTextEditor() to display the text field editor. `CtrlrLabel.cpp` & `.h`, `CtrlrComponentLuaRegistration.cpp`, `CtrlrComponentLuaRegistration.cpp`
* **UPDATED** UTF8 support for LUA string. Thanks to [John Goodland](https://github.com/dnaldoog). `LCore.cpp`, `stdafx_luabind.h`
* **ADDED** uiSlider (all types) showTextEditor() to display the value field editor. `CtrlrComponentLuaRegistration.cpp`, `CtrlrSlider.cpp`  & `.h`, `CtrlrFixedImageSlider.cpp`  & `.h`, `CtrlrFixedSlider.cpp`  & `.h`,, `CtrlrImageSlider.cpp`  & `.h`

#### Version 5.6.35 | 2026.04.20

* **UPDATED** ctrlrAutoSave & ctrlrAutoSaveInterval properties in Preferences only if isStandaloneApp. `CtrlrManager.cpp`, `CtrlrManagerInstance.cpp`, `CtrlrSettings.cpp`
* **UPDATED** flag IS_MIDI_EFFECT FALSE. `CMakeLists.txt`
* **UPDATED** Projucer files without useless linux libs. `CtrlrX 5.6.35.jucer, `CtrlrX 5.6.35 [Without AAX].jucer`, `CtrlrX 5.6.35 [Without AAX & VST].jucer`
* **UPDATED** Get/Set modValue As/From Data with getModulatorValuesAsData() & setModulatorValuesFromData(). Thanks to [John Goodland](https://github.com/dnaldoog) `CtrlrLuaManager.cpp`, `CtrlrMacros.h`, `CtrlrPanel.cpp`
* **FIXED** saving uiSlider positions on uiTabs. Thanks to [John Goodland](https://github.com/dnaldoog) `CtrlrTabsComponents.cpp`
* **FIXED** MemoryBlock(int) faulty constructor returning actual size x2. Thanks to [John Goodland](https://github.com/dnaldoog) `LMemory.cpp`
* **ADDED** GH action to create UML diagrams using clang-uml. Thanks to [Sgorpi](https://github.com/sgorpi). `clang_uml_diagrams.yml`, `.clang-uml`
* **FIXED** Linux Gnome Wayland modal Dialog Windows. Thanks to [John Goodland](https://github.com/dnaldoog) 
* **FIXED** memory issues identified with Valgrind. Thanks to [Sgorpi](https://github.com/sgorpi).
* **FIXED** XML file reading. Thanks to [John Goodland](https://github.com/dnaldoog) `LCore.cpp`
* **FIXED** Highlight Background & Text for FileListBox Component. Thanks to [Dominique Bonptemps](https://github.com/dobo365) `CtrlrFileListBox.cpp`
* **UPDATED** CtrlrLuaMethodEditor with IDE-like autocomplete on typing. `CtrlrLuaMethodAutoCompleteManager.cpp` & `.h`, `CtrlrLuaMethodCodeEditor.cpp` & `.h`, `CtrlrLuaMethodEditor.h`
* **UPDATED** sendMidiMessageNow() compatibility with the old syntax and support table content as message. `CtrlrLuaMethodAutoCompleteManager.cpp` & `.h`
* **ADDED** MIDI Monitor Message Type filtering (Note On, Note Off, Aftertouch, Clock, SYSEX, etc). Thanks to [John Goodland](https://github.com/dnaldoog). `CtrlrIDs.h`, `CtrlrLog.cpp` & `.h`, `CtrlrMacros.h`, `CtrlrMIDIMon.cpp` & `.h`
* **FIXED** AudioUnit type is set to "Virtual Instrument" not "Audio FX" anymore. Thanks to [Chuck Zwicky](https://github.com/zmixnyc) `cmakelists.txt`
* **UPDATED** X11 scaling for HiDPI in Linux. Thanks to [Andrej Radović](https://github.com/randrej)
* **FIXED** parameter index to host interface for AudioUnit. Thanks to [Chuck Zwicky](https://github.com/zmixnyc) `CtrlrProcessor.cpp`
* **UPDATED** comboBox Fuzzy Search with [RapidFuzz CPP](https://github.com/rapidfuzz/rapidfuzz-cpp). `CtrlrCombo.cpp` & `.h`, `CtrlrIDs.xml` & `.h`, `Source/Misc/rapidfuzz`, `CMakeLists.txt`
* **UPDATED** CtrlrPanelLayerList & selected item highlight. Thanks to [John Goodland](https://github.com/dnaldoog). `CtrlrPanelLayerList.cpp`, `CtrlrPanelLayerListItem.cpp` & `.h`
* **UPDATED** CtrlrAbout Window with new donation links & resources to ko-fi. `CtrlrAbout.cpp` & `.h`
* **UPDATED** Windows export panel fileChooser default target folders. `CtrlrWindows.cpp`
* **ADDED** Windows script for faster compilation via NINJA. Thanks to [John Goodland](https://github.com/dnaldoog). `autobuild_win.bat`
* **UPDATED** CtrlrTabsComponents.cpp saving uiSlider positions on uiTabs. `CtrlrTabsComponent.cpp`
* **FIXED** CtrlrLuaMethodAutoCompleteManager crash when self-referencing a custom variable followed with LUA concatenation dot dot. (i.e. myVariable = myVvariable.."abc"). `CtrlrLuaMethodAutoCompleteManager.cpp`, `CtrlrLuaMethodCodeEditor.cpp`
* **ADDED** MIDI Message tokens for Novation Bass Station II with 8-bit Values over 2 CCs. Thanks to [John Goodland](https://github.com/dnaldoog).
* **ADDED** MIDI Message Latch & Stream option to MIDI messages to allow NRPN header just being sent once only when MIDI data changes. Thanks to [John Goodland](https://github.com/dnaldoog). `CtrlrPanel.cpp` & `.h` , `CtrlrMidiMessage.cpp`, `CtrlrIDs.xml`, `CtrlrIDs.h` , `CtrlrSysexProcessor.cpp` 

#### Version 5.6.34 | 2025.11.07

* **NEW** ProTools AAX plugin support (requires AAX SDK 2.8+ and codesigning via PACE)
* **UPDATED** LuaBind Drawable class. `LCore.cpp`, `LGraphics.cpp`
* **UPDATED** LuaBind function `addChild()` to `valueTree` class. `LCore.cpp`
* **ADDED** LuaBind function `jmap()`, `mapToLog10()`, `mapFromLog10()`, `isWithin()` to GlobalFunctions. `LCore.cpp`
* **ADDED** LuaBind Parse XML support to `XmlDocument` class. `LCore.cpp`
* **UPDATED** README.md Licensing terms, compilation instructions and layout improved. `README.md`
* **FIXED** Subsequent modulators with the same CC index as the first one not reacting. `CtrlrManagerInstance.cpp`, `CtrlrMidiInputComparatorSingle.cpp`, `CtrlrMidiInputComparatorMulti.cpp`
* **UPDATED** MIDI Plugin Options default settings set to 'Input from plugin host" & "Input from host to comparator". `CtrlrPanel.cpp`
* **UPDATED** MIDI Keyboard component, Octave DOWN/UP added with key assignment "x" & "z" by default. Top line colour on focus. `CtrlrMidiKeyboard.cpp`, `CtrlrMidiKeyboard.h`, `CtrlrIDs.xml`, `CtrlrIDs.h` 
* **UPDATED** focusGained(), focusLost() to CtrlrComponent. `CtrlrComponent.cpp` & `.h`
* **UPDATED** Plugin Wrapper is used by CtrlrX only on WIN with Ableton Live as VST2. The "ctrlrUseEditorWrapper" property is deprecated. `CtrlrProcessor.cpp` & `.h`, `CtrlrManager.cpp`, `CtrlrManagerInstance.cpp`, `CtrlrPanel.cpp`
* **UPDATED** LuaBind InputStream class. `LCore.cpp`
* **UPDATED** Boost library v1.8.8. `Source/Misc/boost/`
* **ADDED** Visual Studio 2022 configuration. `CtrlrX.jucer`
* **ADDED** JUCE, Boost, LuaBind version infos in the About window. `CtrlrAbout.cpp` & `.h`
* **REMOVED** VST2.4 SDK due to licensing compliance. `/Source/Misc/vst2sdk/`
* **NEW** Support for online plugin authorization (Requires PHP & SQL Server with keygen and authentication scripts and exec). `CtrlrX.jucer`, `LCore.cpp`, `LMachineIDUtilities.cpp` & `.h`, `LOnlineUnlockStatus.cpp` & `.h`, `LOnlineUnlockStatusCheck.cpp`& `.h`, `LRSAKey.cpp` & `.h`
* **UPDATED** askForTextInputWindow fallback for cancel button value to -1. `CtrlrLuaUtils.cpp`
* **UPDATED** LuaMethodEditor, "Open" and "Rename" method items added to the menu. `CtrlrLuaMethodEditor.cpp` & `.h`
* **UPDATED** Close panel alert window buttons set to "Close" & "Cancel". `CtrlrDocumentPanel.cpp`
* **UPDATED** LnF version & ColourScheme selection method centralized to CtrlrInlineUtilitiesGUI. `CtrlrInlineUtilitiesGUI.cpp` & `.h`, `.jucer`, `juce_LookAndFeel_V4.cpp` & `.h`, `CtrlrEditor.cpp`, `CtrlrButton.cpp`, `CtrlrToggleButton.cpp`, `CtrlrCombo.cpp`, `CtrlrGroup.cpp`, `CtrlrFixedImageSlider.cpp`, `CtrlrFixedSlider.cpp`, `CtrlrFixedSlider.cpp`, `CtrlrImageSlider.cpp`, `CtrlrSlider.cpp`, `CtrlrPanelEditor.cpp`
* **UPDATED** Panel tab close button method with canClose(bool) for consistency. `CtrlrDocumentPanel.cpp`, `CtrlrManager.cpp` & `.h`
* **FIXED** Alert windows LnF version not following the global LnF for V3. `CtrlrEditor.cpp` & `.h`, `CtrlrInlineUtilitiesGUI.cpp` & `.h`
* **ADDED** Preferences "ctrlrPropertyLineImprovedLegibility" to force B&W text for property pane text entry fields. `CtrlrPropertyComponent.cpp`, `CtrlrIDs.xml`, `CtrlrIDs.h`
* **FIXED** CtrlrPanel.cpp panelVersionMajor & panelVersionMinor stored as int. `CtrlrPanel.cpp` 
* **ADDED** Algorithm for Roland, E-mu, Korg, Waldorf, Lexicon, Exclusive OR, Akai, Korg, Sequential checksums. `CtrlrSysexProcessor.cpp` & `.h` `CtrlrMacros.h`, `CtrlrPropertyComponent.cpp`, `CtrlrUtilities.cpp`
* **FIXED** Resource Editor colours follow the general LnF colourScheme. `CtrlrPanelResourceEditor.cpp` & `.h`
* **ADDED** Resource Editor file preview thumbnail. `CtrlrPanelResourceEditor.cpp` & `.h`
* **FIXED** Single quote content in comboBox. `CtrlrValueMap.cpp` & `.h`
* **ADDED** CtrlrPanelProperties highlighted tab when active. `CtrlrPanelProperties.cpp`
* **ADDED** alertWindow warning at export when trying to overwrite an existing panel on macOS. `CtrlrMac.cpp`
* **UPDATED** Export instance alertWindows won't show up if cancelled by user. `CtrlrMac.cpp`, `CtrlrPanelFileOperations.cpp`
* **UPDATED** Export instance fileChooser default directory is now set to panelLastSaveDir or falls back to desktop. `CtrlrMac.cpp`
* **FIXED** CtrlrLuaMethodCodeEditorSettings Highlight colours such as Error, comments, operatiors etc. `CtrlrLuaMethodCodeEditorSettings.cpp`, `CtrlrLuaMethodCodeTokeniserFUnctions.h`
* **UPDATED** CtrlrLuaMethodCodeEditorSettings GUI with a better sample code preview. `CtrlrLuaMethodCodeEditorSettings.cpp`
* **UPDATED** CtrlrPropertyComponent Separated colourChooser from colour field. `CtrlrPropertyComponent.cpp` &`.h`, `CtrlrIDs.xml`
* **UPDATED** CtrlrPropertyComponent Layer selection via comboBox. `CtrlrPropertyComponent.cpp` & `.h`, `CtrlrIDs.xml`
* **UPDATED** CtrlrPropertyComponent Font size, kerning & horizontal-scale sliders replaced by drop-down selector for convenience. `CtrlrPropertyComponent.cpp` &`.h`
* **FIXED** CtrlrX crashing or freezing when closing while the notify bar was visible. `CtrlrPanelEditor.cpp` & `.h`
* **UPDATED** Utils get16bitSigned() & get16bitSigned() funtions bind to LUA. `CtrlrLuaUtils.cpp` & `.h`
* **UPDATED** Layer manager complete redesign for clarity and convenience. `CtrlrPanelLayerList.cpp` & `.h`, `CtrlrPanelLayerListItem.cpp` & `.h`, `CtrlrPanelEditorIcon.cpp` & `.h`
* **UPDATED** DEBIAN 12 and Fedora 42 build instructions. `README.md`
* **NEW** GitHub CI/CD [Pamplejuce](https://github.com/sudara/pamplejuce) Release Workflow. Thanks to [sudara](https://github.com/sudara) & [Sgorpi](https://github.com/sgorpi)
* **ADDED** VST3 Instance export support for Linux. Thanks to [John Goodland](https://github.com/dnaldoog).  `CtrlrLinux.cpp`
* **ADDED** ZLIB & GZIP Support with LuaBind. Thanks to [John Goodland](https://github.com/dnaldoog)
* **ADDED** fromLuaString(), fromLuaStringToAscii() and MemoryBlock(int) init function to memoryBlock Class with LuaBind. Thanks to [John Goodland](https://github.com/dnaldoog)
* **ADDED** Commits on GitHub by [Damien Sellier](https://github.com/DamienSellier) are now verified to prove authenticity.
* **ADDED** macOS binaries and installers by [Damien Sellier](https://github.com/DamienSellier) on the GitHub official releases pages are now codesigned.
* **ADDED** Windows binaries and installers by [Damien Sellier](https://github.com/DamienSellier) on the GitHub official releases pages are now codesigned.
* **ADDED** LINUX binaries by [Damien Sellier](https://github.com/DamienSellier) on the GitHub official releases pages are now codesigned.

#### Version 5.6.33 | 2025.05.28

* **FIXED** VST3 Host>CtrlrX interface. Sliders now react to Host automations of parameter values. Related Modulators send MIDI output messages. `CtrlrProcessor.cpp` & `CtrlrProcessor.h`, `CtrlrModulator.cpp` & `CtrlrModulator.h`, `CtrlrModulatorProcessor.cpp` & `CtrlrModulatorProcessor.h`
* **FIXED** Linux Makefile. CtrlrX requires `binutils-dev` & `libsframe1` installed on the system to compile. Thanks @sgorpi for the PR. `Builds/Linux/Makefile/Makefile`
* **FIXED** ADD, REMOVE, RELOAD resource pane buttons not reacting on certain setups. Z-index added. Thanks to @dnaldoog. `CtrlrPanelResourceEditor.cpp`
* **ADDED** Property line height base value in Preferences>GUI. `CrrlrIDs.h`, `CtrlrIDs.xml`, `CtrlrSettings.cpp`, `CtrlrManager.cpp`, `CtrlrManagerInstance.cpp`, `CtrlrPropertyComponent.cpp`
* **ADDED** LuaBind functions `setType()`, `setMidiMessageType()`, `setProperty()`. `CtrlrMidiMessage.cpp`
* **ADDED** "Encrypt exported panel resources", "Delay between steps at export" & "Codesign exported panel" properties to improve panel export process. `CrrlrIDs.h`, `CtrlrIDs.xml`, `CtrlrMac.cpp`, `CtrlrPanel.cpp`
* **FIXED** Useless menu item "Register file extensions" hidden for macOS binaries. `CtrlrEditorApplicationCommandsMenus.cpp`
* **ADDED** LuaBind function `addColumnBreak()` for `PopupMenu`. `LComponents.cpp`
* **ADDED** SliderType LinearBarVertical, RotaryHorizontalVerticalDrag, TwoValueHorizontal, TwoValueVertical, ThreeValueHorizontal, ThreeValueVertical. `CtrlrComponentTypeManager.h` & `CtrlrComponentTypeManager.cpp`, `CtrlrIDs.xml`

#### Version 5.6.32 | 2025.04.05

* **NEW** From now on, `uiSliders` will return `double float` type values to manage decimals. If required values are integers, a new LUA function has been added: `getValueInt()`, `getModulatorInt()`, `getModulatorValueInt()`, `getMinModulatorValueInt()`, `getMaxModulatorValueInt()`, `getValueMappedInt()`, `getValueNonMappedInt()`, `getMinMappedInt()`, `getMaxMappedInt()`
* **NEW** VST3 can now export instances without the need to compile intermediate plugins from the Projucer. VST3 identifiers will be taken from the panel plugin name, plugin ID, Panel Author and manufacturer ID as well as the VST3 plugin type. Exported VST3 plugins are codesigned on export directly with JUCE `childProcess()` automatically, either with local ad-hoc signature or, if selected, with a developer certificate.
* **UPDATED** Exported instances will now hide the preferences and shortcut menu items. `CtrlrEditorApplicationCommandsMenus.cpp`
* **UPDATED** Decimal values are now supported for Sliders (i.e. 3.1416). `CtrlrModulator.cpp` & `h`, `CtrlrModulatorProcessor.cpp` & `h`, `CtrlrIDs.xml`, `CtrlrLuaManager.cpp`, `CtrlrLuaMethodManager.h`, `CtrlrLuaMethodManagerCalls.cpp`, `CtrlrFixedImageSlider.cpp`, `CtrlrFixedImageSlider.cpp`, `CtrlrImageSlider.cpp`, `CtrlrSliderInternal.cpp` & `h`
* **UPDATED** Decimal interval steps are now supported for Sliders (i.e. 0.1)
* **FIXED** Useless shortcuts such as "New Panel", "Export" enabled on restricted instances. `CtrlrEditorApplicationCommands.cpp`
* **FIXED** Build Timestamp not updating on macOS. `CtrlrX.jucer`, `CtrlrRevision.h`
* **FIXED** FileChooser still hanging when exporting instance on OSX Catalina & macOS BigSur. `CtrlrManager.cpp`
* **FIXED** Windows crashing when loading a panel with a faulty modulator callback on value change LUA script. `luabind/detail/call_function.hpp`, `CtrlrModulatorProcessor.cpp`
* **ADDED** Slider value Suffix (i.e. Hz, ms, dB, etc)
* **ADDED** JUCE `systemStats` support for WIN11, macOS 11, macOS 12, macOS 13, macOS 14, macOS 15 and macOS 16. `juce_mac_systemStats.cpp`, `juce_win32_systemStats.cpp`, `juce_systemStats.h`
* **ADDED** Enable/Disable "Run modulator valueChange LUA callback in Bootstrap state". `CtrlrManagerInstance.cpp`, `CtrlrModulatorProcessor.cpp`, `CrrlrIDs.h`, `CtrlrIDs.xml`, `CtrlrSettings.cpp`

#### Version 5.6.31 | 2025.01

* **Security Update:** Encryption of the panel file in the macOS bundle for restricted instances. `CtrlrMac.cpp`, `CtrlrEditorApplicationCommandsMenu.cpp`, `CtrlrManager.cpp`
* **NEW** JUCE Class `MouseInputSource` added to LUA. `LCore.cpp`, `LCore.h`, `LJuce.cpp`, `LJuce.h`, `LMouseInputSource.h`
* **NEW** algorithm for Roland checksums. `CtrlrSysexProcessor.cpp` & `CtrlrUtilities.cpp`
* **NEW** settings for LUA Method Editor. `CtrlrValueTreeEditor.h`, `CtrlrLuaMethodCodeEditorSettings.cpp` & `h`, `CtrlrLuaMethodCodeEditor.cpp`, `CtrlrLuaMethodEditor.cpp`, `CtrlrIDs.h` & `CtrlrIDs.xml`
* **FIXED** MSB 14 bit numbers sending `0xFF` instead of `0x7F`. `CtrlrSysexProcessor.cpp` & `CtrlrUtilities.cpp`
* **FIXED** missing File Management bottom notification bar. `CtrlrPanelFileOperations.cpp`, `CtrlrPanelEditor.cpp`, `CtrlrPanelEditor.h`, `CtrlrPanel.cpp`, `CtrlrPanel.h`
* **FIXED** missing menuBar on export for log and MIDI monitor windows. `CtrlrChildWindowContainer.cpp`
* **FIXED** `uiSlider` value not reaching `maxValue` when using negative values for `minValue`. `CtrlrSlider.cpp`
* **FIXED** `uiImageSlider` value not reaching `maxValue` when using negative values for `minValue`. `CtrlrImageSlider.cpp`
* **FIXED** black text on black background for Modulator List window. `CtrlrPanelModulatorList.cpp`
* **FIXED** typo in alert when closing dirty panel. `CtrlrPanelFileOperations.cpp`
* **FIXED** parameters count passed to VST host is set from the highest vstindex when a panel is an exported VST/VST3 instance, not (64). `CtrlrProcessor.cpp`
* **FIXED** LnF panel close button colour on mouseover follows the panel colourScheme. `CtrlrDocumentPanel.cpp`
* **FIXED** `CtrlrModulator Value` statement precised to help avoid feedback loops between LUA and (delayed) UI. Commit 6e5a0b2 by midibox. `CtrlrLuaManager.cpp`
* **FIXED** exported VST crashing DAW if `panelIsDirty = 0` on export. `CtrlPanelFileOperations.cpp`
* **FIXED** Ctrlr not showing up in Ableton Live. `CtrlrX.jucer`, `CtrlrProcessor.cpp` & `CtrlrProcessor.h`, `CtrlrProcessorEditorForLive.cpp` & `CtrlrProcessorEditorForLive.h`
* **FIXED** MIDI Monitor IN/OUT turned ON by default. `CtrlrManager.cpp`
* **FIXED** Console window & Midi Monitor crashing Cubase if closed from the Menu File>Close. `CtrlrLuaConsole.cpp` & `CtrlrMIDIMonitor.cpp`
* **FIXED** LUA `mod:getMidiMessage():getProperty("propertyName")` `CtrlrMidiMessage.cpp` & `CtrlrMidiMessage.h`
* **FIXED** exported instances not getting the proper LnF version or colourScheme (popup, child windows etc). `CtrlrManagerInstance.cpp`, `CtrlrEditor.cpp`
* **FIXED** property pane tabs not showing up on exported instances. `CtrlrManagerInstance.cpp`
* **FIXED** Property Pane tabs not following panel LnF. `CtrlrPanelProperties.cpp` & `CtrlrPanelProperties.h`
* **FIXED** Child Windows (LUA Editor, console etc) not getting the proper menuBar background. `CtrlrChildWindowContainer.cpp`
* **FIXED** LUA Editor Method Tree selected item not getting the proper colour. `CtrlrLuaMethodEditor.cpp`
* **FIXED** AU AudioUnit version of CtrlX was failing the Apple/Logic Validation Test. `CtrlrX.jucer`, `CtrlrProcessor.cpp` & `CtrlrProcessor.h`
* **FIXED** Panel Tabs showing up on exported instances. `CtrlrDocumentPanel.cpp`
* **FIXED** messy preferences window and settings. `CtrlrSettings.cpp` & `CtrlrSettings.cpp`, `ctrlrIDs.h` & `CtrlrIDs.xml`
* **FIXED** default New Panel LnF not following global LnF from Global Preferences. `ctrlrPanel.cpp`, `ctrlrIDs.h` & `CtrlrIDs.xml`
* **FIXED** CtrlrX.ico too dark to be legible on dark backgrounds. `ctrlr_logo_circle_v3.svg`
* **FIXED** FileChooser hanging when exporting instance on OSX Catalina & macOS BigSur. `CtrlrManager.cpp`
* **FIXED** About popup design refurbished with new CtrlrX logo. `CtrlrAbout.cpp` & `CtrlrAbout.h`
* **FIXED** About popup not getting current build date, fixed with C++ Macro timestamp. `CtrlrRevision.h`
* **FIXED** VST crashing DAW when loading a project with `panelIsDirty = 0`. `CtrlPanelFileOperations.cpp`
* **FIXED** modulator not reacting to MIDI input messages. `CtrlrModulatorProcessor.cpp`

#### Version 5.6.30 | 2024.03.13

* Missing JUCE File Class definitions bound to LUA
* New LookAndFeel_V4 colourScheme added (V4 JetBlack, V4 YamDX, V4 AkAPC, V4 AkMPC, V4 LexiBlue, V4 KurzGreen, V4 KorGrey, V4 KorGold, V4 ArturOrange, V4 AiraGreen).
* Colours fixed in the LUA Method Editor and LUA Console
* File>Save As removes panelDirty asterisk suffix
* `uiButton` & `uiImageButton` can show the MIDI Monitor window by selecting it from the `componentInternalFunction` property
* Legacy mode for older panels protects their background colours
* Close button added to LUA Method Editor Tabs (as in 5.1.198, 5.2 & 5.3 versions)
* LUA Method Editor Tabs won't shrink and will show a `+` sign if the TabBar exceeds the window width
* Close button added to Panel Editor Tabs (as in 5.1.198, 5.2 & 5.3 versions)
* Panel Editor Tabs won't shrink and will show a `+` sign if the TabBar exceeds the window width

#### Version 5.6.29

* Implementation of the entire JUCE LookAndFeel_V4 design with all color schemes
* Panels designed on previous versions (5.3.198 & 5.3.220) are compatible and will automatically use LookAndFeel_V2/V3
* Implementation of the JUCE ColourSelector popup for every colour property
* Description/ID in the property pane switches without selecting other tabs to update
* Overall improvement of Ctrlr GUI, component settings and functionalities

#### Version 5.6.28

* Added support for scalable UI for responsive design via callback on APP/Plugin viewport resize and viewport resize parameters.
* Fixed menuBar not showing up issue for non-restricted exported instance.

#### Version 5.6.27

* `uiPanelViewPortBackgroundColour` property added in the global properties to change the background color of the ViewPort, parent of the Panel canvas.

#### Version 5.6.26

* `setChangeNotificationOnlyOnRelease` added in the component section for all types of sliders. When enabled, it sends the Value only when the mouse button is released.

#### Version 5.6.25

* `mouseUp`, `mouseEnter`, `mouseExit` callbacks added in for Generic Components (buttons, sliders etc)

#### Version 5.6.24

* `ctrlrEditor` window showing scrollbars over canvas in Cubase has been fixed

#### Version 0.0.0

* Current version is forked from Ctrlr 5.6.23
* Requires unified versioning pattern

---

[← 05 — Exporting Instances](05-exporting-instances.md) | [Main Index](../../README.md)