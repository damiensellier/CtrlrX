#include "stdafx.h"
#include "CtrlrManager/CtrlrManager.h"
#include "CtrlrUtilities.h"
#include "CtrlrProcessor.h"
#include "CtrlrMIDIMon.h"

CtrlrMIDIMon::CtrlrMIDIMon (CtrlrManager &_owner)
    : owner(_owner), logIn(false), logOut(false),
      resizer (0),
      outMon (0),
      inMon (0),
      outLabel(0),
      inLabel(0)
{
    addAndMakeVisible (resizer = new StretchableLayoutResizerBar (&layoutManager, 1, false));

    addAndMakeVisible (outMon = new CodeEditorComponent (outputDocument, 0));
    outMon->setName (L"outMon");

    addAndMakeVisible (inMon = new CodeEditorComponent (inputDocument, 0));
    inMon->setName (L"inMon");
	
    addAndMakeVisible(outLabel = new Label("outLabel", "MIDI OUT"));
    outLabel->setFont(Font(14, Font::bold));
    outLabel->setJustificationType(Justification::centred);
    outLabel->setColour(Label::backgroundColourId, Colour(0xffffacac).darker(0.1f));
    outLabel->setColour(Label::textColourId, Colours::black);

    addAndMakeVisible(inLabel = new Label("inLabel", "MIDI IN"));
    inLabel->setFont(Font(14, Font::bold));
    inLabel->setJustificationType(Justification::centred);
    inLabel->setColour(Label::backgroundColourId, Colour(0xffb3ffac).darker(0.1f));
    inLabel->setColour(Label::textColourId, Colours::black);

	layoutManager.setItemLayout (0, -0.001, -1.0, -0.49);
 	layoutManager.setItemLayout (1, -0.001, -0.01, -0.01);
 	layoutManager.setItemLayout (2, -0.001, -1.0, -0.49);

	owner.getCtrlrLog().addListener (this);

	inMon->setFont ( Font(owner.getFontManager().getDefaultMonoFontName(), 12, Font::bold) );
	inMon->setColour (CodeEditorComponent::backgroundColourId, Colour(0xffb3ffac));
	outMon->setFont ( Font(owner.getFontManager().getDefaultMonoFontName(), 12, Font::bold) );
	outMon->setColour (CodeEditorComponent::backgroundColourId, Colour(0xffffacac));

    setSize (500, 400);
}

CtrlrMIDIMon::~CtrlrMIDIMon()
{
	owner.getCtrlrLog().removeListener (this);
    deleteAndZero (resizer);
    deleteAndZero (outMon);
    deleteAndZero (inMon);
    deleteAndZero(outLabel);
    deleteAndZero(inLabel);
}

void CtrlrMIDIMon::paint (Graphics& g)
{
}

void CtrlrMIDIMon::resized()
{
    const int labelHeight = 20;  // Height for the labels

    // Get the current MIDI log options
    int opts = (int)owner.getProperty(Ids::ctrlrLogOptions);
    bool showInput = getBitOption(opts, midiLogInput);
    bool showOutput = getBitOption(opts, midiLogOutput);

    // Show/hide components based on options
    outLabel->setVisible(showOutput);
    outMon->setVisible(showOutput);
    inLabel->setVisible(showInput);
    inMon->setVisible(showInput);

    // If both are shown, use the split layout
    if (showInput && showOutput)
    {
        resizer->setVisible(true);

        int topSectionHeight = proportionOfHeight(0.4900f);
        int bottomSectionHeight = proportionOfHeight(0.4900f);

        outLabel->setBounds(0, 0, getWidth(), labelHeight);
        outMon->setBounds(0, labelHeight, getWidth(), topSectionHeight - labelHeight);

        resizer->setBounds(0, topSectionHeight, getWidth(), proportionOfHeight(0.0100f));

        int inMonStartY = proportionOfHeight(0.5000f);
        inLabel->setBounds(0, inMonStartY, getWidth(), labelHeight);
        inMon->setBounds(0, inMonStartY + labelHeight, getWidth(), bottomSectionHeight - labelHeight);
    }
    // If only output is shown
    else if (showOutput && !showInput)
    {
        resizer->setVisible(false);

        outLabel->setBounds(0, 0, getWidth(), labelHeight);
        outMon->setBounds(0, labelHeight, getWidth(), getHeight() - labelHeight);
    }
    // If only input is shown
    else if (showInput && !showOutput)
    {
        resizer->setVisible(false);

        inLabel->setBounds(0, 0, getWidth(), labelHeight);
        inMon->setBounds(0, labelHeight, getWidth(), getHeight() - labelHeight);
    }
    // If neither is shown (fallback - show both grayed out)
    else
    {
        resizer->setVisible(true);

        int topSectionHeight = proportionOfHeight(0.4900f);
        int bottomSectionHeight = proportionOfHeight(0.4900f);

        outLabel->setBounds(0, 0, getWidth(), labelHeight);
        outMon->setBounds(0, labelHeight, getWidth(), topSectionHeight - labelHeight);

        resizer->setBounds(0, topSectionHeight, getWidth(), proportionOfHeight(0.0100f));

        int inMonStartY = proportionOfHeight(0.5000f);
        inLabel->setBounds(0, inMonStartY, getWidth(), labelHeight);
        inMon->setBounds(0, inMonStartY + labelHeight, getWidth(), bottomSectionHeight - labelHeight);

        // Force them visible for the fallback case
        outLabel->setVisible(true);
        outMon->setVisible(true);
        inLabel->setVisible(true);
        inMon->setVisible(true);
    }
}

void CtrlrMIDIMon::messageLogged (CtrlrLog::CtrlrLogMessage _message) // Updated v5.6.35. MIDI filters Support. Thanks to @dnaldoog
{
	if (!isVisible())
		return;
	
	int filters = (int)owner.getProperty(Ids::ctrlrMidiFilters);
	
	// Only filter if we have actual MIDI data
	if (_message.hasMidiData && (_message.level == CtrlrLog::MidiIn || _message.level == CtrlrLog::MidiOut))
	{
		if (shouldFilterMessage(_message.midiData, filters))
		{
			DBG("Message filtered out!");
			return;
		}
	}
	
	if (_message.level == CtrlrLog::MidiIn)
	{
		inMon->insertTextAtCaret(_message.message + "\n");
		inMon->scrollToKeepCaretOnScreen(); // Keep the new data in view
	}
	else if (_message.level == CtrlrLog::MidiOut)
	{
		outMon->insertTextAtCaret(_message.message + "\n");
		outMon->scrollToKeepCaretOnScreen(); // Keep the new data in view
	}
}

bool CtrlrMIDIMon::shouldFilterMessage(const MidiMessage& m, int filterMask) // Added v5.6.35. MIDI filters Support. Thanks to @dnaldoog
{
	DBG("Checking message against filter mask: " + String(filterMask));
	if (filterMask == 0) return false;
	
	if ((filterMask & Filter_NoteOn) && m.isNoteOn())           		return true;
	if ((filterMask & Filter_NoteOff) && m.isNoteOff())         		return true;
	if ((filterMask & Filter_AftertouchPoly) && m.isAftertouch()) 		return true;
	if ((filterMask & Filter_Control) && m.isController())      		return true;
	if ((filterMask & Filter_Program) && m.isProgramChange())   		return true;
	if ((filterMask & Filter_Sysex) && m.isSysEx())             		return true;
	if ((filterMask & Filter_ChannelPressure) && m.isChannelPressure()) return true;
	if ((filterMask & Filter_PitchWheel) && m.isPitchWheel())   		return true;
	if ((filterMask & Filter_ActiveSense) && m.isActiveSense()) 		return true;
	if ((filterMask & Filter_Clock) && m.isMidiClock()) 				return true;
	
	return false;
}

StringArray CtrlrMIDIMon::getMenuBarNames()
{
	const char* const names[] = { "Window", "View", "Filter", nullptr }; // Updated v5.6.35. MIDI Message Type Filters Support. Thanks to @dnaldoog
	return StringArray (names);
}

PopupMenu CtrlrMIDIMon::getMenuForIndex(int topLevelMenuIndex, const String &menuName)
{
    PopupMenu menu;

    int opts = (int)owner.getProperty(Ids::ctrlrLogOptions);
    int filters = (int)owner.getProperty(Ids::ctrlrMidiFilters);

    if (topLevelMenuIndex == 0)
    {
        menu.addItem(ClearInputLog, "Clear Input");
        menu.addItem(ClearOutputLog, "Clear Output");
        menu.addSeparator();
        menu.addItem(CloseWindow, "Close");
    }
    else if (topLevelMenuIndex == 1)
    {
        // Use ViewMenuBase directly with the option values
        menu.addItem(ViewMenuBase + midiLogName, "Show name", true, getBitOption(opts, midiLogName));
        menu.addItem(ViewMenuBase + midiLogChannel, "Show channel", true, getBitOption(opts, midiLogChannel));
        menu.addItem(ViewMenuBase + midiLogNumber, "Show number", true, getBitOption(opts, midiLogNumber));
        menu.addItem(ViewMenuBase + midiLogValue, "Show value", true, getBitOption(opts, midiLogValue));
        menu.addItem(ViewMenuBase + midiLogRawData, "Show RAW data", true, getBitOption(opts, midiLogRawData));
        menu.addItem(ViewMenuBase + midiLogTimestamp, "Show timestamp", true, getBitOption(opts, midiLogTimestamp));
        menu.addItem(ViewMenuBase + midiLogRawDecimal, "RAW data in decimal", true, getBitOption(opts, midiLogRawDecimal));
        menu.addItem(ViewMenuBase + midiLogDevice, "Show device name", true, getBitOption(opts, midiLogDevice));
        menu.addItem(ViewMenuBase + midiLogDataSize, "Show RAW data size", true, getBitOption(opts, midiLogDataSize));
        menu.addSeparator();
        menu.addColouredItem(ViewMenuBase + midiLogInput, "Monitor input", Colour(0xff21c630), true, getBitOption(opts, midiLogInput));
        menu.addColouredItem(ViewMenuBase + midiLogOutput, "Monitor output", Colour(0xffc62121), true, getBitOption(opts, midiLogOutput));
    }
    else if (topLevelMenuIndex == 2)
    {

        menu.addSectionHeader("Active filters");

        // We add 10000 to the enum value to create a unique Menu ID Just add !getBitOption to fulfill Martin's request
        menu.addItem(FilterMenuBase + Filter_NoteOn, "Note On", true, !getBitOption(filters, Filter_NoteOn));
        menu.addItem(FilterMenuBase + Filter_NoteOff, "Note Off", true, !getBitOption(filters, Filter_NoteOff));
        menu.addItem(FilterMenuBase + Filter_AftertouchPoly, "Aftertouch (Poly)", true, !getBitOption(filters, Filter_AftertouchPoly));
        menu.addItem(FilterMenuBase + Filter_Control, "Control", true, !getBitOption(filters, Filter_Control));
        menu.addItem(FilterMenuBase + Filter_Program, "Program", true, !getBitOption(filters, Filter_Program));
        menu.addItem(FilterMenuBase + Filter_Sysex, "Sysex", true, !getBitOption(filters, Filter_Sysex));
        menu.addItem(FilterMenuBase + Filter_ChannelPressure, "Channel Pressure", true, !getBitOption(filters, Filter_ChannelPressure));
        menu.addItem(FilterMenuBase + Filter_PitchWheel, "Pitch Wheel", true, !getBitOption(filters, Filter_PitchWheel));
        menu.addItem(FilterMenuBase + Filter_ActiveSense, "Active Sense", true, !getBitOption(filters, Filter_ActiveSense));
        menu.addItem(FilterMenuBase + Filter_Clock, "MIDI Clock", true, !getBitOption(filters, Filter_Clock));
        menu.addSeparator();
        menu.addItem(SelectAllFilters, "Select All", true, false);  // Changed from "Clear All"
    }

    return menu;
}

void CtrlrMIDIMon::menuItemSelected(int menuItemID, int topLevelMenuIndex)
{
    if (topLevelMenuIndex == 0) // Updated v5.6.31. From 1 to 0
    {
        if (menuItemID == CloseWindow)
        {
            // Handle close
            // Access the window manager through the owner (CtrlrManager)
            // and tell it to hide the MIDI Monitor window.
            owner.getWindowManager().hide(CtrlrManagerWindowManager::MidiMonWindow);
        }
        if (menuItemID == ClearInputLog)
        {
            // Handle clear input log
            inputDocument.replaceAllContent("");
        }
        if (menuItemID == ClearOutputLog)
        {
            // Handle clear output log
            outputDocument.replaceAllContent("");
        }
    }
    else if (topLevelMenuIndex == 1) // View menu
    {
        int opts = (int)owner.getProperty(Ids::ctrlrLogOptions);
        int bitToFlip = menuItemID - ViewMenuBase;  // Changed from - 10
        setBitOption(opts, bitToFlip, !getBitOption(opts, bitToFlip));
        owner.setProperty(Ids::ctrlrLogOptions, opts);
        resized();
    }
    else if (topLevelMenuIndex == 2) // Filter menu - FIXED
    {
        if (menuItemID == SelectAllFilters) // Select All Filters
        {
            owner.setProperty(Ids::ctrlrMidiFilters, 0);
            DBG("*** SELECT ALL EXECUTED *** Filter mask set to: 0");
      
        }

        else // Individual filter toggle
        {
            int filters = (int)owner.getProperty(Ids::ctrlrMidiFilters);
            int bitToToggle = menuItemID - FilterMenuBase; // This gives us the enum value (1, 2, 4, 8, etc.)
            DBG("Filter toggled. New filter mask: " + String(filters));
            // Toggle the bit using XOR
            filters ^= bitToToggle;

            owner.setProperty(Ids::ctrlrMidiFilters, filters);

            // Debug output to verify
            DBG("Filter toggled. Bit: " + String(bitToToggle) + " New mask: " + String(filters));
        }
    }
}
