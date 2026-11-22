#include "stdafx.h"
#include "CtrlrCombo.h"
#include "CtrlrValueMap.h"
#include "CtrlrIDs.h"
#include "CtrlrFontManager.h"
#include "CtrlrModulator/CtrlrModulator.h"
#include "CtrlrPanel/CtrlrPanel.h"
#include "CtrlrPanel/CtrlrPanelEditor.h"
#include "CtrlrUtilitiesGUI.h"
#include "JuceClasses/LLookAndFeel.h"
#include <algorithm>
#include <rapidfuzz/fuzz.hpp> // Added v5.6.35. Support for rapidfuzz


CtrlrCombo::CtrlrCombo (CtrlrModulator &owner)
    : CtrlrComponent(owner),
      lf(*this),
      ctrlrCombo (nullptr),
      isUpdating (false),
      isSearching (false)
{
	valueMap = std::make_unique<CtrlrValueMap>();
    
    // 1. Attach our fuzzy listener (The code we added earlier)
    addAndMakeVisible (ctrlrCombo = new ComboBox (L"ctrlrCombo"));
    
    // Force default editableText to false on construction like in 5.6.34-
    ctrlrCombo->setEditableText (false);
    ctrlrCombo->setJustificationType (Justification::centred);
    
    ctrlrCombo->addListener (this);
	ctrlrCombo->setLookAndFeel (&lf);
	componentTree.addListener (this);

	setProperty (Ids::uiComboSearch, false);
    setProperty (Ids::uiComboButtonWidthOverride, false);
    setProperty (Ids::uiComboButtonWidth, 16);
    setProperty (Ids::uiComboDynamicContent, 0);
    setProperty (Ids::uiComboSelectedId, -1);
    setProperty (Ids::uiComboSelectedIndex, -1);
    
    setProperty (Ids::uiComboContent, "");
    
    setProperty (Ids::uiButtonLookAndFeel, "Default");
    
    if ( owner.getOwnerPanel().getEditor()->getProperty(Ids::uiPanelLookAndFeel) == "V3"
        || owner.getOwnerPanel().getEditor()->getProperty(Ids::uiPanelLookAndFeel) == "V2"
        || owner.getOwnerPanel().getEditor()->getProperty(Ids::uiPanelLookAndFeel) == "V1" )
    {
        setProperty (Ids::uiComboArrowColour, "0xff0000ff");
        setProperty (Ids::uiComboOutlineColour, "0xff0000ff");
        
        setProperty (Ids::uiComboTextJustification, "centred");
        setProperty (Ids::uiComboFont, FONT2STR (Font(14)));
        setProperty (Ids::uiComboTextColour, "0xff000000");
        
        setProperty (Ids::uiComboMenuFont, FONT2STR (Font(16)));
        setProperty (Ids::uiComboMenuFontColour, "0xff000000");
        
        setProperty (Ids::uiComboButtonColour, "0xff0000ff");
        setProperty (Ids::uiComboBgColour, "0xffffffff");
        
        setProperty (Ids::uiComboMenuBackgroundColour, "0xfff0f0f0");
        
        setProperty (Ids::uiComboMenuHighlightColour, Colours::lightblue.toString());
        setProperty (Ids::uiComboMenuFontHighlightedColour, "0xff232323");
        
        setProperty (Ids::uiComboMenuBackgroundRibbed, true);
        
        setSize (88, 32);
    }
    else
    {
        setProperty (Ids::uiComboArrowColour, (String)findColour(ComboBox::arrowColourId).toString());
        setProperty (Ids::uiComboOutlineColour, (String)findColour(ComboBox::outlineColourId).darker(0.5f).toString());
        
        setProperty (Ids::uiComboTextJustification, "centred");
        setProperty (Ids::uiComboFont, FONT2STR (Font(14)));
        setProperty (Ids::uiComboTextColour, (String)findColour(ComboBox::textColourId).toString());
        
        setProperty (Ids::uiComboMenuFont, FONT2STR (Font(16)));
        setProperty (Ids::uiComboMenuFontColour, (String)findColour(ComboBox::textColourId).toString());
        
        setProperty (Ids::uiComboButtonColour, (String)findColour(ComboBox::buttonColourId).toString());
        setProperty (Ids::uiComboBgColour, (String)findColour(ComboBox::backgroundColourId).toString());
        
        setProperty (Ids::uiComboMenuBackgroundColour, (String)findColour(ComboBox::backgroundColourId).toString());
        
        setProperty (Ids::uiComboMenuHighlightColour, (String)findColour(TextEditor::highlightColourId).toString());
        setProperty (Ids::uiComboMenuFontHighlightedColour, (String)findColour(TextEditor::highlightedTextColourId).toString());
        
        setProperty (Ids::uiComboMenuBackgroundRibbed, false);
        
        setSize (120, 40);
    }
    
    setProperty (Ids::uiComboButtonGradient, false); // Updated v5.6.36. It looks less 2010s
    setProperty (Ids::uiComboButtonGradientColour1, (String)findColour(TextButton::buttonColourId).toString());
    setProperty (Ids::uiComboButtonGradientColour2, (String)findColour(TextButton::buttonColourId).darker(0.2f).toString());
    
    // Explicitly set the TextEditor colors to match the Combo background/text
    // ComboBox level
    ctrlrCombo->setColour (ComboBox::textColourId, findColour(ComboBox::textColourId));
    ctrlrCombo->setColour (ComboBox::backgroundColourId, findColour(ComboBox::backgroundColourId));
    
    // TextEditor level (for active typing)
    ctrlrCombo->setColour (TextEditor::textColourId, findColour(TextEditor::textColourId));
    ctrlrCombo->setColour (TextEditor::highlightColourId, findColour(TextEditor::highlightColourId));
    ctrlrCombo->setColour (TextEditor::highlightedTextColourId, findColour(TextEditor::highlightedTextColourId));
    ctrlrCombo->setColour (TextEditor::outlineColourId, Colours::transparentBlack);
    
    // Label level (the bridge between Combo and Editor)
    ctrlrCombo->setColour (Label::textColourId, findColour(Label::textColourId));
    ctrlrCombo->setColour (Label::backgroundColourId, Colours::transparentBlack); // Keep transparent to see the Combo bg
    ctrlrCombo->setColour (TextEditor::outlineColourId, Colours::transparentBlack);
    ctrlrCombo->setColour (Label::textWhenEditingColourId, findColour(Label::textWhenEditingColourId));
    ctrlrCombo->setColour (Label::backgroundWhenEditingColourId, findColour(Label::backgroundWhenEditingColourId));
    
	_DBG("--- CONSTRUCTION PHASE END ---");
}

CtrlrCombo::~CtrlrCombo()
{
	closeFuzzySearchPopupIfOpen();

	if (ctrlrCombo != nullptr)
		ctrlrCombo->setLookAndFeel(nullptr);

    // deleteAndZero is a JUCE macro that calls 'delete' and sets the pointer to nullptr.
    deleteAndZero (ctrlrCombo);
}

void CtrlrCombo::resized()
{
	if (restoreStateInProgress)
		return;

	ctrlrCombo->setBounds (getUsableRect());
}

void CtrlrCombo::mouseDown (const MouseEvent& e)
{
    if (canPerformFuzzySearch())
    {
		openFuzzySearchPopup();
		return; // Prevent passing click down to child combo
    }
    
    // Crucial: Passes mouse click to CtrlrComponent so resize handles can be grabbed in Edit Mode
    CtrlrComponent::mouseDown (e);
}

bool CtrlrCombo::keyPressed (const KeyPress& key) // Updated v5.6.35. Combined methods
{
    _DBG ("CtrlrCombo::keyPressed - Key: " + key.getTextDescription()
        + " | KeyCode: " + juce::String (key.getKeyCode())
    );
	
    if (getParentComponent())
    {
        if (auto *canvas = dynamic_cast<CtrlrPanelCanvas*>(getParentComponent()))
        {
            return canvas->keyPressed (key, this);
        }
    }
    
    return false;  // Return true if your handler uses this key event, or false to allow it to be passed-on.
}

void CtrlrCombo::visibilityChanged()
{
    // TRACE: Check UI state at the moment visibility is triggered
    if (ctrlrCombo)
    {
        _DBG("GUI_TRACE [" + owner.getName() + "] visibilityChanged ENTER | isVisible: " + String((int)isVisible())
             + " | UI Index: " + String(ctrlrCombo->getSelectedItemIndex())
             + " | UI Text: '" + ctrlrCombo->getText() + "'");
    }

    if (isVisible() && (bool)getProperty(Ids::uiComboSearch))
    {
        _DBG("LIFECYCLE: Component visible. Starting 250ms safety timer...");
        startTimer(250);
    }
}

void CtrlrCombo::focusLost (FocusChangeType cause)
{
    if (ctrlrCombo)
    {
        // This ensures the menu disappears if you click on another modulator or the background
        ctrlrCombo->hidePopup();
    }
}

void CtrlrCombo::lookAndFeelChanged()
{
    // TRACE: Capture state before any JUCE reconstruction happens
    if (ctrlrCombo)
    {
        _DBG("GUI_TRACE [" + owner.getName() + "] lookAndFeelChanged ENTER | Current Index: "
             + String(ctrlrCombo->getSelectedItemIndex()) + " | Text: '" + ctrlrCombo->getText() + "'");
    }

    // Call the base class first
    CtrlrComponent::lookAndFeelChanged();

    if (getProperty(Ids::uiComboSearch))
    {
        _DBG("LIFECYCLE: LookAndFeel changed. This usually wipes the Label state. Re-attaching...");
        
        // We use a slight delay because JUCE's lookAndFeelChanged
        // often happens before the internal Label is re-configured.
        // Use the safe class timer instead of callAfterDelay
        startTimer(100);
    }
    
    // TRACE: Capture state after the base class call.
    // Added Text trace here to see if the GUI wiped while the Index stayed.
    if (ctrlrCombo)
    {
        _DBG("GUI_TRACE [" + owner.getName() + "] lookAndFeelChanged EXIT | Current Index: "
             + String(ctrlrCombo->getSelectedItemIndex()) + " | Text: '" + ctrlrCombo->getText() + "'");
    }
}

void CtrlrCombo::parentHierarchyChanged()
{
    if (isVisible() && getWidth() > 0 && getHeight() > 0) // Added v5.6.36. Prevents ComboBox internal label from blanking out when embedded inside uiTabs with fuzzy search disabled.
    {
        startTimer(50); // Quick check to re-sync display text
    }
}

void CtrlrCombo::timerCallback()
{
    stopTimer();

    // TRACE: Check state when the timer wakes up
    if (ctrlrCombo)
    {
        _DBG("GUI_TRACE [" + owner.getName() + "] timerCallback FIRED | UI Index: "
             + String(ctrlrCombo->getSelectedItemIndex()) + " | UI Text: '" + ctrlrCombo->getText() + "'");
    }

    const bool isInEditMode = owner.getOwnerPanel().getEditMode();
    
    if (isInEditMode)
    {
        closeFuzzySearchPopupIfOpen();
    } else {
        if (ctrlrCombo != nullptr && valueMap != nullptr) {
            valueMap->fillCombo(*ctrlrCombo, true);
        }
    }

	if (ctrlrCombo != nullptr && !isInEditMode)
    {
        _DBG("GUI_TRACE [" + owner.getName() + "] timerCallback EXIT | Final UI Index: "
             + String(ctrlrCombo->getSelectedItemIndex()) + " | Final UI Text: '" + ctrlrCombo->getText() + "'");
		const double modulatorValue = owner.getProcessor().getValue();
		ctrlrCombo->setSelectedId(modulatorValue + 1, dontSendNotification);
    }
}

void CtrlrCombo::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    if (isUpdating) return;
    if (comboBoxThatHasChanged == ctrlrCombo)
    {
        const int id = ctrlrCombo->getSelectedId();
        if (id > 0)
        {
            isSearching = false;
            setComponentValue (id - 1, true);
            ctrlrCombo->setEditableText(false); // Reset for next interaction
        }
    }
}

double CtrlrCombo::getComponentMaxValue()
{
	return (valueMap->getNonMappedMax());
}

double CtrlrCombo::getComponentValue()
{
	return (ctrlrCombo->getSelectedId()-1);
}

int CtrlrCombo::getComponentMidiValue()
{
	return (valueMap->getMappedValue(ctrlrCombo->getSelectedId() - 1));
}

const String CtrlrCombo::getComponentText()
{
	return (ctrlrCombo->getText());
}

void CtrlrCombo::setComponentValue (const double newValue, const bool sendChangeMessage)
{
    // TRACE START
	_DBG("GUI_TRACE [" + owner.getName() + "] setComponentValue START | newValue: " + String(newValue));
	
	if (ctrlrCombo)
	{
		// ADD THIS: Check if the list is actually populated yet
		_DBG("   -> Current Item Count in List: " + String(ctrlrCombo->getNumItems()));
		
		ctrlrCombo->setSelectedId (newValue + 1, sendChangeMessage ? sendNotificationSync : dontSendNotification);
		
		_DBG("GUI_TRACE [" + owner.getName() + "] Post-Selection | UI Index: " + String(ctrlrCombo->getSelectedItemIndex())
			 + " | UI Text: '" + ctrlrCombo->getText() + "'");
		
		if (ctrlrCombo->getSelectedItemIndex() == -1 && newValue != -1)
		{
			_DBG("GUI_TRACE [" + owner.getName() + "] !! WARNING: ComboBox rejected Value " + String(newValue));
		}
	}

    if (sendChangeMessage)
    {
        owner.getProcessor().setValueGeneric (CtrlrModulatorValue(newValue,CtrlrModulatorValue::changedByGUI), sendChangeMessage);
    }
    
    _DBG("GUI_TRACE [" + owner.getName() + "] setComponentValue END");
}

void CtrlrCombo::comboContentChanged()
{
	if ((int)getProperty(Ids::uiComboDynamicContent) > 0)
		return;

	valueMap->copyFrom (owner.getProcessor().setValueMap (getProperty(Ids::uiComboContent)));
	valueMap->fillCombo (*ctrlrCombo, true);
}

void CtrlrCombo::valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property)
{
	// Log EVERY property change during load
    _DBG("PROP CHANGE: " + property.toString() + " = " + getProperty(property).toString());
	
	if (property == Ids::uiComboContent)
	{
		comboContentChanged();
	}
	// PATH 1: User toggles search ON/OFF
	if (property == Ids::uiComboSearch)
	{
		_DBG("PROP: uiComboSearch changed - starting safety timer");
		if (ctrlrCombo != nullptr) {
			const bool allowComboClicks = !canPerformFuzzySearch();
			ctrlrCombo->setInterceptsMouseClicks(allowComboClicks, allowComboClicks);
		}
		startTimer(250);
	}
	else if (property == Ids::uiButtonLookAndFeel)
	{
		updatingLookAndFeel = true; // Set guard flag
        
		String LookAndFeelType = getProperty(property);
        setLookAndFeel(getLookAndFeelFromComponentProperty(LookAndFeelType)); // Updates the current component LookAndFeel

        resetLookAndFeelOverrides(); // Retrieves LookAndFeel colours from selected ColourScheme
        
        updatingLookAndFeel = false; // Clear guard flag
    }
	// Single catch-all for all uiCombo visual property changes
	else if (property.toString().startsWith("uiCombo"))
	{
		// 1. Update colors, font sizes, justification, and force repaints
		updateInternalComponentStyles();
	}
	else if (property == Ids::uiComboDynamicContent)
	{
		fillContent(getProperty(property));
	}
	else if (property == Ids::uiComboSelectedId )
    {
        if ((int)getProperty(property) != -1)
        {
            ctrlrCombo->setSelectedId (getProperty(property), sendNotificationSync);
        }
    }
    else if (property == Ids::uiComboSelectedIndex)
    {
        if ((int)getProperty(property) != -1)
        {
            ctrlrCombo->setSelectedItemIndex (getProperty(property), sendNotificationSync);
        }
    }
    else
    {
        CtrlrComponent::valueTreePropertyChanged(treeWhosePropertyHasChanged, property);
    }

	if (restoreStateInProgress == false)
	{
        resized();
	}
}

void CtrlrCombo::handleAsyncUpdate()
{
    if (ctrlrCombo == nullptr || valueMap == nullptr) return;

    // 1. Capture Caret Position before clearing
    int caretPos = 0;
    juce::TextEditor* ed = nullptr;
    for (int i = 0; i < ctrlrCombo->getNumChildComponents(); ++i) {
        if (auto* lb = dynamic_cast<juce::Label*>(ctrlrCombo->getChildComponent(i))) {
            ed = lb->getCurrentTextEditor();
            if (ed != nullptr) {
                caretPos = ed->getCaretPosition();
                break;
            }
        }
    }

    isUpdating = true;
    
    // 2. Filter Logic (Keep your existing loop)
    StringArray matches;
    Array<int> ids;
    for (int i = 0; i < valueMap->getNumValues(); ++i) {
        String item = valueMap->getTextForIndex(i);
        if (item.containsIgnoreCase(lastSearchText)) {
            matches.add(item);
            ids.add(i + 1);
        }
    }

    // 3. Update UI
    ctrlrCombo->clear(juce::dontSendNotification);
    for (int i = 0; i < matches.size(); ++i)
        ctrlrCombo->addItem(matches[i], ids[i]);

    if (isSearching) {
        // ALWAYS dismiss and show to refresh the list correctly
        juce::PopupMenu::dismissAllActiveMenus();

        juce::MessageManager::callAsync([this, caretPos]() {
            if (ctrlrCombo == nullptr) return;
            
            ctrlrCombo->showPopup();

            // Re-grab focus and restore the EXACT cursor position
            for (int i = 0; i < ctrlrCombo->getNumChildComponents(); ++i) {
                if (auto* lb = dynamic_cast<juce::Label*>(ctrlrCombo->getChildComponent(i))) {
                    if (auto* currentEd = lb->getCurrentTextEditor()) {
                        currentEd->grabKeyboardFocus();
                        currentEd->setHighlightedRegion(juce::Range<int>(0, 0)); // Fixes "Select All" bug
                        currentEd->setCaretPosition(caretPos);
                    }
                }
            }
        });
    }
    
    isUpdating = false;
}

void CtrlrCombo::setComponentText (const String &componentText)
{
	ctrlrCombo->setText (componentText);
}

void CtrlrCombo::fillContent(const int contentType)
{
	Array<File> files;
	const String prewviousContent = ctrlrCombo->getText();
	
	switch (contentType)
	{
		case 1:
			for (int i=0; i<owner.getOwnerPanel().getModulators().size(); i++)
			{
				valueMap->setPair (i, i, owner.getOwnerPanel().getModulatorByIndex(i)->getName());
			}
			owner.getProcessor().setValueMap (*valueMap);
			valueMap->fillCombo (*ctrlrCombo, true);
			ctrlrCombo->setText (prewviousContent, dontSendNotification);
			break;

		case 2:
			File::findFileSystemRoots(files);
			for (int i=0; i<files.size(); i++)
			{
				valueMap->setPair (i, i, files[i].getFullPathName());
			}
			owner.getProcessor().setValueMap (*valueMap);
			valueMap->fillCombo (*ctrlrCombo, true);
			ctrlrCombo->setText (prewviousContent, dontSendNotification);
			break;
		default:
			comboContentChanged();
			break;
	}
}

void CtrlrCombo::panelEditModeChanged(const bool isInEditMode)
{
    _DBG("Combo Edit Mode: " + String(isInEditMode ? "ON" : "OFF"));
    
	if (ctrlrCombo != nullptr && ctrlrCombo->isPopupActive())
		ctrlrCombo->hidePopup();

    // Close active popup if open when entering edit mode
    if (isInEditMode)
		closeFuzzySearchPopupIfOpen();

    if (ctrlrCombo != nullptr)
    {
        // Pass mouse interaction to CtrlrComponent so resize handles work in edit mode
		const bool allowComboClicks = !isInEditMode && !canPerformFuzzySearch();
		ctrlrCombo->setInterceptsMouseClicks(allowComboClicks, allowComboClicks);
        ctrlrCombo->setEditableText(false);
    }
    
    // We always use the timer to decouple from the synchronous mode change.
    // 50ms is enough for 'Entering', 200ms is safer for 'Exiting' (rebuilding UI).
    startTimer (isInEditMode ? 50 : 200);

    // Standard Ctrlr enablement logic
    if ((bool)owner.getOwnerPanel().getEditor()->getProperty(Ids::uiPanelDisabledOnEdit))
    {
        if (ctrlrCombo != nullptr)
            ctrlrCombo->setEnabled (!isInEditMode);
    }

    resized();
}

int CtrlrCombo::getSelectedId()
{
	return (ctrlrCombo->getSelectedId());
}

int CtrlrCombo::getSelectedItemIndex()
{
	return (ctrlrCombo->getSelectedItemIndex());
}

void CtrlrCombo::setSelectedId(const int id, const bool dontNotify)
{
	ctrlrCombo->setSelectedId (id, dontNotify ? dontSendNotification : sendNotificationSync);
}

void CtrlrCombo::setSelectedItemIndex(const int index, const bool dontNotify)
{
	ctrlrCombo->setSelectedItemIndex (index, dontNotify ? dontSendNotification : sendNotificationSync);
}

const String CtrlrCombo::getText()
{
	return (ctrlrCombo->getText());
}

void CtrlrCombo::setText(const String &text, const bool dontNotify)
{
	return (ctrlrCombo->setText(text, dontNotify ? dontSendNotification : sendNotificationSync));
}

void CtrlrCombo::customLookAndFeelChanged(LookAndFeelBase *customLookAndFeel)
{
    if (customLookAndFeel == nullptr)
        ctrlrCombo->setLookAndFeel (&lf);
    else
        ctrlrCombo->setLookAndFeel (customLookAndFeel);
}

LookAndFeel *CtrlrCombo::getLookAndFeelFromComponentProperty(const String &lookAndFeelComponentProperty) // Updated v5.6.34
{
    if (lookAndFeelComponentProperty == "Default")
    {
        // This case still means "use the default LookAndFeel (which might be the global one)"
        // so returning nullptr is appropriate if that's the desired behavior.
        return nullptr;
    }

    // Call your new generic factory function
    // We pass 'false' for the second argument here, as 'Default' is handled separately
    // and an unknown string should likely result in nullptr to fall back to the global L&F.
    return gui::createLookAndFeelFromDescription(lookAndFeelComponentProperty, false);
}

void CtrlrCombo::resetLookAndFeelOverrides()
{
    _DBG("RESET_LF: Start - restoreStateInProgress = " + String(restoreStateInProgress ? "TRUE" : "FALSE"));
	
    if (restoreStateInProgress == false)
    {
        // 1. Log the current LookAndFeel state
        _DBG("RESET_LF: Overriding UI properties from LookAndFeel defaults");
		
        // Let's see if findColour is actually returning what we expect
        _DBG("RESET_LF: Current Text Colour: " + findColour(ComboBox::textColourId).toDisplayString(true));
		
        setProperty (Ids::componentLabelColour, (String)findColour(Label::textColourId).toString());
        
        setProperty (Ids::uiComboArrowColour, (String)findColour(ComboBox::arrowColourId).toString());
        setProperty (Ids::uiComboOutlineColour, (String)findColour(ComboBox::outlineColourId).darker(0.5f).toString());
        
        setProperty (Ids::uiComboTextColour, (String)findColour(ComboBox::textColourId).toString());
        
        setProperty (Ids::uiComboMenuFontColour, (String)findColour(ComboBox::textColourId).toString());
        
        setProperty (Ids::uiComboButtonColour, (String)findColour(ComboBox::buttonColourId).toString());
        setProperty (Ids::uiComboBgColour, (String)findColour(ComboBox::backgroundColourId).toString());
        
        setProperty (Ids::uiComboMenuBackgroundColour, (String)findColour(ComboBox::backgroundColourId).toString());
        
        setProperty (Ids::uiComboMenuHighlightColour, (String)findColour(TextEditor::highlightColourId).toString());
        setProperty (Ids::uiComboMenuFontHighlightedColour, (String)findColour(TextEditor::highlightedTextColourId).toString());
        
        setProperty (Ids::uiComboButtonGradientColour1, (String)findColour(TextButton::buttonColourId).toString());
        setProperty (Ids::uiComboButtonGradientColour2, (String)findColour(TextButton::buttonColourId).darker(0.2f).toString());
        
        _DBG("RESET_LF: End");
		
        updatePropertiesPanel(); // Refreshes property pane
    }
}

void CtrlrCombo::updatePropertiesPanel()
{
    CtrlrPanelProperties *props = owner.getCtrlrManagerOwner().getActivePanel()->getEditor(false)->getPropertiesPanel();
    if (props)
    {
        props->refreshAll(); // Needs extra code to prevent scrolling back to top on refresh
    }
}

bool CtrlrCombo::canPerformFuzzySearch() const
{
    const bool isEnabledInInspector = (bool) getProperty(Ids::uiComboSearch);
    const bool isEditingLayout      = owner.getOwnerPanel().getEditor()->getMode();

    // Search ONLY works if enabled in inspector AND NOT in edit mode
    return isEnabledInInspector && !isEditingLayout;
}

void CtrlrCombo::updateInternalComponentStyles()
{
    _DBG("--- updateInternalComponentStyles ---");
	if (ctrlrCombo == nullptr) return;

    const Colour bg = VAR2COLOUR(getProperty(Ids::uiComboBgColour));
    const Colour txt = VAR2COLOUR(getProperty(Ids::uiComboTextColour));

    // 1. Force the ComboBox itself to update its internal color IDs
    ctrlrCombo->setColour(ComboBox::backgroundColourId, bg);
    ctrlrCombo->setColour(TextEditor::backgroundColourId, bg);
    ctrlrCombo->setColour(TextEditor::highlightColourId, bg);
    ctrlrCombo->setColour(Label::backgroundColourId, bg);
    ctrlrCombo->setColour(Label::backgroundWhenEditingColourId, bg);
    
    ctrlrCombo->setColour(ComboBox::textColourId, txt);
    ctrlrCombo->setColour(TextEditor::textColourId, txt);
    ctrlrCombo->setColour(TextEditor::highlightedTextColourId, txt);
    ctrlrCombo->setColour(Label::textColourId, txt);
    ctrlrCombo->setColour(Label::textWhenEditingColourId, txt);
    
    // For good measure, sync the button color too if you use it
    ctrlrCombo->setColour(ComboBox::buttonColourId, VAR2COLOUR(getProperty(Ids::uiComboButtonColour)));

    ctrlrCombo->setColour(ComboBox::arrowColourId, VAR2COLOUR(getProperty(Ids::uiComboArrowColour)));
    ctrlrCombo->setColour(ComboBox::outlineColourId, VAR2COLOUR(getProperty(Ids::uiComboOutlineColour)));

    // Apply justification
    ctrlrCombo->setJustificationType(justificationFromProperty(getProperty(Ids::uiComboTextJustification)));
	
    // 2. Iterate through children to force the change on existing sub-components
    for (int i = 0; i < ctrlrCombo->getNumChildComponents(); ++i)
    {
        auto* child = ctrlrCombo->getChildComponent(i);
        
        // If the Label exists (idle state), force its color
        if (auto* lb = dynamic_cast<juce::Label*>(child))
        {
            lb->setColour(Label::backgroundColourId, Colours::transparentBlack);
            lb->setColour(Label::textColourId, txt);
        }
        
        // If the TextEditor exists (active search state), force its colors
        if (auto* ed = dynamic_cast<juce::TextEditor*>(child))
        {
            ed->setColour(TextEditor::backgroundColourId, bg);
            ed->setColour(TextEditor::textColourId, txt);
            ed->setColour(TextEditor::highlightColourId, txt.withAlpha(0.2f));
        }
	}
	ctrlrCombo->repaint();
}

//==============================================================================
// CtrlrCombo - Popup Management
//==============================================================================

void CtrlrCombo::openFuzzySearchPopup() {
	if (ctrlrCombo == nullptr || valueMap == nullptr)
		return;
	if (activeSearchPanel != nullptr)
		return;

	auto panel = std::make_unique<FuzzySearchPanel>(*this);
	activeSearchPanel = panel.get();

	auto &box = juce::CallOutBox::launchAsynchronously(std::move(panel), ctrlrCombo->getScreenBounds(), nullptr);
	box.setDismissalMouseClicksAreAlwaysConsumed(true);

	isSearching = true;

	if (activeSearchPanel != nullptr)
		activeSearchPanel->focusSearchField();
}

void CtrlrCombo::closeFuzzySearchPopupIfOpen() {
	if (activeSearchPanel == nullptr)
		return;

	if (auto *box = activeSearchPanel->findParentComponentOfClass<juce::CallOutBox>())
		box->dismiss();

	isSearching = false;
}

//==============================================================================
// CtrlrCombo::FuzzySearchPanel
//==============================================================================

CtrlrCombo::FuzzySearchPanel::FuzzySearchPanel(CtrlrCombo &ownerCombo) : owner(ownerCombo) {
	const Colour bg = VAR2COLOUR(owner.getProperty(Ids::uiComboBgColour));
	const Colour txt = VAR2COLOUR(owner.getProperty(Ids::uiComboTextColour));

	addAndMakeVisible(searchBox);
	searchBox.setMultiLine(false);
	searchBox.setReturnKeyStartsNewLine(false);
	searchBox.setSelectAllWhenFocused(true);
	searchBox.setColour(juce::TextEditor::backgroundColourId, bg);
	searchBox.setColour(juce::TextEditor::textColourId, txt);
	searchBox.setColour(juce::TextEditor::highlightColourId, txt.withAlpha(0.25f));
	searchBox.setColour(juce::TextEditor::highlightedTextColourId, txt);
	searchBox.setFont(owner.getOwner().getOwnerPanel().getOwner().getFontManager().getFontFromString(
		owner.getProperty(Ids::uiComboFont)));
	searchBox.addListener(this);
	searchBox.addKeyListener(this);

	addAndMakeVisible(resultsList);
	resultsList.setModel(this);
	resultsList.setColour(juce::ListBox::backgroundColourId,
						  VAR2COLOUR(owner.getProperty(Ids::uiComboMenuBackgroundColour)));
	resultsList.setRowHeight(22);
	resultsList.setMultipleSelectionEnabled(false);
	resultsList.setWantsKeyboardFocus(false);
	resultsList.setMouseClickGrabsKeyboardFocus(false);

	const int width = jmax(220, owner.getOwnedComboBox()->getWidth());
	setSize(width, 60);

	refreshMatches();
}

CtrlrCombo::FuzzySearchPanel::~FuzzySearchPanel() {
	owner.activeSearchPanel = nullptr;
	owner.isSearching = false;
}

void CtrlrCombo::FuzzySearchPanel::resized() {
	auto area = getLocalBounds().reduced(4);
	searchBox.setBounds(area.removeFromTop(26));
	area.removeFromTop(4);
	resultsList.setBounds(area);
}

void CtrlrCombo::FuzzySearchPanel::visibilityChanged() {
	if (isVisible())
		focusSearchField();
}

void CtrlrCombo::FuzzySearchPanel::focusSearchField() {
    // Queue focus request so the CallOutBox has finished its desktop/parent attachment
    juce::MessageManager::callAsync([safeThis = juce::Component::SafePointer<FuzzySearchPanel>(this)]() {
        if (safeThis != nullptr && safeThis->isShowing()) {
            safeThis->searchBox.grabKeyboardFocus();
            safeThis->searchBox.moveCaretToEnd();
        }
    });
}

void CtrlrCombo::FuzzySearchPanel::paintOverChildren(juce::Graphics &g) {
	if (matches.empty() && searchBox.getText().isNotEmpty()) {
		g.setColour(VAR2COLOUR(owner.getProperty(Ids::uiComboMenuFontColour)).withAlpha(0.6f));
		g.setFont(searchBox.getFont());
		g.drawFittedText("(no matches)", resultsList.getBounds(), juce::Justification::centred, 1);
	}
}

void CtrlrCombo::FuzzySearchPanel::textEditorTextChanged(juce::TextEditor &) {
	refreshMatches();
}

void CtrlrCombo::FuzzySearchPanel::textEditorReturnKeyPressed(juce::TextEditor &) {
	int row = resultsList.getSelectedRow();
	if (row < 0 && !matches.empty())
		row = 0;
	commitRow(row);
}

void CtrlrCombo::FuzzySearchPanel::textEditorEscapeKeyPressed(juce::TextEditor &) {
	closePopup();
}

int CtrlrCombo::FuzzySearchPanel::getNumRows() {
	return (int)matches.size();
}

void CtrlrCombo::FuzzySearchPanel::paintListBoxItem(int rowNumber, juce::Graphics &g, int width, int height,
													bool rowIsSelected) {
	if (!isPositiveAndBelow(rowNumber, (int)matches.size()))
		return;

	const Colour bgColour = VAR2COLOUR(owner.getProperty(Ids::uiComboMenuBackgroundColour));
	const Colour textColour = VAR2COLOUR(owner.getProperty(Ids::uiComboMenuFontColour));
	const Colour hiColour = VAR2COLOUR(owner.getProperty(Ids::uiComboMenuHighlightColour));
	const Colour hiTextColour = VAR2COLOUR(owner.getProperty(Ids::uiComboMenuFontHighlightedColour));

	g.fillAll(rowIsSelected ? hiColour : bgColour);
	g.setColour(rowIsSelected ? hiTextColour : textColour);
	g.setFont(owner.getOwner().getOwnerPanel().getOwner().getFontManager().getFontFromString(
		owner.getProperty(Ids::uiComboMenuFont)));
	g.drawFittedText(matches[(size_t)rowNumber].text, 6, 0, width - 12, height, juce::Justification::centredLeft, 1);
}

void CtrlrCombo::FuzzySearchPanel::listBoxItemClicked(int row, const juce::MouseEvent &) {
	commitRow(row);
}

bool CtrlrCombo::FuzzySearchPanel::keyPressed(const juce::KeyPress &key, juce::Component *) {
	const int numRows = (int)matches.size();

	if (key == juce::KeyPress::downKey) {
		if (numRows > 0)
			resultsList.selectRow(jlimit(0, numRows - 1, resultsList.getSelectedRow() + 1));
		return true;
	}
	if (key == juce::KeyPress::upKey) {
		if (numRows > 0)
			resultsList.selectRow(jlimit(0, numRows - 1, resultsList.getSelectedRow() - 1));
		return true;
	}
	return false;
}

void CtrlrCombo::FuzzySearchPanel::refreshMatches() {
	matches.clear();

	auto &valueMap = owner.getValueMap();
	const String query = searchBox.getText();

	if (query.isEmpty()) {
		for (int i = 0; i < valueMap.getNumValues(); ++i)
			matches.push_back({i + 1, valueMap.getTextForIndex(i), 0.0});
	} else {
		const std::string queryStd = query.toLowerCase().toStdString();

		for (int i = 0; i < valueMap.getNumValues(); ++i) {
			const String itemText = valueMap.getTextForIndex(i);
			const double score = rapidfuzz::fuzz::WRatio(queryStd, itemText.toLowerCase().toStdString());

			if (score > 40.0)
				matches.push_back({i + 1, itemText, score});
		}

		std::stable_sort(matches.begin(), matches.end(),
						 [](const Match &a, const Match &b) { return a.score > b.score; });
	}

	resultsList.updateContent();
	resultsList.selectRow(matches.empty() ? -1 : 0, false, true);

	const int rowsShown = jlimit(1, 8, jmax(1, (int)matches.size()));
	setSize(getWidth(), 34 + rowsShown * 22);

	repaint();
}

void CtrlrCombo::FuzzySearchPanel::commitRow(int row) {
	if (!isPositiveAndBelow(row, (int)matches.size()))
		return;

	owner.setSelectedId(matches[(size_t)row].id, false);
	closePopup();
}

void CtrlrCombo::FuzzySearchPanel::closePopup() {
	if (auto *box = findParentComponentOfClass<juce::CallOutBox>())
		box->dismiss();
}

//==============================================================================
// CtrlrComboLF Implementations
//==============================================================================

void CtrlrCombo::CtrlrComboLF::drawPopupMenuBackground (Graphics &g, int width, int height)
{
	const Colour background = VAR2COLOUR(owner.getProperty(Ids::uiComboMenuBackgroundColour));

    g.fillAll (background);

	if (owner.getProperty (Ids::uiComboMenuBackgroundRibbed))
	{
    g.setColour (background.overlaidWith (Colour (0x2badd8e6)));

    for (int i = 0; i < height; i += 3)
        g.fillRect (0, i, width, 1);

#if ! JUCE_MAC
    g.setColour (findColour (PopupMenu::textColourId).withAlpha (0.6f));
    g.drawRect (0, 0, width, height);
#endif
	}
}

// Updated v5.6.35. Custom LookAndFeel Class and methods moved at the bottom for clarity
void CtrlrCombo::CtrlrComboLF::drawPopupMenuItem (Graphics &g, const Rectangle<int>& area,
                            bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu,
                            const String& text, const String& shortcutKeyText,
                            const Drawable* icon, const Colour* textColourToUse)
{
	const int width = area.getWidth();
	const int height = area.getHeight();

	const float halfH = height * 0.5f;

    if (isSeparator)
    {
        const float separatorIndent = 5.5f;

        g.setColour (findColour(ComboBox::textColourId).withAlpha(0.3f));// Colour (0x33000000));
        g.drawLine (separatorIndent, halfH, width - separatorIndent, halfH);

        g.setColour (findColour(ComboBox::textColourId).withAlpha(0.6f)); // Colour (0x66ffffff));
        g.drawLine (separatorIndent, halfH + 1.0f, width - separatorIndent, halfH + 1.0f);
    }
    else
    {
		Colour textColour = VAR2COLOUR(owner.getProperty(Ids::uiComboMenuFontColour));

        if (textColourToUse != nullptr)
		{
			_DBG("Using passed in colour: "+textColourToUse->toString());
            textColour = *textColourToUse;
		}

        if (isHighlighted)
        {
			g.setColour (VAR2COLOUR(owner.getProperty(Ids::uiComboMenuHighlightColour)));
            g.fillRect (1, 1, width - 2, height - 2);

            g.setColour (VAR2COLOUR(owner.getProperty(Ids::uiComboMenuFontHighlightedColour)));
        }
        else
        {
            g.setColour (textColour);
        }

        if (! isActive)
            g.setOpacity (0.3f);

		Font font = owner.getOwner().getOwnerPanel().getCtrlrManagerOwner().getFontManager().getFontFromString (owner.getProperty(Ids::uiComboMenuFont));

        if (font.getHeight() > height / 1.3f)
            font.setHeight (height / 1.3f);

        g.setFont (font);

        const int leftBorder = (height * 5) / 4;
        const int rightBorder = 4;

        if (icon != nullptr)
        {
			icon->drawWithin (g, Rectangle<float>(2, 1, leftBorder - 4, height - 2), RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize, 1.0f);
        }
        else if (isTicked)
        {
            const Path tick (getTickShape (1.0f));
            const float th = font.getAscent();
            const float ty = halfH - th * 0.5f;

            g.fillPath (tick, tick.getTransformToScaleToFit (2.0f, ty, (float) (leftBorder - 4),
                                                             th, true));
        }

        g.drawFittedText (text,
                          leftBorder, 0,
                          width - (leftBorder + rightBorder), height,
                          Justification::centredLeft, 1);

        if (shortcutKeyText.isNotEmpty())
        {
            Font f2 (font);
            f2.setHeight (f2.getHeight() * 0.75f);
            f2.setHorizontalScale (0.95f);
            g.setFont (f2);

            g.drawText (shortcutKeyText,
                        leftBorder,
                        0,
                        width - (leftBorder + rightBorder + 4),
                        height,
                        Justification::centredRight,
                        true);
        }

        if (hasSubMenu)
        {
            const float arrowH = 0.6f * getPopupMenuFont().getAscent();
            const float x = width - height * 0.6f;

            Path p;
            p.addTriangle (x, halfH - arrowH * 0.5f,
                           x, halfH + arrowH * 0.5f,
                           x + arrowH * 0.6f, halfH);

            g.fillPath (p);
        }
    }
}

void CtrlrCombo::CtrlrComboLF::drawComboBox (Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, ComboBox& box)
{
	int bw=buttonW;
	int bx=buttonX;
	const float outlineThickness = isButtonDown ? 1.2f : 0.5f;

	g.fillAll (box.findColour (ComboBox::backgroundColourId));
    g.setColour (box.findColour (ComboBox::outlineColourId));
    g.drawRect (0, 0, width, height);

	if ((bool)owner.getProperty (Ids::uiComboButtonWidthOverride) == true)
	{
		bw = owner.getProperty (Ids::uiComboButtonWidth);
		bx = width - bw;
	}

    const Colour baseColour (createBaseColour (box.findColour (ComboBox::buttonColourId),
                                                                   box.hasKeyboardFocus (true),
                                                                   false, isButtonDown)
                                .withMultipliedAlpha (1.0f));

	if ((bool)owner.getProperty (Ids::uiComboButtonGradient) == true)
	{
		g.setGradientFill (
							ColourGradient (
												VAR2COLOUR(owner.getProperty(Ids::uiComboButtonGradientColour1)),
												buttonX + outlineThickness,
												buttonY + outlineThickness,
												VAR2COLOUR(owner.getProperty(Ids::uiComboButtonGradientColour2)),
												buttonX + outlineThickness,
												(buttonY + outlineThickness) + (buttonH - outlineThickness * 2.0f),
												false
											)
						);

		g.fillRect (buttonX + outlineThickness, buttonY + outlineThickness, buttonW - outlineThickness * 2.0f, buttonH - outlineThickness * 2.0f);
	}
	else
	{
		drawGlassLozenge (g,
							bx + outlineThickness,
							buttonY + outlineThickness,
							bw - outlineThickness * 2.0f,
							buttonH - outlineThickness * 2.0f,
							baseColour,
							outlineThickness, -1.0f,
							true,
							true,
							true,
							true);
	}

	const float arrowX = 0.3f;
    const float arrowH = 0.2f;

    Path p;
    p.addTriangle (	bx + bw * 0.5f,
					buttonY + buttonH * (0.45f - arrowH),
					bx + bw * (1.0f - arrowX), buttonY + buttonH * 0.45f,
                    bx + bw * arrowX,          buttonY + buttonH * 0.45f);

    p.addTriangle (	bx + bw * 0.5f,
					buttonY + buttonH * (0.55f + arrowH),
                    bx + bw * (1.0f - arrowX), buttonY + buttonH * 0.55f,
                    bx + bw * arrowX,          buttonY + buttonH * 0.55f);

    g.setColour (box.findColour (ComboBox::arrowColourId));
    g.fillPath (p);
}

const Colour CtrlrCombo::CtrlrComboLF::createBaseColour (const Colour& buttonColour, const bool hasKeyboardFocus, const bool isMouseOverButton, const bool isButtonDown)
{
	const float sat = hasKeyboardFocus ? 1.3f : 0.9f;
	const Colour baseColour (buttonColour.withMultipliedSaturation (sat));

	if (isButtonDown)
		return baseColour.contrasting (0.2f);
	else if (isMouseOverButton)
		return baseColour.contrasting (0.1f);

	return baseColour;
}

void CtrlrCombo::CtrlrComboLF::positionComboBoxText (ComboBox& box, Label& label)
{
	int bw			= owner.getProperty (Ids::uiComboButtonWidth);

	if ((bool)owner.getProperty (Ids::uiComboButtonWidthOverride) == true)
	{
		label.setBounds (1, 1, box.getWidth() - bw, box.getHeight() - 2);
	}
	else
	{
		label.setBounds (1, 1, box.getWidth() + 3 - box.getHeight(), box.getHeight() - 2);
	}

    label.setFont (getComboBoxFont (box));
}

void CtrlrCombo::CtrlrComboLF::fillLabelTextEditorBackground (Graphics& g, TextEditor& editor)
{
     // Force the background to match your ComboBox background property
     g.fillAll (VAR2COLOUR(owner.getProperty(Ids::uiComboBgColour)));
    
     // Draw an outline if you want one
     g.setColour (VAR2COLOUR(owner.getProperty(Ids::uiComboOutlineColour)));
     g.drawRect (0, 0, editor.getWidth(), editor.getHeight());
}

Font CtrlrCombo::CtrlrComboLF::getComboBoxFont (ComboBox &box)
{
    return (owner.getOwner().getOwnerPanel().getCtrlrManagerOwner().getFontManager().getFontFromString (owner.getProperty(Ids::uiComboFont)));
}

Font CtrlrCombo::CtrlrComboLF::getPopupMenuFont ()
{
    return (owner.getOwner().getOwnerPanel().getCtrlrManagerOwner().getFontManager().getFontFromString (owner.getProperty(Ids::uiComboMenuFont)));
}

Font CtrlrCombo::CtrlrComboLF::getLabelFont (Label& label) // Added v5.6.35. Font styling sans Colour
{
    return owner.getOwner().getOwnerPanel().getCtrlrManagerOwner().getFontManager().getFontFromString (owner.getProperty(Ids::uiComboFont));
}
