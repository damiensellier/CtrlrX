#include "stdafx.h"
#include "CtrlrLuaMethodEditorTabs.h"
#include "CtrlrLuaMethodEditor.h"
#include "CtrlrLuaMethodCodeEditor.h"
#include "Core/CtrlrPanel/CtrlrPanel.h"

// Tabbed component
CtrlrLuaMethodEditorTabs::CtrlrLuaMethodEditorTabs(CtrlrLuaMethodEditor &_owner)
	:	owner(_owner),
		TabbedComponent(TabbedButtonBar::TabsAtTop)
{
	setTabBarDepth(owner.getOwner().getOwner().getProperty(Ids::ctrlrTabBarDepth)); // Tab height for horizontal bar
    getTabbedButtonBar().setMinimumTabScaleFactor(1.0); // Min tab width ratio, when total tabBar width exceeds the window width, a + sign is displayed
}

CtrlrLuaMethodEditorTabs::~CtrlrLuaMethodEditorTabs()
{
}

TabBarButton *CtrlrLuaMethodEditorTabs::createTabButton (const String &tabName, int tabIndex)
{
	TabBarButton *btn = new TabBarButton (tabName, getTabbedButtonBar());
    
    CtrlrMethodEditorTabCloseButton *close = new CtrlrMethodEditorTabCloseButton(owner, tabIndex); // Added v5.6.30. Creates extra component *close
    close->setMouseCursor (MouseCursor::PointingHandCursor);
    close->setSize (22, 20); // Added v5.6.30
    btn->setExtraComponent (close, TabBarButton::afterText); // Added v5.6.30. Defines *close as extraComponent and sets its position

	return (btn);
}

void CtrlrLuaMethodEditorTabs::currentTabChanged(int newCurrentTabIndex, const String &newCurrentTabName)
{
	CtrlrLuaMethodCodeEditor * codeEditor = dynamic_cast<CtrlrLuaMethodCodeEditor*> (getCurrentContentComponent());
	if (owner.findDialogActive == true)
	{
		if (codeEditor)
			codeEditor->getCodeComponent()->showFindPanel(false);
	}
	else
	{
		if (codeEditor)
			codeEditor->getCodeComponent()->hideFindPanel();
	}
}
