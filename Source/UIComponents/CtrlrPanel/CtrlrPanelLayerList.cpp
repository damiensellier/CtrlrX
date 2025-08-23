#include "stdafx.h"

#include "CtrlrManager/CtrlrManager.h"
#include "CtrlrPanel/CtrlrPanel.h"
#include "CtrlrPanel/CtrlrPanelEditor.h"
#include "CtrlrPanel/CtrlrPanelCanvas.h"
#include "CtrlrPanelLayerListItem.h"
#include "CtrlrInlineUtilitiesGUI.h"

#include "CtrlrPanelLayerList.h"

//==============================================================================
CtrlrPanelLayerList::CtrlrPanelLayerList (CtrlrPanel &_owner)
    : owner(_owner),
      layerList (0)
{
    addAndMakeVisible (layerList = new ListBox ("Layer List", this));


    //[UserPreSize]
	layerList->setRowHeight (40);
	layerList->setMultipleSelectionEnabled (false);
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

CtrlrPanelLayerList::~CtrlrPanelLayerList()
{
    deleteAndZero (layerList);
}

//==============================================================================
void CtrlrPanelLayerList::paint (Graphics& g)
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

	// Calculate the actual layer index (reverse the order)
	int totalLayers = owner.getEditor()->getCanvas()->getNumLayers();
	int actualLayerIndex = totalLayers - 1 - rowNumber;  // Reverse the index

	itemInfo->setRow(actualLayerIndex);  // Use the actual layer index for the row
	itemInfo->setLayer(owner.getEditor()->getCanvas()->getLayerFromArray(actualLayerIndex));
	
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

	// Convert visual row to actual layer index
	int totalLayers = getNumRows();
	int actualLayerIndex = totalLayers - 1 - selectedRow;

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
	
	// Convert visual row to actual layer index
	int totalLayers = getNumRows();
	int actualLayerIndex = totalLayers - 1 - selectedRow;
	
	CtrlrPanelLayerListItem *item = dynamic_cast <CtrlrPanelLayerListItem*> (layerList->getComponentForRowNumber (selectedRow));
	CtrlrPanelCanvasLayer *layer = 0;
	if (item != nullptr)
	{
		layer = item->getLayer();
	}

	if (selectedRow-1 < 0)  // Can't move top visual row up
		return;

	if (owner.getEditor())
	{
		// In reversed view: visual "up" = actual "down" in the array
		owner.getEditor()->getCanvas()->moveLayer(layer, false);  // false = down in actual array
	}
	layerList->updateContent();
	layerList->selectRow (selectedRow-1);
}

void CtrlrPanelLayerList::moveLayerDown()
{
	const int selectedRow = layerList->getSelectedRow();
	
	// Convert visual row to actual layer index
	int totalLayers = getNumRows();
	int actualLayerIndex = totalLayers - 1 - selectedRow;

	CtrlrPanelLayerListItem *item = dynamic_cast <CtrlrPanelLayerListItem*> (layerList->getComponentForRowNumber (selectedRow));
	CtrlrPanelCanvasLayer *layer = 0;
	if (item != nullptr)
	{
		layer = item->getLayer();
	}

	if (selectedRow+1 >= getNumRows())  // Can't move bottom visual row down
		return;

	if (owner.getEditor())
	{
		// In reversed view: visual "down" = actual "up" in the array
		owner.getEditor()->getCanvas()->moveLayer(layer, true);  // true = up in actual array
	}
	layerList->updateContent();
	layerList->selectRow (selectedRow+1);
}

void CtrlrPanelLayerList::refresh()
{
	layerList->updateContent();
	updateAllButtonStates();
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
		menu.addItem (6, "Restore view");
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
			restoreLayerVisibility();
			updateAllButtonStates();
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
	int visualRow = localPos.y / layerList->getRowHeight();

	// Clamp to valid range
	visualRow = jmax(0, jmin(visualRow, getNumRows() - 1));

	// Store the visual row (we'll convert to actual layer index later)
	dropInsertionIndex = visualRow;

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
	int sourceVisualRow = desc.getTrailingIntValue();

	// Calculate target position
	Point<int> localPos = layerList->getLocalPoint(this, dragSourceDetails.localPosition);
	int targetVisualRow = localPos.y / layerList->getRowHeight();
	targetVisualRow = jmax(0, jmin(targetVisualRow, getNumRows() - 1));

	if (targetVisualRow != sourceVisualRow && sourceVisualRow >= 0 && sourceVisualRow < getNumRows())
	{
		// Convert visual rows to actual layer indices
		int totalLayers = getNumRows();
		int sourceActualIndex = totalLayers - 1 - sourceVisualRow;
		int targetActualIndex = totalLayers - 1 - targetVisualRow;

		moveLayerToPosition(sourceActualIndex, targetActualIndex);
	}

	dropInsertionIndex = -1;
	repaint();
}
void CtrlrPanelLayerList::moveLayerToPosition(int sourceActualIndex, int targetActualIndex)
{
	if (owner.getEditor() && owner.getEditor()->getCanvas())
	{
		// Get the layer that's being moved
		CtrlrPanelCanvasLayer* sourceLayer = owner.getEditor()->getCanvas()->getLayerFromArray(sourceActualIndex);

		if (sourceLayer != nullptr)
		{
			if (targetActualIndex < sourceActualIndex)
			{
				// Moving to lower actual index (higher in visual list) - call moveLayerUp
				for (int i = sourceActualIndex; i > targetActualIndex; --i)
				{
					owner.getEditor()->getCanvas()->moveLayer(sourceLayer, true); // true = up in actual array
				}
			}
			else if (targetActualIndex > sourceActualIndex)
			{
				// Moving to higher actual index (lower in visual list) - call moveLayerDown
				for (int i = sourceActualIndex; i < targetActualIndex; ++i)
				{
					owner.getEditor()->getCanvas()->moveLayer(sourceLayer, false); // false = down in actual array
				}
			}

			// Update the list display and select the new visual position
			layerList->updateContent();

			// Convert the target actual index back to visual row for selection
			int totalLayers = getNumRows();
			int targetVisualRow = totalLayers - 1 - targetActualIndex;
			layerList->selectRow(targetVisualRow);
		}
	}
}
void CtrlrPanelLayerList::isolateLayer(int targetLayerIndex)
{
	if (!owner.getEditor() || !owner.getEditor()->getCanvas())
		return;

	// FIRST: Save the current states BEFORE making any changes
	owner.saveLayerVisibilityStates();

	// Remember which layer was isolated
	isolatedLayerIndex = targetLayerIndex;

	// THEN: Hide all layers except the target layer
	for (int i = 0; i < getNumRows(); ++i)
	{
		CtrlrPanelCanvasLayer* layer = owner.getEditor()->getCanvas()->getLayerFromArray(i);
		if (layer)
		{
			if (i == targetLayerIndex)
			{
				layer->setProperty(Ids::uiPanelCanvasLayerVisibility, true);
			}
			else
			{
				layer->setProperty(Ids::uiPanelCanvasLayerVisibility, false);
			}
		}
	}

	layerIsolationActive = true;
	refresh();
	updateAllButtonStates();

	_DBG("Layer " + String(targetLayerIndex) + " isolated - all other layers hidden");
}

// Update the restoreLayerVisibility method:
void CtrlrPanelLayerList::restoreLayerVisibility()
{
	owner.restoreLayerVisibilityStates();
	layerIsolationActive = false;
	isolatedLayerIndex = -1;  // Reset - no layer is isolated anymore
	refresh();
	updateAllButtonStates();
	_DBG("Layer visibility restored");
}

void CtrlrPanelLayerList::updateAllButtonStates()
{
	for (int i = 0; i < getNumRows(); ++i)
	{
		if (Component* comp = layerList->getComponentForRowNumber(i))
		{
			if (CtrlrPanelLayerListItem* item = dynamic_cast<CtrlrPanelLayerListItem*>(comp))
			{
				item->updateButtonStates();
			}
		}
	}
}
bool CtrlrPanelLayerList::isLayerIsolated(int layerIndex) const
{
	return (isolatedLayerIndex == layerIndex && owner.hasLayerVisibilityStates());
}
