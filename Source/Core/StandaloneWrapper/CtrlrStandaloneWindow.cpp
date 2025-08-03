#include "stdafx.h"
#include "CtrlrManager/CtrlrManager.h"
#include "CtrlrProcessor.h"
#include "CtrlrStandaloneWindow.h"
#include "CtrlrPanel/CtrlrPanelEditor.h"

extern AudioProcessor* JUCE_CALLTYPE createPluginFilter();

CtrlrStandaloneWindow::CtrlrStandaloneWindow (const String& title, const Colour& backgroundColour)
	:	DocumentWindow (title, backgroundColour, DocumentWindow::allButtons, true),
		ctrlrProcessor(nullptr),
		filter(nullptr),
		appProperties(nullptr),
        restoreState(true)
{
	filter = createPluginFilter();
	setTitleBarButtonsRequired (DocumentWindow::allButtons, false);
	setUsingNativeTitleBar (true);
    setResizable(true, true);  // default. Set to false, false to lock, hide the corner resizer
    centreWithSize(800, 600); //set Size of the whole app with title bar included H22px and borders 2x1px
    
    if (filter != 0)
	{
        ctrlrProcessor = dynamic_cast<CtrlrProcessor*>(filter);

        if (ctrlrProcessor == nullptr)
        {
            AlertWindow::showMessageBox (AlertWindow::WarningIcon, "CTRLR", "The filter object is not a valid Ctrlr Processor");
            return;
        }

        /* set some default audio stuff so the filter works without the audio card */
        //ctrlrProcessor->setPlayConfigDetails (0, 0, SAMPLERATE, 512);
        ctrlrProcessor->setRateAndBufferSizeDetails(SAMPLERATE, 512);
        addKeyListener (ctrlrProcessor->getManager().getCommandManager().getKeyMappings());

        /* we want to listen too manager actions */
        ctrlrProcessor->getManager().addActionListener (this);

        // We want to be notified by CtrlrProcessor when the active panel changes to update the title bar
        ctrlrProcessor->addChangeListener(this);

        /* get the properties pointer from the manager */
        appProperties = ctrlrProcessor->getManager().getApplicationProperties();

        if (appProperties != nullptr)
        {
            _DBG("appProperties != nullptr");
            ScopedPointer <XmlElement> xml(appProperties->getUserSettings()->getXmlValue(CTRLR_PROPERTIES_FILTER_STATE).release());
            
            if (xml != nullptr)
            {
                _DBG("xml != nullptr");
                ctrlrProcessor->setStateInformation (xml);
            }

            AudioProcessorEditor *editor = ctrlrProcessor->createEditorIfNeeded();
            setName (ctrlrProcessor->getManager().getInstanceName());

            if (appProperties->getUserSettings()->getValue(CTRLR_PROPERTIES_WINDOW_STATE,"") != "")
            {
                _DBG("CTRLR_PROPERTIES_WINDOW_STATE != null");
                restoreWindowStateFromString (appProperties->getUserSettings()->getValue(CTRLR_PROPERTIES_WINDOW_STATE));
            }
            else
            {
                _DBG("CTRLR_PROPERTIES_WINDOW_STATE == null");
                if (ctrlrProcessor->getManager().getInstanceTree().getChildWithName(Ids::uiPanelEditor).isValid())
                {
                    _DBG("uiPanelEditor isValid");
                    ValueTree ed = ctrlrProcessor->getManager().getInstanceTree().getChildWithName(Ids::uiPanelEditor);
                    Rectangle<int> r = VAR2RECT(ed.getProperty(Ids::uiPanelCanvasRectangle, "0 0 800 600"));
                    centreWithSize (r.getWidth(), r.getHeight() + ((bool)ed.getProperty(Ids::uiPanelMenuBarVisible) ? (int)ctrlrProcessor->getManager().getProperty(Ids::ctrlrMenuBarHeight) : 0));
                }
            }

            setContentOwned (editor, false);
        }
        else
        {
            _DBG("No appProperties");
            AlertWindow::showMessageBox (AlertWindow::WarningIcon, "CTRLR", "Can't find any application properties");
        }
    }

    ValueTree ed = ctrlrProcessor->getManager().getInstanceTree().getChildWithName(Ids::uiPanelEditor);
    Rectangle<int> r = VAR2RECT(ed.getProperty(Ids::uiPanelCanvasRectangle));
    panelCanvasWidth = r.getWidth();
    panelCanvasHeight = r.getHeight();
       
    vpResizable = ed.getProperty(Ids::uiViewPortResizable, true);
    vpEnableFixedAspectRatio = ed.getProperty(Ids::uiViewPortEnableFixedAspectRatio, false);
    
    vpOsFrameTop = getPeer()->getFrameSize().getTop(); // OS Native Title Bar Height
    vpOsFrameBtm = getPeer()->getFrameSize().getBottom(); // OS Native Window Border Bottom thickness
    vpOsFrameLeft = getPeer()->getFrameSize().getLeft(); // OS Native Window Border Left thickness
    vpOsFrameRight = getPeer()->getFrameSize().getRight(); // OS Native Window Border Right thickness
    vpOsWindowWidth = getPeer()->getBounds().getWidth(); // OS Native Window Height incl borders
    vpOsWindowHeight = getPeer()->getBounds().getHeight(); // OS Native Window Height incl borders
    //Native Window size is stored in the OS.
    //If the APP has value off the target, it's because the same UID was used previously with different sizes & ratio
    //OR width and Height require to be set from inner content + Borders
       
    //vpStandaloneAspectRatio = double(vpOsWindowWidth) / double(vpOsWindowHeight); // Requires Native Title bar Height and borders
    vpStandaloneAspectRatio = double(panelCanvasWidth + vpOsFrameLeft + vpOsFrameRight) / double(panelCanvasHeight + vpOsFrameTop + vpOsFrameBtm);
    
    vpEnableResizableLimits = ed.getProperty(Ids::uiViewPortEnableResizeLimits);
    vpMinWidth = ed.getProperty(Ids::uiViewPortMinWidth);
    vpMinHeight = ed.getProperty(Ids::uiViewPortMinHeight);
    vpMaxWidth = ed.getProperty(Ids::uiViewPortMaxWidth);
    vpMaxHeight = ed.getProperty(Ids::uiViewPortMaxHeight);
       
    if (ctrlrProcessor->getManager().getInstanceMode() == InstanceSingleRestriced) // restricted instances check flag to be resizable
    {
        _DBG("Restricted Instance Mode");
        setResizable(vpResizable, true);

        if (auto* constrainer = getConstrainer())
        {
            if (vpEnableFixedAspectRatio == true)
            {
                constrainer->setFixedAspectRatio(vpStandaloneAspectRatio); // set window aspect ratio
                
                if (vpEnableResizableLimits == true)
                {
                    if (vpMinWidth != 0 && vpMaxWidth != 0)
                    {
                        setResizeLimits(vpMinWidth,
                                        round(vpMinWidth/vpStandaloneAspectRatio),
                                        vpMaxWidth,
                                        round(vpMaxWidth/vpStandaloneAspectRatio));
                    }
                    else if (vpMinWidth != 0 && vpMinHeight != 0 && vpMaxWidth != 0 && vpMaxHeight != 0)
                    {
                        setResizeLimits(vpMinWidth + vpOsFrameLeft + vpOsFrameRight,
                        vpMinHeight + vpOsFrameTop + vpOsFrameBtm,
                        vpMaxWidth + vpOsFrameLeft + vpOsFrameRight,
                        vpMaxHeight + vpOsFrameTop + vpOsFrameBtm);
                    }
                    else
                    {
                        constrainer->setMinimumSize(panelCanvasWidth, panelCanvasHeight + vpOsFrameTop + vpOsFrameBtm);
                    }
                }
            }
            else if (vpEnableResizableLimits == true && vpMinWidth != 0 && vpMinHeight != 0 && vpMaxWidth != 0 && vpMaxHeight != 0)
            {
                setResizeLimits(vpMinWidth + vpOsFrameLeft + vpOsFrameRight,
                                vpMinHeight + vpOsFrameTop + vpOsFrameBtm,
                                vpMaxWidth + vpOsFrameLeft + vpOsFrameRight,
                                vpMaxHeight + vpOsFrameTop + vpOsFrameBtm);
            }
        }
    }
    else
    {
        setResizable(true, true);
    }
    
    restoreState = false;
    setVisible (true);
}

CtrlrStandaloneWindow::~CtrlrStandaloneWindow()
{
	ctrlrProcessor->removeChangeListener(this);
    ctrlrProcessor->getManager().removeActionListener (this);
    saveStateNow();
    deleteFilter();
}

void CtrlrStandaloneWindow::actionListenerCallback(const String &message)
{
    if (message == "save")
    {
        saveStateNow();
    }
}

void CtrlrStandaloneWindow::changeListenerCallback(ChangeBroadcaster* source)
{	// Check for window title modification
	CtrlrPanel *panel = ctrlrProcessor->getManager().getActivePanel();
	String windowTitle = ctrlrProcessor->getManager().getInstanceName();
	if (panel && !ctrlrProcessor->getManager().isSingleInstance())
	{
		windowTitle += " - " + panel->getPanelWindowTitle();
	}
	setName(windowTitle);
}

void CtrlrStandaloneWindow::saveStateNow()
{
    _DBG("CtrlrStandaloneWindow::saveStateNow");

    if (ctrlrProcessor != nullptr && appProperties != nullptr)
    {
		appProperties->getUserSettings()->setValue (CTRLR_PROPERTIES_WINDOW_STATE, getWindowStateAsString());

		MemoryBlock data;
		ctrlrProcessor->getStateInformation(data);

		if (data.getSize() > 0)
		{
			ScopedPointer <XmlElement> xml(CtrlrProcessor::getXmlFromBinary(data.getData(), (int)data.getSize()));

			if (xml)
			{
				appProperties->getUserSettings()->setValue (CTRLR_PROPERTIES_FILTER_STATE, xml);
			}
		}
	}
}

void CtrlrStandaloneWindow::deleteFilter()
{
    if (filter != 0 && getContentComponent() != 0)
    {
        filter->editorBeingDeleted (dynamic_cast <AudioProcessorEditor*> (getContentComponent()));
		clearContentComponent ();
    }

    deleteAndZero (filter);
}

PropertySet* CtrlrStandaloneWindow::getGlobalSettings()
{
    return ctrlrProcessor->getManager().getCtrlrProperties().getProperties().getUserSettings();
}

void CtrlrStandaloneWindow::closeButtonPressed()
{
	if(ctrlrProcessor->getManager().canCloseWindow())
	{
		JUCEApplication::quit();
	}
}

void CtrlrStandaloneWindow::resized()
{
	DocumentWindow::resized();

	if (appProperties != nullptr && !restoreState)
	{
		appProperties->getUserSettings()->setValue (CTRLR_PROPERTIES_WINDOW_STATE, getWindowStateAsString());
	}
}

void CtrlrStandaloneWindow::moved()
{
	DocumentWindow::moved();

	if (appProperties != nullptr)
	{
		appProperties->getUserSettings()->setValue (CTRLR_PROPERTIES_WINDOW_STATE, getWindowStateAsString());
	}
}

AudioProcessor *CtrlrStandaloneWindow::getFilter()
{
	return (filter);
}

void CtrlrStandaloneWindow::openFileFromCli(const File &file)
{
	if (ctrlrProcessor)
	{
		ctrlrProcessor->openFileFromCli (file);
	}
}
