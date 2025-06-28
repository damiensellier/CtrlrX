#include "stdafx.h"
#ifdef _WIN32
#include <Windows.h>
#endif

#include "CtrlrManager/CtrlrManager.h"
#include "CtrlrProcessorEditorForLive.h"
#include "CtrlrMacros.h"
#include "CtrlrLog.h"

CtrlrEditorWrapper::CtrlrEditorWrapper(CtrlrProcessorEditorForLive &_liveEditorOwner, CtrlrProcessor *ownerFilter, CtrlrManager &ctrlrManager) 
	: DocumentWindow("Ctrlr", Colours::lightgrey, 0, true), liveEditorOwner(_liveEditorOwner)
{
	setTitleBarHeight(1);
    editor = new CtrlrEditor(ownerFilter, ctrlrManager);
    addAndMakeVisible(editor); // Added v5.6.31. Force wrapper visibility
    setContentOwned (editor, true);
    setAlwaysOnTop(true);
	centreWithSize (getWidth(),getHeight());
}

CtrlrEditorWrapper::~CtrlrEditorWrapper()
{
}

void CtrlrEditorWrapper::resized()
{
	DocumentWindow::resized();
	liveEditorOwner.wrapperResized();
}

CtrlrProcessorEditorForLive::CtrlrProcessorEditorForLive(CtrlrProcessor *_filterOwner, CtrlrManager &_owner)
    : owner(_owner),
      filterOwner(_filterOwner),
      AudioProcessorEditor((AudioProcessor *)_filterOwner), // Initialize base class AudioProcessorEditor
      wrapper(*this, filterOwner, owner) // Initialize the DocumentWindow wrapper
{
    // Init log file for debug
    //File debugLogForLive = File::getSpecialLocation(File::userDesktopDirectory).getChildFile("CtrlrX_vst3_live_debug_log.txt");
    //PluginLoggerVst3ForLive logger(debugLogForLive);
    //logger.log("Debug Log for Live initiated");
    
    wrapper.setVisible (true);
    addAndMakeVisible(wrapper); // Added v5.6.31. Force wrapper visibility
	setSize (wrapper.getWidth(), 16);
	startTimer (50);
}

CtrlrProcessorEditorForLive::~CtrlrProcessorEditorForLive()
{
}

void CtrlrProcessorEditorForLive::paint(Graphics &g) // Updated v5.6.34
{
    ValueTree ed = owner.getManagerTree();
    Colour vpBkgColour = VAR2COLOUR(ed.getProperty(Ids::uiPanelViewPortBackgroundColour));
    g.setColour(vpBkgColour);
	g.fillAll();
}

void CtrlrProcessorEditorForLive::timerCallback()
{
	if (getPeer())
	{
#ifdef _WIN32
		HWND hwndMsg = (HWND)getPeer()->getNativeHandle();

		if (hwndMsg)
		{
			wrapper.setVisible (IsWindowVisible(hwndMsg) ? true : false);
		}
#endif
	}

	if (getScreenPosition() != lastScreenPosition)
	{
		wrapper.setTopLeftPosition(getScreenPosition().getX(), getScreenPosition().getY()+24);
	}
}

void CtrlrProcessorEditorForLive::wrapperResized()
{
	setSize(wrapper.getWidth(), 16); // NOTE v5.6.34. This is weird because Height will always reset to 16px when resized.
}
