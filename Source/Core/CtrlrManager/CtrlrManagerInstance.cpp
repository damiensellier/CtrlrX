#include "stdafx.h"
#include "CtrlrManager.h"
#include "CtrlrProperties.h"
#include "CtrlrPanel/CtrlrPanel.h"

void CtrlrManager::setEmbeddedDefaults()
{
	_DBG("CtrlrManager::setEmbeddedDefaults");

	/* first init the ApplicationProperties stuff */
	if (ctrlrPlayerInstanceTree.hasProperty(Ids::restricted))
	{
		ctrlrPlayerInstanceMode	= (CtrlrInstance)(int)ctrlrPlayerInstanceTree.getProperty(Ids::restricted);
	}
	else
	{
		ctrlrPlayerInstanceMode = InstanceSingle;
	}

	ctrlrProperties.reset(new CtrlrProperties (*this));

	setProperty (Ids::ctrlrLogToFile, false);
    setProperty (Ids::ctrlrWarningInBootstrapState, false); // Added v5.6.32
	setProperty (Ids::ctrlrLuaDebug, false);
	setProperty (Ids::ctrlrVersionSeparator, "_");
	setProperty (Ids::ctrlrVersionCompressed, false);
	setProperty (Ids::ctrlrMidiMonInputBufferSize, 8192);
	setProperty (Ids::ctrlrMidiMonOutputBufferSize, 8192);
	setProperty (Ids::ctrlrLuaDisabled, false);
	setProperty (Ids::ctrlrOverwriteResources, true);
	setProperty (Ids::ctrlrAutoSave, true);
	setProperty (Ids::ctrlrAutoSaveInterval, 300);
    // setProperty (Ids::ctrlrLogOptions, 32); // Updated v5.6.31
    setProperty (Ids::ctrlrLogOptions, 6014); // Added v5.6.31. 6014 shows everything by default with MIDI messages in Hex
    setProperty (Ids::ctrlrScrollbarThickness, 16.0f); // Added v5.6.31
    setProperty (Ids::ctrlrMenuBarHeight, 24); // Added v5.6.31
    setProperty (Ids::ctrlrTabBarDepth, 24); // Added v5.6.31
    setProperty (Ids::ctrlrPropertyLineheightBaseValue, 36); // Added v5.6.33.
    setProperty (Ids::ctrlrUseEditorWrapper, (bool)ctrlrPlayerInstanceTree.hasProperty(Ids::ctrlrUseEditorWrapper) ? (bool)ctrlrPlayerInstanceTree.getProperty(Ids::ctrlrUseEditorWrapper) : true);
    setProperty (Ids::uiLuaConsoleInputRemoveAfterRun, true);
     
    // Added v5.6.31
    if (getInstanceMode() == InstanceSingle || getInstanceMode() == InstanceSingleRestriced) // Added v5.6.31. Force LnF V3 for Main & Child Windows if panel is Legacy
    {
        ValueTree ed = getInstanceTree().getChildWithName(Ids::uiPanelEditor);
        
        // Requires passing the lookAndFeel Version to the property ctrlrColourScheme from ctrlrLookAndFeel
        if (ed.getProperty(Ids::uiPanelLegacyMode) == "1"
            || ed.getProperty(Ids::uiPanelLookAndFeel) == "V3"
            || ed.getProperty(Ids::uiPanelLookAndFeel) == "V2"
            || ed.getProperty(Ids::uiPanelLookAndFeel) == "V1")
        {
            setProperty(Ids::ctrlrLegacyMode, "1");
            setProperty(Ids::ctrlrLookAndFeel, "V3");
        }
        else
        {
            setProperty(Ids::ctrlrLegacyMode, "0");
            setProperty(Ids::ctrlrLookAndFeel, "V4");
            
            // Requires passing the colourScheme to the property ctrlrColourScheme from uiPanelLookAndFeel
            if (ed.getProperty(Ids::uiPanelLookAndFeel) == "V4 Light")
            {
                setProperty(Ids::ctrlrColourScheme, "Light");
            }
            else if (ed.getProperty(Ids::uiPanelLookAndFeel) == "V4 Grey")
            {
                setProperty(Ids::ctrlrColourScheme, "Grey");
            }
            else if (ed.getProperty(Ids::uiPanelLookAndFeel) == "V4 Dark")
            {
                setProperty(Ids::ctrlrColourScheme, "Dark");
            }
            else if (ed.getProperty(Ids::uiPanelLookAndFeel) == "V4 Midnight")
            {
                setProperty(Ids::ctrlrColourScheme, "Midnight");
            }
            else if (ed.getProperty(Ids::uiPanelLookAndFeel) == "V4 JetBlack")
            {
                setProperty(Ids::ctrlrColourScheme, "JetBlack");
            }
            else if (ed.getProperty(Ids::uiPanelLookAndFeel) == "V4 YamDX")
            {
                setProperty(Ids::ctrlrColourScheme, "YamDX");
            }
            else if (ed.getProperty(Ids::uiPanelLookAndFeel) == "V4 AkAPC")
            {
                setProperty(Ids::ctrlrColourScheme, "AkAPC");
            }
            else if (ed.getProperty(Ids::uiPanelLookAndFeel) == "V4 AkMPC")
            {
                setProperty(Ids::ctrlrColourScheme, "AkMPC");
            }
            else if (ed.getProperty(Ids::uiPanelLookAndFeel) == "V4 LexiBlue")
            {
                setProperty(Ids::ctrlrColourScheme, "LexiBlue");
            }
            else if (ed.getProperty(Ids::uiPanelLookAndFeel) == "V4 KurzGreen")
            {
                setProperty(Ids::ctrlrColourScheme, "KurzGreen");
            }
            else if (ed.getProperty(Ids::uiPanelLookAndFeel) == "V4 KorGrey")
            {
                setProperty(Ids::ctrlrColourScheme, "KorGrey");
            }
            else if (ed.getProperty(Ids::uiPanelLookAndFeel) == "V4 KorGold")
            {
                setProperty(Ids::ctrlrColourScheme, "KorGold");
            }
            else if (ed.getProperty(Ids::uiPanelLookAndFeel) == "V4 ArturOrange")
            {
                setProperty(Ids::ctrlrColourScheme, "ArturOrange");
            }
            else if (ed.getProperty(Ids::uiPanelLookAndFeel) == "V4 AiraGreen")
            {
                setProperty(Ids::ctrlrColourScheme, "AiraGreen");
            }
        }
    }
}

Result CtrlrManager::initEmbeddedInstance()
{
	ctrlrNativeObject.reset(CtrlrNative::getNativeObject(*this));

	if (ctrlrNativeObject)
	{
		MemoryBlock defaultPanelData;

		if (ctrlrNativeObject->getDefaultPanel(defaultPanelData))
		{
			ctrlrPlayerInstanceTree = ValueTree::readFromGZIPData(defaultPanelData.getData(), defaultPanelData.getSize());

			if (ctrlrPlayerInstanceTree.isValid())
			{
				/* Set any defaults we need **/
				setEmbeddedDefaults();

				/* Look at the resources, see if
					we need to import them **/
				Result result = importInstanceResources(ctrlrNativeObject.get());

				if (result.wasOk())
				{
					/** We need to load the panel here
						but make sure the VST manager is
						done and that the restore state
						will give us only the program for
						the panel **/

					return (addInstancePanel());
				}
				else
				{
					return (result);
				}
			}
			else
			{
				return (Result::fail("InitEmbeddedInstance instance data is invalid"));
			}
		}
	}

	return (Result::fail("InitEmbeddedInstance failed to initialize the OS Native object"));
}

const String CtrlrManager::getInstanceNameForHost() const
{
	if (getInstanceMode() == InstanceSingle || getInstanceMode() == InstanceSingleRestriced)
	{
        #ifdef JucePlugin_Build_AAX
            // This code runs ONLY when building for AAX.
            return (ctrlrPlayerInstanceTree.getProperty(Ids::name).toString());
        #else
            // This code runs for ALL other formats (VST, AU, Standalone).
            return (ctrlrPlayerInstanceTree.getProperty(Ids::name).toString() + " | " + ctrlrPlayerInstanceTree.getProperty(Ids::panelAuthorName).toString()); // Updated v5.6.34.
        #endif
	}
	else
	{
		return ("CtrlrX"); // Updated v5.6.34. Default Plugin Name
	}
}

const String CtrlrManager::getInstanceName() const
{
	if (isSingleInstance())
	{
		return (ctrlrPlayerInstanceTree.getProperty(Ids::name));
	}
	else
	{
		return ("CtrlrX");
	}
}

const bool CtrlrManager::isSingleInstance() const
{
	return (ctrlrPlayerInstanceMode == InstanceSingle || ctrlrPlayerInstanceMode == InstanceSingleRestriced);
}


Result CtrlrManager::addInstancePanel()
{
	if (ctrlrPlayerInstanceTree.isValid())
	{
		CtrlrPanel *panel = new CtrlrPanel(*this, getInstanceName(), ctrlrPanels.size());
		ctrlrPanels.add (panel);

		Result restoreResult = panel->restoreState (ctrlrPlayerInstanceTree);

		if (!restoreResult.wasOk())
		{
			WARN("AddInstancePanel failed to restore the state of the panel");
			ctrlrPanels.clear (true);
			return (restoreResult);
		}

		managerTree.addChild (panel->getPanelTree(), -1, 0);
		addPanel (panel->getEditor(true));

		organizePanels();

		return (Result::ok());
	}
	else
	{
		return (Result::fail("AddInstancePanel failed, the decoded instance tree is invalid"));
	}
}

Result CtrlrManager::importInstanceResources(CtrlrNative *native)
{
	if (getCtrlrProperties().getProperties().getUserSettings())
	{
		if (getCtrlrProperties().getProperties().getUserSettings()->getFile().getParentDirectory().getChildFile(".delete_me_to_reload_resources").existsAsFile())
		{
			return (Result::ok());
		}
		else
		{
			getCtrlrProperties().getProperties().getUserSettings()->getFile().getParentDirectory().getChildFile(".delete_me_to_reload_resources").create();
		}
	}

	MemoryBlock resourcesData;
	Result resourceFetch = native->getDefaultResources (resourcesData);

	if (resourceFetch.failed())
	{
		return (Result::fail("ImportInstanceResources getDefaultResources() failed"));
	}

	ctrlrPlayerInstanceResources = ValueTree::readFromGZIPData(resourcesData.getData(), resourcesData.getSize());

	if (ctrlrPlayerInstanceResources.isValid())
	{
		return (Result::ok());
	}
	else
	{
		return (Result::fail("ImportInstanceResources got some data but couldn't understand it"));
	}
}

void CtrlrManager::restoreInstanceState(const ValueTree &instanceState)
{
	if (ctrlrEditor)
	{
		restoreEditorState();
	}

	getActivePanel()->setInstanceProperties (instanceState);
	getActivePanel()->setProgram (instanceState);

	ctrlrManagerRestoring = false;
}

CtrlrInstance CtrlrManager::getInstanceMode() const
{
	return (ctrlrPlayerInstanceMode);
}

XmlElement *CtrlrManager::saveState()
{
	if (getInstanceMode() == InstanceMulti)
	{
		for (int i=0; i<getNumPanels(); i++)
		{
			CtrlrPanel *panel = getPanel (i);

			if (panel != nullptr)
			{
				panel->generateCustomData();
				if (panel->hasChangedSinceSavePoint()) // Added v5.6.0
				{
					panel->setPanelDirty(true); // Updated v5.6.0
				}
			}
		}
		return (managerTree.createXml().release());
	}
	else
	{
		if (getActivePanel())
		{
			ValueTree program = getActivePanel()->getProgram();
			restoreProperties (managerTree, program);
			restoreProperties (getActivePanel()->getPanelTree(), program, 0, "panelMidi");
			restoreProperties (getActivePanel()->getPanelTree(), program, 0, Ids::panelCustomData.toString());
			program.addChild (getActivePanel()->getCustomData(),-1,nullptr);
			return (program.createXml().release());
		}
		return (nullptr);
	}
}

ValueTree &CtrlrManager::getInstanceTree()
{
	return (ctrlrPlayerInstanceTree);
}

ValueTree &CtrlrManager::getInstanceResourcesTree()
{
	return (ctrlrPlayerInstanceResources);
}
