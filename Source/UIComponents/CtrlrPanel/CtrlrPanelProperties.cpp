#include "stdafx.h"
#include "CtrlrPanelEditor.h"
#include "CtrlrPanelUtilities.h"
#include "CtrlrLog.h"
#include "CtrlrPanelComponentProperties.h"
#include "CtrlrPanelResourceEditor.h"


#include "CtrlrPanelProperties.h"

CtrlrPanelProperties::CtrlrPanelProperties (CtrlrPanelEditor &_owner)
    : Component (L"Properties"),
      owner(_owner),
      tabbedComponent (0)
{

    
    addAndMakeVisible (tabbedComponent = new TabbedComponent (TabbedButtonBar::TabsAtRight));
    
    tabbedComponent->setTabBarDepth (owner.getOwner().getOwner().getProperty(Ids::ctrlrTabBarDepth));
    tabbedComponent->setCurrentTabIndex (-1);
    tabbedComponent->setOutline (1);
    
	CtrlrPanelComponentProperties *props = new CtrlrPanelComponentProperties (owner);
        
	tabbedComponent->addTab ("General",
                             getLookAndFeel().findColour(TabbedComponent::backgroundColourId), // Updated v5.6.31
                             props,
                             true
                             );
    
    tabbedComponent->addTab ("Resources",
                             getLookAndFeel().findColour(TabbedComponent::backgroundColourId), // Updated v5.6.31
                             new CtrlrPanelResourceEditor(owner),
                             true
                             );
    
	tabbedComponent->addTab ("Utility",
                             getLookAndFeel().findColour(TabbedComponent::backgroundColourId), // Updated v5.6.31
                             new CtrlrPanelUtilities(owner),
                             true
                             );

    tabbedComponent->getTabbedButtonBar().setColour(TabbedButtonBar::frontTextColourId, findColour(Label::textColourId)); // Added v5.6.31
    tabbedComponent->getTabbedButtonBar().setColour(TabbedButtonBar::tabTextColourId, findColour(Label::textColourId).withAlpha(0.6f)); // Added v5.6.31
    tabbedComponent->getTabbedButtonBar().setColour(TabbedButtonBar::tabOutlineColourId, findColour(Slider::textBoxOutlineColourId)); // Added v5.6.31
    
    
	ctrlrPanelFindProperty.reset(new CtrlrPanelFindProperty(owner, props));
	addAndMakeVisible(ctrlrPanelFindProperty.get());
	setSize (216, 364);
}

CtrlrPanelProperties::~CtrlrPanelProperties()
{
	CtrlrPanelComponentProperties *p = dynamic_cast <CtrlrPanelComponentProperties*>(tabbedComponent->getTabContentComponent (0));
	if (p)
	{
		owner.getOwner().getCtrlrManagerOwner().removeListener (p);
	}
    deleteAndZero (tabbedComponent);
}

void CtrlrPanelProperties::paint (Graphics& g)
{
    g.fillAll(findColour(DocumentWindow::backgroundColourId));
}

void CtrlrPanelProperties::resized()
{
	ctrlrPanelFindProperty->setBounds(0,0,getWidth() - (int)owner.getOwner().getOwner().getProperty(Ids::ctrlrTabBarDepth),32);
    tabbedComponent->setBounds (0, 32, getWidth() - 0, getHeight() - 32);
    repaint();
}

void CtrlrPanelProperties::lookAndFeelChanged() // Added v5.6.31
{
    tabbedComponent->getTabbedButtonBar().setColour(TabbedButtonBar::frontTextColourId, findColour(Label::textColourId)); // Added v5.6.31
    tabbedComponent->getTabbedButtonBar().setColour(TabbedButtonBar::tabTextColourId, findColour(Label::textColourId).withAlpha(0.6f)); // Added v5.6.31
    tabbedComponent->getTabbedButtonBar().setColour(TabbedButtonBar::tabOutlineColourId, findColour(Slider::textBoxOutlineColourId)); // Added v5.6.31
    repaint(); // Added v5.6.31
}

void CtrlrPanelProperties::changeListenerCallback (ChangeBroadcaster* source)
{
}

void CtrlrPanelProperties::refreshAll()
{
	for (int i=0; i<tabbedComponent->getNumTabs(); i++)
	{
		CtrlrPanelComponentProperties *cp = dynamic_cast<CtrlrPanelComponentProperties*>(tabbedComponent->getTabContentComponent(i));
		if (cp!=0)
		{
			cp->refreshAll();
		}
	}
}

void CtrlrPanelProperties::layoutChanged()
{
	if ((bool)owner.getProperty(Ids::uiPanelPropertiesOnRight) == true)
	{
		tabbedComponent->setOrientation(TabbedButtonBar::TabsAtLeft);
	}
	else
	{
		tabbedComponent->setOrientation(TabbedButtonBar::TabsAtRight);
	}
}
