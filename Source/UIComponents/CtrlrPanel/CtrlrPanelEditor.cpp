#include "stdafx.h"
#include "CtrlrLuaManager.h"
#include "CtrlrManager/CtrlrManager.h"
#include "CtrlrMacros.h"
#include "CtrlrUtilities.h"
#include "CtrlrLog.h"
#include "CtrlrPanelEditor.h"
#include "CtrlrProcessor.h"
#include "CtrlrComponents/CtrlrComponentTypeManager.h"
#include "CtrlrInlineUtilitiesGUI.h"
#include "CtrlrModulator/CtrlrModulator.h"
#include "CtrlrPanel/CtrlrPanel.h"
#include "CtrlrComponents/CtrlrCombo.h"
#include "CtrlrPanel/CtrlrPanelResource.h"
#include "CtrlrPanel/CtrlrPanelCanvas.h"
#include "CtrlrPanel/CtrlrPanelViewport.h"
#include "JuceClasses/LMemoryBlock.h"
#include "CtrlrMIDI/CtrlrMIDISettingsDialog.h"
#include "CtrlrComponents/CtrlrComponent.h"


CtrlrPanelNotifier::CtrlrPanelNotifier(CtrlrPanelEditor &_owner) // Added back v5.6.31 for file management bottom notification bar
    : owner(_owner), background(Colours::lightgrey)
{
    addAndMakeVisible (text = new Label());
    text->addMouseListener (this, true);
    text->setColour (Label::backgroundColourId, Colours::transparentBlack);
    text->setColour (Label::textColourId, Colours::white.withAlpha (0.85f));
    text->setColour (Label::outlineColourId, Colours::transparentBlack);
    text->setFont (Font (12.0f, Font::bold));
    text->setText ("", dontSendNotification); // Default text required
}

void CtrlrPanelNotifier::paint (Graphics &g) // Added back v5.6.31 for file management bottom notification bar
{
    gui::drawSelectionRectangle (g, getWidth(), getHeight(), background); // Updated v5.6.31 (link to GUI class)
}

void CtrlrPanelNotifier::resized() // Added back v5.6.31 for file management bottom notification bar
{
    text->setBounds (0, 0, getWidth(), getHeight());
}

void CtrlrPanelNotifier::setNotification (const String &notification, const CtrlrNotificationType ctrlrNotificationType) // Added back v5.6.31 for file management bottom notification bar
{
    background = getBackgroundColourForNotification(ctrlrNotificationType);
    text->setText (notification, dontSendNotification);
    
}

void CtrlrPanelNotifier::mouseDown (const MouseEvent &e)
{
    owner.notificationClicked(e);
}

Colour CtrlrPanelNotifier::getBackgroundColourForNotification(const CtrlrNotificationType ctrlrNotificationType)  // Added back v5.6.31 for file management bottom notification bar
{
    switch (ctrlrNotificationType)
    {
        case NotifySuccess:
            return (Colours::green.brighter(0.2f));
        case NotifyFailure:
            return (Colours::red.brighter(0.2f));
        case NotifyWarning:
            return (Colours::yellow.darker(0.5f));
        case NotifyInformation:
            return (Colours::grey);
    }

    return (Colours::lightgrey);
}



CtrlrPanelEditor::CtrlrPanelEditor(CtrlrPanel &_owner, CtrlrManager &_ctrlrManager, const String &panelName)
        : Component(L"Ctrlr Panel Editor"),
          lastEditMode(true),
          ctrlrManager(_ctrlrManager),
          owner(_owner),
          panelEditorTree(Ids::uiPanelEditor),
          ctrlrComponentSelection(nullptr),
          ctrlrPanelProperties(nullptr),
          spacerComponent(nullptr)
{   
    ctrlrComponentSelection = new CtrlrComponentSelection(*this);
    //removeColour(TooltipWindow::textColourId);
    
    addAndMakeVisible(ctrlrPanelViewport = new CtrlrPanelViewport(*this));
    addAndMakeVisible(ctrlrPanelProperties = new CtrlrPanelProperties(*this));
    addAndMakeVisible(spacerComponent = new StretchableLayoutResizerBar(&layoutManager, 1, true));
    addAndMakeVisible (ctrlrPanelNotifier = new CtrlrPanelNotifier(*this));  // Added back v5.6.31 for file management bottom notification bar
    
    ctrlrPanelNotifier->setAlwaysOnTop (true);  // Added back v5.6.31 for file management bottom notification bar
    ctrlrPanelNotifier->setVisible (false);  // Added back v5.6.31 for file management bottom notification bar
    //componentAnimator.addChangeListener (this); // Added back v5.6.31 not working
    
    spacerComponent->setName(L"spacerComponent");
    
    getPanelEditorTree().addListener(this);
    
    layoutManager.setItemLayout(0, -0.001, -1.0, -0.7);
    layoutManager.setItemLayout(1, 8, 8, 8);
    layoutManager.setItemLayout(2, -0.001, -1.0, -0.3);
    
    editorComponentsInEditMode[0] = ctrlrPanelViewport;
    editorComponentsInEditMode[1] = spacerComponent;
    editorComponentsInEditMode[2] = ctrlrPanelProperties;
    
    editorComponents[0] = ctrlrPanelViewport;
    
    setProperty(Ids::name, panelName);
    setProperty(Ids::uiPanelEditMode, true);
    setProperty(Ids::uiPanelLock, false);
    setProperty(Ids::uiPanelDisabledOnEdit, false);
    setProperty(Ids::uiPanelMenuBarVisible, true);
    setProperty(Ids::uiPanelMenuBarHideOnExport, false);
    setProperty(Ids::uiPanelProgramsMenuHideOnExport, false);
    setProperty(Ids::uiPanelMidiControllerMenuHideOnExport, false);
    setProperty(Ids::uiPanelMidiThruMenuHideOnExport, false);
    setProperty(Ids::uiPanelMidiChannelMenuHideOnExport, false);
    
    setProperty(Ids::uiPanelViewPortSize, 800);
    setProperty(Ids::uiPanelPropertiesSize, 300);
    
    setProperty(Ids::uiViewPortResizable, true);
    setProperty(Ids::uiViewPortShowScrollBars, true);
    setProperty(Ids::uiViewPortWidth, 400);
    setProperty(Ids::uiViewPortHeight, 400);
    setProperty(Ids::uiViewPortEnableResizeLimits, false);
    setProperty(Ids::uiViewPortMinWidth, 0);
    setProperty(Ids::uiViewPortMinHeight, 0);
    setProperty(Ids::uiViewPortMaxWidth, 0);
    setProperty(Ids::uiViewPortMaxHeight, 0);
    setProperty(Ids::uiViewPortEnableFixedAspectRatio, false);
    setProperty(Ids::uiViewPortFixedAspectRatio, 1.5);
    setProperty(Ids::uiPanelZoom, 1.0);
    
    setProperty(Ids::uiPanelViewPortBackgroundColour, (String) Component::findColour (ResizableWindow::backgroundColourId).withAlpha(0.7f).toString());  // ViewPort background color. was "transparentblack"
    setProperty(Ids::uiPanelBackgroundColour, (String) Component::findColour (ResizableWindow::backgroundColourId).toString()); // Canvas Colour 0xffffffff
    setProperty(Ids::uiPanelBackgroundColour1, (String) Component::findColour (ResizableWindow::backgroundColourId).toString()); // Canvas Colour1 if gradient
    setProperty(Ids::uiPanelBackgroundColour2, (String) Component::findColour (ResizableWindow::backgroundColourId).darker(0.2f).toString()); // Canvas Colour2 if gradient
    setProperty(Ids::uiPanelBackgroundGradientType, 0); // Default set to SolidColor [No background gradient]
    setProperty(Ids::uiPanelImageResource, COMBO_ITEM_NONE);
    setProperty(Ids::uiPanelImageAlpha, 255);
    setProperty(Ids::uiPanelImageLayout, 64);
    setProperty(Ids::uiPanelSnapActive, true);
    
    setProperty(Ids::uiPanelSnapSize, 8);
    setProperty(Ids::uiPanelPropertiesOnRight, false);
    
    setProperty(Ids::luaPanelPaintBackground, COMBO_ITEM_NONE);
    setProperty(Ids::luaViewPortResized, COMBO_ITEM_NONE);
    setProperty(Ids::luaPanelResized, COMBO_ITEM_NONE);
    setProperty(Ids::luaPanelFileDragDropHandler, COMBO_ITEM_NONE);
    setProperty(Ids::luaPanelFileDragEnterHandler, COMBO_ITEM_NONE);
    setProperty(Ids::luaPanelFileDragExitHandler, COMBO_ITEM_NONE);
    
    setProperty(Ids::uiPanelInvisibleComponentAlpha, 0.5);
    
    setProperty(Ids::uiPanelTooltipPlacement, BubbleComponent::below);
    setProperty(Ids::uiPanelTooltipFont, Font(12.0f, Font::plain).toString());
    setProperty(Ids::uiPanelTooltipColour, (String) Component::findColour (Label::textColourId).toString()); // 0xff000000
    setProperty(Ids::uiPanelTooltipBackgroundColour, (String) Component::findColour (BubbleComponent::backgroundColourId).toString()); // 0xffeeeebb
    setProperty(Ids::uiPanelTooltipOutlineColour, (String) Component::findColour (BubbleComponent::outlineColourId).toString()); // 0xff000000
    setProperty(Ids::uiPanelTooltipCornerRound, 1.0);

    
    ValueTree ed = owner.getCtrlrManagerOwner().getManagerTree();
    
    if (ed.getProperty(Ids::ctrlrLegacyMode) == "1"
        || ed.getProperty(Ids::ctrlrLookAndFeel) == "V3"
        || ed.getProperty(Ids::ctrlrLookAndFeel) == "V2"
        || ed.getProperty(Ids::ctrlrLookAndFeel) == "V1")
    {
        setProperty(Ids::uiPanelLegacyMode, "1");
        setProperty(Ids::uiPanelLookAndFeel, "V3");
    }
    else
    {
        setProperty(Ids::uiPanelLegacyMode, false);
        setProperty(Ids::uiPanelLookAndFeel, "V4");
        
        // Requires passing the colourScheme to the property uiPanelLookAndFeel from ctrlrColourScheme
        // Updated v5.6.34. For a generic method schemeName Property--> schemeName. Get the current colour scheme name from the property
        juce::String schemeName = ed.getProperty (Ids::ctrlrColourScheme).toString();
        
        // <fallback for empty instances without any colourscheme yet defined
        if (schemeName.isEmpty())
        {
            schemeName = "Light";
        }
        
        // Determine the LookAndFeel description string
        juce::String lookAndFeelDesc;
        
        if (schemeName.startsWith ("V4 "))
        {
            // If it already has "V4 ", use it as is
            lookAndFeelDesc = schemeName;
        }
        else
        {
            // Otherwise, prepend "V4 " (e.g., "Light" becomes "V4 Light")
            lookAndFeelDesc = "V4 " + schemeName;
        }
        
        // Set the uiPanelLookAndFeel property with the determined string
        setProperty (Ids::uiPanelLookAndFeel, lookAndFeelDesc);
    }
    
    //setProperty(Ids::uiPanelLegacyMode, false);
    //setProperty(Ids::uiPanelLookAndFeel, "V4 Light");
    
//    /** displays the current LookAndFeel colourScheme UIColours */
//    LookAndFeel_V4::setColourScheme(getLightColourScheme());
//
//    setProperty(Ids::uiPanelUIColourWindowBackground, (String) LookAndFeel_V4::getCurrentColourScheme().getUIColour (ColourScheme::UIColour::windowBackground).toString());
//    setProperty(Ids::uiPanelUIColourWidgetBackground, (String) LookAndFeel_V4::getCurrentColourScheme().getUIColour (ColourScheme::UIColour::widgetBackground).toString());
//    setProperty(Ids::uiPanelUIColourMenuBackground, (String) LookAndFeel_V4::getCurrentColourScheme().getUIColour (ColourScheme::UIColour::menuBackground).toString());
//    setProperty(Ids::uiPanelUIColourOutline, (String) LookAndFeel_V4::getCurrentColourScheme().getUIColour (ColourScheme::UIColour::outline).toString());
//    setProperty(Ids::uiPanelUIColourDefaultText, (String) LookAndFeel_V4::getCurrentColourScheme().getUIColour (ColourScheme::UIColour::defaultText).toString());
//    setProperty(Ids::uiPanelUIColourDefaultFill, (String) LookAndFeel_V4::getCurrentColourScheme().getUIColour (ColourScheme::UIColour::defaultFill).toString());
//    setProperty(Ids::uiPanelUIColourHighlightedText, (String) LookAndFeel_V4::getCurrentColourScheme().getUIColour (ColourScheme::UIColour::highlightedText).toString());
//    setProperty(Ids::uiPanelUIColourHighlightedFill, (String) LookAndFeel_V4::getCurrentColourScheme().getUIColour (ColourScheme::UIColour::highlightedFill).toString());
//    setProperty(Ids::uiPanelUIColourMenuText, (String) LookAndFeel_V4::getCurrentColourScheme().getUIColour (ColourScheme::UIColour::menuText).toString());
    
    ctrlrComponentSelection->addChangeListener(ctrlrPanelProperties);
    
    setSize(600, 400);
    
    ctrlrComponentSelection->sendChangeMessage();
}

CtrlrPanelEditor::~CtrlrPanelEditor()
{
    //componentAnimator.removeChangeListener (this); // NOT WORKING Added back v5.6.31 for file management bottom notification bar
    // setLookAndFeel(nullptr); // Updated v5.6.30. reset LnF to global LnF from Preferences when closing panel
    getPanelEditorTree().removeListener(this);
    owner.getPanelTree().removeListener(this);
    owner.getPanelTree().removeChild(getPanelEditorTree(), 0);
    ctrlrComponentSelection->removeChangeListener(ctrlrPanelProperties);
    masterReference.clear();
    deleteAndZero(ctrlrPanelProperties);
    deleteAndZero(spacerComponent);
    deleteAndZero(ctrlrPanelViewport);
    //deleteAndZero(lookAndFeel);
}

void CtrlrPanelEditor::visibilityChanged()
{
}

void CtrlrPanelEditor::resized()
{
    ctrlrPanelViewport->setBounds(0, 0, getWidth() - 608, getHeight()); // Was 308
    ctrlrPanelProperties->setBounds(getWidth() - 600, 32, 600, getHeight() - 32);
    spacerComponent->setBounds(getWidth(), 32, 8, getHeight() - 32);

    setProperty(Ids::uiViewPortWidth, getWidth());
    setProperty(Ids::uiViewPortHeight, getHeight());

    if (ctrlrPanelNotifier) // Added back v5.6.31
    {
        ctrlrPanelNotifier->setBounds (0, getHeight() - 28, getWidth() - 32, 20);
    }
     
    layoutItems();

    if (!getRestoreState())
    {
        saveLayout();
    }
    if (resizedCbk && !resizedCbk.wasObjectDeleted())
    {
        if (resizedCbk->isValid())
        {
            owner.getCtrlrLuaManager().getMethodManager().call (resizedCbk, &owner);
        }
    }
}

CtrlrComponentSelection *CtrlrPanelEditor::getSelection()
{
    return (ctrlrComponentSelection);
}

void CtrlrPanelEditor::layoutItems()
{
    if (getProperty(Ids::uiPanelEditMode))
    {
        if (getProperty(Ids::uiPanelPropertiesOnRight))
        {
            Component *comps[] = {ctrlrPanelProperties, spacerComponent, ctrlrPanelViewport};
            layoutManager.layOutComponents(comps, 3, 0, 0, getWidth(), getHeight(), false, true);
        }
        else
        {
            Component *comps[] = {ctrlrPanelViewport, spacerComponent, ctrlrPanelProperties};
            layoutManager.layOutComponents(comps, 3, 0, 0, getWidth(), getHeight(), false, true);
        }
    }
    else
    {
        layoutManager.layOutComponents(editorComponents, 1, 0, 0, getWidth(), getHeight(), false, true);
    }
}

void CtrlrPanelEditor::saveLayout()
{
    setProperty(Ids::uiPanelViewPortSize, layoutManager.getItemCurrentAbsoluteSize(0));
    setProperty(Ids::uiPanelPropertiesSize, layoutManager.getItemCurrentAbsoluteSize(2));
}

CtrlrPanelCanvas *CtrlrPanelEditor::getCanvas()
{
    if (ctrlrPanelViewport != 0)
    {
        return (ctrlrPanelViewport->getCanvas());
    }

    return (0);
}

void CtrlrPanelEditor::editModeChanged()
{
    const bool editMode = getProperty(Ids::uiPanelEditMode);
    owner.editModeChanged(editMode);

    if (editMode)
    {
        layoutManager.setItemLayout(0, -0.001, -1.0, getProperty(Ids::uiPanelViewPortSize, -0.7));
        layoutManager.setItemLayout(2, -0.001, -1.0, getProperty(Ids::uiPanelPropertiesSize, -0.3));
        spacerComponent->setVisible(true);
        ctrlrPanelProperties->setVisible(true);
        getCanvas()->getResizableBorder()->setVisible(true);

        if ((bool) getProperty(Ids::uiPanelDisableCombosOnEdit))
            setAllCombosDisabled();
    }
    else
    {
        if (getSelection())
            getSelection()->deselectAll();
        spacerComponent->setVisible(false);
        ctrlrPanelProperties->setVisible(false);
        getCanvas()->getResizableBorder()->setVisible(false);

        if ((bool) getProperty(Ids::uiPanelDisableCombosOnEdit))
            setAllCombosEnabled();
    }

    resized();
}

void CtrlrPanelEditor::setAllCombosDisabled()
{
    OwnedArray<CtrlrModulator, CriticalSection> &mods = owner.getModulators();
    for (int i = 0; i < mods.size(); i++)
    {
        CtrlrCombo *cc = dynamic_cast<CtrlrCombo *>(mods[i]->getComponent());
        if (cc != nullptr)
        {
            cc->setEnabled(false);
        }
    }
}

void CtrlrPanelEditor::setAllCombosEnabled()
{
    OwnedArray<CtrlrModulator, CriticalSection> &mods = owner.getModulators();
    for (int i = 0; i < mods.size(); i++)
    {
        CtrlrCombo *cc = dynamic_cast<CtrlrCombo *>(mods[i]->getComponent());
        if (cc != nullptr)
        {
            cc->setEnabled(true);
        }
    }
}

void CtrlrPanelEditor::restoreState(const ValueTree &savedState)
{
    setVisible(false);

    setRestoreState(true);

    restoreProperties(savedState.getChildWithName(Ids::uiPanelEditor), panelEditorTree, 0);
    
    bool IsNotLegacyMode = savedState.getChildWithName(Ids::uiPanelEditor).hasProperty(Ids::uiPanelLegacyMode);  // Legacy mode check for version before 5.6.29
    if (!IsNotLegacyMode)
    {
        setProperty(Ids::uiPanelLegacyMode, true);
        setProperty(Ids::uiPanelLookAndFeel, "V3");
    }

    getCanvas()->restoreState(savedState);

    if (getSelection())
    {
        getSelection()->sendChangeMessage();
    }

    if (owner.getCtrlrManagerOwner().getInstanceMode() == InstanceSingle
        || owner.getCtrlrManagerOwner().getInstanceMode() == InstanceSingleRestriced)
    {
        initSingleInstance();
    }

    editModeChanged();
    setRestoreState(false);

    setVisible(true);
}

CtrlrComponent *CtrlrPanelEditor::getSelected(const Identifier &type)
{
    if (getSelection() == nullptr)
        return (nullptr);

    if (getSelection()->getNumSelected() == 1)
    {
        if (CtrlrComponentTypeManager::findType(getSelection()->getSelectedItem(0)) == type)
        {
            return (getSelection()->getSelectedItem(0));
        }
    }

    return (0);
}

void CtrlrPanelEditor::valueTreePropertyChanged(ValueTree &treeWhosePropertyHasChanged, const Identifier &property)
{
    if (treeWhosePropertyHasChanged.hasType(Ids::uiPanelEditor))
    {
        if (property == Ids::uiPanelEditMode)
        {
            editModeChanged();
        }
        else if (property == Ids::luaViewPortResized)
        {
            if (getProperty(property) == "")
                return;

            resizedCbk = owner.getCtrlrLuaManager().getMethodManager().getMethod(getProperty(property));
        }
        else if (property == Ids::uiPanelSnapSize)
        {
            repaint();
        }
        else if (property == Ids::name)
        {
            // Use getPanelWindowTitle() to get the "*" when the panel is dirty
            Component::setName(owner.getPanelWindowTitle());
        }
        else if (property == Ids::uiPanelPropertiesOnRight)
        {
            ctrlrPanelProperties->layoutChanged();

            if (!owner.getCtrlrManagerOwner().isRestoring())
            {
                resized();
            }
        }
        else if (property == Ids::uiPanelCanvasRectangle)
        {
            getCanvas()->setBounds(VAR2RECT(getProperty(property))); // update canvas size if values in the property field are changed
            canvasHeight = getCanvas()->getHeight(); // Updated v5.6.31 by GoodWeather. Removed type double(canvasHeight)
            canvasWidth = getCanvas()->getWidth(); // Updated v5.6.31 by GoodWeather. Removed type double(canvasWidth)
            canvasAspectRatio = canvasWidth / canvasHeight; // Updated v5.6.31 by GoodWeather. Removed type double(canvasAspectRatio) = double(canvasWidth) / double(canvasHeight)
            setProperty(Ids::uiViewPortFixedAspectRatio, canvasAspectRatio); // update canvas aspect ratio if canvas is resized
            resized();
        }
        else if (property == Ids::uiViewPortResizable
                 || property == Ids::uiViewPortShowScrollBars
                 || property == Ids::uiViewPortEnableFixedAspectRatio
                 || property == Ids::uiViewPortFixedAspectRatio
                 || property == Ids::uiViewPortEnableResizeLimits
                 || property == Ids::uiViewPortMinWidth
                 || property == Ids::uiViewPortMinHeight
                 || property == Ids::uiViewPortMaxWidth
                 || property == Ids::uiViewPortMaxHeight
                 || property == Ids::uiViewPortShowScrollBars
                 )
        {
            resized();
        }
        else if (property == Ids::uiViewPortWidth
                 || property == Ids::uiViewPortHeight
                 )
        {
            resized();
        }
        else if (property == Ids::uiPanelDisableCombosOnEdit)
        {
            if ((bool) getProperty(property) && getMode())
            {
                setAllCombosDisabled();
            }
            else
            {
                setAllCombosEnabled();
            }
        }
        else if (property == Ids::uiPanelZoom)
        {
            getPanelViewport()->setZoom(getProperty(property),
                                        getCanvas()->getBounds().getCentre().getX(),
                                        getCanvas()->getBounds().getCentre().getY());
        }
        else if (property == Ids::uiPanelMenuBarVisible)
        {
            if (owner.getCtrlrManagerOwner().getEditor())
            {
                owner.getCtrlrManagerOwner().getEditor()->activeCtrlrChanged();
            }
        }
        else if (property == Ids::uiPanelLookAndFeel)
        {
            // 1. Create a single new LookAndFeel object and give ownership to a unique_ptr.
            //    This is the only place we call the function that allocates a new object.
            auto newLookAndFeel = std::unique_ptr<juce::LookAndFeel>(getLookAndFeelFromDescription(getProperty(property)));

            // 2. Safely check if the pointer is valid. If not, do nothing.
            if (newLookAndFeel.get() == nullptr)
                return;

            // 3. Now, set all the necessary LookAndFeel pointers using this ONE new object.
            //    The .get() method returns the raw pointer without transferring ownership.
            getCanvas()->setLookAndFeel(newLookAndFeel.get());
            setLookAndFeel(newLookAndFeel.get());
            LookAndFeel::setDefaultLookAndFeel(newLookAndFeel.get());

            // 4. Finally, assign the new unique_ptr to the class member.
            //    This safely manages the lifetime of the new object.
            lookAndFeel = std::move(newLookAndFeel);

            if (!getProperty(Ids::uiPanelLegacyMode))
            {
                // ... (your existing color update code)
            }

//            lookAndFeelChanged();

            if (!getProperty(Ids::uiPanelLegacyMode))
            {
                // Update colors based on the new valid LookAndFeel.
                setProperty(Ids::uiPanelViewPortBackgroundColour, (String)Component::findColour(ResizableWindow::backgroundColourId).withAlpha(0.7f).toString());
                setProperty(Ids::uiPanelBackgroundColour, (String)Component::findColour(ResizableWindow::backgroundColourId).toString());
                setProperty(Ids::uiPanelBackgroundColour1, (String)Component::findColour(ResizableWindow::backgroundColourId).toString());
                setProperty(Ids::uiPanelBackgroundColour2, (String)Component::findColour(ResizableWindow::backgroundColourId).darker(0.2f).toString());
                setProperty(Ids::uiPanelTooltipBackgroundColour, (String)Component::findColour(BubbleComponent::backgroundColourId).toString());
                setProperty(Ids::uiPanelTooltipOutlineColour, (String)Component::findColour(BubbleComponent::outlineColourId).toString());
                setProperty(Ids::uiPanelTooltipColour, (String)Component::findColour(Label::textColourId).toString());
            }
            
//            /** Stores the updated LnF ColourScheme **/
//            auto* currentLookAndFeel = dynamic_cast<LookAndFeel_V4*>(&getLookAndFeel());
//            auto currentLookAndFeelScheme = currentLookAndFeel->getCurrentColourScheme();
//
//            /** Updates Default UIColours  property fields for the new ColourScheme **/
//            setProperty(Ids::uiPanelUIColourWindowBackground, (String) currentLookAndFeelScheme.getUIColour (ColourScheme::UIColour::windowBackground).toString());
//            setProperty(Ids::uiPanelUIColourWidgetBackground, (String) currentLookAndFeelScheme.getUIColour (ColourScheme::UIColour::widgetBackground).toString());
//            setProperty(Ids::uiPanelUIColourMenuBackground, (String) currentLookAndFeelScheme.getUIColour (ColourScheme::UIColour::menuBackground).toString());
//            setProperty(Ids::uiPanelUIColourOutline, (String) currentLookAndFeelScheme.getUIColour (ColourScheme::UIColour::outline).toString());
//            setProperty(Ids::uiPanelUIColourDefaultText, (String) currentLookAndFeelScheme.getUIColour (ColourScheme::UIColour::defaultText).toString());
//            setProperty(Ids::uiPanelUIColourDefaultFill, (String) currentLookAndFeelScheme.getUIColour (ColourScheme::UIColour::defaultFill).toString());
//            setProperty(Ids::uiPanelUIColourHighlightedText, (String) currentLookAndFeelScheme.getUIColour (ColourScheme::UIColour::highlightedText).toString());
//            setProperty(Ids::uiPanelUIColourHighlightedFill, (String) currentLookAndFeelScheme.getUIColour (ColourScheme::UIColour::highlightedFill).toString());
//            setProperty(Ids::uiPanelUIColourMenuText, (String) (String) currentLookAndFeelScheme.getUIColour (ColourScheme::UIColour::menuText).toString());
            
            /** Updates LookAndFeel of the menuBar **/
            if (owner.getCtrlrManagerOwner().getEditor())
            {
                owner.getCtrlrManagerOwner().getEditor()->activeCtrlrChanged();
            }

            /** Updates  the Property List to get textboxes LnF right  **/
            ctrlrPanelProperties->refreshAll();
            if (getSelection())
            {
                getSelection()->sendChangeMessage();  // Brings back the screen position to where it left
            }
        }
        else if (property == Ids::uiPanelBackgroundGradientType
                 || property == Ids::uiPanelViewPortBackgroundColour
                 || property == Ids::uiPanelBackgroundColour
                 || property == Ids::uiPanelBackgroundColour1
                 || property == Ids::uiPanelBackgroundColour2
                )
        {
            resized();
        }
        else if (property == Ids::uiPanelUIColourWindowBackground
                 || property == Ids::uiPanelUIColourWidgetBackground
                 || property == Ids::uiPanelUIColourMenuBackground
                 || property == Ids::uiPanelUIColourOutline
                 || property == Ids::uiPanelUIColourDefaultText
                 || property == Ids::uiPanelUIColourDefaultFill
                 || property == Ids::uiPanelUIColourHighlightedText
                 || property == Ids::uiPanelUIColourHighlightedFill
                 || property == Ids::uiPanelUIColourMenuText
                 )
        {

//            /** Not working yet, need to be fixed */
//            auto* customLookAndFeel = dynamic_cast<LookAndFeel_V4*>(&getLookAndFeel());
//            auto customLookAndFeelScheme = customLookAndFeel->getCurrentColourScheme();
//
//            getCurrentColourScheme().setUIColour(ColourScheme::UIColour::windowBackground, VAR2COLOUR (owner.getProperty(Ids::uiPanelUIColourWindowBackground)));
//            getCurrentColourScheme().setUIColour(ColourScheme::UIColour::widgetBackground, VAR2COLOUR (owner.getProperty(Ids::uiPanelUIColourWidgetBackground)));
//            getCurrentColourScheme().setUIColour(ColourScheme::UIColour::menuBackground, VAR2COLOUR (owner.getProperty(Ids::uiPanelUIColourMenuBackground)));
//            getCurrentColourScheme().setUIColour(ColourScheme::UIColour::outline, VAR2COLOUR (owner.getProperty(Ids::uiPanelUIColourOutline)));
//            getCurrentColourScheme().setUIColour(ColourScheme::UIColour::defaultText, VAR2COLOUR (owner.getProperty(Ids::uiPanelUIColourDefaultText)));
//            getCurrentColourScheme().setUIColour(ColourScheme::UIColour::defaultFill, VAR2COLOUR (owner.getProperty(Ids::uiPanelUIColourDefaultFill)));
//            getCurrentColourScheme().setUIColour(ColourScheme::UIColour::highlightedText, VAR2COLOUR (owner.getProperty(Ids::uiPanelUIColourHighlightedText)));
//            getCurrentColourScheme().setUIColour(ColourScheme::UIColour::highlightedFill, VAR2COLOUR (owner.getProperty(Ids::uiPanelUIColourHighlightedFill)));
//            getCurrentColourScheme().setUIColour(ColourScheme::UIColour::menuText, VAR2COLOUR (owner.getProperty(Ids::uiPanelUIColourMenuText)));
//
//            setLookAndFeel(customLookAndFeel);
//            getCanvas()->setLookAndFeel(customLookAndFeel);
//            LookAndFeel::setDefaultLookAndFeel(customLookAndFeel);
            
        }
    }
}

juce::LookAndFeel *CtrlrPanelEditor::getLookAndFeelFromDescription(const juce::String &lookAndFeelDesc) // Added v5.6.34
{
    // If "Default" has a special meaning for CtrlrPanelEditor, handle it here.
    // Otherwise, you can just directly call the generic function.
    if (lookAndFeelDesc == "Default")
    {
        return nullptr; // Or whatever "Default" means for this specific component
                        // e.g., return new juce::LookAndFeel_V4();
    }

    // Now, simply call your centralized function!
    // The second argument `true` means if the `lookAndFeelDesc` doesn't match
    // any known scheme, it will return a new LookAndFeel_V4 with the LightColourScheme.
    // If you prefer it to return nullptr in unknown cases, change it to `false`.
    return gui::createLookAndFeelFromDescription(lookAndFeelDesc, true);
}

const var &CtrlrPanelEditor::getProperty(const Identifier &name) const
{
    return (panelEditorTree.getProperty(name));
}

const var CtrlrPanelEditor::getProperty(const Identifier &name, const var &defaultReturnValue) const
{
    return (panelEditorTree.getProperty(name, defaultReturnValue));
}

void CtrlrPanelEditor::setProperty(const Identifier &name, const var &newValue, const bool isUndoable)
{
    panelEditorTree.setProperty(name, newValue, 0);
}

const bool CtrlrPanelEditor::getMode()
{
    return (getProperty(Ids::uiPanelEditMode));
}

AffineTransform CtrlrPanelEditor::moveSelectionToPosition(const int newX, const int newY)
{
    if (getSelection() == nullptr)
        return (AffineTransform());

    RectangleList<int> rectangleList;

    for (int i = 0; i < getSelection()->getNumSelected(); i++)
    {
        CtrlrComponent *c = getSelection()->getSelectedItem(i);
        rectangleList.add(c->getBounds());
    }

    RectanglePlacement rp(RectanglePlacement::xLeft);
    return (rp.getTransformToFit(rectangleList.getBounds().toFloat(),
                                 rectangleList.getBounds().withPosition(newX, newY).toFloat()));
}

void CtrlrPanelEditor::initSingleInstance()
{
    owner.setProperty(Ids::uiPanelEditMode, false);
}


void CtrlrPanelEditor::notify(const String &notification, CtrlrNotificationCallback *callback, const CtrlrNotificationType ctrlrNotificationType)  // Added back v5.6.31 for file management bottom notification bar
{
    if (ctrlrPanelNotifier)
    {
        if ((int)owner.getProperty(Ids::panelMessageTime) <= 0)
            return;

        notificationCallback = callback;

        componentAnimator.cancelAllAnimations (true);
        ctrlrPanelNotifier->setVisible (true);
        
        if (notificationCallback != nullptr)
        {
            ctrlrPanelNotifier->setMouseCursor (MouseCursor::PointingHandCursor);
        }
        else
        {
            ctrlrPanelNotifier->setMouseCursor (MouseCursor::NormalCursor);
        }
        ctrlrPanelNotifier->setBounds (0, getHeight() - 28, getWidth(), 20);
        ctrlrPanelNotifier->setAlpha (1.0f);

        ctrlrPanelNotifier->setNotification (notification, ctrlrNotificationType);

        componentAnimator.animateComponent (ctrlrPanelNotifier, ctrlrPanelNotifier->getBounds(), 0.0f, owner.getProperty(Ids::panelMessageTime), false, 1.0, 1.0);
    }
}

void CtrlrPanelEditor::notificationClicked(const MouseEvent e)  // Added back v5.6.31 for file management bottom notification bar
{
    componentAnimator.cancelAllAnimations (true);

    if (!notificationCallback.wasObjectDeleted() && notificationCallback)
    {
        notificationCallback->notificationClicked(e);
    }
}

void CtrlrPanelEditor::changeListenerCallback (ChangeBroadcaster *source)  // Added back v5.6.31 for file management bottom notification bar
{
    if (source == &componentAnimator)
    {
        if (!componentAnimator.isAnimating())
        {
            ctrlrPanelNotifier->setVisible (false);
        }
    }
}



void CtrlrPanelEditor::reloadResources(Array<CtrlrPanelResource *> resourcesThatChanged)
{
    for (int i = 0; i < owner.getNumModulators(); i++)
    {
        if (owner.getModulatorByIndex(i)->getComponent())
        {
            owner.getModulatorByIndex(i)->getComponent()->reloadResources(resourcesThatChanged);
        }
    }

    resized();
}

void CtrlrPanelEditor::searchForProperty()
{
}

bool CtrlrPanelEditor::luaEditorExistsAndIsFocused() // Added v5.6.34. Required to pass keypress to the LUA method manager for menu items. Handles the focus gain/loss.
{
    // We use the public getContent() method on the window manager.
    // This will return a pointer to the component inside the Lua editor window.
    juce::Component* luaEditorContent = owner.getPanelWindowManager().getContent(CtrlrPanelWindowManager::LuaMethodEditor);

    // Now we check if the content component exists and has keyboard focus.
    if (luaEditorContent != nullptr && luaEditorContent->hasKeyboardFocus(true))
    {
        return true;
    }

    return false;
}
