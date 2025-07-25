#include "stdafx.h"
#include "CtrlrManager/CtrlrManager.h"
#include "CtrlrPanel/CtrlrPanel.h"
#include "CtrlrLog.h"
#include "CtrlrEditor.h"
#include "CtrlrProcessor.h"
#include "CtrlrPanel/CtrlrPanelEditor.h"
#include "CtrlrPanel/CtrlrViewport.h"
#include "CtrlrPanel/CtrlrPanelViewport.h"
#include "CtrlrInlineUtilitiesGUI.h"

CtrlrEditor::CtrlrEditor (CtrlrProcessor *_ownerFilter, CtrlrManager &_owner)
	:	AudioProcessorEditor (_ownerFilter),
        ownerFilter(_ownerFilter),
		owner(_owner),
        resizer(this, 0),
		tempResult(Result::ok()),
		menuHandlerCalled(false),
		lastCommandInvocationMillis(0)
{
	LookAndFeel::setDefaultLookAndFeel(this);  // Set V4 Light as Default LnF. Requires updating depending on the selected LnF
    // menuBarLookAndFeel = new CtrlrMenuBarLookAndFeel (*this); // For version 5.3.198 and before
	Rectangle<int> editorRect;
    // http://www.juce.com/forum/topic/applicationcommandmanager-menus-not-active-annoyance#new
    owner.getCommandManager().setFirstCommandTarget (this);

    menuBar = new MenuBarComponent (this);
    addAndMakeVisible(menuBar);

	setApplicationCommandManagerToWatch (&owner.getCommandManager());

	owner.getCommandManager().registerAllCommandsForTarget (this);
	owner.getCommandManager().registerAllCommandsForTarget (JUCEApplication::getInstance());
	ScopedPointer <XmlElement> xml(XmlDocument::parse(owner.getProperty(Ids::ctrlrKeyboardMapping)).release());
    
    if (xml)
    {
        owner.getCommandManager().getKeyMappings()->restoreFromXml (*xml);
    }
    
    owner.setEditor (this);
    
    addAndMakeVisible (&owner.getCtrlrDocumentPanel());
    
    if (!JUCEApplication::isStandaloneApp()) // If Ctrlr is !NOT run as a standalone app but as a plugin or shared lib
    {
        if (owner.getInstanceMode() != InstanceSingleRestriced) // is !NOT restricted instance of the plugin
        {
            addAndMakeVisible(&resizer);
            resizer.setAlwaysOnTop(false);
            resizer.grabKeyboardFocus();
            resizer.toFront(true);
        }
    }
    
    if (owner.getProperty (Ids::ctrlrEditorBounds).toString() != "") // ctrlrEditorBounds is Editor size. AAX Plugin crashes here, passes without it
    {
        if (owner.getInstanceMode() != InstanceSingle && owner.getInstanceMode() != InstanceSingleRestriced)
        {
            editorRect = VAR2RECT(owner.getProperty(Ids::ctrlrEditorBounds)); // Size of full Editor window including top tabs and 1px borders
        }
        else if (owner.getActivePanel())
        {
            ValueTree editorTree = owner.getActivePanel()->getEditor()->getPanelEditorTree();  // owner is CtrlrManager for the current class
            editorRect = VAR2RECT(owner.getProperty(Ids::ctrlrEditorBounds));
            vpMenuBarVisible = editorTree.getProperty(Ids::uiPanelMenuBarVisible);
            vpResizable = editorTree.getProperty(Ids::uiViewPortResizable);
            vpEnableFixedAspectRatio = editorTree.getProperty(Ids::uiViewPortEnableFixedAspectRatio);
            vpFixedAspectRatio = editorTree.getProperty(Ids::uiViewPortFixedAspectRatio);
            
            vpEnableResizableLimits = editorTree.getProperty(Ids::uiViewPortEnableResizeLimits);
            vpMinWidth = editorTree.getProperty(Ids::uiViewPortMinWidth);
            vpMinHeight = editorTree.getProperty(Ids::uiViewPortMinHeight);
            vpMaxWidth = editorTree.getProperty(Ids::uiViewPortMaxWidth);
            vpMaxHeight = editorTree.getProperty(Ids::uiViewPortMaxHeight);
            
            if ((bool)owner.getActivePanel()->getEditor()->getProperty(Ids::uiPanelMenuBarVisible)) // Exp. instances get an override from uiPanelMenuBarHideOnExport
            {
                setMenuBarVisible(true); // Enable visibility
                editorRect.setHeight (editorRect.getHeight() + (int)owner.getProperty(Ids::ctrlrMenuBarHeight, 24));
            }
            else
            {
                editorRect.setWidth (editorRect.getWidth());
                editorRect.setHeight (editorRect.getHeight());
            }
            
            if (!JUCEApplication::isStandaloneApp() && owner.getInstanceMode() == InstanceSingleRestriced)
            {
                setResizable(vpResizable, true);
                
                //if (auto* constrainer = getConstrainer()) // According to GoodWeather, auto* returns type warning in VS.
                if (auto constrainer = getConstrainer()) // Updated v.5.6.31. Though auto* stresses better the intent that var is a pointer.
                {
                    if (vpEnableFixedAspectRatio == true)
                    {
                        constrainer->setFixedAspectRatio(vpFixedAspectRatio);
                        
                        if (vpEnableResizableLimits == true)
                        {
                            if (vpMinWidth != 0 && vpMaxWidth != 0)
                            {
                                setResizeLimits(vpMinWidth, round(vpMinWidth/vpFixedAspectRatio), vpMaxWidth, round(vpMaxWidth/vpFixedAspectRatio));
                            }
                            else if (vpMinWidth != 0 && vpMinHeight != 0 && vpMaxWidth != 0 && vpMaxHeight != 0)
                            {
                                setResizeLimits(vpMinWidth, vpMinHeight, vpMaxWidth, vpMaxHeight);
                            }
                        }
                        else
                        {
                            constrainer->setMinimumSize(editorRect.getWidth(), editorRect.getHeight());
                        }
                    }
                    else if (vpEnableResizableLimits == true && vpMinWidth != 0 && vpMinHeight != 0 && vpMaxWidth != 0 && vpMaxHeight != 0)
                    {
                        setResizeLimits(vpMinWidth, vpMinHeight, vpMaxWidth, vpMaxHeight);
                    }
                }
            }
        }
        setBounds (editorRect);
    }
    else
    {
        if (JUCEApplication::isStandaloneApp())
            centreWithSize(800, 600);
        else
            setSize(800, 600);
    }
    
    if (owner.getProperty(Ids::uiPanelLegacyMode) == "1") // Added v5.6.31. Force exported instances to get LnF v3 for legacy mode if panel has LnF in legacyMode
    {
        LookAndFeel::setDefaultLookAndFeel(new LookAndFeel_V3()); // Added v5.6.31
        setLookAndFeel(new LookAndFeel_V3()); // Added v5.6.31
    }
    else
    {
        setColourScheme(gui::colourSchemeFromProperty(owner.getProperty(Ids::ctrlrColourScheme))); // Added v5.6.31. Sets the LookAndFeel_V4 colourScheme from the Ctrlr General Preferences, not from the loaded Panel
    }
    
    lookAndFeelChanged(); // Added v5.6.31. Update LnF for all components

    getLookAndFeel().setUsingNativeAlertWindows((bool)owner.getProperty(Ids::ctrlrNativeAlerts)); // Sets OS Native alert windows or JUCE
    
    activeCtrlrChanged(); // Refresh CtrlrEditor Template and menuBar LnF, wether panel mode or Editor with or WO menuBar from properties
    
    if (isRestricted() && owner.getActivePanel())
    {
        hideProgramsMenu = owner.getActivePanel()->getEditor()->getProperty(Ids::uiPanelProgramsMenuHideOnExport);
        hideMidiControllerMenu = owner.getActivePanel()->getEditor()->getProperty(Ids::uiPanelMidiControllerMenuHideOnExport);
        hideMidiThruMenu = owner.getActivePanel()->getEditor()->getProperty(Ids::uiPanelMidiThruMenuHideOnExport);
        hideMidiChannelMenu = owner.getActivePanel()->getEditor()->getProperty(Ids::uiPanelMidiChannelMenuHideOnExport);
    }
}

CtrlrEditor::~CtrlrEditor()
{
    deleteAndZero (menuBar);
	masterReference.clear();
}

void CtrlrEditor::paint (Graphics& g)
{
    g.fillAll(Component::findColour(DocumentWindow::backgroundColourId));
}

void CtrlrEditor::resized()
{
	if (menuBar->isVisible())
	{
		menuBar->setBounds (0, 0, getWidth(), (int)owner.getProperty(Ids::ctrlrMenuBarHeight,24));
		owner.getCtrlrDocumentPanel().setBounds (0, (int)owner.getProperty(Ids::ctrlrMenuBarHeight,24), getWidth(), getHeight() - ((int)owner.getProperty(Ids::ctrlrMenuBarHeight,24)));
	}
	else
	{
		owner.getCtrlrDocumentPanel().setBounds (0, 0, getWidth(), getHeight());
	}
	resizer.setBounds (getWidth()-24, getHeight()-24, 24, 24);
	owner.setProperty (Ids::ctrlrEditorBounds, getBounds().toString());
}


void CtrlrEditor::activeCtrlrChanged()
{
	ownerFilter->activePanelChanged();
	bool menuBarVisible = true;

	if (owner.getActivePanel() && owner.getActivePanel()->getEditor())
	{
		menuBarVisible = owner.getActivePanel()->getEditor()->getProperty(Ids::uiPanelMenuBarVisible);

		if (menuBarVisible != menuBar->isVisible())
		{
			setMenuBarVisible(menuBarVisible);
		}
        
        String currentLookAndFeel = owner.getActivePanel()->getEditor()->getProperty(Ids::uiPanelLookAndFeel);
        setMenuBarLookAndFeel(currentLookAndFeel); // Updates the current component LookAndFeel : PanelEditor
        
        lookAndFeelChanged();

//        menuBar colour properties are deprecated and need to be removed in v5.6.30
//        String customMenuBarBackgroundColour1 = owner.getActivePanel()->getEditor()->getProperty(Ids::ctrlrMenuBarBackgroundColour1);
//        String customMenuBarBackgroundColour2 = owner.getActivePanel()->getEditor()->getProperty(Ids::ctrlrMenuBarBackgroundColour2);
//        String customMenuBarItemBackgroundColour = owner.getActivePanel()->getEditor()->getProperty(Ids::ctrlrMenuItemBackgroundColour);
//        String customMenuBarItemTextColour = owner.getActivePanel()->getEditor()->getProperty(Ids::ctrlrMenuItemTextColour);
//        String customMenuBarItemHighlightedTextColour = owner.getActivePanel()->getEditor()->getProperty(Ids::ctrlrMenuItemHighlightedTextColour);
//        String customMenuBarItemHighlightColour = owner.getActivePanel()->getEditor()->getProperty(Ids::ctrlrMenuItemHighlightColour);
//        String customMenuBarItemFont = owner.getActivePanel()->getEditor()->getProperty(Ids::ctrlrMenuItemFont);
//        String customMenuBarItemSeparatorColour = owner.getActivePanel()->getEditor()->getProperty(Ids::ctrlrMenuItemSeparatorColour);
//        String customMenuBarItemHeaderColour = owner.getActivePanel()->getEditor()->getProperty(Ids::ctrlrMenuItemHeaderColour);
//        String customMenuBarTextColour = owner.getActivePanel()->getEditor()->getProperty(Ids::ctrlrMenuBarTextColour);
//        String customMenuBarHighlightedTextColour = owner.getActivePanel()->getEditor()->getProperty(Ids::ctrlrMenuBarHighlightedTextColour);
//        String customMenuBarHighlightColour = owner.getActivePanel()->getEditor()->getProperty(Ids::ctrlrMenuBarHighlightColour);
//        String customMenuBarFont = owner.getActivePanel()->getEditor()->getProperty(Ids::ctrlrMenuBarFont);
	}
}

MenuBarComponent *CtrlrEditor::getMenuBar()
{
	return (menuBar);
}

CtrlrPanel *CtrlrEditor::getActivePanel()
{
	return (owner.getActivePanel());
}

bool CtrlrEditor::isRestricted()
{
	return (owner.getInstanceMode() == InstanceSingleRestriced);
}

CtrlrPanelEditor *CtrlrEditor::getActivePanelEditor()
{
	if (owner.getActivePanel())
	{
		return (owner.getActivePanel()->getEditor());
	}

	return (nullptr);
}

bool CtrlrEditor::isPanelActive(const bool checkRestrictedInstance)
{
	if (getActivePanel())
	{
		if (checkRestrictedInstance)
		{
			if (owner.getInstanceMode() == InstanceSingleRestriced)
			{
				return (false);
			}

			return (true);
		}
		else
		{
			return (true);
		}
	}

	return (false);
}

void CtrlrEditor::setMenuBarVisible(const bool shouldBeVisible)
{
	menuBar->setVisible (shouldBeVisible);
	resized();
}

void CtrlrEditor::setMenuBarLookAndFeel(const juce::String &lookAndFeelDesc) // Updated v5.6.34.
{
    // The previous LookAndFeel set on menuBar will be automatically deleted by JUCE
    // when you call setLookAndFeel again.
    menuBar->setLookAndFeel(gui::createLookAndFeelFromDescription(lookAndFeelDesc, true));
    // The `true` argument ensures that if the description is unknown, it defaults to V4 Light.
}
