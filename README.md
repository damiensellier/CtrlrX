![alt text](/Source/Resources/Icons/CtrlrX-README-250x315.png?raw=true "CtrlrX")



About
-----

CtlrX is an alternative fork of Roman Kubiak's Ctrlr.
This project is ONLY aimed at delivering updates, a wiki, documentation, tutorials or anything that the community cannot share on the original Ctrlr github due to credential restrictions. 
Let's keep the Ctrlr github alive and keep up with what we were all doing there. But for anything that deserves special credentials unavailable there, just let's do it here on CtrlrX.


ToDo
----

* Update to the latest version of JUCE 7
* Upgrade LUABIND to another LUA/C++ library
* Implementation of the entire JUCE ValueTree Class with LUA
* File path needs to be updated on save while the panel ID is changed (Save vs. Save as...)
* Re-generating UID must update resource path automatically
* Modulators located within a tabs must have their properties updated while the tab properties are changed


Changelog
---------
#### Version 5.6.34 | 2025.06.16

* NEW ProTools AAX plugin support (requires codesigning via PACE)

* UPDATED LuaBind Drawable class. LCore.cpp, LGraphics.cpp
* UPDATED LuaBind function addChild() to valueTree class. LCore.cpp
* ADDED LuaBind function jmap() mapToLog10(), mapFromLog10(), isWithin() to GlobalFunctions. LCore.cpp
* ADDED LuaBind Parse XML support to XmlDocument class. LCore.cpp



#### Version 5.6.33 | 2025.05.28
* FIXED VST3 Host>CtrlrX interface. Sliders now reacts to Host automations of parameter values. Related Modulators send MIDI output messages. CtrlrProcessor.cpp & .h, CtrlrModulator.cpp & .h, CtrlrModulatorProcessor.cpp & .h
* FIXED Linux Makefile. CtrlrX requires binutils-dev & libsframe1 installed on the system to compile. Thanks @sgorpi for the PR. Builds/Linux/Makefile/Makefile
* FIXED ADD, REMOVE, RELOAD resource pane buttons not reacting on certain setups. Z-index added. Thanks to @dnaldoog. CtrlrPanelResourceEditor.cpp
* ADDED Property line height base value in Preferences>GUI. CrrlrIDs.h, CtrlrIDs.xml, CtrlrSettings.cpp, CtrlrManager.cpp, CtrlrManagerInstance.cpp, CtrlrPropertyComponent.cpp
* ADDED LuaBind functions setType(), setMidiMessageType(), setProperty(). CtrlrMidiMessage.cpp
* ADDED "Encrypt exported panel resources", "Delay between steps at export" & "Codesign exported panel" properties to improve panel export process. CrrlrIDs.h, CtrlrIDs.xml, CtrlrMac.cpp, CtrlrPanel.cpp
* FIXED Useless menu item "Register file extensions" hidden for macOS binaries. CtrlrEditorApplicationCommandsMenus.cpp
* ADDED LuaBind function addColumnBreak() for PopupMenu. LComponents.cpp
* ADDED SliderType LinearBarVertical, RotaryHorizontalVerticalDrag, TwoValueHorizontal, TwoValueVertical, ThreeValueHorizontal, ThreeValueVertical. CtrlrComponentTypeManager.h & CtrlrComponentTypeManager.cpp, CtrlrIDs.xml

#### Version 5.6.32 | 2025.04.05
* NEW From now on, uisliders will return double float type values to manage decimals. If required values are integers,  new LUA function has been added : getValueInt()
getModulatorInt(), getModulatorValueInt(), getMinModulatorValueInt(), getMaxModulatorValueInt(), getValueMappedInt(), getValueNonMappedInt(), getMinMappedInt(), getMaxMappedInt()

* NEW VST3 can now export instances without the need to compile intermediate plugins from the Projucer. VST3 identifiers will be taken from the panel plugin name, plugin ID, Panel Author and manufacturer ID as well as the VST3 plugin type. Exported VST3 plugins are codesigned on export directly with JUCE childProcess() automatically, either with local ad-hoc signature or, if selected, with a developer certificate.

* UPDATED Exported instances will now hide the preferences and shortcut menu items. CtrlrEditorApplicationCommandsMenus.cpp
* UPDATED Decimal value are now supported for Sliders (ie. 3.1416). CtrlrModulator.cpp & h, CtrlrModulatorProcessor.cpp & h, CtrlrIDs.xml, CtrlrLuaManager.cpp, CtrlrLuaMethodManager.h, CtrlrLuaMethodManagerCalls.cpp, CtrlrFixedImageSlider.cpp, CtrlrFixedImageSlider.cpp, CtrlrImageSlider.cpp, CtrlrSliderInternal.cpp & h
* UPDATED Decimal interval steps are now supported for Sliders (ie. 0.1)
* FIXED Useless shortcuts such as "New Panel", "Export" enabled on restricted instances. CtrlrEditorApplicationCommands.cpp
* FIXED Build Timestamp not updating on macOS. CtrlrX.jucer, CtrlrRevision.h
* FIXED FileChooser still hanging when exporting instance on OSX Catalina & macOS BigSur. CtrlrManager.cpp
* FIXED Windows crashing when loading a panel with a faulty modulator callback on value change LUA script. luabind/detail/call_function.hpp, CtrlrModulatorProcessor.cpp
* ADDED Slider value Suffix (ie. Hz, ms, dB, etc)
* ADDED JUCE systemStats support for WIN11, macOS 11, macOS 12, macOS 13, macOS 14, macOS 15 and macOS 16. juce_mac_systemStats.cpp, juce_win32_systemStats.cpp, juce_systemStats.h
* ADDED Enable/Disable  "Run modulator valueChange LUA callback in Bootstrap state". CtrlrManagerInstance.cpp, CtrlrModulatorProcessor.cpp, CrrlrIDs.h, CtrlrIDs.xml, CtrlrSettings.cpp

#### Version 5.6.31 | 2025.01
* Security update : Encryption of the panel file in the macOS bundle for restricted instances. CtrlrMac.cpp, CtrlrEditorApplicationCommandsMenu.cpp, CtrlrManager.cpp

* NEW JUCE Class MouseInputSource added to LUA. LCore.cpp,LCore.h, LJuce.cpp, LJuce.h, LMouseInputSource.h
* NEW algorithm for Roland checksums. CtrlrSysexProcessor.cpp & CtrlrUtilities.cpp
* NEW settings for LUA Method Editor. CtrlrValueTreeEditor.h CtrlrLuaMethodCodeEditorSettings.cpp & .h CtrlrLuaMethodCodeEditor.cpp CtrlrLuaMethodEditor.cpp CtrlrIDs.h & CtrlrIDs.xml

* FIXED MSB 14 bit numbers sending 0xFF instead of 0x7F. CtrlrSysexProcessor.cpp & CtrlrUtilities.cpp
* FIXED missing File Management bottom notification bar. CtrlrPanelFileOperations.cpp, CtrlrPanelEditor.cpp, CtrlrPanelEditor.h, CtrlrPanel.cpp, CtrlrPanel.h
* FIXED missing menuBar on export for log and MIDI monitor windows. CtrlrChildWindowContainer.cpp
* FIXED uiSlider value not reaching maxValue when using negative values for minValue. CtrlrSlider.cpp
* FIXED uiImageSlider value not reaching maxValue when using negative values for minValue. CtrlrImageSlider.cpp
* FIXED black text on black background for Modulator List window. CtrlrPanelModulatorList.cpp
* FIXED typo in alert when closing dirty panel. CtrlrPanelFileOperations.cpp
* FIXED parameters count passed to VST host is set from the highest vstindex when a panel is an exported VST/VST3 instance, not (64). CtrlrProcessor.cpp 
* FIXED LnF panel close button colour on mouseover follows the panel colourScheme. CtrlrDocumentPanel.cpp 
* FIXED CtrlrModulator Value statement precised to help avoid feedback loops between LUA and (delayed) UI. Commit 6e5a0b2 by midibox. CtrlrLuaManager.cpp
* FIXED exported VST crashing DAW if panelIsDirty = 0 on export. CtrlPanelFileOperations.cpp
* FIXED Ctrlr not showing up in Ableton Live. CtrlrX.jucer, CtrlrProcessor.cpp & CtrlrProcessor.h, CtrlrProcessorEditorForLive.cpp & CtrlrProcessorEditorForLive.h
* FIXED MIDI Monitor IN/OUT turned ON by default. CtrlrManager.cpp
* FIXED Console window & Midi Monitor crashing Cubase if closed from the Menu File>Close. CtrlrLuaConsole.cpp & CtrlrMIDIMonitor.cpp
* FIXED LUA mod:getMidiMessage():getProperty("propertyName") CtrlrMidiMessage.cpp & CtrlrMidiMessage.h
* FIXED exported instances not getting the proper LnF version or colourScheme (popup, child windows etc). CtrlrManagerInstance.cpp, CtrlrEditor.cpp
* FIXED property pane tabs not showing up on exported instances. CtrlrManagerInstance.cpp
* FIXED Property Pane tabs not following panel LnF. CtrlrPanelProperties.cpp & CtrlrPanelProperties.h
* FIXED Child Windows (LUA Editor, console etc) not getting the proper menuBar background. CtrlrChildWindowContainer.cpp
* FIXED LUA Editor Method Tree selected item not getting the proper colour. CtrlrLuaMethodEditor.cpp
* FIXED AU AudioUnit version of CtrlX was failing the Apple/Logic Validation Test. CtrlrX.jucer, CtrlrProcessor.cpp & CtrlrProcessor.h
* FIXED Panel Tabs showing up on exported instances. CtrlrDocumentPanel.cpp
* FIXED messy preferences window and settings. CtrlrSettings.cpp & CtrlrSettings.cpp ctrlrIDs.h & CtrlrIDs.xml
* FIXED default New Panel LnF not following global LnF from Global Preferences. ctrlrPanel.cpp ctrlrIDs.h & CtrlrIDs.xml
* FIXED CtrlrX.ico too dark to be legible on dark backgrounds. ctrlr_logo_circle_v3.svg
* FIXED FileChooser hanging when exporting instance on OSX Catalina & macOS BigSur. CtrlrManager.cpp
* FIXED About popup design refurbished with new CtrlrX logo. CtrlrAbout.cpp & CtrlrAbout.h
* FIXED About popup not getting current build date, fixed with C++ Macro timestamp. CtrlrRevision.h
* FIXED VST crashing DAW when loading a project with panelIsDirty = 0. CtrlPanelFileOperations.cpp
* FIXED modulator not reacting to MIDI input messages. CtrlrModulatorProcessor.cpp


#### Version 5.6.30 | 2024.03.13
* Missing JUCE File Class definitions bound to LUA
* New LookAndFeel_V4 colourScheme added (V4 JetBlack, V4 YamDX, V4 AkAPC, V4 AkMPC, V4 LexiBlue, V4 KurzGreen, V4 KorGrey, V4 KorGold, V4 ArturOrange, V4 AiraGreen).
* Colours fixed in the LUA Method Editor and LUA Console
* File>Save As removes panelDirty asterisk suffix
* uiButton & uiImageButton can show the MIDI Monitor window by selecting it from the componentInternalFunction property
* Legacy mode for older panels protects their background colours
* Close button added to LUA Method Editor Tabs (as in 5.1.198, 5.2 & 5.3 versions)
* LUA Method Editor Tabs won't shrink and will show a + sign if the TabBar exceeds the window W
* Close button added to Panel Editor Tabs (as in 5.1.198, 5.2 & 5.3 versions)
* Panel Editor Tabs won't shrink and will show a + sign if the TabBar exceeds the window W


#### Version 5.6.29

* Implementation of the entire JUCE LookAndFeel_V4 design with all color schemes
* Panels designed on previous versions (5.3.198 & 5.3.220) are compatible and will automatically use LookAndFeel_V2/V3
* Implementation of the JUCE ColourSelector popup for every coloUr properties
* Description/ID in the property pane switches without selecting other tabs to update
* Overall improvement of Ctrlr GUI, component settings and functionalities


#### Version 5.6.28
*  Added support for scalable UI for responsive design via callback on APP/Plugin viewport resize and viewport resize parameters.
*  Fixed menuBar not showing up issue for non-restricted exported instance.


#### Version 5.6.27
*  uiPanelViewPortBackgroundColour property added in the global properties to change the background color of the ViewPort, parent of the Panel canvas.


#### Version 5.6.26
*  setChangeNotificationOnlyOnRelease added in the component section for all types of sliders. When enabled, it sends the Value only when the mouse button is released.


#### Version 5.6.25
* mouseUp, mouseEnter, mouseExit callbacks added in for Generic Components (buttons, sliders etc)


#### Version 5.6.24
* ctrlrEditor window showing scrollbars over canvas in Cubase has been fixed


#### Version 0.0.0
* Current version is forked from Ctrlr 5.6.23
* Requires unified versioning pattern



# About Ctrlr

Control any MIDI enabled hardware: synthesizers, drum machines, samplers, effects. Create custom User Interfaces. Host them as VST or AU plugins in your favorite DAWs.


Cross Platform
--------------
Works on Windows (XP and up, both 64 and 32bit binaries are available), macOS (10.5 and up), Linux (any modern distro should run it).
Host in your DAW

Each platform has a VST build of Ctrlr so you can host your panels as normal VST plugins, for macOS a special AU build is available.

Customize
---------
Each Panel can be customized by the user, the panels are XML files, every panel can be Edited in Ctrlr to suite your specific needs.

Open Source
-----------
Need special functionality or want to propose a patch/feature update, know a bit about C++/JUCE framework etc. You can always download the source code and build Ctrlr by yourself.

Extend
------
With the scripting possibilities inside Ctrlr you can extend you panels in various ways. The LUA scripting language gives you access to all panels elements and hooks to various events.


# How to build Ctrlr


## Windows

Summary will be added here in the future, links to pdf of build guides by @bijlevel and @dnaldoog can be found here

[Compiling on Windows 11 with Visual Studio 2022](https://github.com/user-attachments/files/19642077/How.to.compile.Ctrlr.or.CtrlrX.5.6.versions.in.Visual.Studio.2022.pdf)

[Compiling on Windows 10 with Visual Studio 2019](https://godlike.com.au/fileadmin/godlike/techtools/ctrlr/guides/Compiling_Ctrlr_for_Windows_10_v2.1.pdf)


## macOS

[Compiling on OSX](https://godlike.com.au/fileadmin/godlike/techtools/ctrlr/guides/My_guide_to_compiling_Ctrlr_for_macOS__Mojave__v2.pdf)


## Linux

A build.sh script is provided in Builds/Generated/Linux/Standalone, a symlink of that
script is location in Builds/Generated/Linux/VST and is used to create a precompiled header
and then to trigger the build using make. You can do that manualy if you like just have
a look at the script, it's really simple.

A more complex solution exists in Scripts/post-commit, this script will build all solutions
for the current architecture, it will also prepare the system for the build, unpack boost.zip
check some packages (on Ubuntu only for now) and start the build. At the end it will create
a self extracting Ctrlr.sh file in Packaging/Linux (create using makeself.sh), it will also
try to scp it to ctrlr.org but that will fail without the correct ssh key, you can just comment
out the scp line in post-commit.

The post-commit script takes an argument "clean" if you wish to clean all the intermididate
files before building. If you want to ignore any package errors that it reports (i assume you
know your system better then my script) then just add -f as an option when building.

# How to export plugin instances (AU, AUv3, VST & VST3)

## VST2 Support
Since Steinberg has discontinued the VST2 API we no longer distribute a VST2. If you are a licensee to the VST2SDK, though, you can still build it. 
The first thing is to be sure to check the path to the VST2 sdk (only available from Steinberg's VST3 directory sdk) in Projucer before calling any script builds.

## VST3 Support
### UPDATE v5.6.32+
VST3 exported instances of CtrlrX panels are finally working properly. On macOS, you need to export VST3 with at least an ad-hoc signature. This process is managed automatically by CtrlrX, if you have an Apple developer certificate, select it in the appropriate field, it will prevent the VST3 to be held by the Gatekeeper. If you want to sign your WIN exported instances with a PFX Certificate, it's now also possible.

If for some reasons you still want to produce your own VST3 intermediates you can simply disable the unique identifiers replacement process from the appropriate field.

### Prior v5.6.32
Currently VST3 instances of CTRLR panels are not working properly because CTRLR is not able to generate different VST3 compliant plugin identifiers. 
Unfortunately, exported VST3 instances of your panel will always be named after CTRLR | Instigator. 
The only way to get the correct identifiers for a panel project is to force them at the core during the building step of the VST3 in Xcode/VS/IDE.
To export properly identified VST3 plugins it is then required to build a different stock CTRLR VST3 plugin with JUCE Projucer and Xcode/VS/IDE. 
However, this alternative version of CTRLR VST3 will have the desired panel/plugin identifiers predefined in the Projucer settings. (Plugin Name, Manufacturer Name, Plugin ID, Manufacturer ID etc). 
This intermediate VST3 plugin will then be able to export a final VST3 version of the panel with the proper identifiers. 

## AU Support
Tutorial coming soon

## AUv3 Support
Tutorial coming soon

# Credits
* Thanks to @romankubiak for developing Ctrlr
* Links to contributors coming soon

# Notes
* VST is a registered trademark of Steinberg Media Technologies GmbH
