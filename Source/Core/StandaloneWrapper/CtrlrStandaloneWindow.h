#ifndef __CTRLR_STANDALONE_WINDOW__
#define __CTRLR_STANDALONE_WINDOW__

#include "CtrlrMacros.h"
class CtrlrProcessor;

class CtrlrStandaloneWindow	: public DocumentWindow, public ActionListener, public ChangeListener
{
	public:
		CtrlrStandaloneWindow (const String& title, const Colour& backgroundColour);
		~CtrlrStandaloneWindow();
		AudioProcessor *getFilter();
		virtual PropertySet* getGlobalSettings();
		void closeButtonPressed();
		void resized();
		void moved();
		void openFileFromCli(const File &file);
		void saveStateNow();
		void actionListenerCallback(const String &message);
		void changeListenerCallback(ChangeBroadcaster* source);
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CtrlrStandaloneWindow);

	private:
        bool vpResizable;
        double vpFixedAspectRatio;
        bool vpEnableFixedAspectRatio;
        bool vpEnableResizableLimits;
        int vpCurrentWidth;
        int vpCurrentHeight;
        int vpMinWidth;
        int vpMinHeight;
        int vpMaxWidth;
        int vpMaxHeight;
        int panelCanvasHeight;
        int panelCanvasWidth;
        int editorWidth;
        int editorHeight;
        double vpStandaloneAspectRatio;
        const Rectangle<int> vpvpOsWindowRect;
        int vpOsWindowWidth;
        int vpOsWindowHeight;
        double vpOsWindowAspectRatio;
        int vpOsFrameTop;
        int vpOsFrameBtm;
        int vpOsFrameLeft;
        int vpOsFrameRight;
        double vpPanelZoom;
		bool restoreState;
		AudioProcessor *filter;
		CtrlrProcessor *ctrlrProcessor;
		ApplicationProperties *appProperties;
		void deleteFilter();
};

#endif
