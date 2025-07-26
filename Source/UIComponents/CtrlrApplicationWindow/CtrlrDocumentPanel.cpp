#include "stdafx.h"
#include "CtrlrManager/CtrlrManager.h"
#include "CtrlrEditor.h"
#include "CtrlrMacros.h"
#include "CtrlrProcessor.h"
#include "CtrlrPanel/CtrlrPanelEditor.h"
#include "CtrlrDocumentPanel.h"
#include "CtrlrInlineUtilitiesGUI.h"

CtrlrDocumentPanel::CtrlrDocumentPanel (CtrlrManager &_owner)
    : ctrlrEditor(0), owner(_owner)
{
	/* Full screen mode is not completely implemented in JUCE 6
	   we get some assertions when adding the first Tab to an invisble
	   panel, as it wants to grep the focus, which is not possible at this stage.
	*/
	// useFullscreenWhenOneDocument (true);
    setSize (600, 400);
    setBackgroundColour((Colours::lightgrey).darker(0.2f)); // Added v.6.30. Updated v5.6.31 for (0.2f). Sets background colour behind main window by default on grey to please everyone :)
}

CtrlrDocumentPanel::~CtrlrDocumentPanel()
{
}


CtrlrDocumentPanelCloseButton::CtrlrDocumentPanelCloseButton(const String& buttonName) // Added v5.6.30
    : Button ("")
{
    setSize (18, 18);
}

CtrlrDocumentPanelCloseButton::~CtrlrDocumentPanelCloseButton() // Added v5.6.30
{
}


void CtrlrDocumentPanel::resized()
{
	MultiDocumentPanel::resized();
}

bool CtrlrDocumentPanel::tryToCloseDocument (Component* component)
{
	return (true);
}

void CtrlrDocumentPanel::activeDocumentChanged()
{
    CtrlrEditor *ed = dynamic_cast <CtrlrEditor*> (getParentComponent());
    if (ed)
    {
        ed->activeCtrlrChanged();
    }
    
    if (getCurrentTabbedComponent()) {
        
        if (owner.getInstanceMode() == InstanceSingleRestriced) // Added v5.6.31. Hides tabs on exported restricted instances
        {
            getCurrentTabbedComponent()->setTabBarDepth(0); // Tab height for horizontal bar
            getCurrentTabbedComponent()->getTabbedButtonBar().setMinimumTabScaleFactor(0); // Min tab width ratio
        }
        else
        {
            getCurrentTabbedComponent()->setTabBarDepth(owner.getProperty(Ids::ctrlrTabBarDepth)); // Tab height for horizontal bar
            getCurrentTabbedComponent()->getTabbedButtonBar().setMinimumTabScaleFactor(1.0); // Min tab width ratio
        }
        
        TabbedButtonBar &bar = getCurrentTabbedComponent()->getTabbedButtonBar();
        
        for (int i=0; i<bar.getNumTabs(); i++)
        {
            TabBarButton *button = bar.getTabButton (i); // Gets the panel tab button(i)
            
            if (button)
            {
                CtrlrDocumentPanelCloseButton *closeTabButton = new CtrlrDocumentPanelCloseButton("x"); // Added v5.6.30. Brings back the close button for panel tabs
                closeTabButton->addListener (this);
                closeTabButton->setSize(20, 20);
                closeTabButton->getProperties().set ("index", bar.indexOfTabButton(button));
                closeTabButton->setMouseCursor (MouseCursor::PointingHandCursor);
                button->setExtraComponent (closeTabButton, TabBarButton::afterText);
            }
        }
        
    }
}


void CtrlrDocumentPanel::buttonClicked (Button *button)
{
    int index = (int)button->getProperties().getWithDefault("index", -1);
    TabbedComponent *tc = getCurrentTabbedComponent();

    if (tc != nullptr)
    {
        CtrlrPanelEditor *ed = dynamic_cast<CtrlrPanelEditor*> (tc->getTabContentComponent(index));

        if (ed != nullptr)
        {
            if (AlertWindow::showYesNoCancelBox (AlertWindow::QuestionIcon, "Close panel", "Are you sure you want to close this panel [" + ed->getName() + "]", "Yes", "No", "Cancel", this) == 1)
            {
                owner.removePanel(ed);
            }
        }
    }
}

void CtrlrDocumentPanel::setEditor (CtrlrEditor *_editorToSet)
{
	ctrlrEditor = _editorToSet;
}

/** lookAndFeelChanged() override works great for a single panel instance but not when having multiple panels loaded in CtrlrX.
    Problem with LookAndFeelChanged() is that the whole APP lookandfeel is following the last global LnF selected.
    It won't allow multi panel to have their own colours pushed to the top level APP design elements while switching panels. */

//void CtrlrDocumentPanel::lookAndFeelChanged()
//{
//    setBackgroundColour(Component::findColour(DocumentWindow::backgroundColourId));
//    //setBackgroundColour((Colours::lightgrey).darker(0.2)); // Sets background colour behind main window by default on grey to please everyone :)
//
//    if (getCurrentTabbedComponent()) {
//        getCurrentTabbedComponent()->setTabBarDepth(owner.getProperty(Ids::ctrlrTabBarDepth));
//        getCurrentTabbedComponent()->getTabbedButtonBar().setTabBackgroundColour(getCurrentTabbedComponent()->getTabbedButtonBar().getCurrentTabIndex(), findColour(TextButton::buttonColourId)); // Tab colour background
//        getCurrentTabbedComponent()->getTabbedButtonBar().setColour(TabbedButtonBar::tabTextColourId, findColour(TextButton::textColourOffId)); // Tab text colour
//        getCurrentTabbedComponent()->getTabbedButtonBar().setColour(TabbedButtonBar::tabOutlineColourId, findColour(TabbedButtonBar::tabOutlineColourId));  // Tab outline colour
//        getCurrentTabbedComponent()->getTabbedButtonBar().setColour(TabbedButtonBar::frontTextColourId, findColour(TextButton::textColourOffId));  // Selected tab text colour
//        getCurrentTabbedComponent()->getTabbedButtonBar().setColour(TabbedButtonBar::frontOutlineColourId, findColour(TabbedButtonBar::frontOutlineColourId));  // Selected tab outline colour
//    }
//}


/** Added v5.6.30. Brings back the panel tab close button */
void CtrlrDocumentPanelCloseButton::resized()
{
    /* static position */
    internalPath1.clear();
    internalPath1.startNewSubPath ((float) (proportionOfWidth (0.7000f)), (float) (proportionOfHeight (0.2500f)));
    internalPath1.lineTo ((float) (proportionOfWidth (0.7500f)), (float) (proportionOfHeight (0.3000f)));
    internalPath1.lineTo ((float) (proportionOfWidth (0.3000f)), (float) (proportionOfHeight (0.7500f)));
    internalPath1.lineTo ((float) (proportionOfWidth (0.2500f)), (float) (proportionOfHeight (0.7000f)));
    internalPath1.closeSubPath();

    internalPath2.clear();
    internalPath2.startNewSubPath ((float) (proportionOfWidth (0.3000f)), (float) (proportionOfHeight (0.2500f)));
    internalPath2.lineTo ((float) (proportionOfWidth (0.7500f)), (float) (proportionOfHeight (0.7000f)));
    internalPath2.lineTo ((float) (proportionOfWidth (0.7000f)), (float) (proportionOfHeight (0.7500f)));
    internalPath2.lineTo ((float) (proportionOfWidth (0.2500f)), (float) (proportionOfHeight (0.3000f)));
    internalPath2.closeSubPath();

    /* Mouse over Position */
    internalPath3.clear();
    internalPath3.startNewSubPath ((float) (proportionOfWidth (0.7000f)), (float) (proportionOfHeight (0.2500f)));
    internalPath3.lineTo ((float) (proportionOfWidth (0.7500f)), (float) (proportionOfHeight (0.3000f)));
    internalPath3.lineTo ((float) (proportionOfWidth (0.3000f)), (float) (proportionOfHeight (0.7500f)));
    internalPath3.lineTo ((float) (proportionOfWidth (0.2500f)), (float) (proportionOfHeight (0.7000f)));
    internalPath3.closeSubPath();

    internalPath4.clear();
    internalPath4.startNewSubPath ((float) (proportionOfWidth (0.3000f)), (float) (proportionOfHeight (0.2500f)));
    internalPath4.lineTo ((float) (proportionOfWidth (0.7500f)), (float) (proportionOfHeight (0.7000f)));
    internalPath4.lineTo ((float) (proportionOfWidth (0.7000f)), (float) (proportionOfHeight (0.7500f)));
    internalPath4.lineTo ((float) (proportionOfWidth (0.2500f)), (float) (proportionOfHeight (0.3000f)));
    internalPath4.closeSubPath();
    
    /* Mouse down Position */
    internalPath5.clear();
    internalPath5.startNewSubPath ((float) (proportionOfWidth (0.7000f)), (float) (proportionOfHeight (0.2500f)));
    internalPath5.lineTo ((float) (proportionOfWidth (0.7500f)), (float) (proportionOfHeight (0.3000f)));
    internalPath5.lineTo ((float) (proportionOfWidth (0.3000f)), (float) (proportionOfHeight (0.7500f)));
    internalPath5.lineTo ((float) (proportionOfWidth (0.2500f)), (float) (proportionOfHeight (0.7000f)));
    internalPath5.closeSubPath();

    internalPath6.clear();
    internalPath6.startNewSubPath ((float) (proportionOfWidth (0.3000f)), (float) (proportionOfHeight (0.2500f)));
    internalPath6.lineTo ((float) (proportionOfWidth (0.7500f)), (float) (proportionOfHeight (0.7000f)));
    internalPath6.lineTo ((float) (proportionOfWidth (0.7000f)), (float) (proportionOfHeight (0.7500f)));
    internalPath6.lineTo ((float) (proportionOfWidth (0.2500f)), (float) (proportionOfHeight (0.3000f)));
    internalPath6.closeSubPath();
}

void CtrlrDocumentPanelCloseButton::paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown)
{
    if (isButtonDown)
    {
        //g.setColour (Colour(findColour(TextButton::buttonOnColourId)).brighter(0.6));
        g.setColour (Colour (0xdfe7e7e8));
        
        g.fillRoundedRectangle((float) (proportionOfWidth (0.0500f)),
                               (float) (proportionOfHeight (0.0500f)),
                               (float) (proportionOfWidth (0.9000f)),
                               (float) (proportionOfHeight (0.9000f)),
                               (float) (proportionOfWidth(0.1f)));

        g.setColour (Colour (0xdf3f3e45));
        g.fillPath (internalPath5);

        g.setColour (Colour (0xdf3f3e45));
        g.fillPath (internalPath6);
    }
    
    else if (isMouseOverButton)
    {
        //g.setColour (Colour(findColour(TextButton::buttonOnColourId)).brighter(0.4)); // v.5.2.198
        //g.setColour (Colour (0xdfe7e7e8)); // Added v5.6.30
        //g.setColour (Colours::red); // Added v5.6.31 by GoodWeather
        g.setColour (findColour(TextButton::buttonOnColourId));; // Added v5.6.31
        
        g.fillRoundedRectangle((float) (proportionOfWidth (0.0500f)),
                               (float) (proportionOfHeight (0.0500f)),
                               (float) (proportionOfWidth (0.9000f)),
                               (float) (proportionOfHeight (0.9000f)),
                               (float) (proportionOfWidth(0.1f)));

        //g.setColour (Colour (0xdf3f3e45)); // Added v5.6.30
        g.setColour (Colours::white); // Added v5.6.31
        g.fillPath (internalPath3);

        //g.setColour (Colour (0xdf3f3e45)); // Added v5.6.30
        g.setColour (Colours::white); // Added v5.6.31
        g.fillPath (internalPath4);
    }
    
    else
    {
        g.setColour (Colour (0xdf2e2d32));
        g.fillPath (internalPath1);

        g.setColour (Colour (0xdf2e2d32));
        g.fillPath (internalPath2);
    }
}
