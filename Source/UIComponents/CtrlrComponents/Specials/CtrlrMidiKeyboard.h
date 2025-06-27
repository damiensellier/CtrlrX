/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  14 Apr 2011 10:26:04pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_CTRLRMIDIKEYBOARD_CTRLRMIDIKEYBOARD_51983327__
#define __JUCER_HEADER_CTRLRMIDIKEYBOARD_CTRLRMIDIKEYBOARD_51983327__

//[Headers]     -- You can add your own extra header files here --
#include "JuceHeader.h"
#include "../CtrlrComponent.h"
#include "CtrlrPanel/CtrlrPanel.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class CtrlrMidiKeyboard  : public CtrlrComponent,
                           public MidiKeyboardStateListener,
						   public CtrlrPanel::Listener,
                           public KeyListener        // Added v5.6.34. Added for key press handling.
                           // public FocusChangeListener // Added v5.6.34. Added for focus outline handling. Not working with JUCE 6.0.8
{
public:
    //==============================================================================
    CtrlrMidiKeyboard (CtrlrModulator &owner);
    ~CtrlrMidiKeyboard();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void setComponentValue (const double newValue, const bool sendChangeMessage=false);
	double getComponentValue();
	int getComponentMidiValue();
	double getComponentMaxValue();
	void valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property);
	void valueTreeChildrenChanged (ValueTree &treeWhoseChildHasChanged){}
	void valueTreeParentChanged (ValueTree &treeWhoseParentHasChanged){}
	void valueTreeChildAdded (ValueTree& parentTree, ValueTree& childWhichHasBeenAdded){}
    void valueTreeChildRemoved (ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int){}
	void valueTreeChildOrderChanged (ValueTree& parentTreeWhoseChildrenHaveMoved, int, int){}
	void handleNoteOn (MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity);
	void handleNoteOff (MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity);
	void midiReceived(MidiMessage &message);
	void customLookAndFeelChanged(LookAndFeelBase *customLookAndFeel = nullptr) {}
    // KeyListener overrides
    bool keyPressed (const KeyPress& key, Component* originatingComponent) override; // Added v5.6.34. For Octave UP/DOWN

    // FocusChangeListener overrides
    void focusGained (FocusChangeType cause) override;  // Added v5.6.34. For Octave UP/DOWN
    void focusLost (FocusChangeType cause) override; // Added v5.6.34. For Octave UP/DOWN
    void focusOfChildComponentChanged (FocusChangeType cause) override;
    void mouseEnter (const MouseEvent& event) override;
    void mouseExit  (const MouseEvent& event) override;
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();


    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	MidiKeyboardState keyboardState;
	CtrlrMidiMessage noteOn, noteOff;
    int currentBaseOctave; // Added v5.6.34
    bool isMouseHovering = false; // Added v5.6.34
    //[/UserVariables]

    //==============================================================================
    MidiKeyboardComponent* midiKeyboard;

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    CtrlrMidiKeyboard (const CtrlrMidiKeyboard&);
    const CtrlrMidiKeyboard& operator= (const CtrlrMidiKeyboard&);
};


#endif   // __JUCER_HEADER_CTRLRMIDIKEYBOARD_CTRLRMIDIKEYBOARD_51983327__
