#ifndef __CTRLR_PROCESSOR_EDITOR_FOR_LIVE__
#define __CTRLR_PROCESSOR_EDITOR_FOR_LIVE__

#include "CtrlrApplicationWindow/CtrlrEditor.h"
#include "CtrlrMacros.h"
#include "CtrlrLog.h"
#include <fstream> // Added v5.6.33. Required for vst3 logger

class CtrlrLog;
class CtrlrManager;
class CtrlrProcessorEditorForLive;

class CtrlrEditorWrapper : public DocumentWindow
{
	public:
		CtrlrEditorWrapper(CtrlrProcessorEditorForLive &_liveEditorOwner, CtrlrProcessor *_filterOwner, CtrlrManager &_owner);
		~CtrlrEditorWrapper();
		void resized();

	private:
		CtrlrEditor *editor;
		CtrlrProcessorEditorForLive &liveEditorOwner;
};

class CtrlrProcessorEditorForLive : public AudioProcessorEditor, public Timer
{
	public:
		CtrlrProcessorEditorForLive(CtrlrProcessor *_filterOwner, CtrlrManager &_owner);
		~CtrlrProcessorEditorForLive();
		void paint(Graphics &g);
		void timerCallback();
		void wrapperResized();
		
	private:
		CtrlrProcessor *filterOwner;
		CtrlrManager &owner;
		CtrlrEditorWrapper wrapper;
		Point<int> lastScreenPosition;
};


class PluginLoggerVst3ForLive { // Added v5.6.32
public:
    PluginLoggerVst3ForLive(const juce::File& pluginExecutableFile) {
        File logFile =pluginExecutableFile.getParentDirectory().getChildFile("CtrlrX_vst3_live_debug_log.txt");
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
