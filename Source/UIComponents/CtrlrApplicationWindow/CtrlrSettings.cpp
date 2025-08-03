#include "stdafx.h"
#include "CtrlrSettings.h"

CtrlrSettings::CtrlrSettings (CtrlrManager &_owner) : Component ("Global Properties"), owner(_owner), propertyPanel (0)
{
    addAndMakeVisible (propertyPanel = new PropertyPanel());
    
    propertyPanel->setName ("propertyPanel");
    
    Array <PropertyComponent*> globalProperties;
    Array <PropertyComponent*> midiProperties;
    Array <PropertyComponent*> guiProperties;
    Array <PropertyComponent*> debugProperties;
    Array <PropertyComponent*> directoriesProperties;
    
    Array <PropertyComponent*> emptyProperties; // Placeholder
    Array <PropertyComponent*> instanceProperties; // Useless
    
// Removed v5.6.31
//	for (int i=0; i<owner.getManagerTree().getNumProperties(); i++)
//	{
//		globalProperties.add
//		(
//			// owner.getIDManager().createComponentForProperty (owner.getManagerTree().getPropertyName(i), owner.getManagerTree(), nullptr)
//		);
//	}
    
    // Ctrlr Behaviour section
    globalProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrVersionSeparator"), owner.getManagerTree(), nullptr));
    globalProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrVersionCompressed"), owner.getManagerTree(), nullptr));
    globalProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrAutoSave"), owner.getManagerTree(), nullptr));
    globalProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrAutoSaveInterval"), owner.getManagerTree(), nullptr));

    // Useless because ctrlrShutdownDelay is overriden in ctrlrProcessor.cpp to 512
    // #ifdef JUCE_MAC
    // globalProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrShutdownDelay"), owner.getManagerTree(), nullptr)); // ifdef JUCE_OSX not working
    // #endif
    
    globalProperties.add(owner.getIDManager().createComponentForProperty(Identifier("luaCtrlrSaveState"), owner.getManagerTree(), nullptr));
    globalProperties.add(owner.getIDManager().createComponentForProperty(Identifier("luaCtrlrRestoreState"), owner.getManagerTree(), nullptr));

    // Midi section
    // midiProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrLogMidiInput"), owner.getManagerTree(), nullptr)); // Useless, not assigned
    // midiProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrLogMidiOutput"), owner.getManagerTree(), nullptr)); // Useless, not assigned
    // midiProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrLogOptions"), owner.getManagerTree(), nullptr)); // Useless because it helps storing Monitor Log options as Byte value
    midiProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrMidiMonInputBufferSize"), owner.getManagerTree(), nullptr));
    midiProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrMidiMonOutputBufferSize"), owner.getManagerTree(), nullptr));

    // GUI section
    // guiProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrPropertiesAreURLs"), owner.getManagerTree(), nullptr)); // Useless, not assigned
    guiProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrNativeFileDialogs"), owner.getManagerTree(), nullptr));
    guiProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrNativeAlerts"), owner.getManagerTree(), nullptr));
    guiProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrPropertyLineImprovedLegibility"), owner.getManagerTree(), nullptr));
    // guiProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrUseEditorWrapper"), owner.getManagerTree(), nullptr)); // Useless, only for Ableton live.  Automatically set to true via CtrlrProcessor.cpp OS & DAW detection
    
    if (JUCEApplication::isStandaloneApp())
    {
        guiProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrLookAndFeel"), owner.getManagerTree(), nullptr));
        guiProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrColourScheme"), owner.getManagerTree(), nullptr));
    }

    guiProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrFontSizeBaseValue"), owner.getManagerTree(), nullptr));
    guiProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrScrollbarThickness"), owner.getManagerTree(), nullptr));
    guiProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrMenuBarHeight"), owner.getManagerTree(), nullptr));
    guiProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrTabBarDepth"), owner.getManagerTree(), nullptr));
    guiProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrPropertyLineheightBaseValue"), owner.getManagerTree(), nullptr)); // Added v5.6.33.
    
    // Debug section
    debugProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrLogToFile"), owner.getManagerTree(), nullptr));
    debugProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrWarningInBootstrapState"), owner.getManagerTree(), nullptr));
    debugProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrLuaDebug"), owner.getManagerTree(), nullptr));
    debugProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrLuaDisabled"), owner.getManagerTree(), nullptr));
    debugProperties.add(owner.getIDManager().createComponentForProperty(Identifier("uiLuaConsoleInputRemoveAfterRun"), owner.getManagerTree(), nullptr)); // Clear Console After Execute
    
    // Directory section
    directoriesProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrRecenetOpenedPanelFiles"), owner.getManagerTree(), nullptr));
    directoriesProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrLastBrowsedFileDirectory"), owner.getManagerTree(), nullptr));
    directoriesProperties.add(owner.getIDManager().createComponentForProperty(Identifier("ctrlrLastBrowsedResourceDir"), owner.getManagerTree(), nullptr));
    
    // propertyPanel->addSection("Global | Restart to apply settings", globalProperties, true);
    propertyPanel->addSection("Global", globalProperties, true);
    propertyPanel->addSection("MIDI", midiProperties, true);
    propertyPanel->addSection("GUI", guiProperties, true);
    propertyPanel->addSection("Debug", debugProperties, true);
    propertyPanel->addSection("Paths", directoriesProperties, true);
    
	propertyPanel->getViewport().setScrollBarThickness(owner.getManagerTree().getProperty(Ids::ctrlrScrollbarThickness));
    
    const int totalContentHeight = propertyPanel->getTotalContentHeight(); // Added v5.6.31. Returns totalContentHeight
    setSize (600, totalContentHeight);
}

void CtrlrSettings::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (DocumentWindow::backgroundColourId)); // Added v5.6.31
}

void CtrlrSettings::resized()
{
    propertyPanel->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
}

CtrlrSettings::~CtrlrSettings()
{
    deleteAndZero (propertyPanel);
    
    if (JUCEApplication::isStandaloneApp())
    {
    // Show Ok/Cancel dialog to confirm restart
    const auto callbackRestart = juce::ModalCallbackFunction::create([this] (int resultBox){
        if (resultBox == 0){nullptr;} // for Cancel
        if (resultBox == 1){restart();} // for OK
    });
    juce::NativeMessageBox::showOkCancelBox(juce::AlertWindow::QuestionIcon,"CtrlrX", "Restarting CtrlrX is required to apply new settings.", nullptr, callbackRestart);
    }
    else{
        // For VST/AU instances
        AlertWindow::showMessageBox (AlertWindow::WarningIcon, "CtrlrX", "Restart to apply new settings."); // Added v5.6.31
    }
}

void CtrlrSettings::restart()
{
    // Check if multiple instances are allowed
    auto runningInstance = JUCEApplication::getInstance();
    bool multiInstance = runningInstance->JUCEApplication::moreThanOneInstanceAllowed();
        
    if (multiInstance) {
        Logger::writeToLog("Multiple instances allowed.");
    }
    else{
        Logger::writeToLog("Multiple instances not allowed.");
    }
    
    String executablePath = File::getSpecialLocation(File::currentExecutableFile).getFullPathName();
    File exeFile = File(executablePath);
    
    if (exeFile.exists())
    {
        Logger::writeToLog("Executable file found");
        
        int result = 1; // Assume failure initially
        
        auto osType = SystemStats::getOperatingSystemType();
        Logger::writeToLog("OS type: " + String(osType));
        
        String osName = SystemStats::getOperatingSystemName();
        Logger::writeToLog("OS Name: " + String(osName));
        
        if ((SystemStats::getOperatingSystemType() & juce::SystemStats::MacOSX) != 0) {  // To test whether any version of OSX is running
            // For OSX & macOS
            Logger::writeToLog("Launching on macOS using ChildProcess.");
            ChildProcess childProcess;
            
            
            // Attempt to start the process with ChildProcess
            if (childProcess.start(exeFile.getFullPathName())) {
                Logger::writeToLog("ChildProcess start() successful.");
                result = 0;
            } else {
                Logger::writeToLog("ChildProcess start() failed.");

                // If ChildProcess fails, try startAsProcess()
                Logger::writeToLog("Trying startAsProcess().");
                result = exeFile.startAsProcess();
                if (result == 0) {
                    Logger::writeToLog("startAsProcess() successful.");
                } else {
                    Logger::writeToLog("startAsProcess() failed.");
                }
            }
        }
        
        else if (SystemStats::getOperatingSystemType() == SystemStats::Windows){
            // For Windows
            Logger::writeToLog("Launching on Windows using exeFile.startAsProcess().");
            result = exeFile.startAsProcess();
        }
        
        else{
            // Handle unsupported operating systems
            Logger::writeToLog("Launching on other operating system.");
            std::cerr << "Launching on other operating system." << std::endl;
            result = 1;
        }
                
        if (result == 1){
            // Handle error (e.g., log error message)
            std::cerr << "Error launching executable: " << result << std::endl;
            Logger::writeToLog("Error launching executable: " + String(result));
        }
        else
        {
            Logger::writeToLog("Executable launched successfully.");
        }
    }
    else{
        // Handle error: executable file not found
        std::cerr << "Executable file not found." << std::endl;
        Logger::writeToLog("Executable file not found.");
    }
        
    Logger::writeToLog("Sending quit request to current application.");
    JUCEApplication::getInstance()->systemRequestedQuit();
    Logger::writeToLog("Quit request sent. Waiting for 500ms.");
    Thread::sleep(500);
}
