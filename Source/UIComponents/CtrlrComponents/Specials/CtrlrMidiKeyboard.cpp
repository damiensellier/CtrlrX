#include "stdafx.h"
#include "CtrlrUtilities.h"
#include "../CtrlrComponentTypeManager.h"
#include "CtrlrMidiKeyboard.h"

CtrlrMidiKeyboard::CtrlrMidiKeyboard (CtrlrModulator &owner)
    : CtrlrComponent(owner),
      midiKeyboard (0)
{
    addAndMakeVisible (midiKeyboard = new MidiKeyboardComponent (keyboardState, MidiKeyboardComponent::horizontalKeyboard ));
    midiKeyboard->setName ("midiKeyboard");
	keyboardState.addListener (this);
	setProperty (Ids::uiMidiKeyboardOrientation, "horizontalKeyboard");
	setProperty (Ids::uiMidiKeyboardWhiteButtonColour, "0xffffffff");
	setProperty (Ids::uiMidiKeyboardBlackButtonColour, "0xff000000");
	setProperty (Ids::uiMidiKeyboardSeparatorLineColour, "0xff000000");
	setProperty (Ids::uiMidiKeyboardMouseOverColour, "0xffff0000");
	setProperty (Ids::uiMidiKeyboardMouseDownColour, "0xff0000ff");
	setProperty (Ids::uiMidiKeyboardTextLabelColour, "0xff000000");
	setProperty (Ids::uiMidiKeyboardButtonBackgroundColour, "0xff0f0f0f");
	setProperty (Ids::uiMidiKeyboardButtonArrowColour, "0xff000000");
	setProperty (Ids::uiMidiKeyboardLowestVisibleKey, 48);
    setProperty (Ids::uiMidiKeyboardBaseOctaveKeyPress, 5); // Updated v5.6.34. Was (0) which didn't make any sense from JUCE octave index property
    setProperty (Ids::uiMidiKeyboardOctaveFroMiddleC, 4);
	setProperty (Ids::uiMidiKeyboardMapToNoteNumber, false);
    setProperty (Ids::uiMidiKeyboardOctaveKeyDown, "z"); // Added v5.6.34. Default for octave down
    setProperty (Ids::uiMidiKeyboardOctaveKeyUp, "x"); // Added v5.6.34. Default for octave up

    setSize (256, 64);
	owner.getOwnerPanel().addPanelListener (this);
    
    // Apply properties to MidiKeyboardComponent
    currentBaseOctave = getProperty(Ids::uiMidiKeyboardBaseOctaveKeyPress);
    midiKeyboard->setKeyPressBaseOctave(currentBaseOctave);

    // Apply the labeling for Middle C
    midiKeyboard->setOctaveForMiddleC(getProperty(Ids::uiMidiKeyboardOctaveFroMiddleC));
    midiKeyboard->setLowestVisibleKey(getProperty(Ids::uiMidiKeyboardLowestVisibleKey));

    setWantsKeyboardFocus(true);
    setMouseClickGrabsKeyboardFocus(true);
    addKeyListener(this);

    midiKeyboard->setWantsKeyboardFocus(true);
    midiKeyboard->setMouseClickGrabsKeyboardFocus(true);
    midiKeyboard->addKeyListener(this);
}

CtrlrMidiKeyboard::~CtrlrMidiKeyboard()
{
    owner.getOwnerPanel().removePanelListener (this);
    removeKeyListener(this);
    deleteAndZero (midiKeyboard);
}

void CtrlrMidiKeyboard::paint (Graphics& g) // Or paintOverChildren
{
    // This is the core logic for determining if the outline should be drawn.
    // It checks if the internal midiKeyboard component has keyboard focus.
    bool hasKeyboardFocusActual = (midiKeyboard && midiKeyboard->hasKeyboardFocus(true));
    bool isMouseHoveringActual = isMouseOver(); // Use isMouseOver() from Component

    juce::String debugMessagePaint;
    debugMessagePaint << "CtrlrMidiKeyboard::paint called. Keyboard Focus: "
                      << (hasKeyboardFocusActual ? "TRUE" : "FALSE")
                      << ", Mouse Hover: "
                      << (isMouseHoveringActual ? "TRUE" : "FALSE")
                      << ", Current Focused Component: "
                      << (Component::getCurrentlyFocusedComponent() ? Component::getCurrentlyFocusedComponent()->getName() : juce::String("NULL"));
    _DBG(debugMessagePaint);

    // We only want the outline if 'hasKeyboardFocusActual' is true.
    if (hasKeyboardFocusActual)
    {
        // g.setColour (Colours::red);
        g.setColour(VAR2COLOUR(getProperty(Ids::uiMidiKeyboardMouseOverColour)));
        // Draw around the midiKeyboard component itself, expanded by 4 pixels, with a 4-pixel thick line.
        g.drawRect (midiKeyboard->getBounds().expanded (4.0f), 4.0f);
        juce::String outlineDrawnMsg = "Outline drawn based on keyboard focus.";
        _DBG(outlineDrawnMsg);
    }
    else
    {
        g.setColour (Colours::transparentBlack);
        g.drawRect (midiKeyboard->getBounds().expanded (0.0f), 0.0f);
        juce::String noOutlineDrawnMsg = "No outline drawn (not focused).";
        _DBG(noOutlineDrawnMsg);
    }
}

void CtrlrMidiKeyboard::resized()
{
	midiKeyboard->setBounds (getUsableRect());
    repaint();
}


double CtrlrMidiKeyboard::getComponentValue()
{
	return (owner.getMidiMessage().getValue());
}

int CtrlrMidiKeyboard::getComponentMidiValue()
{
	return (owner.getMidiMessage().getValue());
}

double CtrlrMidiKeyboard::getComponentMaxValue()
{
	return (127);
}

void CtrlrMidiKeyboard::setComponentValue (const double newValue, const bool sendChangeMessage)
{
	// keyboardState.noteOn (owner.getMidiMessage().getChannel(), owner.getMidiMessage().getNumber(), (float)(127/newValue));
}

void CtrlrMidiKeyboard::valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property)
{
	if (property == Ids::uiMidiKeyboardOrientation)
	{
		midiKeyboard->setOrientation (CtrlrComponentTypeManager::orientationFromString(getProperty(property)));
	}
	else if (property == Ids::uiMidiKeyboardWhiteButtonColour)
	{
		midiKeyboard->setColour (MidiKeyboardComponent::whiteNoteColourId, VAR2COLOUR(getProperty(property)));
	}
	else if (property == Ids::uiMidiKeyboardBlackButtonColour)
	{
		midiKeyboard->setColour (MidiKeyboardComponent::blackNoteColourId, VAR2COLOUR(getProperty(property)));
	}
	else if (property == Ids::uiMidiKeyboardSeparatorLineColour)
	{
		midiKeyboard->setColour (MidiKeyboardComponent::keySeparatorLineColourId, VAR2COLOUR(getProperty(property)));
	}
	else if (property == Ids::uiMidiKeyboardMouseOverColour)
	{
		midiKeyboard->setColour (MidiKeyboardComponent::mouseOverKeyOverlayColourId, VAR2COLOUR(getProperty(property)));
	}
	else if (property == Ids::uiMidiKeyboardMouseDownColour)
	{
		midiKeyboard->setColour (MidiKeyboardComponent::keyDownOverlayColourId, VAR2COLOUR(getProperty(property)));
	}
	else if (property == Ids::uiMidiKeyboardTextLabelColour)
	{
		midiKeyboard->setColour (MidiKeyboardComponent::textLabelColourId, VAR2COLOUR(getProperty(property)));
	}
	else if (property == Ids::uiMidiKeyboardButtonBackgroundColour)
	{
		midiKeyboard->setColour (MidiKeyboardComponent::upDownButtonBackgroundColourId, VAR2COLOUR(getProperty(property)));
	}
	else if (property == Ids::uiMidiKeyboardButtonArrowColour)
	{
		midiKeyboard->setColour (MidiKeyboardComponent::upDownButtonArrowColourId, VAR2COLOUR(getProperty(property)));
	}
	else if (property == Ids::uiMidiKeyboardLowestVisibleKey)
	{
		midiKeyboard->setLowestVisibleKey((int)getProperty(property));
	}
	else if (property == Ids::uiMidiKeyboardBaseOctaveKeyPress) // Updated v5.6.34
    {
        // When this property changes (e.g., user adjusts it in the UI),
        // update currentBaseOctave and pass it directly.
        currentBaseOctave = getProperty(property);
        midiKeyboard->setKeyPressBaseOctave (currentBaseOctave);
    }
    else if (property == Ids::uiMidiKeyboardOctaveFroMiddleC)
    {
        midiKeyboard->setOctaveForMiddleC (getProperty(property));
    }
    else if (property == Ids::uiMidiKeyboardOctaveKeyDown) // Updated v5.6.34
    {
        // No direct UI update needed, keyPressed will read the new value.
        // _DBG("Octave Down Key changed to: " << getProperty(property).toString());
    }
    else if (property == Ids::uiMidiKeyboardOctaveKeyUp) // Updated v5.6.34
    {
        // No direct UI update needed, keyPressed will read the new value.
        // _DBG("Octave Up Key changed to: " << getProperty(property).toString());
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

void CtrlrMidiKeyboard::handleNoteOn (MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity)
{
	owner.getMidiMessage().setMidiMessageType(NoteOn);
	if (getProperty(Ids::uiMidiKeyboardMapToNoteNumber))
    {
        owner.getMidiMessage().setValue (127.0*velocity);
        owner.getProcessor().setValueGeneric (CtrlrModulatorValue(midiNoteNumber,CtrlrModulatorValue::changedByGUI), true);
    }
	else
	{
        owner.getMidiMessage().setNumber (midiNoteNumber);
        owner.getProcessor().setValueGeneric (CtrlrModulatorValue((int)(velocity*127),CtrlrModulatorValue::changedByGUI), true);
	}
}

void CtrlrMidiKeyboard::handleNoteOff (MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity)
{
	owner.getMidiMessage().setMidiMessageType(NoteOff);

	if (getProperty(Ids::uiMidiKeyboardMapToNoteNumber))
    {
        owner.getMidiMessage().setValue (0);
        owner.getProcessor().setValueGeneric (CtrlrModulatorValue(midiNoteNumber,CtrlrModulatorValue::changedByGUI), true);
    }
	else
    {
        owner.getMidiMessage().setNumber (midiNoteNumber);
        owner.getProcessor().setValueGeneric (CtrlrModulatorValue(0,CtrlrModulatorValue::changedByGUI), true);
    }
}

void CtrlrMidiKeyboard::midiReceived(MidiMessage &message)
{
	keyboardState.processNextMidiEvent (message);
}

bool CtrlrMidiKeyboard::keyPressed (const KeyPress& key, Component* originatingComponent)
{
    const juce::juce_wchar pressedChar = key.getTextCharacter();
    
    juce::String debugMessageKeyPress;
    debugMessageKeyPress << "CtrlrMidiKeyboard::keyPressed called. Key: "
                         << juce::String::charToString(pressedChar) // Ensure char_t is converted to String
                         << ", Originating Component: "
                         << (originatingComponent ? originatingComponent->getName() : juce::String("NONE"));
    _DBG(debugMessageKeyPress); // Pass the fully constructed String

    if ((originatingComponent == midiKeyboard || originatingComponent == this) && (hasKeyboardFocus(true) || isMouseHovering))
    {
        if (midiKeyboard)
        {
            if (pressedChar == 'x')
            {
                currentBaseOctave = juce::jmin (10, currentBaseOctave + 1);
                setProperty (Ids::uiMidiKeyboardBaseOctaveKeyPress, currentBaseOctave);
                // --- Building the debug message string ---
                juce::String debugMessageOctaveUp;
                debugMessageOctaveUp << "Octave up to: " << currentBaseOctave;
                _DBG(debugMessageOctaveUp); // Pass the fully constructed String
                return true;
            }
            else if (pressedChar == 'z')
            {
                currentBaseOctave = juce::jmax (0, currentBaseOctave - 1);
                setProperty (Ids::uiMidiKeyboardBaseOctaveKeyPress, currentBaseOctave);
                // --- Building the debug message string ---
                juce::String debugMessageOctaveDown;
                debugMessageOctaveDown << "Octave down to: " << currentBaseOctave;
                _DBG(debugMessageOctaveDown); // Pass the fully constructed String
                return true;
            }
        }
    }
    return false;
}

void CtrlrMidiKeyboard::mouseEnter (const MouseEvent &e)
{
    juce::String debugMessageEnter;
    debugMessageEnter << "CtrlrMidiKeyboard mouseEnter.";
    _DBG(debugMessageEnter);
    // No repaint needed here for outline purposes, as outline is focus-driven.
}

void CtrlrMidiKeyboard::mouseExit (const MouseEvent &e)
{
    juce::String debugMessageExit;
    debugMessageExit << "CtrlrMidiKeyboard mouseExit.";
    _DBG(debugMessageExit);
    // The focusLost() method will trigger a repaint if focus is actually lost.
}

void CtrlrMidiKeyboard::focusGained (FocusChangeType cause)
{
    juce::String debugMessageFocusGained;
    debugMessageFocusGained << "CtrlrMidiKeyboard focusGained! Cause: " << String(cause);
    _DBG(debugMessageFocusGained);
    CtrlrComponent::focusGained(cause); // Still call the base class
    repaint();
}

void CtrlrMidiKeyboard::focusLost (FocusChangeType cause)
{
    juce::String debugMessageFocusLost;
    debugMessageFocusLost << "CtrlrMidiKeyboard focusLost! Cause: " << String(cause);
    _DBG(debugMessageFocusLost);
    CtrlrComponent::focusLost(cause); // Still call the base class
    repaint();
}

void CtrlrMidiKeyboard::focusOfChildComponentChanged (FocusChangeType cause)
{
    juce::String debugMessageChildFocus;
    debugMessageChildFocus << "CtrlrMidiKeyboard focusOfChildComponentChanged! Cause: " << String(cause)
                           << ", Child Focused: " << (Component::getCurrentlyFocusedComponent() ? Component::getCurrentlyFocusedComponent()->getName() : "NULL");
    _DBG(debugMessageChildFocus);

    // This is the ideal place to trigger a repaint when the child (midiKeyboard) gains or loses focus. The paint method's logic will then check midiKeyboard->hasKeyboardFocus(true) and draw the outline.
    repaint();
}
