#include "stdafx.h"
#include "CtrlrValueTreeEditor.h"
#include "CtrlrInlineUtilitiesGUI.h"

//=========================================================================================================
bool CtrlrValueTreeEditorTree::keyPressed (const KeyPress &key)
{
	if (getNumSelectedItems() == 1)
	{
		if (getSelectedItem(0))
		{
			CtrlrValueTreeEditorItem *item = dynamic_cast<CtrlrValueTreeEditorItem*>(getSelectedItem(0));

			if (item != nullptr)
			{
				item->keyPressed(key);
			}
		}
	}

	return (TreeView::keyPressed(key));
}



//=========================================================================================================

CtrlrValueTreeEditorItem::CtrlrValueTreeEditorItem(CtrlrValueTreeEditorLookProvider &_provider, ValueTree _treeToEdit, const Identifier &_nameIdentifier)
	: treeToEdit(_treeToEdit), provider(_provider), nameIdentifier(_nameIdentifier)
{
	treeToEdit.addListener (this);
}

CtrlrValueTreeEditorItem::~CtrlrValueTreeEditorItem()
{
	treeToEdit.removeListener (this);
}

bool CtrlrValueTreeEditorItem::mightContainSubItems()
{
	return (treeToEdit.getNumChildren() ? true : false);
}

String CtrlrValueTreeEditorItem::getUniqueName () const
{
	return (provider.getUniqueName(treeToEdit));
}

void CtrlrValueTreeEditorItem::itemSelectionChanged (bool isNowSelected)
{
}

int	CtrlrValueTreeEditorItem::getItemWidth () const
{
	return (jmax<int>(152, 48 + provider.getItemFont(treeToEdit).getStringWidth (provider.getUniqueName(treeToEdit)))); // Updated v5.6.31
}

int	CtrlrValueTreeEditorItem::getItemHeight () const
{
	return (provider.getItemHeight(treeToEdit));
}

bool CtrlrValueTreeEditorItem::canBeSelected () const
{
	return (provider.canBeSelected(treeToEdit));
}

void CtrlrValueTreeEditorItem::itemOpennessChanged (bool isNowOpen)
{
	if (isNowOpen)
	{
		if (getNumSubItems() == 0)
		{
			for (int i=0; i<treeToEdit.getNumChildren(); i++)
			{
				addSubItem (new CtrlrValueTreeEditorItem (provider, treeToEdit.getChild(i), nameIdentifier));
			}
		}
	}
	else
	{
		clearSubItems();
	}

	provider.itemOpennessChanged(isNowOpen);
}

var CtrlrValueTreeEditorItem::getDragSourceDescription ()
{
	Array <ValueTree> selectedTreeItems;
	String returnValue;

	if (getOwnerView())
	{
		for (int i=0; i<getOwnerView()->getNumSelectedItems(); i++)
		{
			CtrlrValueTreeEditorItem *item = dynamic_cast<CtrlrValueTreeEditorItem*>(getOwnerView()->getSelectedItem(i));
			selectedTreeItems.add (item->getTree());
		}
	}

	if ((returnValue = provider.getDragSourceDescription(selectedTreeItems)) != "")
	{
		return (returnValue);
	}
	else
	{
		for (int i=0; i<selectedTreeItems.size(); i++)
		{
			returnValue << selectedTreeItems[i].getType().toString() + ";";
		}
	}

	return (returnValue);
}

bool CtrlrValueTreeEditorItem::isInterestedInDragSource (const DragAndDropTarget::SourceDetails &dragSourceDetails)
{
	return (provider.isInterestedInDragSource (treeToEdit, dragSourceDetails));
}

// GUI For TreeView Items. Updated v5.6.31
void CtrlrValueTreeEditorItem::paintItem (Graphics &g, int width, int height)
{
    std::unique_ptr<Drawable> icon(provider.getIconForItem (treeToEdit));
	auto &currentLnF = getDefaultLookAndFeel(); // Added v5.6.31
    
    if (isSelected())
	{
        g.setColour(currentLnF.findColour(TextEditor::highlightedTextColourId));
        gui::drawSelectionRectangle (g, width, height, currentLnF.findColour(CodeEditorComponent::lineNumberBackgroundId), 1.0f, 1.0f, 0.0f, 0.0f); // Updated v5.6.31
        // gui::drawSelectionRectangle (g, width, height)); // Removed v5.6.31
	}
	// g.setColour (Colours::black); // Removed v5.6.31
    g.setColour(currentLnF.findColour(TextEditor::textColourId)); // Added v5.6.31
       
	AttributedString as = provider.getDisplayString(treeToEdit);
	as.setJustification (Justification (Justification::centredLeft));
	as.draw (g, Rectangle <float> (24.0, 0.0, width - 24.0, height));

	if (icon)
		icon->drawWithin(g, Rectangle<float>(4,0, 16, height), RectanglePlacement::centred, 1.0f);
	// g.drawImageWithin (icon, 4, 0, 16, height, RectanglePlacement (RectanglePlacement::centred));
}

void CtrlrValueTreeEditorItem::itemDropped (const DragAndDropTarget::SourceDetails &dragSourceDetails, int insertIndex)
{
	provider.itemDropped (treeToEdit, dragSourceDetails, insertIndex);
}

void CtrlrValueTreeEditorItem::itemClicked (const MouseEvent &e)
{
	provider.itemClicked (e, treeToEdit);
}

void CtrlrValueTreeEditorItem::itemDoubleClicked (const MouseEvent &e)
{
	provider.itemDoubleClicked (e, treeToEdit);
}

void CtrlrValueTreeEditorItem::keyPressed (const KeyPress &key)
{
	if (key.getKeyCode() == 65649) // F2
	{
		if (!provider.canBeRenamed(treeToEdit))
			return;

		AlertWindow wnd("Rename item","Rename current item type: " + treeToEdit.getType().toString(), AlertWindow::QuestionIcon, getOwnerView());
		wnd.addTextEditor ("name", treeToEdit.getProperty(nameIdentifier), "Name:", false);
		wnd.addButton ("OK", 1, KeyPress(KeyPress::returnKey));
		wnd.addButton ("Cancel", 0, KeyPress(KeyPress::escapeKey));
		if (wnd.runModalLoop())
		{
			if (provider.renameItem (treeToEdit, wnd.getTextEditorContents("name")))
			{
				treeToEdit.setProperty (Ids::name, wnd.getTextEditorContents("name"), nullptr);
			}

			provider.triggerAsyncUpdate();
		}
	}
}

void CtrlrValueTreeEditorItem::valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property)
{
	repaintItem();
}
