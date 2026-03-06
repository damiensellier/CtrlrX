#include "stdafx.h"
#include "CtrlrPropertyComponent.h"
#include "CtrlrLua/MethodEditor/CtrlrLuaMethodEditor.h"
#include "CtrlrLuaManager.h"
#include "CtrlrInlineUtilitiesGUI.h"
#include <juce_gui_basics/juce_gui_basics.h>  // ADDED v5.6.35. For Multi MIDI Message. Thanks to @dnaldoog
using namespace juce; // ADDED v5.6.35. For Multi MIDI Message. Thanks to @dnaldoog

CtrlrPropertyComponent::CtrlrPropertyComponent (const Identifier &_propertyName,
								const ValueTree &_propertyElement,
								const ValueTree &_identifierDefinition,
								CtrlrPanel *_panel,
								StringArray *_possibleChoices,
								Array<var>  *_possibleValues)
	:	PropertyComponent (_propertyName.toString()),
		identifierDefinition(_identifierDefinition),
		propertyName(_propertyName),
		propertyElement(_propertyElement),
		panel(_panel),
		possibleChoices(_possibleChoices),
		possibleValues(_possibleValues)
{

//    if (propertyName == Ids::midiMessageCtrlrValue) // ADDED v5.6.35. For Multi MIDI Message. Thanks to @dnaldoog
//    {
//        setVisible(false); // this hides the midi message ctrlr value slider because @dnaldoog doesn't think it does anything
//        visibleText = identifierDefinition.isValid() ? identifierDefinition.getProperty("text").toString()
//        : propertyName.toString();
//        visibleText.clear();
//        propertyType = CtrlrIDManager::Numeric;
//        return;
//    }

    if (propertyName == Ids::midiMessageCtrlrNumber)
    {
        propertyElement.addListener(this);
    }
    
    if (!identifierDefinition.isValid())
	{
		addAndMakeVisible (new CtrlrUnknownPropertyComponent (propertyName, propertyElement, identifierDefinition));
		visibleText	 = propertyName.toString();
		propertyType = CtrlrIDManager::UnknownProperty;
	}
	else
	{
		addAndMakeVisible (getPropertyComponent());
	}
}

CtrlrPropertyComponent::~CtrlrPropertyComponent()
{
	deleteAllChildren();
}

const String CtrlrPropertyComponent::getPropertyName()
{
	return propertyName.toString();
}

const String CtrlrPropertyComponent::getVisibleText()
{
	return visibleText;
}
void CtrlrPropertyComponent::paint (Graphics &g) // Property ID/Description
{
	getLookAndFeel().drawPropertyComponentBackground (g, getLookAndFeel().getPropertyComponentContentPosition (*this).getX(), getHeight(), *this);
	if (isMouseOver(false) && !currentFont.isUnderlined())
	{
		currentFont.setUnderline(true);
	}
	else if (currentFont.isUnderlined())
	{
		currentFont.setUnderline(false);
	}
  
    g.setFont (currentFont);
	g.setColour (findColour(CtrlrPropertyComponent::labelTextColourId));
    g.drawFittedText (visibleText, 6, 0, getLookAndFeel().getPropertyComponentContentPosition (*this).getX()-12, getHeight(), Justification::centredLeft, 2, 1.0f);
}

void CtrlrPropertyComponent::resized()
{
	// currentFont.setHeight (jmin (getHeight(), 24) * 0.55f);

	if (getNumChildComponents() > 0)
	{
        getChildComponent (0)->setBounds (getLookAndFeel().getPropertyComponentContentPosition (*this));
	}
}

void CtrlrPropertyComponent::refresh()
{
	if (getNumChildComponents() > 0)
	{
		CtrlrPropertyChild *child = dynamic_cast<CtrlrPropertyChild*>(getChildComponent (0));
		if (child != nullptr)
		{
			child->refresh();
		}
	}
}
void CtrlrPropertyComponent::valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property) // ADDED v5.6.35. For Multi MIDI Message. Thanks to @dnaldoog
{
    // When midiMessageCtrlrNumberSize changes, refresh the midiMessageCtrlrNumber slider
    if (propertyName == Ids::midiMessageCtrlrNumber &&
        property == Ids::midiMessageCtrlrNumberSize)
    {
        // Remove old component
        deleteAllChildren();
        
        // Recreate with new max value
        addAndMakeVisible(getPropertyComponent());
        resized();
        repaint();
    }
}
Component *CtrlrPropertyComponent::getPropertyComponent()
{
	Value valueToControl = propertyElement.getPropertyAsValue (propertyName, panel ? panel->getUndoManager() : nullptr);

	if (panel)
	{
		if ((bool)panel->getProperty(Ids::panelPropertyDisplayIDs) == false)
		{
            visibleText = identifierDefinition.getProperty ("text").toString();
		}
		else
		{
            visibleText = propertyName.toString();
		}
	}
	else
	{
        visibleText = identifierDefinition.getProperty ("text").toString();
	}
    
	propertyType = CtrlrIDManager::stringToType(identifierDefinition.getProperty("type"));
    
    // Added v5.6.34. Thanks to @dnaldoog
    _DBG("Property Name: " + propertyName.toString() + " | XML Type: " + identifierDefinition.getProperty("type").toString() + " | Mapped Type: " + String(propertyType));
    if (propertyName == Ids::componentLayerUid)
    {
        possibleChoices = new StringArray();
        possibleValues = new Array<var>();

        if (panel != nullptr && panel->getCanvas() != nullptr)
        {
            CtrlrPanelCanvas* canvas = panel->getCanvas();
            for (int i = 0; i < canvas->getNumLayers(); i++)
            {
                CtrlrPanelCanvasLayer* layer = canvas->getLayerFromArray(i);
                if (layer != nullptr)
                {
                    possibleChoices->add(layer->getProperty(Ids::uiPanelCanvasLayerName).toString());
                    possibleValues->add(layer->getProperty(Ids::uiPanelCanvasLayerUid).toString());
                }
            }
        }
    }
    _DBG("CtrlrPropertyComponent::getPropertyComponent [POST] propertyType==" + String((int)propertyType) + " visibleText==" + visibleText);
    
    // END of addon
    
    int propertyLineheightBaseValue = 36; // Declare the variable outside the if-else block. Mandatory for Preference window property lines.
    bool propertyLineImprovedLegibility = false; // Declare the variable outside the if-else block. Mandatory for Preference window property lines.
    
    if (panel) // Added v5.5.33. Accessing to managerTree directly will crashes CtrlrX from Preferences window, checking panel will prevent that.
    {
        propertyLineheightBaseValue = panel->getOwner().getManagerTree().getProperty(Ids::ctrlrPropertyLineheightBaseValue, 36); // Added v5.6.33.
        propertyLineImprovedLegibility = panel->getOwner().getManagerTree().getProperty(Ids::ctrlrPropertyLineImprovedLegibility, false); // Added v5.6.34.
    }
    
    switch (propertyType)
	{
		case CtrlrIDManager::ReadOnly:
            // preferredHeight = 36;
            preferredHeight = roundDoubleToInt(propertyLineheightBaseValue * 1.0); // Updated v5.6.33.
			// return (new CtrlrTextPropertyComponent (valueToControl, 1024, false, true)); // valueToControl, maxNumChars, isMultiLine, isReadOnly
            // return (new CtrlrTextPropertyComponent (valueToControl, 1024, false, true, propertyLineImprovedLegibility)); // valueToControl, maxNumChars, isMultiLine, isReadOnly, propertyLineImprovedLegibility
            return (new CtrlrReadOnlyProperty(propertyName, propertyElement, identifierDefinition, panel)); // Updated v5.6.34. Thanks to @dnaldoog.
            
		case CtrlrIDManager::Text:
            // preferredHeight = 36;
            preferredHeight = roundDoubleToInt(propertyLineheightBaseValue * 1.0); // Updated v5.6.33.
			// return (new CtrlrTextPropertyComponent (valueToControl, 1024, false));
            return (new CtrlrTextPropertyComponent (valueToControl, 1024, false, false, propertyLineImprovedLegibility)); // valueToControl, maxNumChars, isMultiLine, isReadOnly, propertyLineImprovedLegibility

		case CtrlrIDManager::MultiLine:
			// preferredHeight = 96;
            preferredHeight = roundDoubleToInt(propertyLineheightBaseValue * 2.7); // Updated v5.6.33.
			// return (new CtrlrTextPropertyComponent (valueToControl, 8192*4, true));
            return (new CtrlrTextPropertyComponent (valueToControl, 8192*4, true, false, propertyLineImprovedLegibility)); // valueToControl, maxNumChars, isMultiLine, isReadOnly, propertyLineImprovedLegibility

		case CtrlrIDManager::MultiLineSmall:
			// preferredHeight = 64;
            preferredHeight = roundDoubleToInt(propertyLineheightBaseValue * 1.78); // Updated v5.6.33.
			// return (new CtrlrTextPropertyComponent (valueToControl, 8192, true));
            return (new CtrlrTextPropertyComponent (valueToControl, 8192, true, false, propertyLineImprovedLegibility)); // valueToControl, maxNumChars, isMultiLine, isReadOnly, propertyLineImprovedLegibility

		case CtrlrIDManager::Expression:
			// preferredHeight = 64;
            preferredHeight = roundDoubleToInt(propertyLineheightBaseValue * 1.78); // Updated v5.6.33.
			return (new CtrlrExpressionProperty (valueToControl));

		case CtrlrIDManager::Colour:
            // preferredHeight = 36;
            preferredHeight = roundDoubleToInt(propertyLineheightBaseValue * 1.0); // Updated v5.6.33.
			return (new CtrlrColourPropertyComponent (valueToControl));
            
		case CtrlrIDManager::Font:
            // preferredHeight = 36;
            preferredHeight = roundDoubleToInt(propertyLineheightBaseValue * 1.0); // Updated v5.6.33.
			return (new CtrlrFontPropertyComponent (valueToControl, panel));

		case CtrlrIDManager::Bool:
            // preferredHeight = 36;
            preferredHeight = roundDoubleToInt(propertyLineheightBaseValue * 1.0); // Updated v5.6.33.
			return (new CtrlrBooleanPropertyComponent(valueToControl, identifierDefinition.getProperty ("defaults")));
            
		case CtrlrIDManager::MultiMidi:
			// preferredHeight = 128;
            preferredHeight = roundDoubleToInt(propertyLineheightBaseValue * 3.56); // Updated v5.6.33.
			return (new CtrlrMultiMidiPropertyComponent(valueToControl));

		case CtrlrIDManager::SysEx:
			// preferredHeight = 36;
            preferredHeight = roundDoubleToInt(propertyLineheightBaseValue * 1.0); // Updated v5.6.33.
			return (new CtrlrSysExPropertyComponent(valueToControl, propertyElement, propertyName, panel));

		case CtrlrIDManager::LuaMethod:
            // preferredHeight = 36;
            preferredHeight = roundDoubleToInt(propertyLineheightBaseValue * 1.0); // Updated v5.6.33.
			return (new CtrlrLuaMethodProperty(valueToControl, propertyName, panel));
            
		case CtrlrIDManager::ActionButton:
            // preferredHeight = 36;
            preferredHeight = roundDoubleToInt(propertyLineheightBaseValue * 1.0); // Updated v5.6.33.
			return (new CtrlrButtonPropertyComponent(valueToControl, visibleText));
        
        case CtrlrIDManager::ActionButtonText: // Added v5.6.32
            // preferredHeight = 36;
            preferredHeight = roundDoubleToInt(propertyLineheightBaseValue * 1.0); // Updated v5.6.33.
            buttonText = identifierDefinition.getProperty ("buttonText").toString();
            return (new CtrlrButtonTextPropertyComponent(valueToControl, visibleText, buttonText));
            
		case CtrlrIDManager::Numeric:
            // preferredHeight = 36;
            preferredHeight = roundDoubleToInt(propertyLineheightBaseValue * 1.0); // Updated v5.6.33.
			// Special handling for midiMessageCtrlrNumber to make max dynamic
			if (propertyName == Ids::midiMessageCtrlrNumber)
			{
				bool is14Bit = propertyElement.getProperty(Ids::midiMessageCtrlrNumberSize, false);
				double maxValue = is14Bit ? 16383 : 127;
                return (new CtrlrSliderPropertyComponent(valueToControl, (double)identifierDefinition.getProperty("min", 0), maxValue, (double)identifierDefinition.getProperty("int", 1)));
			}
			return (new CtrlrSliderPropertyComponent(valueToControl, (double)identifierDefinition.getProperty ("min", 0), (double)identifierDefinition.getProperty ("max", 127), (double)identifierDefinition.getProperty ("int", 1)));
            
		case CtrlrIDManager::VarNumeric:
            // preferredHeight = 36;
            preferredHeight = roundDoubleToInt(propertyLineheightBaseValue * 1.0); // Updated v5.6.33.
			return (new CtrlrChoicePropertyComponent(valueToControl, possibleChoices, possibleValues, true));
            
		case CtrlrIDManager::VarText:
            // preferredHeight = 36;
            preferredHeight = roundDoubleToInt(propertyLineheightBaseValue * 1.0); // Updated v5.6.33.
			return (new CtrlrChoicePropertyComponent(valueToControl, possibleChoices, possibleValues, false));
            
		case CtrlrIDManager::FileProperty:
            // preferredHeight = 36;
            preferredHeight = roundDoubleToInt(propertyLineheightBaseValue * 1.0); // Updated v5.6.33.
			return (new CtrlrFileProperty (valueToControl));
            
		case CtrlrIDManager::Timestamp:
            // preferredHeight = 36;
            preferredHeight = roundDoubleToInt(propertyLineheightBaseValue * 1.0); // Updated v5.6.33.
			return (new CtrlrTimestampProperty (valueToControl));
            
		case CtrlrIDManager::ModulatorList:
            // preferredHeight = 36;
            preferredHeight = roundDoubleToInt(propertyLineheightBaseValue * 1.0); // Updated v5.6.33.
			return (new CtrlrModulatorListProperty (valueToControl, panel));

		default:
			break;
	}

	return (new CtrlrUnknownPropertyComponent (propertyName, propertyElement, identifierDefinition));
}

const String CtrlrPropertyComponent::getElementSubType()
{
	if (propertyElement.getType() == Ids::panel)
	{
		return ("");
	}
	else if (propertyElement.getType() == Ids::component)
	{
		return (propertyElement.getProperty("uiType"));
	}
	else if (propertyElement.getType() == Ids::modulator)
	{
		return ("");
	}
	else if (propertyElement.getType() == Ids::midi)
	{
		return ("midi");
	}
	else if (propertyElement.getType() == Ids::uiPanelEditor)
	{
		return ("");
	}
	else if (propertyElement.getType() == Ids::uiPanelCanvasLayer) // Updated v5.6.34. Thanks to @dnaldoog.
	{
	    // Debug: Print all properties of this element
        _DBG("Layer element properties:");
        for (int i = 0; i < propertyElement.getNumProperties(); i++)
        {
            auto name = propertyElement.getPropertyName(i);
            auto value = propertyElement.getProperty(name);
            _DBG("  " + name.toString() + " = " + value.toString());
        }

        // Then try to get the layer name
        String layerName = propertyElement.getProperty(Ids::uiPanelCanvasLayerName).toString();
        return layerName.isEmpty() ? "Unnamed Layer" : layerName;
	}
	else
	{
		return ("unknownSubType");
	}
}

const String CtrlrPropertyComponent::getElementType()
{
	if (propertyElement.getType() == Ids::midi)
	{
		return ("modulator");
	}
	else if (propertyElement.getType() == Ids::panel)
	{
		return ("");
	}
	else
	{
		return (propertyElement.getType().toString());
	}
}

// Constructor 1:  custom true/false text
CtrlrBooleanPropertyComponent::CtrlrBooleanPropertyComponent(const Value& _valueToControl, const juce::String& _trueText, const juce::String& _falseText)
	: valueToControl(_valueToControl),
	stateText(""),
	onText(_trueText),      // onText = "14-bit"
	offText(_falseText)     // offText = "7-bit"
{
	addAndMakeVisible(&button);
	button.addListener(this);
	button.setClickingTogglesState(false);
	button.setButtonText(offText);  // Start with "7-bit" when false
	button.setClickingTogglesState(true);
	button.setToggleState(valueToControl.getValue(), dontSendNotification);
}

// Constructor 2:  stateText
CtrlrBooleanPropertyComponent::CtrlrBooleanPropertyComponent (const Value& _valueToControl, const String& _stateText)
    : valueToControl(_valueToControl), stateText(_stateText)
{
	if (stateText.contains ("/"))
	{
		onText	= stateText.upToFirstOccurrenceOf ("/", false, false);
		offText	= stateText.fromFirstOccurrenceOf ("/", false, false);
	}
	else
	{
		onText = offText = stateText;
	}

    addAndMakeVisible (&button);
	button.addListener (this);
    button.setClickingTogglesState (false);
    button.setButtonText (offText);
    button.setClickingTogglesState (true);
	button.setToggleState (valueToControl.getValue(), dontSendNotification);
}

CtrlrBooleanPropertyComponent::~CtrlrBooleanPropertyComponent()
{
}

void CtrlrBooleanPropertyComponent::paint (Graphics& g)
{
    g.setColour (findColour(ComboBox::backgroundColourId));
    g.fillRect (button.getBounds());

    g.setColour (findColour (ComboBox::outlineColourId));
    g.drawRect (button.getBounds());
}

void CtrlrBooleanPropertyComponent::refresh()
{
	button.setToggleState (button.getToggleState(), dontSendNotification);
	button.setButtonText (button.getToggleState() ? onText : offText);
}

void CtrlrBooleanPropertyComponent::resized()
{
	button.setBounds (0, 0, getWidth(), getHeight());
}

void CtrlrBooleanPropertyComponent::buttonClicked (Button*)
{
	button.setButtonText (button.getToggleState() ? onText : offText);
	valueToControl = button.getToggleState();
}

CtrlrButtonPropertyComponent::CtrlrButtonPropertyComponent(const Value &_valueToControl, const String &_propertyName)
	: valueToControl(_valueToControl), propertyName(_propertyName)
{
	button.setButtonText (propertyName);
	button.addListener (this);
	addAndMakeVisible (&button);
}

CtrlrButtonPropertyComponent::~CtrlrButtonPropertyComponent()
{
}

void CtrlrButtonPropertyComponent::resized()
{
	button.setBounds (0, 0, getWidth(), getHeight());
}

void CtrlrButtonPropertyComponent::refresh()
{
}

void CtrlrButtonPropertyComponent::buttonClicked (Button *_button)
{
	valueToControl = true;
}

CtrlrButtonTextPropertyComponent::CtrlrButtonTextPropertyComponent(const Value &_valueToControl, const String &_propertyName, const String &_buttonText)
    : valueToControl(_valueToControl), propertyName(_propertyName), buttonText(_buttonText) // Added v5.6.32
{
    button.setButtonText (buttonText);
    button.addListener (this);
    addAndMakeVisible (&button);
}

CtrlrButtonTextPropertyComponent::~CtrlrButtonTextPropertyComponent() // Added v5.6.32
{
}

void CtrlrButtonTextPropertyComponent::resized() // Added v5.6.32
{
    button.setBounds (0, 0, getWidth(), getHeight());
}

void CtrlrButtonTextPropertyComponent::refresh() // Added v5.6.32
{
}

void CtrlrButtonTextPropertyComponent::buttonClicked (Button *_button) // Added v5.6.32
{
    valueToControl = true;
}

CtrlrChoicePropertyComponent::CtrlrChoicePropertyComponent (const Value &_valueToControl,
                                                            const StringArray *_choices,
                                                            const Array<var> *_values,
                                                            const bool _numeric)
    : valueToControl(_valueToControl),
      combo (nullptr),
	  numeric(_numeric)
{
    addAndMakeVisible (combo = new ComboBox (""));
    combo->setEditableText (false);
    combo->setJustificationType (Justification::centredLeft);
    combo->setTextWhenNothingSelected ("");
    combo->setTextWhenNoChoicesAvailable (L"(no choices)");
    combo->addListener (this);

	if (_choices != nullptr) // FIXED. Thanks to @dnaldoog. Was crashing APP when changing global LnF
	{
		choices = *_choices;

		for (int i=0; i<choices.size(); i++)
		{
			combo->addItem (choices[i], i+1);
		}

        if (choices.size() > 0)
            combo->setTextWhenNothingSelected (choices[0]);
	}

	if (_values != nullptr)
	{
		values = *_values;

		if (values.size() == 0)
		{
			for (int i=0; i<choices.size(); i++)
			{
				values.add (i);
			}
		}
	}
    setSize (256, 25);
	refresh();
}

CtrlrChoicePropertyComponent::~CtrlrChoicePropertyComponent() // Updated v5.6.34. Prevents crash on Windows when switching global LnF colourScheme
{
    if (combo != nullptr)
    {
        combo->removeListener(this);
    }
}

void CtrlrChoicePropertyComponent::resized()
{
	combo->setBounds (0, 0, getWidth(), getHeight());
}

void CtrlrChoicePropertyComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
	changed();
}

void CtrlrChoicePropertyComponent::refresh()
{
	if (numeric)
	{
		// Change this line to use `indexOf` with `var` to find the correct index
        const int i = values.indexOf(valueToControl.getValue());
		combo->setSelectedItemIndex (i, sendNotification);
	}
	else
	{
		combo->setText (valueToControl.toString(), sendNotification);
	}
}

void CtrlrChoicePropertyComponent::changed()
{
	if (numeric)
	{
		valueToControl = values[combo->getSelectedItemIndex()];
	}
	else
	{
		valueToControl = combo->getText();
	}
    
	// CRASH REPORT! According to @dnaldoog , this will cause a crash when user changes L&F in Property Editor
	//	if (owner)
	//    {
	//        sendChangeMessage ();
	//    }
}

/** CtrlrColourEditorComponent **/

CtrlrColourEditorComponent::CtrlrColourEditorComponent(ChangeListener* defaultListener)
    : canResetToDefault(true), colourPickerButton(std::make_unique<ColourPickerButton>("colourPicker"))
{
	addAndMakeVisible(&colourTextInput);
    colourTextInput.setJustificationType(juce::Justification::centred);
    colourTextInput.setFont(colourTextInput.getFont().withStyle(juce::Font::bold));
    colourTextInput.setEditable(true, false, false);
    colourTextInput.setAlwaysOnTop(true);
    colourTextInput.addListener(this);
    
    colourPickerButton->setTooltip("Choose custom colour");
    colourPickerButton->addListener(this);
    addAndMakeVisible(colourPickerButton.get());
    
    if (defaultListener)
        addChangeListener(defaultListener);
}

CtrlrColourEditorComponent::~CtrlrColourEditorComponent()
{
    // The ColourPickerButton now manages its own memory
   //  delete colourPickerButton;
}

void CtrlrColourEditorComponent::resized()
{
    if (colourPickerButton != nullptr)
    {
        const int buttonWidth = getHeight();
        colourPickerButton->setBounds(getWidth() - buttonWidth, 0, buttonWidth, getHeight());
        colourTextInput.setBounds(0, 0, getWidth() - buttonWidth - 2, getHeight());
    }
}

void CtrlrColourEditorComponent::lookAndFeelChanged()
{
    // Simply repaint the button to force it to redraw with the new colors
    if (colourPickerButton != nullptr)
        colourPickerButton->repaint();
}

void CtrlrColourEditorComponent::updateLabel()
{
    if (colourPickerButton != nullptr)
    {
        // Set the colors
        colourTextInput.setColour(juce::Label::backgroundColourId, getColour());
        colourTextInput.setColour(juce::Label::textColourId, getColour().contrasting().darker(0.25f));
        colourTextInput.setText(getColour().toDisplayString(true), juce::dontSendNotification);
        
        // Repaint the components to reflect the changes
        colourTextInput.repaint();
        colourPickerButton->repaint();
    }
}

void CtrlrColourEditorComponent::buttonClicked(juce::Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == colourPickerButton.get())
    {
        openColourPicker();
    }
}

// Added v5.6.34. Required extra class for the colour picker button to recover it's init state when clicking just outside the colour selector popup and not the button itself again.
void CtrlrColourEditorComponent::openColourPicker()
{
    // The color selector popup to display
    auto colourSelector = std::make_unique<juce::ColourSelector>();

    // Set its size and initial colour
    colourSelector->setSize (300, 400);
    colourSelector->setCurrentColour (getColour());
    
    // The component itself listens for changes from the selector
    colourSelector->addChangeListener (this);
    
    // Set the button's toggle state to true when the popup is launched.
    colourPickerButton->setToggleState (true, juce::dontSendNotification);

    // This is the correct way to launch the CallOutBox in JUCE 6.
    juce::CallOutBox::launchAsynchronously (std::move (colourSelector),
                                          colourPickerButton->getScreenBounds(),
                                          nullptr); // The parent component is not needed here.
}

void CtrlrColourEditorComponent::labelTextChanged(juce::Label* labelThatHasChanged)
{
    // This method is called when the user types in the text box.
    // It should parse the text and then call the same logic as the colour picker.
    setColour(juce::Colour::fromString(labelThatHasChanged->getText()), true);
}

void CtrlrColourEditorComponent::setColour(const juce::Colour& newColour, const bool sendChangeMessageNow)
{
    colour = newColour;
    updateLabel(); // This updates the visual appearance of the label and button
    
    // Now send the change message to listeners like the LayerListItem
    if (sendChangeMessageNow)
        sendChangeMessage();
}

void CtrlrColourEditorComponent::changeListenerCallback (juce::ChangeBroadcaster* source)
{
    // Check if the source of the change is the colour selector
    if (auto* cs = dynamic_cast<juce::ColourSelector*>(source))
    {
        // Update the component's internal colour and notify listeners
        setColour(cs->getCurrentColour(), true);

        // Reset the button's toggle state to false when the popup closes.
        if (colourPickerButton != nullptr)
        {
            colourPickerButton->setToggleState (false, juce::dontSendNotification);
        }
    }
}

/** CtrlrColourPropertyComponent **/

CtrlrColourPropertyComponent::CtrlrColourPropertyComponent (const Value &_valueToControl) : valueToControl(_valueToControl)
{
	addAndMakeVisible (&cs);
	cs.addChangeListener (this);
}

CtrlrColourPropertyComponent::~CtrlrColourPropertyComponent()
{
}

void CtrlrColourPropertyComponent::refresh()
{
	cs.setColour (Colour::fromString (valueToControl.toString()));
}

void CtrlrColourPropertyComponent::changeListenerCallback (ChangeBroadcaster* source)
{
	valueToControl = cs.getColour().toString();
}

void CtrlrColourPropertyComponent::resized()
{
	cs.setBounds (0, 0, getWidth(), getHeight());
}

/** A read-only component **/

CtrlrReadOnlyProperty::CtrlrReadOnlyProperty(const Identifier& _propertyName,
    const ValueTree& _propertyElement,
    const ValueTree& identifier,
    CtrlrPanel* _panel,
    StringArray* possibleChoices,
    StringArray* possibleValues)
    : propertyName(_propertyName), propertyElement(_propertyElement), panel(_panel)
{
    addAndMakeVisible(&value);

    value.setColour(Label::backgroundColourId, findColour(Slider::backgroundColourId).withAlpha(0.5f));
    value.setColour(Label::outlineColourId, findColour(Slider::textBoxTextColourId).withAlpha(0.5f));
}

CtrlrReadOnlyProperty::~CtrlrReadOnlyProperty()
{
}

void CtrlrReadOnlyProperty::refresh()
{
	// Get the value from the property element and convert it to a string.
	String displayValue = propertyElement.getPropertyAsValue(propertyName, 0).toString(); // Added v5.6.34. Thanks to @dnaldoog
	// Set the text of the 'value' Label.
	value.setText(displayValue, dontSendNotification); // Added v5.6.34. Thanks to @dnaldoog
}

void CtrlrReadOnlyProperty::resized()
{
	value.setBounds (0, 2, getWidth(), getHeight()-4);
}

CtrlrExpressionProperty::CtrlrExpressionProperty (const Value &_valeToControl) : valeToControl(_valeToControl), externalEditorWindow(nullptr)
{
    addAndMakeVisible (text = new TextEditor (""));
    text->setMultiLine (true, true);
    text->setReturnKeyStartsNewLine (false);
    text->setReadOnly (false);
    text->setScrollbarsShown (true);
    text->setCaretVisible (true);
    text->setPopupMenuEnabled (true);
    text->setText ("");
    text->setColour (TextEditor::backgroundColourId, findColour (TextEditor::backgroundColourId));
	text->setColour (TextEditor::textColourId, findColour (TextEditor::textColourId));
    text->setColour (TextEditor::outlineColourId, findColour (TextEditor::outlineColourId));
	text->setColour (TextEditor::highlightedTextColourId, findColour(TextEditor::highlightedTextColourId));
	text->setColour (TextEditor::highlightColourId, findColour(TextEditor::focusedOutlineColourId));

    addAndMakeVisible (apply = gui::createDrawableButton("Apply", BIN2STR(bug_svg)));
    apply->addListener (this);
	apply->setTooltip ("Compile expression, if it's valid set the property");
	apply->setMouseCursor (MouseCursor::PointingHandCursor);

	text->setFont (Font (Font::getDefaultMonospacedFontName(), 10.0f, Font::plain));
	text->setText (valeToControl.toString(), false);
	text->addListener (this);

    setSize (256, 48);
}

CtrlrExpressionProperty::~CtrlrExpressionProperty()
{
    deleteAndZero (text);
    deleteAndZero (apply);
}

void CtrlrExpressionProperty::resized()
{
	text->setBounds (0, 0, getWidth() - 24, getHeight() - 0);
    apply->setBounds (getWidth() - 24, 0, 24, getHeight()/2);
}

void CtrlrExpressionProperty::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == apply)
    {
		if (compile(true))
		{
			AlertWindow::showMessageBox (AlertWindow::InfoIcon, "Expression validation", "Expression is valid");
		}
    }
}

void CtrlrExpressionProperty::refresh()
{
	text->setText (valeToControl.toString(), false);
}

void CtrlrExpressionProperty::textEditorTextChanged (TextEditor &editor)
{
}

void CtrlrExpressionProperty::textEditorReturnKeyPressed (TextEditor &editor)
{
	compile (true);
}

void CtrlrExpressionProperty::textEditorFocusLost (TextEditor &editor)
{
	compile (true);
}

const bool CtrlrExpressionProperty::compile(const bool setPropertyIfValid)
{
	String parseError;
	Expression e = Expression(text->getText(), parseError);

	if (!parseError.isEmpty())
	{
		text->setColour (TextEditor::backgroundColourId, Colours::deeppink);
		AlertWindow::showMessageBox (AlertWindow::WarningIcon, "Expression validation", "Validation failed: "+parseError, "OK", this);
		return (false);
	}

	text->setColour (TextEditor::backgroundColourId, findColour(TextEditor::backgroundColourId));

	if (setPropertyIfValid)
	{
		valeToControl = text->getText();
	}

	return (true);
}

CtrlrFileProperty::CtrlrFileProperty (const Value &_valeToControl) : valueToControl(_valeToControl)
{
    addAndMakeVisible (path = new Label (""));
	path->setText (valueToControl.toString(), dontSendNotification);
	path->addListener (this);
	path->setColour (Label::backgroundColourId, findColour(ComboBox::backgroundColourId));
	path->setColour (Label::outlineColourId, findColour (ComboBox::outlineColourId));

    addAndMakeVisible (browse = new TextButton ("Browse", "Browse"));
    browse->addListener (this);
	// browse->setConnectedEdges (TextButton::ConnectedOnLeft);
    browse->setColour (TextButton::buttonColourId, findColour(TextButton::buttonColourId)); // Added v5.6.32
    browse->setColour (TextButton::textColourOffId, findColour(TextButton::textColourOffId)); // Added v5.6.32
    browse->setSize(64, 48);
    setSize (256, 48);
}

CtrlrFileProperty::~CtrlrFileProperty()
{
    deleteAndZero (path);
    deleteAndZero (browse);
}

void CtrlrFileProperty::resized()
{
	browse->setBounds (0, 0, 54, getHeight()); // Updated v5.6.32
    path->setBounds (54, 0, getWidth() - 54, getHeight()); // Updated v5.6.32
}

void CtrlrFileProperty::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == browse)
    {
		FileChooser myChooser ("Select a file",
								File::getSpecialLocation (File::userHomeDirectory),
								"*.*",
								false);

        if (myChooser.browseForFileToOpen())
        {
			valueToControl = myChooser.getResult().getFullPathName();
			path->setText (valueToControl.toString(), dontSendNotification);
        }
    }
}

void CtrlrFileProperty::refresh()
{
	path->setText (valueToControl.toString(), dontSendNotification);
}

void CtrlrFileProperty::labelTextChanged (Label* labelThatHasChanged)
{
	valueToControl = labelThatHasChanged->getText();
}

CtrlrFontPropertyComponent::CtrlrFontPropertyComponent (const Value &_valueToControl, CtrlrPanel *_owner)
    : valueToControl(_valueToControl), owner(_owner),
      typeface (0),
      fontBold (0),
      fontItalic (0),
      fontUnderline (0),
      fontSizeComboBox (0),
	  kerningComboBox(0),
	  horizontalScaleComboBox(0)
{
    addAndMakeVisible (typeface = new ComboBox (""));
    typeface->setEditableText (false);
    typeface->setJustificationType (Justification::centredLeft);
    typeface->setTextWhenNothingSelected (L"<Sans-Serif>");
    typeface->setTextWhenNoChoicesAvailable (L"<Sans-Serif>");
    typeface->addListener (this);

    addAndMakeVisible (fontBold = gui::createDrawableButton("Bold", BIN2STR(bold_svg)));
    fontBold->setTooltip (L"Bold");
    fontBold->addListener (this);

    addAndMakeVisible (fontItalic = gui::createDrawableButton("Italic", BIN2STR(italic_svg)));
    fontItalic->setTooltip (L"Italic");
    fontItalic->addListener (this);

    addAndMakeVisible (fontUnderline = gui::createDrawableButton("Underline", BIN2STR(underline_svg)));
    fontUnderline->setTooltip (L"Underline");
    fontUnderline->addListener (this);

	// In your constructor, after creating the sliders:
	addAndMakeVisible(fontSizeLabel = new Label("", "Size"));
	fontSizeLabel->setFont(Font(10.0f, Font::plain));
	fontSizeLabel->setJustificationType(Justification::centred);
	fontSizeLabel->setColour(Label::textColourId, findColour(Label::textColourId));

	 // Create and add the new ComboBox for font size
    addAndMakeVisible(fontSizeComboBox = new ComboBox(""));
    fontSizeComboBox->setEditableText(true); // Allow custom values
	fontSizeComboBox->setTooltip (L"Font size");
    fontSizeComboBox->addListener(this);

    const int sizes[] = { 8, 9, 10, 12, 14, 18, 24, 30, 36, 48, 60, 72};
    for (int size : sizes)
    {
        fontSizeComboBox->addItem(String(size), size);
    }
	
	fontSizeComboBox->setSelectedId(12); // Default font value.
	
	addAndMakeVisible(horizontalScaleLabel = new Label("", "Scale"));
	horizontalScaleLabel->setFont(Font(10.0f, Font::plain));
	horizontalScaleLabel->setJustificationType(Justification::centred);
	horizontalScaleLabel->setColour(Label::textColourId, findColour(Label::textColourId));

	addAndMakeVisible(kerningLabel = new Label("", "Kerning"));
	kerningLabel->setFont(Font(10.0f, Font::plain));
	kerningLabel->setJustificationType(Justification::centred);
	kerningLabel->setColour(Label::textColourId, findColour(Label::textColourId));

	addAndMakeVisible(horizontalScaleLabel = new Label("", "Scale"));
    horizontalScaleLabel->setFont(Font(10.0f, Font::plain));
    horizontalScaleLabel->setJustificationType(Justification::centred);
    horizontalScaleLabel->setColour(Label::textColourId, findColour(Label::textColourId));

    addAndMakeVisible(horizontalScaleComboBox = new ComboBox(""));
    horizontalScaleComboBox->setEditableText(true); // Allow custom values
    horizontalScaleComboBox->addListener(this);
	
    // Populate the ComboBox with common horizontal scale values
    const float scaleValues[] = {0.50f, 0.75f, 0.85f, 0.90f, 1.00f, 1.10f, 1.25f, 1.50f, 2.00f};
	int nextId = 1; // Also required for kerning comboBox
    for (float value : scaleValues)
    {
        horizontalScaleComboBox->addItem(String(value, 2), nextId++);
    }

    addAndMakeVisible(kerningLabel = new Label("", "Kerning"));
    kerningLabel->setFont(Font(10.0f, Font::plain));
    kerningLabel->setJustificationType(Justification::centred);
    kerningLabel->setColour(Label::textColourId, findColour(Label::textColourId));

    addAndMakeVisible(kerningComboBox = new ComboBox(""));
    kerningComboBox->setEditableText(true); // Allow custom values
	kerningComboBox->setTooltip (L"Extra kerning");
    kerningComboBox->addListener(this);

	// Populate the ComboBox with common kerning values
	const float kerningValues[] = {0.00f, 0.05f, 0.10f, 0.15f, 0.20f, 0.25f, 0.30f, 0.40f, 0.50f, 0.75f, 1.00f};
	for (float value : kerningValues)
	{
		kerningComboBox->addItem(String(value, 2), nextId++);
	}

	// Set a default value for the ComboBox
	kerningComboBox->setSelectedId(1); // 1 corresponds to the first item: 0.00

	fontBold->setClickingTogglesState (true);
	fontBold->setMouseCursor (MouseCursor::PointingHandCursor);
	fontItalic->setClickingTogglesState (true);
	fontItalic->setMouseCursor (MouseCursor::PointingHandCursor);
	fontUnderline->setClickingTogglesState (true);
	fontUnderline->setMouseCursor (MouseCursor::PointingHandCursor);

    owner->getCtrlrManagerOwner().getFontManager().fillCombo (*typeface, true, true, true, true);

    setSize (300, 32);
}

CtrlrFontPropertyComponent::~CtrlrFontPropertyComponent()
{
    // Remove listeners first to avoid dangling pointers
    typeface->removeListener (this);
    fontBold->removeListener (this);
    fontItalic->removeListener (this);
    fontUnderline->removeListener (this);
	
    // Remove listener for the new ComboBox
    if (kerningComboBox) {
        kerningComboBox->removeListener(this);
    }
	
    // Remove listener for the new ComboBox
    if (horizontalScaleComboBox) {
        horizontalScaleComboBox->removeListener(this);
    }
    
    // Remove listener for the new ComboBox
    if (fontSizeComboBox) {
        fontSizeComboBox->removeListener(this);
    }

    // Then delete the components
    deleteAndZero (typeface);
    deleteAndZero (fontBold);
    deleteAndZero (fontItalic);
    deleteAndZero (fontUnderline);
    deleteAndZero (fontSizeComboBox);
    deleteAndZero (kerningComboBox);
    deleteAndZero (horizontalScaleComboBox);

    // The labels don't have listeners so they are fine to delete
    deleteAndZero(fontSizeLabel);
    deleteAndZero(horizontalScaleLabel);
    deleteAndZero(kerningLabel);
}

void CtrlrFontPropertyComponent::resized()
{
    // Re-using the logic from your provided code
    const int labelHeight = 12;
    const int sliderHeight = getHeight() - labelHeight;
    const int totalWidth = getWidth();
    
    // Define the widths for each section.
    const float typefaceWidth = 0.4f;
    const float buttonWidth = 0.05f;
    const float remainingWidth = 1.0f - typefaceWidth - (buttonWidth * 3);
    const float comboBoxWidth = remainingWidth / 3.0f;

    // Typeface ComboBox
    typeface->setBounds(0, labelHeight, totalWidth * typefaceWidth, sliderHeight);

    // Font Style Buttons
    fontBold->setBounds(totalWidth * typefaceWidth, labelHeight, totalWidth * buttonWidth, sliderHeight);
    fontItalic->setBounds(totalWidth * typefaceWidth + (totalWidth * buttonWidth), labelHeight, totalWidth * buttonWidth, sliderHeight);
    fontUnderline->setBounds(totalWidth * typefaceWidth + 2 * (totalWidth * buttonWidth), labelHeight, totalWidth * buttonWidth, sliderHeight);
    
	// Calculate the starting X position for the three ComboBoxes
    int startX = totalWidth * typefaceWidth + 3 * (totalWidth * buttonWidth);
    
    // Font Size ComboBox
    fontSizeLabel->setBounds(startX, 0, totalWidth * comboBoxWidth, labelHeight);
    fontSizeComboBox->setBounds(startX, labelHeight, totalWidth * comboBoxWidth, sliderHeight);
    
    startX += totalWidth * comboBoxWidth;
    
    // Horizontal Scale ComboBox
    horizontalScaleLabel->setBounds(startX, 0, totalWidth * comboBoxWidth, labelHeight);
    horizontalScaleComboBox->setBounds(startX, labelHeight, totalWidth * comboBoxWidth, sliderHeight);
    
    startX += totalWidth * comboBoxWidth;
    
    // Kerning ComboBox
    kerningLabel->setBounds(startX, 0, totalWidth * comboBoxWidth, labelHeight);
    kerningComboBox->setBounds(startX, labelHeight, totalWidth * comboBoxWidth, sliderHeight);
}

void CtrlrFontPropertyComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
	valueToControl = owner->getCtrlrManagerOwner().getFontManager().getStringFromFont(getFont());
}

void CtrlrFontPropertyComponent::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == fontBold || buttonThatWasClicked == fontItalic || buttonThatWasClicked == fontUnderline)
    {
		valueToControl = owner->getCtrlrManagerOwner().getFontManager().getStringFromFont(getFont());
	}
}

void CtrlrFontPropertyComponent::sliderValueChanged (Slider* sliderThatWasMoved)
{
	valueToControl = owner->getCtrlrManagerOwner().getFontManager().getStringFromFont(getFont());
}

void CtrlrFontPropertyComponent::refresh()
{
	Font font = owner->getCtrlrManagerOwner().getFontManager().getFontFromString(valueToControl.toString());
	typeface->setText (font.getTypefaceName(), sendNotification);
	
	fontSizeComboBox->setText(String(font.getHeight()), dontSendNotification); // This is the updated line to use the new ComboBox
    kerningComboBox->setText(String(font.getExtraKerningFactor(), 2), dontSendNotification); // This is the updated line to use the new ComboBox
    horizontalScaleComboBox->setText(String(font.getHorizontalScale(), 2), dontSendNotification); // This is the updated line to use the new ComboBox

	fontBold->setToggleState (font.isBold(), sendNotification);
	fontItalic->setToggleState (font.isItalic(), sendNotification);
	fontUnderline->setToggleState (font.isUnderlined(), sendNotification);
}

Font CtrlrFontPropertyComponent::getFont()
{
    Font font;

    if (typeface)
        font.setTypefaceName (typeface->getText());
    else
        return (font);

    // Get the font size from the new ComboBox
    float newFontSize = 10.0f; // A default value in case of invalid input
    if (fontSizeComboBox)
    {
        // Get the text from the editable ComboBox and convert it to a float.
        newFontSize = (float) fontSizeComboBox->getText().getFloatValue();
        // If the conversion fails (e.g., text is not a number), use a default.
        if (newFontSize <= 0.0f)
            newFontSize = 10.0f;
    }
    font.setHeight (newFontSize);
    
	// Get the kerning value from the new ComboBox
	float newKerningValue = 0.0f;
	if (kerningComboBox)
	{
		newKerningValue = kerningComboBox->getText().getFloatValue();
	}
	font.setExtraKerningFactor(newKerningValue);
	
	// Get the horizontal scale value from the new ComboBox
    float newHorizontalScaleValue = 1.0f;
    if (horizontalScaleComboBox)
    {
        newHorizontalScaleValue = horizontalScaleComboBox->getText().getFloatValue();
    }
    font.setHorizontalScale(newHorizontalScaleValue);
	
    font.setBold (fontBold->getToggleState());
    font.setItalic (fontItalic->getToggleState());
    font.setUnderline (fontUnderline->getToggleState());
    
    return font;
}

CtrlrLuaMethodProperty::CtrlrLuaMethodProperty (const Value &_valeToControl, const Identifier &_id, CtrlrPanel *_owner)
    : valeToControl(_valeToControl), owner(_owner), id(_id),
      methodSelectorCombo (0),
      editMethodButton (0),
      newMethodButton (0),
      deleteMethodButton (0)
{
    addAndMakeVisible (methodSelectorCombo = new ComboBox (L"methodSelectorCombo"));
    methodSelectorCombo->setEditableText (false);
    methodSelectorCombo->setJustificationType (Justification::centredLeft);
    methodSelectorCombo->setTextWhenNothingSelected ("");
    methodSelectorCombo->setTextWhenNoChoicesAvailable (L"(no choices)");
    methodSelectorCombo->addListener (this);

    addAndMakeVisible (editMethodButton = gui::createDrawableButton("Edit Metod", BIN2STR(edit_svg)));
    editMethodButton->setTooltip (L"Edit selected method");
    editMethodButton->setButtonText (L"new button");
    editMethodButton->addListener (this);

    addAndMakeVisible (newMethodButton = gui::createDrawableButton("New Method", BIN2STR(file_svg)));
    newMethodButton->setTooltip (L"Add new method");
    newMethodButton->setButtonText (L"new button");
    newMethodButton->addListener (this);

    addAndMakeVisible (deleteMethodButton = gui::createDrawableButton("Delete Method", BIN2STR(clear_svg)));
    deleteMethodButton->setTooltip (L"Remove selected method");
    deleteMethodButton->setButtonText (L"new button");
    deleteMethodButton->addListener (this);

	editMethodButton->setMouseCursor(MouseCursor::PointingHandCursor);
	newMethodButton->setMouseCursor(MouseCursor::PointingHandCursor);
	deleteMethodButton->setMouseCursor(MouseCursor::PointingHandCursor);

    setSize (200, 25);
}

CtrlrLuaMethodProperty::~CtrlrLuaMethodProperty()
{
    deleteAndZero (methodSelectorCombo);
    deleteAndZero (editMethodButton);
    deleteAndZero (newMethodButton);
    deleteAndZero (deleteMethodButton);
}


void CtrlrLuaMethodProperty::resized()
{
    methodSelectorCombo->setBounds ((48) + (24), 0, getWidth() - 72, getHeight() - 0);
    editMethodButton->setBounds (0, 0, 24, getHeight() - 0);
    newMethodButton->setBounds (24, 0, 24, getHeight() - 0);
    deleteMethodButton->setBounds (48, 0, 24, getHeight() - 0);
}

void CtrlrLuaMethodProperty::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == methodSelectorCombo)
    {
		valeToControl = methodSelectorCombo->getText();
    }
}

void CtrlrLuaMethodProperty::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == editMethodButton)
    {
		if (methodSelectorCombo->getText() == "" || methodSelectorCombo->getText() == COMBO_NONE_ITEM)
		{
			return;
		}
		if (owner)
		{
			owner->getPanelWindowManager().show (CtrlrPanelWindowManager::LuaMethodEditor);
			CtrlrLuaMethodEditor *ed = dynamic_cast<CtrlrLuaMethodEditor*>(owner->getPanelWindowManager().getContent(CtrlrPanelWindowManager::LuaMethodEditor));
			if (ed != nullptr)
			{
				ed->setEditedMethod (methodSelectorCombo->getText());
			}
		}
    }
    else if (buttonThatWasClicked == newMethodButton)
    {
		AlertWindow w("Method name", "New method name", AlertWindow::QuestionIcon, this);
		w.addTextEditor ("methodName", "myMethod", "Method", false);
		w.addButton ("OK", 1, KeyPress(KeyPress::returnKey));
		w.addButton ("Cancel", 0, KeyPress(KeyPress::escapeKey));
		if (w.runModalLoop())
		{
			if (owner)
			{
				owner->getCtrlrLuaManager().getMethodManager().addMethod (ValueTree(), w.getTextEditorContents("methodName"), "", id.toString());
			}
		}

		refresh();
    }
    else if (buttonThatWasClicked == deleteMethodButton)
    {
		if (owner)
		{
			owner->getCtrlrLuaManager().getMethodManager().deleteMethod(methodSelectorCombo->getText());
		}

		refresh();
    }
}

void CtrlrLuaMethodProperty::refresh()
{
	if (owner == 0)
		return;
	methodSelectorCombo->clear();
	methodSelectorCombo->addItem (COMBO_NONE_ITEM, 1);
	methodSelectorCombo->addItemList (owner->getCtrlrLuaManager().getMethodManager().getMethodList(), 2);
	methodSelectorCombo->setText (valeToControl.toString(), sendNotification);
}

CtrlrModulatorListProperty::CtrlrModulatorListProperty(const Value &_valueToControl, CtrlrPanel *_owner)
	: owner(_owner), valueToControl(_valueToControl)
{
	addAndMakeVisible (combo = new ComboBox (""));
    combo->setEditableText (false);
    combo->setJustificationType (Justification::centredLeft);
    combo->setTextWhenNothingSelected (COMBO_ITEM_NONE);
    combo->setTextWhenNoChoicesAvailable ("No modulators");
    combo->addListener (this);

	listChanged();
}

CtrlrModulatorListProperty::~CtrlrModulatorListProperty()
{
}

void CtrlrModulatorListProperty::resized()
{
	combo->setBounds (0, 0, getWidth(), getHeight());
}

void CtrlrModulatorListProperty::refresh()
{
	if (choices.contains (valueToControl.toString()))
	{
		combo->setColour (ComboBox::textColourId, Colours::black);
	}
	else
	{
		combo->setColour (ComboBox::textColourId, Colours::red);
	}

	combo->setText (valueToControl.toString(), sendNotification);
}

void CtrlrModulatorListProperty::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
	valueToControl = combo->getText();
}

void CtrlrModulatorListProperty::listChanged()
{
	choices.clear();
	choices.add (COMBO_ITEM_NONE);
	combo->addItem (COMBO_ITEM_NONE, 1);

	for (int i=0; i<owner->getModulators().size(); i++)
	{
		choices.add (owner->getModulatorByIndex(i)->getName());
		combo->addItem (owner->getModulatorByIndex(i)->getName(), i+2);
	}

	const String storedModulatorName = valueToControl.toString();

	if (choices.contains (storedModulatorName))
	{
		combo->setColour (ComboBox::textColourId, findColour(ComboBox::textColourId));
		combo->setText (storedModulatorName, sendNotification);
	}
	else
	{
		combo->setColour (ComboBox::textColourId, Colours::red);
		combo->setText (storedModulatorName, dontSendNotification);
	}
}

void CtrlrModulatorListProperty::modulatorChanged (CtrlrModulator *modulatorThatChanged)
{
}

void CtrlrModulatorListProperty::modulatorAdded (CtrlrModulator *modulatorThatWasAdded)
{
	listChanged();
}

void CtrlrModulatorListProperty::modulatorRemoved (CtrlrModulator *modulatorRemoved)
{
	listChanged();
}


// UDPATED v5.6.35. CtrlrMultiMidiPropertyComponent. Thanks to @dnaldoog

CtrlrMultiMidiPropertyComponent::CtrlrMultiMidiPropertyComponent(const Value& _valueToControl)
	: valueToControl(_valueToControl),
	addMulti(0),
	removeMulti(0),
	listMulti(0),
	copy(0),
	paste(0),
	helpMmidi(0)
{
	// Create Add button
	auto addIcon = SvgIconManager::getDrawable(IconType::UlBars, *this);
	addMulti = new juce::DrawableButton("Add Multi", juce::DrawableButton::ImageFitted);
	addMulti->setImages(addIcon.release());
	addAndMakeVisible(addMulti);
	addMulti->setTooltip(L"Add message");
	addMulti->addListener(this);
	addMulti->setMouseCursor(MouseCursor::PointingHandCursor);

	// Create Remove button
	removeMulti = gui::createDrawableButton("Remove", BIN2STR(clear_svg));
	addAndMakeVisible(removeMulti);
	removeMulti->setTooltip(L"Remove selected message");
	removeMulti->addListener(this);
	removeMulti->setMouseCursor(MouseCursor::PointingHandCursor);

	// Create ListBox
	listMulti = new ListBox("list", this);
	addAndMakeVisible(listMulti);
	listMulti->setRowHeight(14);

	// Create Copy button
	copy = gui::createDrawableButton("Copy", BIN2STR(copy_svg));
	addAndMakeVisible(copy);
	copy->setTooltip(L"Copy to clipboard");
	copy->addListener(this);
	copy->setMouseCursor(MouseCursor::PointingHandCursor);

	// Create Paste button
	paste = gui::createDrawableButton("Paste", BIN2STR(paste_svg));
	addAndMakeVisible(paste);
	paste->setTooltip(L"Paste from clipboard");
	paste->addListener(this);
	paste->setMouseCursor(MouseCursor::PointingHandCursor);

	// Create Help button
	auto helpIcon = SvgIconManager::getDrawable(IconType::SolidQuest, *this);
	helpMmidi = new juce::DrawableButton("Help", juce::DrawableButton::ImageFitted);
	helpMmidi->setImages(helpIcon.release());
	addAndMakeVisible(helpMmidi);
	helpMmidi->setTooltip(L"Click to see Multi MIDI message syntax");
	helpMmidi->addListener(this);
	helpMmidi->setMouseCursor(MouseCursor::PointingHandCursor);

	loadAdditionalTemplates(File());
	setSize(256, 96);

	// Initialize button icons for current look and feel
	updateButtonIcons();
}

CtrlrMultiMidiPropertyComponent::~CtrlrMultiMidiPropertyComponent()
{
	deleteAndZero(addMulti);
	deleteAndZero(removeMulti);
	deleteAndZero(listMulti);
	deleteAndZero(copy);
	deleteAndZero(paste);
	deleteAndZero(helpMmidi);
}

void CtrlrMultiMidiPropertyComponent::lookAndFeelChanged()
{
	Component::lookAndFeelChanged();
	updateButtonIcons();

	if (listMulti)
		listMulti->repaint();
}

void CtrlrMultiMidiPropertyComponent::paint(Graphics& g)
{
	// Use look-and-feel colors instead of hardcoded ones
	auto bgColour = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
	auto lighterBg = bgColour.brighter(0.1f);
	auto darkerBg = bgColour.darker(0.2f);

	g.setGradientFill(ColourGradient(lighterBg,
		(float)((getWidth() / 2)), 0.0f,
		bgColour,
		(float)((getWidth() / 2)), 32.0f,
		false));
	g.fillRect(0, 0, getWidth(), 32);

	g.setGradientFill(ColourGradient(bgColour,
		(float)((getWidth() / 2)), 29.0f,
		darkerBg,
		(float)((getWidth() / 2)), 32.0f,
		false));
	g.fillRect(0, 29, getWidth(), 3);
}

void CtrlrMultiMidiPropertyComponent::resized()
{
	addMulti->setBounds(8, 4, 24, 24);
	helpMmidi->setBounds(40, 4, 24, 24);
	removeMulti->setBounds(72, 4, 24, 24);
	copy->setBounds((getWidth() - 32) + -32, 4, 24, 24);
	paste->setBounds(getWidth() - 32, 4, 24, 24);
	listMulti->setBounds(0, 32, getWidth() - 0, getHeight() - 32);
}

void CtrlrMultiMidiPropertyComponent::paintListBoxItem(int rowNumber, Graphics &g, int width, int height, bool rowIsSelected)
{
	// Just paint the background, no text (Labels handle text rendering)
	if (rowIsSelected)
	{
		g.fillAll(getLookAndFeel().findColour(juce::ListBox::backgroundColourId).contrasting(0.2f));
	}
	else
	{
		g.fillAll(getLookAndFeel().findColour(juce::ListBox::backgroundColourId));
	}

	// DON'T draw text here - the Label components do that
}

void CtrlrMultiMidiPropertyComponent::buttonClicked(Button* buttonThatWasClicked)
{
	if (buttonThatWasClicked == helpMmidi)
	{
		CtrlrSysexProcessor::showMidiHelp();
	}
	else if (buttonThatWasClicked == addMulti)
	{
		PopupMenu m;

		// Add XML templates dynamically
		StringArray templateKeys = templates.getAllKeys();
		for (int i = 0; i < templateKeys.size(); ++i)
			m.addItem(i + 1, templateKeys[i]);

		m.addSeparator();

		// Standard MIDI types
		struct StandardType
		{
			const char* name;
			const char* defaultCsv;
		};

		const StandardType standardTypes[] = {
			{ "CC",              "CC,-2,-1" },
			{ "Program Change",  "ProgramChange,-1" },
			{ "SysEx",           "SysEx,F0 00 xx F7" },
			{ "NRPN",            "CC,ByteValue,MSB7bitValue,99,-2:CC,ByteValue,LSB7bitValue,98,-2:CC,ByteValue,MSB7bitValue,6,-1:CC,ByteValue,LSB7bitValue,38,-1" },
			{ "NRPN (Korg)",     "CC,ByteValue,MSB7bitValue,99,-2:CC,ByteValue,LSB7bitValue,98,-2:CC,ByteValue,MSB7bitValue,6,-1" },
			{ "RPN",             "CC,ByteValue,MSB7bitValue,101,-2:CC,ByteValue,LSB7bitValue,100,-2:CC,ByteValue,MSB7bitValue,6,-1:CC,ByteValue,LSB7bitValue,38,-1" },
			{ "NRPN Null",       "CC,ByteValue,LSB7bitValue,101,127:CC,ByteValue,LSB7bitValue,100,127" }
		};

		int standardStartId = templateKeys.size() + 1;
		for (int i = 0; i < numElementsInArray(standardTypes); ++i)
		{
			if (i % 3 == 0)
				m.addSeparator();
			m.addItem(standardStartId + i, standardTypes[i].name);
		}
		m.addSeparator();

		// Custom editor
		int customId = standardStartId + numElementsInArray(standardTypes);
		m.addItem(customId, "Custom...");

		// Show popup and handle selection
		int ret = m.show();

		if (ret <= 0)
			return; // cancelled

		if (ret == customId) // Custom editor
		{
			CtrlrSysexProcessor sysexProcessor;
			String newCsv = sysexProcessor.openAdvancedMessageEditor();

			if (newCsv.isNotEmpty())
			{
				String currentValue = valueToControl.toString();
				if (currentValue.isNotEmpty())
					valueToControl = currentValue + ":" + newCsv;
				else
					valueToControl = newCsv;
				refresh();
			}
		}
		else if (ret <= templateKeys.size()) // XML template
		{
			String data = templates.getValue(templateKeys[ret - 1], "");
			if (data.isNotEmpty())
			{
				String currentValue = valueToControl.toString();
				if (currentValue.isNotEmpty())
					valueToControl = currentValue + ":" + data;
				else
					valueToControl = data;
				refresh();
			}
		}
		else // Standard MIDI type
		{
			int index = ret - standardStartId;
			if (index >= 0 && index < numElementsInArray(standardTypes))
			{
				String currentValue = valueToControl.toString();
				if (currentValue.isNotEmpty())
					valueToControl = currentValue + ":" + standardTypes[index].defaultCsv;
				else
					valueToControl = standardTypes[index].defaultCsv;
				refresh();
			}
		}
	}
	else if (buttonThatWasClicked == removeMulti)
	{
		int selectedRow = listMulti->getSelectedRow();
		if (selectedRow >= 0)
		{
			StringArray temp;
			temp.addTokens(valueToControl.toString().trim(), ":", "\"\'");
			if (selectedRow < temp.size())
			{
				temp.remove(selectedRow);
				valueToControl = temp.joinIntoString(":");
				refresh();
			}
		}
	}
	else if (buttonThatWasClicked == copy)
	{
		SystemClipboard::copyTextToClipboard(values.joinIntoString(":"));
	}
	else if (buttonThatWasClicked == paste)
	{
		valueToControl = SystemClipboard::getTextFromClipboard();
		refresh();
	}
}

void CtrlrMultiMidiPropertyComponent::updateButtonIcons()
{
	auto bgColour = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
	bool isDarkTheme = bgColour.getBrightness() < 0.5f;
	auto iconColour = isDarkTheme ? juce::Colours::white : juce::Colours::black;

	// Update Add button
	if (addMulti)
	{
		auto icon = SvgIconManager::getDrawable(IconType::UlBars, *this);
		if (icon)
		{
			icon->replaceColour(juce::Colours::black, iconColour);
			addMulti->setImages(icon.get());
		}
	}

	// Update Help button
	if (helpMmidi)
	{
		auto icon = SvgIconManager::getDrawable(IconType::SolidQuest, *this);
		if (icon)
		{
			icon->replaceColour(juce::Colours::black, iconColour);
			helpMmidi->setImages(icon.get());
		}
	}

	// Update Remove button (BIN2STR)
	if (removeMulti)
	{
		String svgData = String(BIN2STR(clear_svg));
		auto icon = juce::Drawable::createFromImageData(svgData.toRawUTF8(), svgData.length());
		if (icon)
		{
			icon->replaceColour(juce::Colours::black, iconColour);
			removeMulti->setImages(icon.get());
		}
	}

	// Update Copy button (BIN2STR)
	if (copy)
	{
		String svgData = String(BIN2STR(copy_svg));
		auto icon = juce::Drawable::createFromImageData(svgData.toRawUTF8(), svgData.length());
		if (icon)
		{
			icon->replaceColour(juce::Colours::black, iconColour);
			copy->setImages(icon.get());
		}
	}

	// Update Paste button (BIN2STR)
	if (paste)
	{
		String svgData = String(BIN2STR(paste_svg));
		auto icon = juce::Drawable::createFromImageData(svgData.toRawUTF8(), svgData.length());
		if (icon)
		{
			icon->replaceColour(juce::Colours::black, iconColour);
			paste->setImages(icon.get());
		}
	}
}

Component* CtrlrMultiMidiPropertyComponent::refreshComponentForRow(int rowNumber, bool isRowSelected,
	Component* existingComponentToUpdate)
{
	Label* l = (Label*)existingComponentToUpdate;

	if (l == 0)
	{
		l = new Label("", values[rowNumber]);
		l->setEditable(false, true, false);
		l->setColour(Label::backgroundColourId, Colours::transparentBlack);
		l->getProperties().set("dOb", rowNumber);
		l->addListener(this);
		l->addMouseListener(static_cast<juce::Component*>(this), false);
	}
	else
	{
		l->getProperties().set("dOb", rowNumber);
		l->setText(values[rowNumber], dontSendNotification);
		l->addMouseListener(static_cast<juce::Component*>(this), false);
	}

	return l;
}

void CtrlrMultiMidiPropertyComponent::mouseDown(const MouseEvent &e)
{
	Label* l = dynamic_cast<Label*>(e.eventComponent);
	if (l)
	{
		const int id = l->getProperties().getWithDefault("dOb", -1);
		listMulti->selectRow(id, true, true);
	}
}

void CtrlrMultiMidiPropertyComponent::mouseDoubleClick(const MouseEvent& e)
{
	Label* l = dynamic_cast<Label*>(e.eventComponent);
	if (l)
	{
		const int id = l->getProperties().getWithDefault("dOb", -1);
		listMulti->selectRow(id, true, true);
	}
}

int CtrlrMultiMidiPropertyComponent::getNumRows()
{
	return (values.size());
}

void CtrlrMultiMidiPropertyComponent::refresh()
{
	values.clear();
	values.addTokens (valueToControl.toString().trim(), ":", "\"\'");
	listMulti->updateContent();
	listMulti->repaint();
}

void CtrlrMultiMidiPropertyComponent::loadAdditionalTemplates(const File &templateFile)
{
	XmlDocument staticTemplates(MemoryBlock (BinaryData::CtrlrMidiMultiTemplate_xml, BinaryData::CtrlrMidiMultiTemplate_xmlSize).toString());
	XmlDocument dynamicTemplates(templateFile);

	ScopedPointer <XmlElement> dynamicXml(dynamicTemplates.getDocumentElement().release());
	ScopedPointer <XmlElement> staticXml(staticTemplates.getDocumentElement().release());

	if (dynamicXml)
	{
		forEachXmlChildElement (*dynamicXml, t)
		{
			if (t->hasTagName("template"))
			{
				templates.set (t->getStringAttribute ("name"), t->getAllSubText().trim());
			}
		}
	}

	if (staticXml)
	{
		forEachXmlChildElement (*staticXml, t)
		{
			if (t->hasTagName("template"))
			{
				templates.set (t->getStringAttribute ("name"), t->getAllSubText().trim());
			}
		}
	}
}

void CtrlrMultiMidiPropertyComponent::labelTextChanged (Label* l)
{
	const int id = l->getProperties().getWithDefault("dOb", -1);

	if (id >= 0)
	{
		values.set (id, l->getText());
		valueToControl = values.joinIntoString (":");
	}
}

//==============================================================================
CtrlrSliderPropertyComponent::CtrlrSliderPropertyComponent (const Value &_valueToControl, double rangeMin, double rangeMax, double interval)
	: valueToControl(_valueToControl)
{
    addAndMakeVisible (&slider);
    slider.setRange (rangeMin, rangeMax, interval);
    slider.setSliderStyle (Slider::LinearBar);
    slider.getValueObject().referTo (valueToControl);
}

CtrlrSliderPropertyComponent::~CtrlrSliderPropertyComponent()
{
}

void CtrlrSliderPropertyComponent::setValue (const double newValue)
{
}

double CtrlrSliderPropertyComponent::getValue() const
{
    return slider.getValue();
}

void CtrlrSliderPropertyComponent::refresh()
{
	slider.setValue (getValue(), dontSendNotification);
}

void CtrlrSliderPropertyComponent::sliderValueChanged (Slider *sliderThatChanged)
{
    if (getValue() != slider.getValue())
        setValue (slider.getValue());
}

void CtrlrSliderPropertyComponent::resized()
{
	slider.setBounds (0, 0, getWidth(), getHeight());
}

//==============================================================================
// CtrlrSysExEditor Constructor
CtrlrSysExEditor::CtrlrSysExEditor(Value &_val, CtrlrPanel *_owner)
	: val(_val),
	  messageLength(nullptr),
	  label(nullptr),
	  addTokenButton(nullptr),
	  owner(_owner),
	  lastFocusedLabel(nullptr)
{
	// Slider for message length
	addAndMakeVisible(messageLength = new Slider("messageLength"));
	messageLength->setRange(0, 512, 1);
	messageLength->setSliderStyle(Slider::IncDecButtons);
	messageLength->setTextBoxStyle(Slider::TextBoxLeft, false, 32, 20);
	messageLength->addListener(this);

	// Length label
	addAndMakeVisible(label = new Label("Length", "Length"));
	label->setFont(Font(14.0f, Font::bold));
	label->setJustificationType(Justification::centred);
	label->setEditable(true, true, true);
	label->addListener(this);

	// Add Token button
	addAndMakeVisible(addTokenButton = new TextButton("Add Token"));
addTokenButton->onClick = [this]()
{
	if (byteValueLabels.size() == 0)
	{
		// Show a warning dialog if there are no labels
		AlertWindow::showMessageBoxAsync(
			AlertWindow::WarningIcon,
			"Add Token",
			"No bytes available. Please add a value first before inserting a token."
		);
		return;
	}

	// Determine target label
	Label* target = lastFocusedLabel;
	if (!target)
		target = byteValueLabels[0]; // default to first label

	// Highlight the focused label
	updateLabelHighlights(target);

	// Show token menu
	showTokenMenuForLabel(target);

	// Keep the highlight after popup closes
	lastFocusedLabel = target;
};

	// Split initial value into labels
	splitMessage.addTokens(val.toString(), " :;", "\'\"");
	setLength(splitMessage.size());

	setSize(612, 256);
}

//==============================================================================
// Destructor
CtrlrSysExEditor::~CtrlrSysExEditor()
{
	deleteAndZero(messageLength);
	deleteAndZero(label);
	deleteAndZero(addTokenButton);
}

//==============================================================================
// Paint
void CtrlrSysExEditor::paint(Graphics &g)
{
	Colour backGroundColor = findColour(TextEditor::backgroundColourId);
	Colour lightBackGroundColor = findColour(TextEditor::outlineColourId);
	g.fillAll(backGroundColor);

	g.setGradientFill(ColourGradient(backGroundColor, getWidth() / 2.0f, 0.0f,
									 lightBackGroundColor, getWidth() / 2.0f, 32.0f, false));
	g.fillRect(0, 0, getWidth(), 32);

	g.setGradientFill(ColourGradient(Colour(0xffd6d6d6), getWidth() / 2.0f, 29.0f,
									 Colour(0xff767676), getWidth() / 2.0f, 32.0f, false));
	g.fillRect(0, 29, getWidth(), 3);
}

//==============================================================================
// Resized
void CtrlrSysExEditor::resized()
{
	messageLength->setBounds(72, 4, 88, 22);
	label->setBounds(8, 8, 55, 16);
	addTokenButton->setBounds(170, 4, 80, 22);

	int y;
	int x = 0;
	for (int i = 0; i < byteValueLabels.size(); i++)
	{
		y = 48 + ((i / 16) * 48);
		byteValueLabels[i]->setBounds(16 + (x * 36), y, 32, 32);
		x++;
		if (x == 16) x = 0;
	}

	for (int i = 0; i < rows.size(); i++)
	{
		rows[i]->setBounds(16, 40 + (i * 48), getWidth() - 32, 8);
	}
}

//==============================================================================
// Slider listener
void CtrlrSysExEditor::sliderValueChanged(Slider* sliderThatWasMoved)
{
	if (sliderThatWasMoved == messageLength)
		setLength((int)messageLength->getValue());
}

//==============================================================================
// Add a byte label
Label* CtrlrSysExEditor::addByte(const String &byteAsString)
{
	Label* byteLabel = new Label("byteLabel", byteAsString);
	addAndMakeVisible(byteLabel);

	byteLabel->setFont(Font(Font::getDefaultMonospacedFontName(), 15.0f, Font::plain));
	byteLabel->setJustificationType(Justification::centred);
	byteLabel->setEditable(true, true, false);

	// Fully visible colors
	byteLabel->setColour(Label::backgroundColourId, Colours::white);
	byteLabel->setColour(Label::outlineColourId, Colours::black);
	byteLabel->setColour(Label::textColourId, Colours::black);
	byteLabel->setColour(TextEditor::highlightColourId, Colours::skyblue);
	byteLabel->setColour(TextEditor::highlightedTextColourId, Colours::white);

	byteLabel->addListener(this);
	byteLabel->addMouseListener(this, false);

	return byteLabel;
}


//==============================================================================
// Label focus tracking
void CtrlrSysExEditor::labelTextChanged(Label* labelThatHasChanged)
{
	sendChangeMessage();
}

//==============================================================================
// Mouse down for token menu
void CtrlrSysExEditor::mouseDown(const MouseEvent& e)
{
	if (auto* l = dynamic_cast<Label*>(e.originalComponent))
	{
		lastFocusedLabel = l; // store focus for Add Token button
		updateLabelHighlights(l); // highlight clicked label
	}
}


//==============================================================================
void CtrlrSysExEditor::showTokenMenuForLabel(Label* l)
{
	if (!l) return;

	PopupMenu m;

	// --- Variables ---
	m.addSectionHeader("Insert variable");
	m.addItem(1, "MIDI Channel (7bits)");
	m.addItem(2, "MIDI Channel (4bits)");
	m.addItem(3, "LSB part of value (7bits)");
	m.addItem(4, "MSB part of value (7bits)");
	m.addItem(5, "LSB part of value (4bits)");
	m.addItem(6, "MSB part of value (4bits)");
	m.addItem(7, "Roland JV1010 upper byte");
	m.addItem(8, "Roland JV1010 upper middle byte");
	m.addItem(9, "Roland JV1010 lower middle byte");
	m.addItem(10, "Roland JV1010 lower byte");

	// --- 16-bit nibbles ---
	m.addSectionHeader("16-bit value nibbles");
	for (int i = 0; i < 4; ++i)
		m.addItem(19 + i, "16-bit LSB nibble " + String(i));
	for (int i = 0; i < 4; ++i)
		m.addItem(23 + i, "16-bit MSB nibble " + String(i));

	// --- Static items ---
	m.addSectionHeader("Insert static");
	m.addItem(11, "SysEx Start");
	m.addItem(12, "SysEx EOM");
	m.addSubMenu("Vendor ID", getVendorIdMenu());

	// --- Program variables ---
	m.addSectionHeader("Program variables");
	m.addItem(8192, "Current program number");
	m.addItem(8193, "Current bank number");

	// --- Checksums ---
	m.addSectionHeader("Checksums (tN)");
	m.addItem(13, "2's Complement (Roland, Yamaha)");
	m.addItem(14, "Exclusive OR (XOR) (Akai, Korg, Sequential)");
	m.addItem(15, "Simple Summing (Waldorf, Lexicon, Oberheim)");
	m.addItem(16, "XOR Byte 1 (Technics)");
	m.addItem(17, "1's Complement (E-mu, Korg)");
	m.addItem(18, "Ignore this byte on input");

	// --- Global variables ---
	PopupMenu km, lm, mm, nm;
	for (int i = 0; i < 16; ++i)
	{
		km.addItem(20+i, "Global variable [k" + String::toHexString(i) + "]");
		lm.addItem(37+i, "Global variable [o" + String::toHexString(i) + "]");
		mm.addItem(53+i, "Global variable [p" + String::toHexString(i) + "]");
		nm.addItem(69+i, "Global variable [n" + String::toHexString(i) + "]");
	}
	m.addSubMenu("Global variable[0]", km);
	m.addSubMenu("Global variable[1]", lm);
	m.addSubMenu("Global variable[2]", mm);
	m.addSubMenu("Global variable[3]", nm);

	// --- Show menu ---
	const int ret = m.show();

	// --- Handle selection ---
	if      (ret == 1) l->setText("yy", sendNotification);
	else if (ret == 2) l->setText("0y", sendNotification);
	else if (ret == 3) l->setText("LS", sendNotification);
	else if (ret == 4) l->setText("MS", sendNotification);
	else if (ret == 5) l->setText("ls", sendNotification);
	else if (ret == 6) l->setText("ms", sendNotification);
	else if (ret == 7) l->setText("r1", sendNotification);
	else if (ret == 8) l->setText("r2", sendNotification);
	else if (ret == 9) l->setText("r3", sendNotification);
	else if (ret == 10) l->setText("r4", sendNotification);
	else if (ret == 11) l->setText("f0", sendNotification);
	else if (ret == 12) l->setText("f7", sendNotification);
	else if (ret == 13) l->setText("z5", sendNotification);
	else if (ret == 14) l->setText("X5", sendNotification);
	else if (ret == 15) l->setText("w5", sendNotification);
	else if (ret == 16) l->setText("tc", sendNotification);
	else if (ret == 17) l->setText("O5", sendNotification);
	else if (ret == 18) l->setText("ii", sendNotification);
	else if (ret >= 19 && ret < 27) l->setText("q" + String(ret-19), sendNotification);
	else if (ret >= 27 && ret < 43) l->setText("k" + String::toHexString(ret-27), sendNotification);
	else if (ret >= 43 && ret < 59) l->setText("o" + String::toHexString(ret-43), sendNotification);
	else if (ret >= 59 && ret < 75) l->setText("p" + String::toHexString(ret-59), sendNotification);
	else if (ret >= 75 && ret < 91) l->setText("n" + String::toHexString(ret-75), sendNotification);
	else if (ret > 1024 && ret < 4096)
	{
		ValueTree vendor = owner->getCtrlrManagerOwner().getIDManager().getVendorTree().getChild(ret-1024);
		const String vendorId = vendor.getProperty(Ids::id).toString();
		if (vendorId.length() == 2) l->setText(vendorId, sendNotification);
		else if (vendorId.length() == 6)
		{
			int index = byteValueLabels.indexOf(l);
			if (byteValueLabels[index+1] && byteValueLabels[index+2])
			{
				byteValueLabels[index]->setText(vendorId.substring(0,2), dontSendNotification);
				byteValueLabels[index+1]->setText(vendorId.substring(2,4), dontSendNotification);
				byteValueLabels[index+2]->setText(vendorId.substring(4,6), dontSendNotification);
			}
		}
	}
	else if (ret == 8192) l->setText("tp", sendNotification);
	else if (ret == 8193) l->setText("tb", sendNotification);
}

//==============================================================================
// Vendor menu
const PopupMenu CtrlrSysExEditor::getVendorIdMenu()
{
    PopupMenu m;
    ValueTree vendorTree = owner->getCtrlrManagerOwner().getIDManager().getVendorTree();

    for (int i=0; i<vendorTree.getNumChildren(); i++)
	{
		m.addItem (1024+i, vendorTree.getChild(i).getProperty(Ids::name));
	}
    return (m);
}

//==============================================================================
// Set length
void CtrlrSysExEditor::setLength (const int newLength)
{
	currentMessageLength = newLength;
	messageLength->setValue (currentMessageLength, dontSendNotification);

	if (byteValueLabels.size() < currentMessageLength)
	{
		for (int i=byteValueLabels.size(); i<currentMessageLength; i++)
			byteValueLabels.add(addByte(splitMessage[i]));
	}
	else if (byteValueLabels.size() > currentMessageLength)
	{
		byteValueLabels.removeLast(byteValueLabels.size() - currentMessageLength);
	}

	rows.clear();
	for (int i=0; i<=byteValueLabels.size()/16; i++)
	{
		SysExRow *r = new SysExRow(i);
		addAndMakeVisible(r);
		rows.add (r);
	}

	resized();

	sendChangeMessage();
}
void CtrlrSysExEditor::updateLabelHighlights(Label* focusedLabel)
{
	for (auto* label : byteValueLabels)
	{
		if (label == focusedLabel)
			label->setColour(Label::backgroundColourId, Colours::lightblue);
		else
			label->setColour(Label::backgroundColourId, Colours::white);
	}
}
//==============================================================================
// Get value
const String CtrlrSysExEditor::getValue()
{
	String ret;
	for (int i=0; i<byteValueLabels.size(); i++)
		ret << byteValueLabels[i]->getText() + " ";
	return ret.trim();
}

//==============================================================================
// SysExRow
SysExRow::SysExRow(const int _n, const int _w, const int _gap) : n(_n), w(_w), gap(_gap) {}
void SysExRow::paint(Graphics &g)
{
	g.setFont (Font (Font::getDefaultMonospacedFontName(), 8.0f, Font::plain));
	g.setColour (findColour(TextEditor::textColourId));
	for (int i=0; i<16; i++)
	{
		g.drawFittedText (String ((16*n)+(i+1)), i*(w+gap), 0, w, 8, Justification::centred, 1);
	}

	g.fillRect (0,getHeight()-1,getWidth(),1);
}

void SysExRow::resized()
{
}

CtrlrSysExFormulaEditor::CtrlrSysExFormulaEditor ()
    : forwardFormula (0),
      reverseFormula (0),
      forwardLabel (0),
      reverseLabel (0),
      label (0)
{
    addAndMakeVisible (forwardFormula = new CodeEditorComponent (forwardFormulaDocument, 0));

    addAndMakeVisible (reverseFormula = new CodeEditorComponent (reverseFormulaDocument, 0));

    addAndMakeVisible (forwardLabel = new Label (L"forwardLabel",
                                                 L"Forward"));
    forwardLabel->setFont (Font (16.0000f, Font::plain));
    forwardLabel->setJustificationType (Justification::centredLeft);
    forwardLabel->setEditable (false, false, false);
    forwardLabel->setColour (TextEditor::textColourId, findColour(TextEditor::textColourId));
    forwardLabel->setColour (TextEditor::backgroundColourId, findColour(TextEditor::backgroundColourId));

    addAndMakeVisible (reverseLabel = new Label (L"reverseLabel",
                                                 L"Reverse"));
    reverseLabel->setFont (Font (16.0000f, Font::plain));
    reverseLabel->setJustificationType (Justification::centredLeft);
    reverseLabel->setEditable (false, false, false);
    reverseLabel->setColour (TextEditor::textColourId, findColour(TextEditor::textColourId));
    reverseLabel->setColour (TextEditor::backgroundColourId, findColour(TextEditor::backgroundColourId));

    addAndMakeVisible (label = new Label (L"new label",
                                          L"SysEx Formula ()"));
    label->setFont (Font (24.0000f, Font::plain));
    label->setJustificationType (Justification::centred);
    label->setEditable (false, false, false);
	label->setColour(TextEditor::textColourId, findColour(TextEditor::textColourId));
	label->setColour(TextEditor::backgroundColourId, findColour(TextEditor::backgroundColourId));


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

CtrlrSysExFormulaEditor::~CtrlrSysExFormulaEditor()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    deleteAndZero (forwardFormula);
    deleteAndZero (reverseFormula);
    deleteAndZero (forwardLabel);
    deleteAndZero (reverseLabel);
    deleteAndZero (label);


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void CtrlrSysExFormulaEditor::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void CtrlrSysExFormulaEditor::resized()
{
    forwardFormula->setBounds (0, (32) + (16), getWidth() - 0, proportionOfHeight (0.4200f));
    reverseFormula->setBounds (0, (((32) + (16)) + (proportionOfHeight (0.4200f))) + (16), getWidth() - 0, proportionOfHeight (0.4200f));
    forwardLabel->setBounds (0, 32, getWidth() - 0, 16);
    reverseLabel->setBounds (0, ((32) + (16)) + (proportionOfHeight (0.4200f)), getWidth() - 0, 16);
    label->setBounds (0, 0, getWidth() - 0, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]

CtrlrSysExPropertyComponent::CtrlrSysExPropertyComponent (const Value &_valueToControl, const ValueTree &_propertyTree, const Identifier &_propertyName, CtrlrPanel *_owner)
    : valueToControl(_valueToControl),
      propertyTree(_propertyTree),
      propertyName(_propertyName),
      sysexPreview (0),
      editButton (0),
      copy (0),
      paste (0),
      owner(_owner)
{
    addAndMakeVisible (sysexPreview = new Label (L"sysexPreview",
                                                 L"F0 00 F7"));
    sysexPreview->setFont (Font (Font::getDefaultMonospacedFontName(), 12.0000f, Font::plain));
    sysexPreview->setJustificationType (Justification::centredLeft);
    sysexPreview->setEditable (true, false, false);
    sysexPreview->addListener (this);
	sysexPreview->setColour (Label::backgroundColourId, findColour(ComboBox::backgroundColourId));
	sysexPreview->setColour (Label::outlineColourId, findColour (ComboBox::outlineColourId));
	sysexPreview->setColour (TextEditor::highlightColourId, findColour(TextEditor::focusedOutlineColourId));

    addAndMakeVisible (editButton = new TextButton (L"editButton"));
    editButton->setButtonText (L"Edit");
    editButton->addListener (this);

    addAndMakeVisible (copy = gui::createDrawableButton("Copy", BIN2STR(copy_svg)));
    copy->setTooltip (L"Copy to clipboard");
    copy->addListener (this);

    addAndMakeVisible (paste = gui::createDrawableButton("Paste", BIN2STR(paste_svg)));
    paste->setTooltip (L"Paste from clipboard");
    paste->addListener (this);

	copy->setMouseCursor(MouseCursor::PointingHandCursor);
	paste->setMouseCursor(MouseCursor::PointingHandCursor);
    setSize (200, 25);
	refresh();
}

CtrlrSysExPropertyComponent::~CtrlrSysExPropertyComponent()
{
    deleteAndZero (sysexPreview);
    deleteAndZero (editButton);
    deleteAndZero (copy);
    deleteAndZero (paste);
}

void CtrlrSysExPropertyComponent::resized()
{
    editButton->setBounds (0, 0, 48, 24);
    copy->setBounds (52, 0, 24, 24);
    paste->setBounds (80, 0, 24, 24);
	sysexPreview->setBounds (108, 0, getWidth()-108, getHeight());
}

void CtrlrSysExPropertyComponent::labelTextChanged (Label* labelThatHasChanged)
{
    if (labelThatHasChanged == sysexPreview)
    {
		valueToControl = sysexPreview->getText();
    }
}

void CtrlrSysExPropertyComponent::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == editButton)
    {
    	DialogWindow::LaunchOptions o;

    	CtrlrSysExEditor *editor 		= new CtrlrSysExEditor (valueToControl, owner);
    	editor->addChangeListener (this);

    	if (propertyTree.hasType (Ids::midi) && propertyTree.getParent().hasType (Ids::modulator))
			o.dialogTitle 					= STR (owner->getName() + "/" + propertyTree.getParent().getProperty(Ids::name).toString() + "/" + propertyName.toString());
		else
			o.dialogTitle					= STR (owner->getName() + "/" + "SysEx editor");

		o.content.set (editor, true);
		o.resizable						= true;
		o.useNativeTitleBar				= true;
		o.dialogBackgroundColour 		= findColour(TextEditor::backgroundColourId);
		o.escapeKeyTriggersCloseButton	= true;
		o.componentToCentreAround		= this;
		o.launchAsync();
	}
	else if (buttonThatWasClicked == copy)
	{
		SystemClipboard::copyTextToClipboard (valueToControl.toString());
    }
    else if (buttonThatWasClicked == paste)
    {
		const String v = SystemClipboard::getTextFromClipboard();
		if (v.containsOnly("0123456789abcdefABCDEFxyXYlsLSMmkKrzi :;"))
		{
			valueToControl = v;
			refresh();
		}
    }
}

void CtrlrSysExPropertyComponent::changeListenerCallback (ChangeBroadcaster* source)
{
	if (auto* editor = dynamic_cast<CtrlrSysExEditor*>(source))
	{
		valueToControl = editor->getValue();
		sysexPreview->setText (valueToControl.toString(), dontSendNotification);
	}
}

void CtrlrSysExPropertyComponent::refresh()
{
	sysexPreview->setText (valueToControl.toString(), dontSendNotification);
}

class CtrlrTextPropLabel  : public Label  // Text Box for Type In Properties such as Panel Name etc
{
public:
    CtrlrTextPropLabel (CtrlrTextPropertyComponent& owner_,
                        const int maxChars_,
                        const bool isMultiline_,
                        const bool useImprovedLegibility) // Updated v5.6.34. useImprovedLegibility member added
        : Label ("", ""),
          owner (owner_),
          maxChars (maxChars_),
          isMultiline (isMultiline_),
          useImprovedLegibility (useImprovedLegibility) // Initialize from the parameter
    {
        setEditable (true, true, false); // Default: editable (CtrlrTextPropertyComponent will set to false if read-only)

        if (useImprovedLegibility)
        {
            setColour(juce::Label::backgroundColourId, juce::Colour(0xfffffefa)); // halfwhite
            setColour(juce::Label::textColourId, juce::Colour(0xff000000)); // black
        }
        else
        {
            setColour (Label::backgroundColourId, findColour(Slider::backgroundColourId));
            setColour(Label::textColourId, findColour(Slider::textBoxTextColourId));
        }

        setColour(Label::outlineColourId, findColour (Slider::textBoxOutlineColourId));
        setColour(Label::backgroundWhenEditingColourId, findColour(Slider::backgroundColourId).withAlpha(0.7f));
        setColour(Label::textWhenEditingColourId, findColour(Label::textWhenEditingColourId).withAlpha(0.7f));
        setColour(Label::outlineWhenEditingColourId, findColour(Slider::textBoxOutlineColourId));
    }

    TextEditor* createEditorComponent()
    {
        TextEditor* const textEditor = Label::createEditorComponent();
        textEditor->setInputRestrictions (maxChars);
        textEditor->setJustification(juce::Justification::centredLeft);

        if (useImprovedLegibility) // Uses the member variable
        {
			textEditor->setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xffffffff)); // white
            textEditor->setColour(juce::TextEditor::textColourId, juce::Colour(0xff000000)); // black
            textEditor->setColour(juce::TextEditor::highlightColourId, juce::Colour(0xffF5F5F5)); //  CSS WhiteSmoke. Added v5.6.34
            textEditor->setColour(juce::TextEditor::highlightedTextColourId, juce::Colour(0xff000000)); // black. Added v5.6.34
        }
        else
        {
            textEditor->setColour(juce::TextEditor::backgroundColourId, findColour(juce::Slider::backgroundColourId));
            textEditor->setColour(juce::TextEditor::textColourId, findColour(juce::Slider::textBoxTextColourId));
            textEditor->setColour(juce::TextEditor::highlightColourId, findColour(juce::TextEditor::highlightColourId));
            textEditor->setColour(juce::TextEditor::highlightedTextColourId, findColour(juce::TextEditor::highlightColourId));
            textEditor->setColour(juce::TextEditor::outlineColourId, findColour(juce::Slider::textBoxOutlineColourId));
        }

        if (isMultiline)
        {
            textEditor->setMultiLine (true, true);
            textEditor->setReturnKeyStartsNewLine (true);
            textEditor->setJustification(juce::Justification::topLeft);
            // textEditor->setIndents(0, 0);
        }
        return textEditor;
    }

    void textWasEdited()
    {
        owner.textWasEdited();
    }

private:
    CtrlrTextPropertyComponent& owner;
    int maxChars;
    bool isMultiline;
    bool useImprovedLegibility;
};

//==============================================================================
CtrlrTextPropertyComponent::CtrlrTextPropertyComponent (const Value& _valueToControl,
														const int maxNumChars,
														const bool isMultiLine,
                                                        const bool isReadOnly,
                                                        const bool useImprovedLegibility) // Updated v5.6.34. useImprovedLegibility added
    : valueToControl(_valueToControl),
      isReadOnly (isReadOnly),
      useImprovedLegibility (useImprovedLegibility) // Updated v5.6.34. useImprovedLegibility parameter added
{
    createEditor (maxNumChars, isMultiLine);
    textEditor->getTextValue().referTo (valueToControl);

	if (isReadOnly)
	{
        textEditor->setColour (Label::backgroundColourId, findColour(Label::backgroundColourId));
        textEditor->setColour (Label::textColourId, findColour(Label::textColourId));
        textEditor->setEditable (false, false, false);
	}
}

CtrlrTextPropertyComponent::~CtrlrTextPropertyComponent()
{
}

void CtrlrTextPropertyComponent::setText (const String& newText)
{
    textEditor->setText (newText, sendNotification);
}

String CtrlrTextPropertyComponent::getText() const
{
    return textEditor->getText();
}

void CtrlrTextPropertyComponent::createEditor (const int maxNumChars, const bool isMultiLine)
{
    addAndMakeVisible (textEditor = new CtrlrTextPropLabel (*this, maxNumChars, isMultiLine, useImprovedLegibility)); // Updated v5.6.34. useImprovedLegibility arg added

    if (isMultiLine)
    {
        textEditor->setJustificationType (Justification::topLeft);
    }
}

void CtrlrTextPropertyComponent::resized()
{
	if (textEditor)
		textEditor->setBounds (0,0,getWidth(),getHeight());
}

void CtrlrTextPropertyComponent::textWasEdited()
{
    const String newText (textEditor->getText());

    if (getText() != newText)
        setText (newText);
}

void CtrlrTextPropertyComponent::refresh()
{
	if (textEditor)
		textEditor->setText (valueToControl.toString(), dontSendNotification);
}

CtrlrTimestampProperty::CtrlrTimestampProperty (const Value& _valueToControl) : valueToControl(_valueToControl)
{
	addAndMakeVisible (textEditor = new Label ());
	textEditor->setColour (Label::backgroundColourId, Colours::white.withAlpha(0.2f));
}

CtrlrTimestampProperty::~CtrlrTimestampProperty()
{
}

void CtrlrTimestampProperty::refresh()
{
	if (textEditor)
	{
		textEditor->setText (Time ((int64)valueToControl.getValue()).formatted("%Y-%m-%d %H:%M:%S"), dontSendNotification);
	}
}

void CtrlrTimestampProperty::resized()
{
	if (textEditor)
		textEditor->setBounds (0,0,getWidth(),getHeight());
}


CtrlrUnknownPropertyComponent::CtrlrUnknownPropertyComponent(const Identifier &_propertyName,
															const ValueTree &_propertyElement,
															const ValueTree &identifier,
															CtrlrPanel *panel,
															StringArray *possibleChoices,
															StringArray *possibleValues) : propertyName(_propertyName), propertyElement(_propertyElement)
{
	l.setColour (Label::backgroundColourId, findColour(Label::backgroundColourId));
	l.setColour (Label::textColourId, Colours::red.brighter());
	l.setText (propertyElement.getProperty(propertyName), dontSendNotification);
	addAndMakeVisible (&l);
}

CtrlrUnknownPropertyComponent::~CtrlrUnknownPropertyComponent()
{
}

void CtrlrUnknownPropertyComponent::resized()
{
	l.setBounds (0, 0, getWidth(), getHeight());
}

void CtrlrUnknownPropertyComponent::refresh()
{
	l.setText (propertyElement.getPropertyAsValue(propertyName, 0).toString(), dontSendNotification);
}
