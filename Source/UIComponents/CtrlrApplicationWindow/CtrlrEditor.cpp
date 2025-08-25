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

CtrlrEditor::CtrlrEditor(CtrlrProcessor* _ownerFilter, CtrlrManager& _owner)
    : AudioProcessorEditor(_ownerFilter),
    ownerFilter(_ownerFilter),
    owner(_owner),
    resizer(this, 0),
    tempResult(Result::ok()),
    menuHandlerCalled(false),
    currentLookAndFeel(nullptr),
    lastCommandInvocationMillis(0),
    previousGlobalLookAndFeel(nullptr)
{
    // Initialize currentLookAndFeel to a default LookAndFeel_V4 Light.
    // This provides a starting point before properties are loaded.
    previousGlobalLookAndFeel = &LookAndFeel::getDefaultLookAndFeel();
    currentLookAndFeel = new LookAndFeel_V4(juce::LookAndFeel_V4::getLightColourScheme());
    setLookAndFeel(currentLookAndFeel); // Set the editor's LookAndFeel initially

    Rectangle<int> editorRect;
    // http://www.juce.com/forum/topic/applicationcommandmanager-menus-not-active-annoyance#new
    owner.getCommandManager().setFirstCommandTarget(this);

    menuBar = new MenuBarComponent(this);
    addAndMakeVisible(menuBar);

    setApplicationCommandManagerToWatch(&owner.getCommandManager());

    owner.getCommandManager().registerAllCommandsForTarget(this);
    owner.getCommandManager().registerAllCommandsForTarget(JUCEApplication::getInstance());
    ScopedPointer <XmlElement> xml(XmlDocument::parse(owner.getProperty(Ids::ctrlrKeyboardMapping)).release());

    if (xml)
    {
        owner.getCommandManager().getKeyMappings()->restoreFromXml(*xml);
    }

    owner.setEditor(this);

    addAndMakeVisible(&owner.getCtrlrDocumentPanel());

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

    if (owner.getProperty(Ids::ctrlrEditorBounds).toString() != "") // ctrlrEditorBounds is Editor size. AAX Plugin crashes here, passes without it
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
                editorRect.setHeight(editorRect.getHeight() + (int)owner.getProperty(Ids::ctrlrMenuBarHeight, 24));
            }
            else
            {
                editorRect.setWidth(editorRect.getWidth());
                editorRect.setHeight(editorRect.getHeight());
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
                                setResizeLimits(vpMinWidth, round(vpMinWidth / vpFixedAspectRatio), vpMaxWidth, round(vpMaxWidth / vpFixedAspectRatio));
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
        setBounds(editorRect);
    }
    else
    {
        if (JUCEApplication::isStandaloneApp())
            centreWithSize(800, 600);
        else
            setSize(800, 600);
    }

    // --- LOOK AND FEEL AND COLOUR SCHEME LOGIC ---
    String lookAndFeelVersionToApply;
    var colourSchemePropertyToApply; // Will be passed to setEditorLookAndFeel

    // 1. Check for global legacy mode first
    bool isLegacyModeGlobal = owner.getProperty(Ids::ctrlrLegacyMode); // This is a Bool property

    if (isLegacyModeGlobal)
    {
        lookAndFeelVersionToApply = "V3"; // Force V3 if legacy mode is on
        colourSchemePropertyToApply = var(); // No colour scheme for V3
    }
    else
    {
        // 2. If not in legacy mode, check global LookAndFeel version
        lookAndFeelVersionToApply = owner.getProperty(Ids::ctrlrLookAndFeel).toString();
        colourSchemePropertyToApply = owner.getProperty(Ids::ctrlrColourScheme);

        // 3. If no global L&F version or colour scheme, check panel properties
        if (lookAndFeelVersionToApply.isEmpty() && owner.getActivePanel())
        {
            lookAndFeelVersionToApply = owner.getActivePanel()->getEditor()->getProperty(Ids::uiPanelLookAndFeel).toString();
        }
        if (colourSchemePropertyToApply.isUndefined() && owner.getActivePanel())
        {
            colourSchemePropertyToApply = owner.getActivePanel()->getEditor()->getProperty(Ids::ctrlrColourScheme);
        }
    }

    // Apply the determined LookAndFeel and ColourScheme
    setEditorLookAndFeel(lookAndFeelVersionToApply, colourSchemePropertyToApply);

    menuBar->setLookAndFeel(currentLookAndFeel); // Ensure menuBar uses the current L&F

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
    // 1. First detach LookAndFeel from all components
    setLookAndFeel(nullptr);
    if (menuBar)
    {
        menuBar->setLookAndFeel(nullptr);
    }

    // 2. Restore the original global LookAndFeel
    if (previousGlobalLookAndFeel != nullptr)
    {
        LookAndFeel::setDefaultLookAndFeel(previousGlobalLookAndFeel);
    }

    // 3. Now it's safe to delete the menuBar and our custom LookAndFeel
    deleteAndZero(menuBar);
    currentLookAndFeel = nullptr; // ScopedPointer will delete automatically

    masterReference.clear();
}

void CtrlrEditor::paint(Graphics& g)
{
    g.fillAll(Component::findColour(DocumentWindow::backgroundColourId));
}

void CtrlrEditor::resized()
{
    if (menuBar->isVisible())
    {
        menuBar->setBounds(0, 0, getWidth(), (int)owner.getProperty(Ids::ctrlrMenuBarHeight, 24));
        owner.getCtrlrDocumentPanel().setBounds(0, (int)owner.getProperty(Ids::ctrlrMenuBarHeight, 24), getWidth(), getHeight() - ((int)owner.getProperty(Ids::ctrlrMenuBarHeight, 24)));
    }
    else
    {
        owner.getCtrlrDocumentPanel().setBounds(0, 0, getWidth(), getHeight());
    }
    resizer.setBounds(getWidth() - 24, getHeight() - 24, 24, 24);
    owner.setProperty(Ids::ctrlrEditorBounds, getBounds().toString());
}

// Added v5.6.34. New method to set the main LookAndFeel for the editor and its children
void CtrlrEditor::setEditorLookAndFeel(const String& lookAndFeelDesc, const var& colourSchemeProperty)
{
    // 1. Create the new LookAndFeel object based on the description (V1, V2, V3, V4 default)
    // Pass an empty var for colourSchemeProperty to gui::createLookAndFeelFromDescription,
    // as it will only return the base L&F instance type.
    ScopedPointer<LookAndFeel> newLookAndFeel = gui::createLookAndFeelFromDescription(lookAndFeelDesc, juce::var());

    // If a valid LookAndFeel was created, update the current one
    if (newLookAndFeel != nullptr)
    {
        // 2. If it's a LookAndFeel_V4, apply the specific ColourScheme from the property.
        if (LookAndFeel_V4* lnf4 = dynamic_cast<LookAndFeel_V4*>(newLookAndFeel.get()))
        {
            // Only apply a colour scheme if the property is a valid string
            if (colourSchemeProperty.isString() && !colourSchemeProperty.toString().isEmpty())
            {
                lnf4->setColourScheme(gui::colourSchemeFromProperty(colourSchemeProperty));
            }
        }

        // Explicitly set the L&F of the editor and menubar to nullptr first
        // This ensures no components are using the old L&F before we destroy it.
        setLookAndFeel(nullptr);
        menuBar->setLookAndFeel(nullptr);

        currentLookAndFeel = newLookAndFeel; // Transfers ownership from ScopedPointer
        setLookAndFeel(currentLookAndFeel); // Set the editor's LookAndFeel, which propagates to children

        // --- THESE ARE THE CRUCIAL LINES FOR THE MENUBAR UPDATE ---
        menuBar->setLookAndFeel(currentLookAndFeel); // Ensure menuBar also gets the new L&F
        menuBar->lookAndFeelChanged();               // Explicitly notify the menu bar that its L&F has changed
        menuBar->repaint();                          // Force a repaint of the menu bar's background

        // Force a repaint and update of all components to reflect the new LookAndFeel
        lookAndFeelChanged(); // This calls lookAndFeelChanged() on this component and its children (like the main panel)
        repaint();            // Force a repaint of the CtrlrEditor itself

        // This makes sure any subsequently created JUCE components (like dialogs, other windows)
        // will default to *your* currentLookAndFeel instance.
        //LookAndFeel::setDefaultLookAndFeel(currentLookAndFeel); 
    }
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

        // Re-determine LookAndFeel and ColourScheme based on active panel
        String lookAndFeelVersionToApply;
        var colourSchemePropertyToApply;

        // Check panel's legacy mode first (from uiPanelLegacyMode property on the panel)
        bool isLegacyModePanel = owner.getActivePanel()->getEditor()->getProperty(Ids::uiPanelLegacyMode);

        if (isLegacyModePanel)
        {
            lookAndFeelVersionToApply = "V3"; // Force V3 if panel legacy mode is on
            colourSchemePropertyToApply = var(); // No colour scheme for V3
        }
        else
        {
            // If not in panel's legacy mode, get L&F and color scheme from panel properties
            lookAndFeelVersionToApply = owner.getActivePanel()->getEditor()->getProperty(Ids::uiPanelLookAndFeel).toString();
            colourSchemePropertyToApply = owner.getActivePanel()->getEditor()->getProperty(Ids::ctrlrColourScheme);
        }

        // Apply the determined LookAndFeel and ColourScheme
        // This single call handles setting the L&F for the editor and explicitly for the menuBar
        setEditorLookAndFeel(lookAndFeelVersionToApply, colourSchemePropertyToApply);

        //String currentLookAndFeel = owner.getActivePanel()->getEditor()->getProperty(Ids::uiPanelLookAndFeel);
        //setMenuBarLookAndFeel(currentLookAndFeel); // Updates the current component LookAndFeel : PanelEditor

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
    else // logic for no active panel
    {
        // No active panel, set L&F to "V4 Light"
        setEditorLookAndFeel("V4 Light", juce::var());

        // Also ensure menu bar visibility is handled when no panel is active.
        // You might want the menu bar to always be visible, or hide if no panel.
        // For fallback, let's assume it should be visible with the default L&F.
        if (!menuBar->isVisible()) // If it was hidden by a panel before
        {
            setMenuBarVisible(true);
        }
    }
}

MenuBarComponent* CtrlrEditor::getMenuBar()
{
    return (menuBar);
}

CtrlrPanel* CtrlrEditor::getActivePanel()
{
    return (owner.getActivePanel());
}

bool CtrlrEditor::isRestricted()
{
    return (owner.getInstanceMode() == InstanceSingleRestriced);
}

CtrlrPanelEditor* CtrlrEditor::getActivePanelEditor()
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
    menuBar->setVisible(shouldBeVisible);
    resized();
}