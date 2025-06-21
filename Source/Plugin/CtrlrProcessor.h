#ifndef __CTRLR_PROCESSOR__
#define __CTRLR_PROCESSOR__

#include "CtrlrProperties.h"
#include "CtrlrPanel/CtrlrPanelProcessor.h"
#include "juce_PluginHostType.h"
#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "CtrlrMacros.h"
#include "CtrlrLog.h"

#include <fstream> // Added v5.6.33. Required for vst3 logger

class CtrlrLog;
class CtrlrManager;
class CtrlrMidiMessage;

typedef WeakReference<CtrlrPanelProcessor>	PanelProcessorReference;
typedef WeakReference<CtrlrModulator> ModulatorReference;

struct DelayedPluginDeleter  : private Timer, private DeletedAtShutdown
{
    DelayedPluginDeleter (AudioPluginInstance* p)  : plugin (p)
    {
        startTimer (1000);
    }

    void timerCallback() override
    {
        delete this;
    }

    ScopedPointer<AudioPluginInstance> plugin;
};

struct CtrlrParameterFromHost
{
	CtrlrParameterFromHost(const int _index, const float _value)
		: index(_index), value(_value) {}
	CtrlrParameterFromHost()
		: index(-1), value(0.0f) {}
	int index;
	float value;
};

class CtrlrProcessor : public AudioProcessor, public ChangeBroadcaster
{
	public:
	    CtrlrProcessor();
		~CtrlrProcessor();
		void prepareToPlay (double sampleRate, int samplesPerBlock);   
		void releaseResources() override;
    
        #ifndef JucePlugin_PreferredChannelConfigurations
            bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
        #endif
    
        double rawVolume;

		void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages) override;
    
		AudioProcessorEditor* createEditor() override;
		bool useWrapper();
    
        bool hasEditor() const override;
    
		const String getName() const override;

    
        // DEPRECATED WITH JUCE 5
        int getNumParameters() override;
        float getParameter (int index) override;
        void setParameter (int index, float newValue) override;
        const juce::String getParameterName (int index) override;
        const juce::String getParameterText (int index) override;
           
        // const String getInputChannelName (int channelIndex) const;
        // const String getOutputChannelName (int channelIndex) const;
        // bool isInputChannelStereoPair (int index) const;
		// bool isOutputChannelStereoPair (int index) const;
		// int getNumInputChannels()										{ return (2); }
		// int getNumOutputChannels()										{ return (2); }
    
    	
        bool acceptsMidi() const override;
        bool producesMidi() const override;
        bool isMidiEffect() const override;
        
        double getTailLengthSeconds() const override;
       
        int getNumPrograms() override;
        String getParameterID(int index) override; // Added v5.6.33
        int getCurrentProgram() override;
        void setCurrentProgram (int index) override;
        const String getProgramName (int index) override;
    
        void changeProgramName (int index, const String& newName) override;
    
        // DEPRECATED WITH JUCE 5
		// bool silenceInProducesSilenceOut() const						{ return (true); }
    
    
        // For ProTools projects generated with a < 4.3.0 JUCE versions of AAX Plugin
        int32 getAAXPluginIDForMainBusConfig (const AudioChannelSet& mainInputLayout,
                                              const AudioChannelSet& mainOutputLayout,
                                              bool idForAudioSuite) const
        {
            int uniqueFormatId = 0;
            if (mainOutputLayout == AudioChannelSet::stereo())
            {
                if (mainInputLayout == AudioChannelSet::mono())
                {
                    uniqueFormatId = 1;
                }
            }
            else uniqueFormatId = 2;
            return (idForAudioSuite ? 0x6a796161  : 0x6a636161 ) + uniqueFormatId;
        }
    
    
		static XmlElement* getXmlFromBinary (const void* data, const int sizeInBytes);
		static void copyXmlToBinary (const XmlElement& xml, juce::MemoryBlock& destData);
    
		void getStateInformation (MemoryBlock& destData);
		void setStateInformation (const void* data, int sizeInBytes);
		void setStateInformation (const XmlElement *xmlState);

        CtrlrLog &getCtrlrLog()										{ return (*ctrlrLog); }
		CtrlrManager &getManager()									{ return (*ctrlrManager); }
		ValueTree getOverrides()									{ return (overridesTree); }

        void addMidiToOutputQueue (CtrlrMidiMessage &m);
		void addMidiToOutputQueue (const CtrlrMidiMessage &m);
		void addMidiToOutputQueue (const MidiMessage &m);
		void addMidiToOutputQueue (const MidiBuffer &buffer);
		void setMidiOptions(const bool _thruHostToHost, const bool _thruHostToDevice, const bool _outputToHost, const bool _inputFromHost, const bool _thruFromHostChannelize);

        void processPanels(MidiBuffer &midiMessages, const AudioPlayHead::CurrentPositionInfo &positionInfo);
		void addPanelProcessor (CtrlrPanelProcessor *processorToAdd);
		void removePanelProcessor (CtrlrPanelProcessor *processorToRemove);
		void setParameterHandler (int index, float value);

        void openFileFromCli(const File &file);
		AudioPlayHead::CurrentPositionInfo lastPosInfo;
		const var &getProperty (const Identifier& name) const;
		bool hasProperty(const Identifier &name) const;
		void activePanelChanged();
    
	private:
        ::PluginHostType host;
		MidiBuffer leftoverBuffer;
		CtrlrLog *ctrlrLog;
		ScopedPointer <CtrlrManager> ctrlrManager;
		File currentExec;
		File overridesFile;
		MidiMessage logResult;
		int logSamplePos;
		ValueTree overridesTree;
		MidiMessageCollector midiCollector;
		Array <PanelProcessorReference,CriticalSection> panelProcessors;
		Array <CtrlrParameterFromHost,CriticalSection> parameterUpdates;
		bool	thruHostToHost,
				thruHostToDevice,
				thruFromHostChannelize,
				outputToHost,
				inputFromHost;
    
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CtrlrProcessor);
};



class PluginLoggerVst3 { // Added v5.6.32
public:
    PluginLoggerVst3(const juce::File& pluginExecutableFile) {
        logFile = pluginExecutableFile.getParentDirectory().getChildFile("CtrlrX_vst3_debug_log.txt");
        if (!logFile.exists()) {
            logFile.create();
        }
    }

    void log(const juce::String& message) {
        std::ofstream outfile(logFile.getFullPathName().toStdString(), std::ios_base::app);
        if (outfile.is_open()) {
            outfile << juce::Time::getCurrentTime().toString(true, true, true, true) << ": " << message.toStdString() << std::endl;
            outfile.close();
        } else {
            std::cerr << "Error: Could not open log file for writing." << std::endl;
        }
    }

    void logResult(const juce::Result& result) {
        if (result.wasOk()) {
            log("Result: OK");
        } else {
            log("Result: FAIL - " + result.getErrorMessage());
        }
    }

private:
    juce::File logFile;
};


#endif
