#ifndef __CTRLR_PROPERTY_COMPONENT__
#define __CTRLR_PROPERTY_COMPONENT__

#include "CtrlrIDManager.h"
#include "CtrlrPanel/CtrlrPanel.h"
#include "CtrlrPanel/CtrlrPanelCanvas.h"
#include "CtrlrPanel/CtrlrPanelCanvasLayer.h"
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "Ctrlrlog.h"

#include <functional>

class CtrlrFloatingWindow;

class CtrlrPropertyChild: public ChangeBroadcaster
{
	public:
		virtual ~CtrlrPropertyChild() {}
		virtual void refresh()=0;
};

class CtrlrPropertyComponent  : public PropertyComponent
{
	public:
		CtrlrPropertyComponent (const Identifier &_propertyName,
								const ValueTree &_propertyElement,
								const ValueTree &_identifierDefinition,
								CtrlrPanel *_panel=nullptr,
								StringArray *_possibleChoices=nullptr,
								Array<var>  *_possibleValues=nullptr);

		virtual ~CtrlrPropertyComponent();
		int getPreferredHeight() const noexcept                 { return preferredHeight; }
		void setPreferredHeight (int newHeight) noexcept        { preferredHeight = newHeight; }
		Component *getPropertyComponent();
		void resized();
		void paint (Graphics &g);
		void refresh();
		const String getPropertyName();
		const String getVisibleText();
		const String getElementSubType();
		const String getElementType();

	private:
	    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CtrlrPropertyComponent);
		String visibleText;
        String buttonText;
		ValueTree propertyElement;
		Identifier propertyName;
		ValueTree identifierDefinition;
		Font currentFont;
		CtrlrIDManager::PropertyType propertyType;
		CtrlrPanel *panel;
		StringArray *possibleChoices;
		Array<var>  *possibleValues;
		URL url;
		String urlString;
};

class CtrlrBooleanPropertyComponent : public Component, public Button::Listener, public CtrlrPropertyChild
{
	public:
		CtrlrBooleanPropertyComponent (const Value& _valueToControl, const String& _stateText);
		~CtrlrBooleanPropertyComponent();
		void paint (Graphics& g);
		void refresh();
		void resized();
	    void buttonClicked (Button*);

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CtrlrBooleanPropertyComponent);

		Value valueToControl;
		ToggleButton button;
		String onText,offText;
		String stateText;
};

class CtrlrButtonPropertyComponent : public Component, public Button::Listener, public CtrlrPropertyChild
{
	public:
		CtrlrButtonPropertyComponent (const Value &_valueToControl, const String &_propertyName);
		~CtrlrButtonPropertyComponent();
		void refresh();
		void resized();
		void buttonClicked (Button *button);

	private:
		Value valueToControl;
		String propertyName;
		TextButton button;
};

// Added v5.6.32
class CtrlrButtonTextPropertyComponent : public Component, public Button::Listener, public CtrlrPropertyChild
{
    public:
    CtrlrButtonTextPropertyComponent (const Value &_valueToControl, const String &_propertyName, const String &_buttonText);
        ~CtrlrButtonTextPropertyComponent();
        void refresh();
        void resized();
        void buttonClicked (Button *button);

    private:
        Value valueToControl;
        String propertyName;
        TextButton button;
        String buttonText;
};

class CtrlrChoicePropertyComponent  : public Component,
									  public ComboBox::Listener,
									  public CtrlrPropertyChild
{
	public:
		CtrlrChoicePropertyComponent (const Value &_valueToControl,
                                        const StringArray *_choices,
                                        const Array<var> *_values,
                                        const bool _numeric);
		~CtrlrChoicePropertyComponent();
		void refresh();
		void resized();
		void comboBoxChanged (ComboBox* comboBoxThatHasChanged);
		void changed();
        //void changeListenerCallback (ChangeBroadcaster* source);

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CtrlrChoicePropertyComponent)
		Value valueToControl;
		Array <var> values;
		StringArray choices;
		ScopedPointer <ComboBox> combo;
		CtrlrPropertyComponent *owner;
		bool numeric;
};

/* Colour Property */
class CtrlrColourLabel : public Label
{
	TextEditor *createEditorComponent ()
	{
		TextEditor* const ed = new TextEditor (getName());
		ed->setColour (TextEditor::highlightColourId, findColour (Label::backgroundColourId).contrasting (0.5f));
		ed->setInputRestrictions (8, "ABCDEFabcdef01234567890");
		ed->applyFontToAllText (getLookAndFeel().getLabelFont (*this));
		copyAllExplicitColoursTo (*ed);
		return ed;
	}
};


// Helper class to get a callback when the modal window is dismissed
class ModalCallback  : public juce::ModalComponentManager::Callback
{
public:
    std::function<void()> onDismissed;

    void modalStateFinished (int) override
    {
        if (onDismissed)
            onDismissed();
    }
};

// Added v5.6.34. Required extra class for the colour picker button so that it follows the lookAndFeel colourScheme in the panel windows as well as the others (layer manager etc)
class ColourPickerButton : public juce::Button
{
public:
    ColourPickerButton(const juce::String& name = juce::String())
        : juce::Button(name)
    {
        setWantsKeyboardFocus(false);
    }

    void paintButton(juce::Graphics& g, bool isMouseOver, bool isMouseDown) override
    {
        // 1. Get the button's background color from the LookAndFeel
        auto buttonColour = getLookAndFeel().findColour(juce::TextButton::buttonColourId);

        if (isMouseOver)
            // buttonColour = getLookAndFeel().findColour(juce::TextButton::buttonOnColourId);
			buttonColour = getLookAndFeel().findColour(juce::TextButton::buttonColourId).contrasting(0.05f);

        // 2. Draw the button's background rectangle
        g.setColour(buttonColour);
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 4.0f);
        
        // 3. Draw the button's outline
        g.setColour(getLookAndFeel().findColour(juce::ComboBox::outlineColourId));
        g.drawRoundedRectangle(getLocalBounds().toFloat(), 4.0f, 1.0f);

        // --- The existing SVG icon code ---
        const juce::String eyedropperSVG = R"(
        <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 16 16">
            <path fill="#000000" d="M13.354.646a1.207 1.207 0 0 0-1.708 0L8.5 3.793l-.646-.647a.5.5 0 1 0-.708.708L8.293 5l-7.147 7.146A.5.5 0 0 0 1 12.5v1.793l-.854.853a.5.5 0 1 0 .708.707L1.707 15H3.5a.5.5 0 0 0 .354-.146L11 7.707l1.146 1.147a.5.5 0 0 0 .708-.708l-.647-.646 3.147-3.146a1.207 1.207 0 0 0 0-1.708zM2 12.707l7-7L10.293 7l-7 7H2z"/>
        </svg>
        )";

        std::unique_ptr<juce::Drawable> icon = juce::Drawable::createFromImageData(eyedropperSVG.toRawUTF8(), strlen(eyedropperSVG.toRawUTF8()));

        if (icon)
        {
            auto iconColour = getLookAndFeel().findColour(juce::TextButton::textColourOffId);
            icon->replaceColour(juce::Colours::black, iconColour);
            
            // Calculate a new, smaller rectangle for the icon to be drawn in.
            const float iconSizeFactor = 0.4f; // % of the button's size
            auto iconBounds = getLocalBounds().toFloat().reduced(getLocalBounds().getWidth() * (1.0f - iconSizeFactor) / 2.0f);

            icon->drawWithin(g, iconBounds, juce::RectanglePlacement::centred, 1.0f);
        }
    }
};

class CtrlrColourEditorComponent : 	public Component,
									public ChangeListener,
									public ChangeBroadcaster,
									public Label::Listener,
									public Button::Listener
{
	public:
		CtrlrColourEditorComponent(ChangeListener *defaultListener=0);
		~CtrlrColourEditorComponent();
	
		void updateLabel();
		void labelTextChanged (Label *labelThatHasChanged);
		void resized();
		void resetToDefault() { colour = Colours::black; sendChangeMessage(); }
		void setColour (const Colour& newColour, const bool sendChangeMessageNow=false);
		const Colour getColour(){ return (colour); }
		void refresh() { updateLabel(); }
		void buttonClicked(Button* buttonThatWasClicked) override; // Added v5.6.34.
		// void mouseDown (const MouseEvent &e);
		void changeListenerCallback (ChangeBroadcaster* source) override; // Added override
		void lookAndFeelChanged() override;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CtrlrColourEditorComponent)

	private:
		CtrlrColourLabel colourTextInput;
		// ColourPickerButton* colourPickerButton;
		std::unique_ptr<ColourPickerButton> colourPickerButton;
		Colour colour;
		bool canResetToDefault;

		void openColourPicker(); // Add this helper method
		// void updateButtonColour(); // Helper to update button appearance
};

class CtrlrColourPropertyComponent : public Component, public ChangeListener, public CtrlrPropertyChild
{
	public:
		CtrlrColourPropertyComponent (const Value &_valueToControl);
		~CtrlrColourPropertyComponent();
	
		void refresh() override;
		void changeListenerCallback (ChangeBroadcaster* source) override;
		void resized() override;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CtrlrColourPropertyComponent)

	private:
		Value valueToControl;
		CtrlrColourEditorComponent cs;
};

/* Read Only Property */
class CtrlrReadOnlyProperty : public Component, public CtrlrPropertyChild
{
	public:
		CtrlrReadOnlyProperty (const Identifier &_propertyName,
										const ValueTree &_propertyElement,
										const ValueTree &identifier,
										CtrlrPanel *panel=nullptr,
										StringArray *possibleChoices=nullptr,
										StringArray *possibleValues=nullptr);
		~CtrlrReadOnlyProperty();
		void refresh();
		void resized();

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CtrlrReadOnlyProperty)

	private:
		Label value;
		ValueTree propertyElement;
		Identifier propertyName;
		CtrlrPanel* panel; // Added v5.6.34. Thanks to @dnaldoog. Used to get the name of the layer, not the useless long id string
};

class CtrlrExpressionProperty  : public Component,
                                 public TextEditor::Listener,
								 public Button::Listener,
								 public CtrlrPropertyChild
{
	public:
		CtrlrExpressionProperty (const Value &_valeToControl);
		~CtrlrExpressionProperty();
		void refresh();
		void textEditorTextChanged (TextEditor &editor);
		void textEditorReturnKeyPressed (TextEditor &editor);
		void textEditorFocusLost (TextEditor &editor);
		const bool compile(const bool setPropertyIfValid);
		void resized();
		void buttonClicked (Button* buttonThatWasClicked);

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CtrlrExpressionProperty);

	private:
		CtrlrFloatingWindow *externalEditorWindow;
		Value valeToControl;
		TextEditor* text;
		DrawableButton* apply;
};

class CtrlrFileProperty  : public Component, public Label::Listener, public Button::Listener, public CtrlrPropertyChild
{
	public:
		CtrlrFileProperty (const Value &_valeToControl);
		~CtrlrFileProperty();
		void refresh();
		void resized();
		void buttonClicked (Button* buttonThatWasClicked);
		void labelTextChanged (Label* labelThatHasChanged);

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CtrlrFileProperty);
		Value valueToControl;
		Label *path;
		TextButton *browse;
};

class CtrlrFontPropertyComponent  : public Component,
									public ComboBox::Listener,
									public Button::Listener,
									public Slider::Listener,
									public CtrlrPropertyChild
									// public LookAndFeel_V2
{
	public:
		CtrlrFontPropertyComponent (const Value &_valueToControl, CtrlrPanel *_owner);
		~CtrlrFontPropertyComponent() override; // Mark destructor with override
    void refresh() override; // Mark as override if it overrides a base class method
    Font getFont();
    void resized() override; // Mark as override

    // These are listener methods and should be marked override
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;

    // This function seems to be part of an old LookAndFeel, so it's best to remove it if you're not using it.
    // Label* createSliderTextBox (Slider& slider);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CtrlrFontPropertyComponent);

    class SliderLabelComp : public Label
    {
    public:
        SliderLabelComp() : Label ("", "") {}
        void mouseWheelMove (const MouseEvent&, const MouseWheelDetails&) override {} // Mark as override
    };

	private:
		Value valueToControl;
		StringArray choices;
		CtrlrPanel *owner;
	
		// Pointers for components whose memory will be managed by the parent
		ComboBox* typeface;
		ComboBox* fontSizeComboBox; // Replaces the Slider
		ComboBox* horizontalScaleComboBox; // Replaces the Slider
		ComboBox* kerningComboBox; // Replaces the Slider
		DrawableButton* fontBold;
		DrawableButton* fontItalic;
		DrawableButton* fontUnderline;

		Label* fontSizeLabel; // Added v5.6.34. Thanks to @dnaldoog
		Label* horizontalScaleLabel; // Added v5.6.34. Thanks to @dnaldoog
		Label* kerningLabel; // Added v5.6.34. Thanks to @dnaldoog
};

class CtrlrLuaMethodProperty  : public Component,
								public ComboBox::Listener,
								public Button::Listener,
								public CtrlrPropertyChild
{
	public:
		CtrlrLuaMethodProperty (const Value &_valeToControl, const Identifier &_id, CtrlrPanel *_owner);
		~CtrlrLuaMethodProperty();
		void refresh();
		void resized();
		void comboBoxChanged (ComboBox* comboBoxThatHasChanged);
		void buttonClicked (Button* buttonThatWasClicked);

	private:
		Value valeToControl;
		Identifier id;
		String propertyName;
		CtrlrPanel *owner;
		ComboBox* methodSelectorCombo;
		DrawableButton* editMethodButton;
        DrawableButton* newMethodButton;
        DrawableButton* deleteMethodButton;
};

class CtrlrModulatorListProperty :	public CtrlrPropertyChild,
									public Component,
									public ComboBox::Listener,
									public CtrlrPanel::Listener
{
	public:
		CtrlrModulatorListProperty (const Value &_valueToControl, CtrlrPanel *_owner);
		~CtrlrModulatorListProperty();
		void refresh();
		void comboBoxChanged (ComboBox* comboBoxThatHasChanged);
		void resized();
		void modulatorChanged (CtrlrModulator *modulatorThatChanged);
		void modulatorAdded (CtrlrModulator *modulatorThatWasAdded);
		void modulatorRemoved (CtrlrModulator *modulatorRemoved);
		void listChanged();

	private:
		CtrlrPanel *owner;
		Value valueToControl;
		StringArray choices;
		ScopedPointer <ComboBox> combo;
		bool numeric;
};

class MultiMidiAlert : public AlertWindow
{
	public:
		MultiMidiAlert ()
			:	AlertWindow ("", "Add a new message to the multi list\n[-1 for parent value setting, -2 for parent number setting. For SysEx message the formula is the same as in the SysEx editor.]", AlertWindow::QuestionIcon, 0),
				valueSlider("Controller Value"), numberSlider("Controller Number")
		{
			const char *types[] = { "CC", "Aftertouch", "ChannelPressure", "NoteOn", "NoteOff", "SysEx", "--", "ProgramChange", "PitchWheel", 0 };
			const char *v[] = { "Direct", "LSB7bit", "MSB7bit", "LSB4bit", "MSB4bit", 0};
			addComboBox ("messageType", StringArray(types), "Midi message type");
			addComboBox ("value", StringArray(v), "Value mapping");
			addComboBox ("number", StringArray(v), "Number mapping");
			addTextEditor ("sysexFormula", "F0 00 F7", "SysEx Formula", false);
			valueSlider.setSize (300,32);
			valueSlider.setSliderStyle (Slider::LinearBar);
			valueSlider.setRange (-2,127,1);
			valueSlider.setValue (-1);

			numberSlider.setSize (300,32);
			numberSlider.setSliderStyle (Slider::LinearBar);
			numberSlider.setRange (-2,127,1);
			numberSlider.setValue (-1);

			addCustomComponent (&numberSlider);
			addCustomComponent (&valueSlider);

			addButton ("OK", 1);
			getComboBoxComponent("messageType")->setSelectedId (1, dontSendNotification);
			getComboBoxComponent("value")->setSelectedId (1, dontSendNotification);
			getComboBoxComponent("number")->setSelectedId (1, dontSendNotification);
		}

		void buttonClicked (Button* button)
		{
			getParentComponent()->exitModalState(1);
		}

		const String getValue()
		{
			String ret;
			ret << getComboBoxComponent("messageType")->getText() + ",";
			ret << getComboBoxComponent("number")->getText() + ",";
			ret << getComboBoxComponent("value")->getText() + ",";
			ret << String (((Slider*)getCustomComponent(0))->getValue()) + ",";
			ret << String (((Slider*)getCustomComponent(1))->getValue()) + ",";
			ret << getTextEditor ("sysexFormula")->getText();
			return (ret);
		}

	private:
		Slider valueSlider, numberSlider;
};

class CtrlrMultiMidiPropertyComponent  : public Component,
                                         public ListBoxModel,
                                         public Label::Listener,
										 public Button::Listener,
										 public CtrlrPropertyChild
{
	public:
		CtrlrMultiMidiPropertyComponent (const Value &_valueToControl);
		~CtrlrMultiMidiPropertyComponent();
		void paintListBoxItem(int rowNumber, Graphics &g, int width, int height, bool rowIsSelected);
		int getNumRows();
		void refresh();
		void loadAdditionalTemplates(const File &templateFile);
		Component *refreshComponentForRow (int rowNumber, bool isRowSelected, Component *existingComponentToUpdate);
		void labelTextChanged (Label* labelThatHasChanged);
		void paint (Graphics& g);
		void resized();
		void buttonClicked (Button* buttonThatWasClicked);
		void mouseDown (const MouseEvent& e);
		void mouseDoubleClick (const MouseEvent& e);

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CtrlrMultiMidiPropertyComponent)
		MultiMidiAlert questionWindow;
		StringArray values;
		Value valueToControl;
		StringPairArray templates;
        DrawableButton* add;
        DrawableButton* remove;
		ListBox* list;
		DrawableButton* copy;
        DrawableButton* paste;
        DrawableButton* insert;
};

class CtrlrSliderPropertyComponent   : public Component, private Slider::Listener, public CtrlrPropertyChild
{
	public:
		CtrlrSliderPropertyComponent (const Value &_valueToControl, double rangeMin, double rangeMax, double interval);
		~CtrlrSliderPropertyComponent();
		virtual void setValue (double newValue);
		virtual double getValue() const;
		void refresh();
		void sliderValueChanged (Slider *sliderThatChanged);
		void resized();

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CtrlrSliderPropertyComponent);
		Slider slider;
		Value valueToControl;
};

class SysExRow : public Component
{
	public:
		SysExRow(const int _n=1, const int _w=32, const int _gap=4);
		void paint (Graphics &g);
		void resized();

	private:
		int n,w,gap;
};

class CtrlrSysExEditor  : public Component,
						  public Slider::Listener,
						  public Label::Listener,
                          public ChangeBroadcaster
{
	public:
		CtrlrSysExEditor (Value &_val, CtrlrPanel *_panel);
		~CtrlrSysExEditor();
		void setLength (const int newLength);
		Label *addByte(const String &byteAsString);
		const String getValue();
		const PopupMenu getVendorIdMenu();
		void paint (Graphics& g);
		void resized();
		void sliderValueChanged (Slider* sliderThatWasMoved);
		void labelTextChanged (Label* labelThatHasChanged);
		void mouseDown (const MouseEvent& e);

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CtrlrSysExEditor)

	private:
		Value val;
		CtrlrPanel *owner;
		StringArray splitMessage;
		OwnedArray <Label> byteValueLabels;
		OwnedArray <SysExRow> rows;
		int currentMessageLength;
		Slider* messageLength;
		Label* label;

};

class CtrlrSysExFormulaEditor  : public Component
{
public:
    //==============================================================================
    CtrlrSysExFormulaEditor ();
    ~CtrlrSysExFormulaEditor();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();



    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	CodeDocument forwardFormulaDocument, reverseFormulaDocument;
    //[/UserVariables]

    //==============================================================================
    CodeEditorComponent* forwardFormula;
    CodeEditorComponent* reverseFormula;
    Label* forwardLabel;
    Label* reverseLabel;
    Label* label;


    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    CtrlrSysExFormulaEditor (const CtrlrSysExFormulaEditor&);
    const CtrlrSysExFormulaEditor& operator= (const CtrlrSysExFormulaEditor&);
};

class CtrlrSysExPropertyComponent  : public Component,
									 public Label::Listener,
									 public Button::Listener,
									 public CtrlrPropertyChild,
									 public ChangeListener
{
	public:
		CtrlrSysExPropertyComponent (const Value &_valueToControl, const ValueTree &_propertyTree, const Identifier &_propertyName, CtrlrPanel *_owner);
		~CtrlrSysExPropertyComponent();
		void refresh();
		void resized();
		void labelTextChanged (Label* labelThatHasChanged);
		void buttonClicked (Button* buttonThatWasClicked);
		void changeListenerCallback (ChangeBroadcaster *cb);
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CtrlrSysExPropertyComponent)

	private:
		Value valueToControl;
		Label* sysexPreview;
		TextButton* editButton;
		DrawableButton* copy;
        DrawableButton* paste;
		ValueTree propertyTree;
		Identifier propertyName;
		CtrlrPanel *owner;
};

class CtrlrTextPropertyComponent : public Component, public CtrlrPropertyChild
{
	public:
		CtrlrTextPropertyComponent (const Value& _valueToControl,
									const int maxNumChars,
									const bool isMultiLine,
									const bool isReadOnly=false,
                                    const bool useImprovedLegibility=false); // Added v5.6.34.

		~CtrlrTextPropertyComponent();
		void refresh();
		void setText (const String& newText);
		String getText() const;
		void textWasEdited();
		void resized();

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CtrlrTextPropertyComponent);

		ScopedPointer<Label> textEditor;
		void createEditor (int maxNumChars, bool isMultiLine);
		
        Value valueToControl;
        bool isReadOnly; // Added v5.6.34
        bool useImprovedLegibility; // Added v5.6.34
};

class CtrlrTimestampProperty : public Component, public CtrlrPropertyChild
{
	public:
		CtrlrTimestampProperty (const Value& _valueToControl);
		~CtrlrTimestampProperty();
		void refresh();
		void resized();

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CtrlrTimestampProperty);
		ScopedPointer<Label> textEditor;
		Value valueToControl;
};

class CtrlrUnknownPropertyComponent : public Component, public CtrlrPropertyChild
{
	public:
		CtrlrUnknownPropertyComponent(const Identifier &_propertyName,
										const ValueTree &_propertyElement,
										const ValueTree &identifier,
										CtrlrPanel *panel=nullptr,
										StringArray *possibleChoices=nullptr,
										StringArray *possibleValues=nullptr);
		~CtrlrUnknownPropertyComponent();
		void resized();
		void refresh();

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CtrlrUnknownPropertyComponent);
		Label l;
		Identifier propertyName;
		ValueTree propertyElement;
};

#endif
