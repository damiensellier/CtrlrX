#include "stdafx.h"
/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  14 Nov 2011 8:14:25pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
#include "CtrlrManager/CtrlrManager.h"
#include "CtrlrPanel/CtrlrPanel.h"
#include "CtrlrPanel/CtrlrPanelEditor.h"
#include "CtrlrPanel/CtrlrPanelCanvas.h"
#include "CtrlrPanelLayerListItem.h"
#include "CtrlrInlineUtilitiesGUI.h"
//[/Headers]

#include "CtrlrPanelLayerList.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
CtrlrPanelLayerList::CtrlrPanelLayerList(CtrlrPanel& _owner)
	: owner(_owner),
	layerList(0),
	dropInsertionIndex(-1),
	layerIsolationActive(false)
{
	addAndMakeVisible(layerList = new ListBox("Layer List", this));

	layerList->setRowHeight(40);
	layerList->setMultipleSelectionEnabled(false);

	setSize(600, 400);
}

CtrlrPanelLayerList::~CtrlrPanelLayerList()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    deleteAndZero (layerList);


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void CtrlrPanelLayerList::paint(Graphics& g)
{
	// Draw drop insertion indicator
	if (dropInsertionIndex >= 0)
	{
		g.setColour(Colours::blue);
		int y = dropInsertionIndex * layerList->getRowHeight();
		g.fillRect(0, y - 1, getWidth(), 3);
	}
	if (dropInsertionIndex >= 0)
	{
		g.setColour(Colours::blue);
		int y = dropInsertionIndex * layerList->getRowHeight();
		g.fillRect(0, y - 1, getWidth(), 3);
	}

	// Draw isolation indicator
	if (layerIsolationActive)
	{
		g.setColour(Colours::orange.withAlpha(0.3f));
		g.fillRect(2, 2, getWidth() - 4, 20);

		g.setColour(Colours::orange.darker());
		g.setFont(Font(11.0f, Font::bold));
		g.drawText("LAYER ISOLATION ACTIVE", 5, 2, getWidth() - 10, 20, Justification::centredLeft);
	}
}

void CtrlrPanelLayerList::resized()
{
    layerList->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
int CtrlrPanelLayerList::getNumRows()
{
	return (owner.getEditor()->getCanvas()->getNumLayers());
}

void CtrlrPanelLayerList::paintListBoxItem (int rowNumber, Graphics& g, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
    {
        gui::drawSelectionRectangle (g, width, height);
    }
}

Component* CtrlrPanelLayerList::refreshComponentForRow (int rowNumber, bool isRowSelected, Component* existingComponentToUpdate)
{
	CtrlrPanelLayerListItem* itemInfo = (CtrlrPanelLayerListItem*) existingComponentToUpdate;

	if (itemInfo == 0)
		itemInfo = new CtrlrPanelLayerListItem (*this);

	itemInfo->setRow (rowNumber);
	itemInfo->setLayer (owner.getEditor()->getCanvas()->getLayerFromArray(rowNumber));
	return itemInfo;
}

void CtrlrPanelLayerList::setSelectedRow(const int rowToSelect)
{
	layerList->selectRow (rowToSelect);
}

void CtrlrPanelLayerList::buttonClicked (Button *button)
{
}

CtrlrPanel &CtrlrPanelLayerList::getOwner()
{
	return (owner);
}

void CtrlrPanelLayerList::addLayer()
{
	if (owner.getEditor())
	{
		owner.getEditor()->getCanvas()->addLayer(ValueTree());
	}
	layerList->updateContent();
}

void CtrlrPanelLayerList::removeLayer()
{
	const int selectedRow = layerList->getSelectedRow();
	CtrlrPanelLayerListItem *item = dynamic_cast <CtrlrPanelLayerListItem*> (layerList->getComponentForRowNumber (selectedRow));
	CtrlrPanelCanvasLayer *layer = 0;
	if (item != nullptr)
	{
		layer = item->getLayer();
	}

	if (owner.getEditor())
	{
		owner.getEditor()->getCanvas()->removeLayer(layer);
	}
	layerList->updateContent();
}

void CtrlrPanelLayerList::moveLayerUp()
{
	const int selectedRow = layerList->getSelectedRow();
	CtrlrPanelLayerListItem *item = dynamic_cast <CtrlrPanelLayerListItem*> (layerList->getComponentForRowNumber (selectedRow));
	CtrlrPanelCanvasLayer *layer = 0;
	if (item != nullptr)
	{
		layer = item->getLayer();
	}

	if (selectedRow-1 < 0)
		return;

	if (owner.getEditor())
	{
		owner.getEditor()->getCanvas()->moveLayer(layer);
	}
	layerList->updateContent();
	layerList->selectRow (selectedRow-1);
}

void CtrlrPanelLayerList::moveLayerDown()
{
	const int selectedRow = layerList->getSelectedRow();
	CtrlrPanelLayerListItem *item = dynamic_cast <CtrlrPanelLayerListItem*> (layerList->getComponentForRowNumber (selectedRow));
	CtrlrPanelCanvasLayer *layer = 0;
	if (item != nullptr)
	{
		layer = item->getLayer();
	}

	if (selectedRow+1 >= getNumRows())
		return;

	if (owner.getEditor())
	{
		owner.getEditor()->getCanvas()->moveLayer(layer,false);
	}
	layerList->updateContent();
	layerList->selectRow (selectedRow+1);
}

void CtrlrPanelLayerList::refresh()
{
	layerList->updateContent();
}

StringArray CtrlrPanelLayerList::getMenuBarNames()
{
	const char* const names[] = { "File", "Edit", "View", nullptr };
	return StringArray (names);
}

PopupMenu CtrlrPanelLayerList::getMenuForIndex(int topLevelMenuIndex, const String &menuName)
{
	PopupMenu menu;
	if (topLevelMenuIndex == 0)
	{
		menu.addItem (1, "Close");
	}
	else if (topLevelMenuIndex == 1)
	{
		menu.addItem (2, "Add layer");
		menu.addItem (3, "Remove layer");
		menu.addSectionHeader ("Reposition");
		menu.addItem (4, "Move up");
		menu.addItem (5, "Move down");
	}
	else if (topLevelMenuIndex == 2)
	{
		menu.addItem (6, "Refresh view");
	}
	return (menu);
}

void CtrlrPanelLayerList::menuItemSelected(int menuItemID, int topLevelMenuIndex)
{
	if (topLevelMenuIndex == 1)
	{
		if (menuItemID == 2)
			addLayer();
		if (menuItemID == 3)
			removeLayer();
		if (menuItemID == 4)
			moveLayerUp();
		if (menuItemID == 5)
			moveLayerDown();
	}
	if (topLevelMenuIndex == 2)
	{
		if (menuItemID == 6)
			refresh();
	}
	if (topLevelMenuIndex == 0 && menuItemID==1)
	{
        // close handle
        owner.getWindowManager().toggle (CtrlrPanelWindowManager::LayerEditor, false);
	}
	
}
bool CtrlrPanelLayerList::isInterestedInDragSource(const SourceDetails& dragSourceDetails)
{
	// We're interested if the drag source contains "layer_item" in the description
	return dragSourceDetails.description.toString().contains("layer_item");
}

void CtrlrPanelLayerList::itemDragEnter(const SourceDetails& dragSourceDetails)
{
	repaint();
}

void CtrlrPanelLayerList::itemDragMove(const SourceDetails& dragSourceDetails)
{
	// Calculate which row the mouse is over
	Point<int> localPos = layerList->getLocalPoint(this, dragSourceDetails.localPosition);
	dropInsertionIndex = localPos.y / layerList->getRowHeight();

	// Clamp to valid range
	dropInsertionIndex = jmax(0, jmin(dropInsertionIndex, getNumRows() - 1));

	repaint();
}

void CtrlrPanelLayerList::itemDragExit(const SourceDetails& dragSourceDetails)
{
	dropInsertionIndex = -1;
	repaint();
}

void CtrlrPanelLayerList::itemDropped(const SourceDetails& dragSourceDetails)
{
	if (!isInterestedInDragSource(dragSourceDetails))
		return;

	// Extract the source row index from the description
	String desc = dragSourceDetails.description.toString();
	int sourceRow = desc.getTrailingIntValue();

	// Calculate target position
	Point<int> localPos = layerList->getLocalPoint(this, dragSourceDetails.localPosition);
	int targetRow = localPos.y / layerList->getRowHeight();
	targetRow = jmax(0, jmin(targetRow, getNumRows() - 1));

	if (targetRow != sourceRow && sourceRow >= 0 && sourceRow < getNumRows())
	{
		moveLayerToPosition(sourceRow, targetRow);
	}

	dropInsertionIndex = -1;
	repaint();
}

void CtrlrPanelLayerList::moveLayerToPosition(int sourceIndex, int targetIndex)
{
	if (owner.getEditor() && owner.getEditor()->getCanvas())
	{
		// Get the layer that's being moved
		CtrlrPanelCanvasLayer* sourceLayer = owner.getEditor()->getCanvas()->getLayerFromArray(sourceIndex);

		if (sourceLayer != nullptr)
		{
			// You'll need to implement moveLayerToIndex in your canvas class
			// For now, we'll use the existing move methods as a fallback

			if (targetIndex < sourceIndex)
			{
				// Moving up - call moveLayerUp multiple times
				for (int i = sourceIndex; i > targetIndex; --i)
				{
					owner.getEditor()->getCanvas()->moveLayer(sourceLayer, true); // true = up
				}
			}
			else if (targetIndex > sourceIndex)
			{
				// Moving down - call moveLayerDown multiple times  
				for (int i = sourceIndex; i < targetIndex; ++i)
				{
					owner.getEditor()->getCanvas()->moveLayer(sourceLayer, false); // false = down
				}
			}

			// Update the list display
			layerList->updateContent();
			layerList->selectRow(targetIndex);
		}
	}
}
void CtrlrPanelLayerList::isolateLayer(int targetLayerIndex)
{
	if (!owner.getEditor() || !owner.getEditor()->getCanvas())
		return;

	// Store current visibility states if not already in isolation mode
	if (!layerIsolationActive)
	{
		originalLayerVisibility.clear();

		// Store all current layer visibility states
		for (int i = 0; i < getNumRows(); ++i)
		{
			CtrlrPanelCanvasLayer* layer = owner.getEditor()->getCanvas()->getLayerFromArray(i);
			if (layer)
			{
				bool isVisible = layer->getProperty(Ids::uiPanelCanvasLayerVisibility);
				originalLayerVisibility.add(isVisible);
			}
			else
			{
				originalLayerVisibility.add(true); // Default to visible if layer is null
			}
		}
		layerIsolationActive = true;
	}

	// Hide all layers above the target layer
	for (int i = 0; i < getNumRows(); ++i)
	{
		CtrlrPanelCanvasLayer* layer = owner.getEditor()->getCanvas()->getLayerFromArray(i);
		if (layer)
		{
			if (i < targetLayerIndex)
			{
				// Hide layers above (lower index = higher in stack)
				layer->setProperty(Ids::uiPanelCanvasLayerVisibility, false);
			}
			else if (i == targetLayerIndex)
			{
				// Ensure target layer is visible
				layer->setProperty(Ids::uiPanelCanvasLayerVisibility, true);
			}
			// Leave layers below unchanged
		}
	}

	// Refresh the list to show updated visibility states
	refresh();

	// Optional: Show status message
	_DBG("Layer isolation active - layers above layer " + String(targetLayerIndex) + " are hidden");
}

void CtrlrPanelLayerList::restoreLayerVisibility()
{
	if (!layerIsolationActive || !owner.getEditor() || !owner.getEditor()->getCanvas())
		return;

	// Restore all original visibility states
	for (int i = 0; i < getNumRows() && i < originalLayerVisibility.size(); ++i)
	{
		CtrlrPanelCanvasLayer* layer = owner.getEditor()->getCanvas()->getLayerFromArray(i);
		if (layer)
		{
			layer->setProperty(Ids::uiPanelCanvasLayerVisibility, originalLayerVisibility[i]);
		}
	}

	// Clear isolation state
	layerIsolationActive = false;
	originalLayerVisibility.clear();

	// Refresh the list to show restored visibility states
	refresh();

	// Optional: Show status message
	_DBG("Layer visibility restored to original state");
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="CtrlrPanelLayerList" componentName=""
                 parentClasses="public CtrlrChildWindowContent, public ListBoxModel"
                 constructorParams="CtrlrPanel &amp;_owner" variableInitialisers="owner(_owner)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330000013"
                 fixedSize="1" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ffffff"/>
  <GENERICCOMPONENT name="" id="bf3c104833fd7aa" memberName="layerList" virtualName=""
                    explicitFocusOrder="0" pos="0 0 0M 0M" class="ListBox" params="&quot;Layer List&quot;, this"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
