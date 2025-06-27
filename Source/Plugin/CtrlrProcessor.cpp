#include "stdafx.h"
#include "CtrlrUtilities.h"
#include "CtrlrProcessor.h"
#include "CtrlrApplicationWindow/CtrlrEditor.h"
#include "CtrlrManager/CtrlrManager.h"
#include "CtrlrProcessorEditorForLive.h"
#include "CtrlrMacros.h"
#include "CtrlrLog.h"
#include "CtrlrPanel/CtrlrPanelMIDIInputThread.h"
#include "CtrlrPanel/CtrlrPanel.h"

const uint32 magicXmlNumber = 0x00001040;


CtrlrProcessor::CtrlrProcessor() :
                                    #ifndef JucePlugin_PreferredChannelConfigurations
                                    AudioProcessor (BusesProperties()
                                        #if ! JucePlugin_IsMidiEffect
                                        //#if ! JucePlugin_IsSynth // Removed v5.6.32. Was disabling all required Inputs for Audio Channel Insert FX Mode
                                                    .withInput  ("Input",  AudioChannelSet::stereo(), true)
                                        //#endif
                                                    .withOutput ("Output", AudioChannelSet::stereo(), true)
                                        #endif
                                                    ),
                                        #endif

                                    overridesTree (Ids::ctrlrOverrides),
                                    ctrlrManager (nullptr),
                                    ctrlrLog (nullptr) // Added v5.6.34. Could be useful
{
	_DBG("CtrlrProcessor::ctor");

	midiCollector.reset (SAMPLERATE);
    
	currentExec		= File::getSpecialLocation(File::currentApplicationFile);
	overridesFile	= currentExec.withFileExtension("overrides");

	overridesTree.setProperty (Ids::ctrlrMaxExportedVstParameters, 64, 0);
	overridesTree.setProperty (Ids::ctrlrShutdownDelay, 512, 0);

	if (overridesFile.existsAsFile())
	{
		ScopedPointer <XmlElement> xml (XmlDocument(overridesFile).getDocumentElement().release());
		if (xml)
		{
			overridesTree = ValueTree::fromXml (*xml);
		}
	}

    #if JUCE_DEBUG // Added v5.6.34. Will show the debug log. Was set to (false) by default from the CtrlrManager property ctrlrLogToFile.
    // If we are in a Debug build, force logging ON
        ctrlrLog                = new CtrlrLog(true);
    #else
    // If we are in any other build (like Release), force logging OFF
        ctrlrLog                = new CtrlrLog(overridesTree.getProperty (Ids::ctrlrLogToFile));
    #endif
    
	ctrlrManager			= new CtrlrManager(this, *ctrlrLog);

	if (!ctrlrManager->initEmbeddedInstance())
	{
		ctrlrManager->setDefaults();
	}

	ctrlrManager->setManagerReady();

	if (ctrlrManager->getInstanceMode() == InstanceSingle || ctrlrManager->getInstanceMode() == InstanceSingleRestriced)
	{
		if (ctrlrManager->getActivePanel())
		{
			ctrlrManager->getActivePanel()->initEmbeddedInstance();
		}
	}
}

CtrlrProcessor::~CtrlrProcessor() // Updated v5.6.34. Prevents AAX from crashing when deleting the plugin from the instrument track insert slot.
{
    // ***** CRITICAL AAX-SPECIFIC WORKAROUND *****
    // This entire block is excluded for AAX builds because:
    // 1. The MessageManager::runDispatchLoopUntil() call causes crashes on AAX plugin removal.
    //    (Confirmed even when property exists and has a default value).
    // 2. Explicitly deleting ctrlrLog (if it were done here) caused crashes on AAX plugin startup scan.
    // The safest approach for AAX is to do minimal work for these components in the destructor.
    #ifndef JucePlugin_Build_AAX
        // For all plugin formats *EXCEPT* AAX:
        // Perform explicit deletion for the raw pointer (ctrlrLog).
        // ScopedPointer (ctrlrManager) will handle its own deletion automatically.
        if (ctrlrLog != nullptr)
        {
            delete ctrlrLog;
            ctrlrLog = nullptr;
        }

        #ifdef JUCE_MAC
            // This line was causing crashes on AAX removal, so it's excluded for AAX by the #ifndef above.
            // It remains here for other macOS builds if it's considered necessary for them. Mainly for panels with timer process handled in the background and delayed saveState() process.
            MessageManager::getInstance()->runDispatchLoopUntil((int)overridesTree.getProperty(Ids::ctrlrShutdownDelay));
        #endif
    #else
        // For JUCE_AAX builds:
        // We ensure raw pointers are nullified without deletion to avoid crashes during scan/removal.
        // ScopedPointer will still automatically delete its content (ctrlrManager) when the CtrlrProcessor object is destroyed.
        ctrlrLog = nullptr;
    #endif
}


// NAMING FROM getName() WORKS ONLY FOR WIN VST2.
// macOS VST2 ARE NAMED AFTER CFBundleName string in /Contents/Info.plist

const String CtrlrProcessor::getName() const
{
	if (ctrlrManager)
    {
        return (ctrlrManager->getInstanceNameForHost());
    }
	else
    {
		return ("CtrlrX"); // Updated v5.5.34
    }
}


void CtrlrProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    midiCollector.reset (sampleRate);
    leftoverBuffer.ensureSize (8192);
}

void CtrlrProcessor::releaseResources()
{
}

//void CtrlrProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages) // Deprecated v5.6.34
//{
//    // _DBG("processBlock MIDI: NumEvents=" + String(midiMessages.getNumEvents()) + ", IsEmpty=" + (midiMessages.isEmpty() ? "true" : "false"));
//    AudioPlayHead::CurrentPositionInfo info;
//    if (getPlayHead())
//    {
//        getPlayHead()->getCurrentPosition(info);
//    }
//
//    if (midiMessages.getNumEvents() > 0)
//    {
//        processPanels(midiMessages, info);
//    }
//
//
//    midiCollector.removeNextBlockOfMessages (midiMessages, (buffer.getNumSamples() > 0) ? buffer.getNumSamples() : 1);
//    MidiBuffer::Iterator i(midiMessages);
//    while (i.getNextEvent(logResult, logSamplePos))
//    _MOUT("VST OUTPUT", logResult, logSamplePos);
//}


void CtrlrProcessor::processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer& midiMessages) // Updated v5.6.34
{
    // _DBG("processBlock MIDI: NumEvents=" + String(midiMessages.getNumEvents()) + ", IsEmpty=" + (midiMessages.isEmpty() ? "true" : "false"));
    
    // 1. Clear audio buffer
    buffer.clear();

    // 2. Get playhead info (optional)
    AudioPlayHead::CurrentPositionInfo info;
    if (getPlayHead())
    {
        getPlayHead()->getCurrentPosition(info);
    }

    // 3. Add incoming host MIDI to your collector via your custom function
    // This will now apply the 0-timestamp workaround inside addMidiToOutputQueue (const MidiBuffer &buffer)
    if (midiMessages.getNumEvents() > 0) // Only process if there are events
    {
        addMidiToOutputQueue (midiMessages);
    }

    // 4. Call processPanels with the raw host input (as you had it)
    if (midiMessages.getNumEvents() > 0)
    {
        processPanels(midiMessages, info);
    }

    // 5. Clear the incoming midiMessages buffer for output
    midiMessages.clear();

    // 6. Get MIDI messages from the collector for output
    // (This part of midiCollector should still work as intended for output)
    midiCollector.removeNextBlockOfMessages (midiMessages, buffer.getNumSamples());

    // 7. Log outgoing MIDI (if enabled and declared locally)
    /*
    MidiBuffer::Iterator i(midiMessages);
    juce::MidiMessage logResult;
    int logSamplePos;
    while (i.getNextEvent(logResult, logSamplePos))
    {
        _MOUT("VST OUTPUT", logResult, logSamplePos);
    }
    */
}

//==============================================================================

// These methods are all deprecated in favour of using AudioProcessorParameter
// and AudioProcessorParameterGroup
//
//JUCE_DEPRECATED (virtual int getNumParameters());
//JUCE_DEPRECATED (virtual const String getParameterName (int parameterIndex));
//JUCE_DEPRECATED (virtual String getParameterID (int index));
//JUCE_DEPRECATED (virtual float getParameter (int parameterIndex));
//JUCE_DEPRECATED (virtual String getParameterName (int parameterIndex, int maximumStringLength));
//JUCE_DEPRECATED (virtual const String getParameterText (int parameterIndex));
//JUCE_DEPRECATED (virtual String getParameterText (int parameterIndex, int maximumStringLength));

int CtrlrProcessor::getNumParameters() // Updated 5.6.31. VST Host was assigned (64) params most of the time since panels hardly have more than 64 params passed as VST controls
{
    if (ctrlrManager)
        if (ctrlrManager->isSingleInstance()) // Added v5.6.31
        {
            return (ctrlrManager->getNumModulators(true)); // Added v5.6.31. Will pass the highest vstIndex value as the total number of VST params to the host.
        }
        else
        {
            return (jmax(ctrlrManager->getNumModulators(true), (int)overridesTree.getProperty (Ids::ctrlrMaxExportedVstParameters))); // Pass jmax ctrlrMaxExportedVstParameters value or default (64) params to the host when designing or loading a panel in ctrlr.vst or ctrlr.vst3
        }
    else
        return (CTRLR_DEFAULT_PARAMETER_COUNT);
}

String CtrlrProcessor::getParameterID(int index) // Added v5.6.33. Will pass vstIndex as VST3 ID
{
    //File debugLog = File::getSpecialLocation(File::currentApplicationFile);
    //PluginLoggerVst3 logger(debugLog); // Create logger instance
    
    //logger.log("getParameterID() called with index: " + String(index));

    if (CtrlrModulator* m = ctrlrManager->getModulatorByVstIndex (index))
    {
        int vstIndex = m->getVstIndex();
        //logger.log("  Found modulator. vstIndex: " + String(vstIndex));
        String idPrefix = "CtrlrMod_";
        String vstIndexString = String(vstIndex);
        String fullId = idPrefix + vstIndexString;
        //logger.log("  ID Prefix: " + idPrefix);
        //logger.log("  vstIndex as String: " + vstIndexString);
        //logger.log("  Constructed ID: " + fullId);
        return fullId;
    }
    else
    {
        String fallbackId = "undefined_ID_" + String(index);
        //logger.log("  No modulator found for index: " + String(index) + ". Returning fallback ID: " + fallbackId);
        return fallbackId;
    }
}

const String CtrlrProcessor::getParameterName (int index)
{
    if (ctrlrManager->getModulatorByVstIndex (index))
    {
        return (ctrlrManager->getModulatorByVstIndex (index)->getNameForHost());
    }
    else
    {
        return ("undefined_"+String(index));
    }
}

float CtrlrProcessor::getParameter (int index) // Updated v5.6.33
{
    CtrlrModulator *m = ctrlrManager->getModulatorByVstIndex (index);
    if (m)
    {
        return (m->getProcessor().getValueForHost());
    }
    else
    {
        return 0.0f;
    }
}

void CtrlrProcessor::setParameter (int index, float newValue)
{
    CtrlrModulator *m = ctrlrManager->getModulatorByVstIndex (index);
    if (m == nullptr)
        return;
    
    m->getProcessor().setValueFromHost(newValue);
}

const String CtrlrProcessor::getParameterText (int index)
{
    if (ctrlrManager->getModulatorByVstIndex (index))
    {
        const String text = ctrlrManager->getModulatorByVstIndex (index)->getTextForHost();
        return (text);
    }
    else
    {
        return ("value_"+String(index));
    }
}

//==============================================================================

double CtrlrProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

//==============================================================================

bool CtrlrProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CtrlrProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CtrlrProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}


int CtrlrProcessor::getNumPrograms()
{
    return 1;
}

int CtrlrProcessor::getCurrentProgram()
{
    return 0;
}

void CtrlrProcessor::setCurrentProgram (int index)
{
	_DBG("CtrlrProcessor::setCurrentProgram id:"+_STR(index));
}

const String CtrlrProcessor::getProgramName (int index)
{
	_DBG("CtrlrProcessor::getProgramName id:"+_STR(index));
    return ("Default CTRLR program");
}

void CtrlrProcessor::changeProgramName (int index, const String& newName)
{
	_DBG("CtrlrProcessor::changeProgramName id:"+_STR(index)+" newName:"+newName);
}

//==============================================================================

#ifndef JucePlugin_PreferredChannelConfigurations
bool CtrlrProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const  // Updated v5.6.31
{
  #if JucePlugin_IsMidiEffect // Ableton Live VST3 requires Midi Effect disabled in ProJucer
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

//==============================================================================

void CtrlrProcessor::addMidiToOutputQueue (CtrlrMidiMessage &m)
{

	for (int i=0; i<m.getNumMessages(); i++)
	{
		m.getReference(i).m.setTimeStamp(i+1);
		midiCollector.addMessageToQueue (m.getReference(i).m);
	}
}

void CtrlrProcessor::addMidiToOutputQueue (const CtrlrMidiMessage &m)
{
	for (int i=0; i<m.getNumMessages(); i++)
	{
		m.getReference(i).m.setTimeStamp(i+1);
		midiCollector.addMessageToQueue (m.getReference(i).m);
	}
}

void CtrlrProcessor::addMidiToOutputQueue (const MidiMessage &m)
{
	midiCollector.addMessageToQueue (m);
}

//void CtrlrProcessor::addMidiToOutputQueue (const MidiBuffer &buffer)
//{
//	MidiBuffer::Iterator i(buffer);
//	MidiMessage m;
//	int time;
//
//	while (i.getNextEvent (m, time))
//	{
//		midiCollector.addMessageToQueue (m);
//	}
//}

void CtrlrProcessor::addMidiToOutputQueue (const MidiBuffer &buffer) // Updated v5.6.34
{
    MidiBuffer::Iterator i(buffer);
    MidiMessage m;
    int time;

    while (i.getNextEvent (m, time))
    {
        // WORKAROUND FOR JASSERT IN JUCE'S INTERNAL MidiMessageCollector::addMessageToQueue
        // If the message is at sample position 0, we temporarily shift it to 1
        // to bypass the strict jassert.
        if (time == 0)
            time = 1;

        // Use the only available method, applying the corrected 'time'
        // JUCE's addMessageToQueue *expects* the message's timestamp to be set.
        m.setTimeStamp(time); // Set the timestamp of the MidiMessage itself
        midiCollector.addMessageToQueue (m);
    }
}

//==============================================================================

void CtrlrProcessor::processPanels(MidiBuffer &midiMessages, const AudioPlayHead::CurrentPositionInfo &positionInfo)
{
	leftoverBuffer.clear();

	for (int i=0; i<panelProcessors.size(); i++)
	{
		if (!panelProcessors[i].wasObjectDeleted())
		{
			panelProcessors[i]->processBlock (midiMessages, leftoverBuffer, positionInfo);
		}
	}
	midiMessages.swapWith(leftoverBuffer);
}

//==============================================================================

bool CtrlrProcessor::hasEditor() const
{
    return true;
}

//==============================================================================

const var &CtrlrProcessor::getProperty (const Identifier& name) const
{
	if (ctrlrManager)
	{
		if (ctrlrManager->getManagerTree().hasProperty(name))
			return (ctrlrManager->getProperty(name));
		else
			return (overridesTree.getProperty(name));
	}
	else
	{
		return (overridesTree.getProperty(name));
	}
}

bool CtrlrProcessor::hasProperty(const Identifier &name) const
{
	if (ctrlrManager->getManagerTree().hasProperty(name) || overridesTree.hasProperty(name))
		return (true);
	else
		return (false);
}

void CtrlrProcessor::activePanelChanged()
{
	sendChangeMessage();
}


bool CtrlrProcessor::useWrapper()
{
	if (JUCEApplication::isStandaloneApp())
	{
		return (false);
	}

	if (((SystemStats::getOperatingSystemType() & SystemStats::Windows) != 0) && host.isAbletonLive())
	{
		if (hasProperty(Ids::ctrlrUseEditorWrapper))
		{
			return ((bool)getProperty(Ids::ctrlrUseEditorWrapper));
		}
		else
		{
			return (true);
		}
	}
	return (false);
}

AudioProcessorEditor* CtrlrProcessor::createEditor()
{
    if (useWrapper())
    {
        return new CtrlrProcessorEditorForLive (this, *ctrlrManager);
    }
    else
    {
        return new CtrlrEditor (this, *ctrlrManager);
    }
}

//==============================================================================

void CtrlrProcessor::getStateInformation (MemoryBlock& destData)
{
	_DBG("CtrlrProcessor::getStateInformation");
	ScopedPointer <XmlElement> xmlState(ctrlrManager->saveState());
	if (xmlState)
	{
		CtrlrProcessor::copyXmlToBinary (*xmlState, destData);
	}
}

void CtrlrProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	_DBG("CtrlrProcessor::setStateInformation");
	ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
	if (xmlState)
	{
		setStateInformation (xmlState);
	}
}

void CtrlrProcessor::setStateInformation (const XmlElement *xmlState)
{
	if (KeyPress::isKeyCurrentlyDown(KeyPress::createFromDescription("ctrl + R").getKeyCode()))
	{
		AlertWindow::showMessageBox (AlertWindow::WarningIcon, "Ctrlr v5", "Ctrl+R key is pressed, resetting to defaults");
		return;
	}
    
    if (xmlState)
    {
        _DBG("CtrlrProcessor::setStateInformation - xmlState is valid. About to call ctrlrManager->restoreState.");

        // This will call your logMessage, which internally checks 'fileLogger'.
        if (CtrlrLog::ctrlrLog != nullptr)
        {
            CtrlrLog::ctrlrLog->logMessage("--- TEST: Direct Log from setStateInformation ---", CtrlrLog::Debug);
        }
        // The AAX Plugin hanging problem when initiating is subsequent to this line
        ctrlrManager->restoreState (*xmlState);

        _DBG("CtrlrProcessor::setStateInformation - ctrlrManager->restoreState returned. Check for further execution.");
    }
}

//==============================================================================

void CtrlrProcessor::setMidiOptions(const bool _thruHostToHost, const bool _thruHostToDevice, const bool _outputToHost, const bool _inputFromHost, const bool _thruFromHostChannelize)
{
	const ScopedLock sl(getCallbackLock());

	thruHostToHost				= _thruHostToHost;
	thruHostToDevice			= _thruHostToDevice;
	thruFromHostChannelize		= _thruFromHostChannelize;
	outputToHost				= _outputToHost;
	inputFromHost				= _inputFromHost;
}

void CtrlrProcessor::addPanelProcessor (CtrlrPanelProcessor *processorToAdd)
{
	panelProcessors.addIfNotAlreadyThere (processorToAdd);
}

void CtrlrProcessor::removePanelProcessor (CtrlrPanelProcessor *processorToRemove)
{
	panelProcessors.removeAllInstancesOf (processorToRemove);
}

void CtrlrProcessor::copyXmlToBinary (const XmlElement& xml, juce::MemoryBlock& destData)
{
    const String xmlString (xml.createDocument ("", true, false));
    const int stringLength = (int)xmlString.getNumBytesAsUTF8();

    destData.setSize ((size_t) stringLength + 10);

    char* const d = static_cast<char*> (destData.getData());
    *(uint32*) d = ByteOrder::swapIfBigEndian ((const uint32) magicXmlNumber);
    *(uint32*) (d + 4) = ByteOrder::swapIfBigEndian ((const uint32) stringLength);

    xmlString.copyToUTF8 (d + 8, stringLength + 1);
}

XmlElement* CtrlrProcessor::getXmlFromBinary (const void* data, const int sizeInBytes)
{
    if (sizeInBytes > 8
         && ByteOrder::littleEndianInt (data) == magicXmlNumber)
    {
        const int stringLength = (int) ByteOrder::littleEndianInt (addBytesToPointer (data, 4));

        if (stringLength > 0)
            return XmlDocument::parse (String::fromUTF8 (static_cast<const char*> (data) + 8,
                                                         jmin ((sizeInBytes - 8), stringLength))).release();
    }

    return nullptr;
}

void CtrlrProcessor::openFileFromCli(const File &file)
{
	if (ctrlrManager)
	{
		ctrlrManager->openPanelInternal (file);
	}
}

//==============================================================================
// This creates new instances of the plugin.

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    _DBG("createPluginFilter");
    return new CtrlrProcessor();
}
