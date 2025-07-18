#include "stdafx.h"
#include "CtrlrManager/CtrlrManager.h"
#include "CtrlrChildWindowContent.h"
#include "CtrlrLog.h"
#include "CtrlrChildWindow.h"
#include "CtrlrPanelWindowManager.h"
#include "CtrlrPanel/CtrlrPanel.h"
#include "CtrlrChildWindowContainer.h"

CtrlrChildWindowContainer::CtrlrChildWindowContainer (CtrlrWindowManager &_owner)
    : content(nullptr), owner(_owner),
      menuBar (0)
{
    addAndMakeVisible (menuBar = new MenuBarComponent (this));

    setSize (600, 400);

}

CtrlrChildWindowContainer::~CtrlrChildWindowContainer()
{
    deleteAndZero (menuBar);
}


void CtrlrChildWindowContainer::paint (Graphics& g)
{
    g.fillAll(findColour(DocumentWindow::backgroundColourId)); // Added v5.6.31
}

void CtrlrChildWindowContainer::resized()
{
	if (content)
	{
        if (content->getMenuBarNames().size() != 0) // Added v5.6.31
        {
            menuBar->setBounds (0, 0, getWidth(), (int)(owner.managerOwner.getProperty(Ids::ctrlrMenuBarHeight), 24)); // added v5.6.31. Fallback 24px value added to fix for hidden menuBar on exported instance
            addAndMakeVisible (menuBar);
            content->setBounds (0, (int)(owner.managerOwner.getProperty(Ids::ctrlrMenuBarHeight), 24), getWidth(), getHeight() - (int)(owner.managerOwner.getProperty(Ids::ctrlrMenuBarHeight), 24)); // added v5.6.31. Fallback 24px value added to fix for hidden menuBar on exported instance
        }
        else
        {
            content->setBounds (0, 0, getWidth(), getHeight());
        }
	}
}


void CtrlrChildWindowContainer::setContent(CtrlrChildWindowContent *_content)
{
	content = _content;

	if (content != nullptr)
	{
		deleteAndZero (menuBar);
		addAndMakeVisible (menuBar = new MenuBarComponent (this));
		addAndMakeVisible (content);
		content->setContainer (this);
	}

	resized();
}

CtrlrChildWindowContent *CtrlrChildWindowContainer::getContent()
{
	return (content);
}

CtrlrWindowManager &CtrlrChildWindowContainer::getOwner()
{
	return (owner);
}

void CtrlrChildWindowContainer::closeWindow()
{
	if (getParentComponent())
	{
		delete getParentComponent();
	}
}

StringArray CtrlrChildWindowContainer::getMenuBarNames()
{
	if (content)
	{
		if (content->getMenuBarNames().size() != 0)
		{
			return (content->getMenuBarNames());
		}
	}
    const char* const names[] = { "File", nullptr };
    return StringArray (names);
}

PopupMenu CtrlrChildWindowContainer::getMenuForIndex(int topLevelMenuIndex, const String &menuName)
{
	if (content)
	{
		if (content->getMenuForIndex (topLevelMenuIndex, menuName).getNumItems() > 0)
		{
			return (content->getMenuForIndex (topLevelMenuIndex, menuName));
		}
	}

	PopupMenu m;
	if (topLevelMenuIndex == 0)
	{
		 m.addItem (1, "Close", false); // Requires close handle
	}

	return (m);
}

void CtrlrChildWindowContainer::menuItemSelected(int menuItemID, int topLevelMenuIndex)
{
    if (content)
    {
        content->menuItemSelected(menuItemID, topLevelMenuIndex);
    }
}
