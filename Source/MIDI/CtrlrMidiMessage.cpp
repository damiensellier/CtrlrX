#include "stdafx.h"
#include "stdafx_luabind.h"
#include "CtrlrMidiMessage.h"
#include "CtrlrUtilities.h"
#include "CtrlrIDs.h"
#include "CtrlrLog.h"
#include "CtrlrLuaObjectWrapper.h"
#include "CtrlrManager/CtrlrManager.h"
#include "JuceClasses/LMemoryBlock.h"
#include <string> // Added v5.6.35. To convert object to actual MIDI Message

CtrlrMidiMessage::CtrlrMidiMessage()
	:	messageType(None), midiTree(Ids::midi), multiMasterValue(1),
		multiMasterNumber(1), messagePattern(0,true), restoring(false),
		initializationResult(Result::ok())
{
	initializeEmptyMessage();
}

CtrlrMidiMessage::CtrlrMidiMessage (const String& hexData)  // Updated v5.6.35. Improved sanitization
    :   messageType(None), midiTree(Ids::midi), multiMasterValue(1),
        multiMasterNumber(1), messagePattern(0,true), restoring(false),
        initializationResult(Result::ok())
{
    initializeEmptyMessage();

    String sanitizedHex = hexData.removeCharacters(" \t\r\n");
    _DBG("CTOR HEX 1: Processing [" + sanitizedHex + "]");

    if (stringIsHexadecimal (sanitizedHex))
    {
        MemoryBlock rawData;
        rawData.loadFromHexString(sanitizedHex);
        
        if (rawData.getSize() > 0)
        {
            const uint8* data = (const uint8*)rawData.getData();
            
            if (data[0] < 0x80) {
                initializationResult = Result::fail("Invalid MIDI Status Byte");
                return;
            }

            MidiMessage m(rawData.getData(), (int)rawData.getSize());
            
            if (m.getRawDataSize() > 0)
            {
                // 1. Set the Type immediately
                if (m.isSysEx()) messageType = (CtrlrMidiMessageType)1;
                else if (m.isController()) messageType = (CtrlrMidiMessageType)2;
                
                // 2. Sync properties (This might try to clear things, so we do it first)
                initializationResult = fillMessagePropertiesFromJuceMidi(m);

                // 3. Clear and Force the Array - This is what getData() reads
                messageArray.clear();
                messageArray.add(m);

                // 4. Manual Pattern Sync
                messagePattern.replaceWith(rawData.getData(), rawData.getSize());

                // 5. Only run the engine for CC, skip for SysEx
                if (!m.isSysEx())
                {
                    patternChanged();
                }

                _DBG("CTOR HEX 5a: Final Pattern Size: " + String(messagePattern.getSize()));
                _DBG("CTOR HEX 5b: Array Count: " + String(messageArray.size()));
                _DBG("CTOR HEX 5c: toString Check: [" + toString() + "]");
            }
        }
    }
}

CtrlrMidiMessage::CtrlrMidiMessage (const MidiMessage& other)
	:	messageType(None), midiTree(Ids::midi), messagePattern(0,true),
		initializationResult(Result::ok())
{
	initializeEmptyMessage();

	/* Copy the passed in midi message
		initialize properties based on that message*/

	initializationResult = fillMessagePropertiesFromJuceMidi (other);

	if (!initializationResult.wasOk())
	{
		_WRN("CtrlrMidiMessage::ctor other MidiMessage was invalid");
	}
	else if (messageType == SysEx)
	{
		messageArray.add (other);
	}
}

CtrlrMidiMessage::CtrlrMidiMessage (MemoryBlock& other)
	:	messageType(None), midiTree(Ids::midi), messagePattern(0,true), initializationResult(Result::ok())
{
	initializeEmptyMessage();

	/* Copy the passed in midi message
		initialize properties based on that message*/

	MidiMessage m = MidiMessage(other.getData(), (int)other.getSize());

	initializationResult = fillMessagePropertiesFromJuceMidi (m);

	if (!initializationResult.wasOk())
	{
		_WRN("CtrlrMidiMessage::ctor other MemoryBlock was invalid");
	}
	else
	{
		// Sysex is not added automagicly (since we need a valid formula), do it now
		if (messageType == SysEx)
		{
			messageArray.add (m);
		}
	}
}

CtrlrMidiMessage::CtrlrMidiMessage (const luabind::object& tableData) // Added v5.6.35. Handle table to MIDI message from LUA
	:	messageType(None), midiTree(Ids::midi), multiMasterValue(1),
		multiMasterNumber(1), messagePattern(0,true),
		initializationResult(Result::ok())
{
	initializeEmptyMessage();
	MemoryBlock mb;

	if (luabind::type(tableData) == LUA_TTABLE)
	{
		try
		{
			for (luabind::iterator i(tableData), end; i != end; ++i)
			{
				uint8 b = (uint8)luabind::object_cast<int>(*i);
				mb.append (&b, 1);
			}

			if (mb.getSize() > 0)
			{
				// Check if the first byte is a valid status byte (bit 7 must be 1)
				const uint8* data = (const uint8*)mb.getData();
				if (data[0] < 0x80)
				{
					_WRN("CtrlrMidiMessage: First byte is not a valid status byte!");
					// You can choose to ignore this or handle it,
					// but passing it to MidiMessage is what causes the JUCE assertion.
				}
				initializationResult = fillMessagePropertiesFromData(mb);
				
				if (messageType == SysEx)
				{
					messageArray.add(MidiMessage(mb.getData(), (int)mb.getSize()));
				}
			}
		}
		catch (...)
		{
			_WRN("CtrlrMidiMessage::ctor failed to parse Lua table.");
			initializationResult = Result::fail("Failed to parse Lua table");
		}
	}
}

CtrlrMidiMessage::CtrlrMidiMessage (const CtrlrLuaObjectWrapper &luaArray)
	:	messageType(None), midiTree(Ids::midi), multiMasterValue(1),
		multiMasterNumber(1), messagePattern(0,true),
		initializationResult(Result::ok())
{
	initializeEmptyMessage();

	/* Convert the luaArray to a MemoryBlock treating it as an array of UINT8 numbers
		use the created MemoryBlock as initial data for a MidiMessage
		initialize properties based on that message*/

	MemoryBlock possibleMidiData = luaArrayTomemoryBlock(luaArray.getObject());

	if (possibleMidiData.getSize() > 0)
	{
		initializationResult = fillMessagePropertiesFromData(possibleMidiData);

		if (!initializationResult.wasOk())
		{
			_WRN("CtrlrMidiMessage::ctor LUA can't create midi message from input data: " + memoryBlockToString(possibleMidiData));
		}
		else
		{
			// Sysex is not added automagicly (since we need a valid formula), do it now
			if (messageType == SysEx)
			{
				messageArray.add (MidiMessage (possibleMidiData.getData(), (int)possibleMidiData.getSize()));
			}
		}
	}
	else
	{
		_WRN("CtrlrMidiMessage::ctor LUA input data is zero size");
	}
}

CtrlrMidiMessage::CtrlrMidiMessage (const CtrlrMidiMessage &other)
	: messageType(None), midiTree(other.midiTree), initializationResult(Result::ok()),
		messageArray(other.messageArray)
{

	fillMessagePropertiesFromData();

}

CtrlrMidiMessage::CtrlrMidiMessage (const Identifier &treeType)
    : messageType(None), midiTree(treeType), messagePattern(0,true), initializationResult(Result::ok())
{
    initializeEmptyMessage();
}

CtrlrMidiMessage::~CtrlrMidiMessage()
{
	midiTree.removeListener (this);
}

void CtrlrMidiMessage::initializeEmptyMessage()
{
	setProperty (Ids::midiMessageType, None);
	setProperty (Ids::midiMessageChannelOverride, false);
	setProperty (Ids::midiMessageChannel, 1);
	setProperty (Ids::midiMessageCtrlrNumber, 1);
	setProperty (Ids::midiMessageCtrlrValue, 0);
	setProperty (Ids::midiMessageCtrlrNumberSize, 0); // Added v5.6.35. For Multi MIDI Message. Added by DAM
	setProperty (Ids::midiMessageMultiList, "");
	setProperty (Ids::midiMessageSysExFormula, "");

	midiTree.addListener (this);
}

Result CtrlrMidiMessage::fillMessagePropertiesFromJuceMidi(const MidiMessage &m)
{
	if (m.isController())
	{
		messageType = CC;
		setProperty (Ids::midiMessageType, CC);
		setProperty (Ids::midiMessageChannel, m.getChannel());
		setProperty (Ids::midiMessageCtrlrNumber, m.getControllerNumber());
		setProperty (Ids::midiMessageCtrlrValue, m.getControllerValue());

		return (Result::ok());
	}
	else if (m.isAftertouch())
	{
		messageType = Aftertouch;
		setProperty (Ids::midiMessageType, Aftertouch);
		setProperty (Ids::midiMessageChannel, m.getChannel());
		setProperty (Ids::midiMessageCtrlrNumber, m.getNoteNumber());
		setProperty (Ids::midiMessageCtrlrValue, m.getAfterTouchValue());

		return (Result::ok());
	}
	else if (m.isNoteOn())
	{
		messageType = NoteOn;
		setProperty (Ids::midiMessageType, NoteOn);
		setProperty (Ids::midiMessageChannel, m.getChannel());
		setProperty (Ids::midiMessageCtrlrNumber, m.getNoteNumber());
		setProperty (Ids::midiMessageCtrlrValue, m.getVelocity());

		return (Result::ok());
	}
	else if (m.isNoteOff())
	{
		messageType = NoteOff;
		setProperty (Ids::midiMessageType, NoteOff);
		setProperty (Ids::midiMessageChannel, m.getChannel());
		setProperty (Ids::midiMessageCtrlrNumber, m.getNoteNumber());
		setProperty (Ids::midiMessageCtrlrValue, m.getVelocity());

		return (Result::ok());
	}
	else if (m.isChannelPressure())
	{
		messageType = ChannelPressure;
		setProperty (Ids::midiMessageType, ChannelPressure);
		setProperty (Ids::midiMessageChannel, m.getChannel());
		setProperty (Ids::midiMessageCtrlrValue, m.getChannelPressureValue());

		return (Result::ok());
	}
	else if (m.isProgramChange())
	{
		messageType = ProgramChange;
		setProperty (Ids::midiMessageType, ProgramChange);
		setProperty (Ids::midiMessageChannel, m.getChannel());
		setProperty (Ids::midiMessageCtrlrValue, m.getProgramChangeNumber());

		return (Result::ok());
	}
	else if (m.isPitchWheel())
	{
		messageType = PitchWheel;
		setProperty (Ids::midiMessageType, PitchWheel);
		setProperty (Ids::midiMessageChannel, m.getChannel());
		setProperty (Ids::midiMessageCtrlrValue, m.getPitchWheelValue());

		return (Result::ok());
	}
	else if (m.isSysEx())
	{
		messageType = SysEx;
		setProperty (Ids::midiMessageType, SysEx);
		return (Result::ok());
	}
	else if (m.isMidiClock())
	{
		messageType = MidiClock;
		setProperty (Ids::midiMessageType, MidiClock);
		return (Result::ok());
	}
	else if (m.isMidiContinue())
	{
		messageType = MidiClockContinue;
		setProperty (Ids::midiMessageType, MidiClockContinue);
		return (Result::ok());
	}
	else if (m.isMidiStart())
	{
		messageType = MidiClockStart;
		setProperty (Ids::midiMessageType, MidiClockStart);
		return (Result::ok());
	}
	else if (m.isMidiStop())
	{
		messageType = MidiClockStop;
		setProperty (Ids::midiMessageType, MidiClockStop);
		return (Result::ok());
	}
	else if (m.isActiveSense())
	{
		messageType = ActiveSense;
		setProperty (Ids::midiMessageType, ActiveSense);
		return (Result::ok());
	}
	else
	{
		messageType = None;
		setProperty (Ids::midiMessageType, None);

		return (Result::ok());
	}
}

Result CtrlrMidiMessage::fillMessagePropertiesFromData(const MemoryBlock &data)
{
	CtrlrMidiMessageEx mex(MidiMessage(data.getData(), (int)data.getSize()));

	if (mex.m.isController())
	{
		messageType = CC;
		setProperty (Ids::midiMessageType, CC);
		setProperty (Ids::midiMessageChannel, mex.m.getChannel());
		setProperty (Ids::midiMessageCtrlrNumber, mex.m.getControllerNumber());
		setProperty (Ids::midiMessageCtrlrValue, mex.m.getControllerValue());

		return (Result::ok());
	}
	else if (mex.m.isAftertouch())
	{
		messageType = Aftertouch;
		setProperty (Ids::midiMessageType, Aftertouch);
		setProperty (Ids::midiMessageChannel, mex.m.getChannel());
		setProperty (Ids::midiMessageCtrlrNumber, mex.m.getNoteNumber());
		setProperty (Ids::midiMessageCtrlrValue, mex.m.getAfterTouchValue());

		return (Result::ok());
	}
	else if (mex.m.isNoteOn())
	{
		messageType = NoteOn;
		setProperty (Ids::midiMessageType, NoteOn);
		setProperty (Ids::midiMessageChannel, mex.m.getChannel());
		setProperty (Ids::midiMessageCtrlrNumber, mex.m.getNoteNumber());
		setProperty (Ids::midiMessageCtrlrValue, mex.m.getVelocity());

		return (Result::ok());
	}
	else if (mex.m.isNoteOff())
	{
		messageType = NoteOff;
		setProperty (Ids::midiMessageType, NoteOff);
		setProperty (Ids::midiMessageChannel, mex.m.getChannel());
		setProperty (Ids::midiMessageCtrlrNumber, mex.m.getNoteNumber());
		setProperty (Ids::midiMessageCtrlrValue, mex.m.getVelocity());

		return (Result::ok());
	}
	else if (mex.m.isChannelPressure())
	{
		messageType = ChannelPressure;
		setProperty (Ids::midiMessageType, ChannelPressure);
		setProperty (Ids::midiMessageChannel, mex.m.getChannel());
		setProperty (Ids::midiMessageCtrlrValue, mex.m.getChannelPressureValue());

		return (Result::ok());
	}
	else if (mex.m.isProgramChange())
	{
		messageType = ProgramChange;
		setProperty (Ids::midiMessageType, ProgramChange);
		setProperty (Ids::midiMessageChannel, mex.m.getChannel());
		setProperty (Ids::midiMessageCtrlrValue, mex.m.getProgramChangeNumber());

		return (Result::ok());
	}
	else if (mex.m.isPitchWheel())
	{
		messageType = PitchWheel;
		setProperty (Ids::midiMessageType, PitchWheel);
		setProperty (Ids::midiMessageChannel, mex.m.getChannel());
		setProperty (Ids::midiMessageCtrlrValue, mex.m.getPitchWheelValue());

		return (Result::ok());
	}
	else if (mex.m.isSysEx())
	{
		messageType = SysEx;
		setProperty (Ids::midiMessageType, SysEx);
		return (Result::ok());
	}
	else if (mex.m.isMidiClock())
	{
		messageType = MidiClock;
		setProperty (Ids::midiMessageType, MidiClock);
		return (Result::ok());
	}
	else if (mex.m.isMidiContinue())
	{
		messageType = MidiClockContinue;
		setProperty (Ids::midiMessageType, MidiClockContinue);
		return (Result::ok());
	}
	else if (mex.m.isMidiStart())
	{
		messageType = MidiClockStart;
		setProperty (Ids::midiMessageType, MidiClockStart);
		return (Result::ok());
	}
	else if (mex.m.isMidiStop())
	{
		messageType = MidiClockStop;
		setProperty (Ids::midiMessageType, MidiClockStop);
		return (Result::ok());
	}
	else if (mex.m.isActiveSense())
	{
		messageType = ActiveSense;
		setProperty (Ids::midiMessageType, ActiveSense);
		return (Result::ok());
	}
	else
	{
		messageType = None;
		setProperty (Ids::midiMessageType, None);

		return (Result::ok());
	}
}

Result CtrlrMidiMessage::fillMessagePropertiesFromData()
{
	if (messageArray.size() == 0)
	{
		setProperty (Ids::midiMessageType, (uint8)None);
		setProperty (Ids::midiMessageChannelOverride, false);
		setProperty (Ids::midiMessageChannel, 1);
		setProperty (Ids::midiMessageCtrlrNumber, 1);
		setProperty (Ids::midiMessageCtrlrValue, 0);
		setProperty (Ids::midiMessageCtrlrNumberSize, 0); // Added v5.6.35. For Multi MIDI Message. Thanks to @dnaldoog
		setProperty (Ids::midiMessageMultiList, "");
		setProperty (Ids::midiMessageSysExFormula, "");

		return (Result::fail("MessageArray is empty, initializing to empty NONE message"));
	}
	else
	{
		return (fillMessagePropertiesFromData( MemoryBlock (messageArray[0].m.getRawData(), messageArray[0].m.getRawDataSize()) ));
	}
}

void CtrlrMidiMessage::restoreState (const ValueTree &savedState)
{
	for (int i=0; i<savedState.getNumProperties(); i++)
	{
		midiTree.setProperty (savedState.getPropertyName(i), savedState.getProperty(savedState.getPropertyName(i), 0), 0);
	}

	for (int i=0; i<savedState.getNumChildren(); i++)
	{
		midiTree.addChild (savedState.getChild(i).createCopy(), -1, 0);
	}

	if (getProperty(Ids::midiMessageSysExFormula).toString().length() > 0 && (int)getProperty(Ids::midiMessageType) != SysEx)
        
	{
		setProperty (Ids::midiMessageSysExFormula, "");
	}
	else if (getProperty(Ids::midiMessageSysExFormula).toString().length() > 0 && (int)getProperty(Ids::midiMessageType) == SysEx)
	{
		setProperty (Ids::midiMessageSysExFormula, getProperty(Ids::midiMessageSysExFormula));
		setProperty (Ids::midiMessageType, SysEx);
	}
}

void CtrlrMidiMessage::setValue (const int value)
{
	switch (messageType)
	{
		case CC:
		case Aftertouch:
		case ChannelPressure:
		case NoteOn:
		case NoteOff:
		case SysEx:
		case PitchWheel:
		case ProgramChange:
			setValueToSingle (0, value);
			break;
		case Multi:
			setValueToMulti (value);
			break;
		case MidiClock:
		case MidiClockContinue:
		case MidiClockStop:
		case kMidiMessageType:
		case ActiveSense:
		case MidiClockStart:
		case None:
			break;
	}
}

void CtrlrMidiMessage::setValueToSingle(const int index, const int value)
{
	if (messageArray.size() > index && messageArray.getReference(index).overrideValue == -2)
	{
		return;
	}
	else if ( (messageArray.size() > index && messageArray.getReference(index).m.isSysEx())
			|| (messageArray.size() > index && messageType == SysEx))
	{
		if (getSysexProcessor())
		{
			getSysexProcessor()->sysExProcess (messageArray.getReference(index).getTokenArray(), messageArray.getReference(index).m, value, getChannel());
		}
	}
	else
	{
		if (messageArray.size() > index)
			messageArray.getReference(index).setValue (value);
	}
}

void CtrlrMidiMessage::setValueToMulti (const int value)
{
	for (int i=0; i<messageArray.size(); i++)
	{
		setValueToSingle (i, value);
	}
}

int CtrlrMidiMessage::getValue()
{
	if (messageType != Multi && messageType != SysEx)
	{
		if (messageArray.size() == 1)
			return (messageArray.getReference(0).getValue());
		else
			return (0);
	}
	else if (messageType == Multi)
	{
		BigInteger value(0);

		for (int i=0; i<messageArray.size(); i++)
		{
			if (messageArray.getReference(i).overrideValue == -2)
				continue;

			value |= messageArray.getReference(i).getBitValue();
		}
		return (value.getBitRangeAsInt(0,14));
	}
	else if (messageType == SysEx)
	{
		if (messageArray.size() == 1)
		{
			if (getSysexProcessor())
			{
				return (getSysexProcessor()->getValueFromSysExData (messageArray.getReference(0).getTokenArray(), messageArray.getReference(0)));
			}
			else
			{
				return (CtrlrSysexProcessor::getValue (messageArray.getReference(0).getTokenArray(), messageArray.getReference(0)));
			}
		}
	}

	return (1);
}

void CtrlrMidiMessage::setMultiMessageFromString(const String& savedState)
{
    // Clear old
    multiMessages.clear();
    messageArray.clear();

    const String s = savedState.trim();
    if (s.isEmpty())
        return;

    // --- NEW/UPDATED REDIRECTION LOGIC ---
    // If the saved string contains the old NRPN/RPN tokens OR if it contains the new
    // split tokens (which we want to abandon), redirect the whole string to the
    // static CtrlrSysexProcessor::setMultiMessageFromString handler.
    if (s.containsIgnoreCase("ByteValue") || s.containsIgnoreCase("MSB7bitValue") || s.containsIgnoreCase("LSB7bitValue"))
    {
        // Your legacy handler needs the original full string to parse all colon-separated parts.

        // This static function is assumed to populate messageArray or another internal structure.
        // **IMPORTANT:** Ensure the function signature is correct for your codebase!
        CtrlrSysexProcessor::setMultiMessageFromString(*this, s);

        return; // Skip the rest of the new parser
    }

    // helper to interpret token (handles "Direct"/"Default" etc. as -1)
    auto tokenToInt = [](const String& tok) -> int {
        const String t = tok.trim();
        if (t.isEmpty()) return -1;
        if (t.equalsIgnoreCase("Direct") || t.equalsIgnoreCase("Default") || t.equalsIgnoreCase("Value"))
            return -1;
        if (t.equalsIgnoreCase("Number") || t.equalsIgnoreCase("CtrlNumber") || t.equalsIgnoreCase("ByteValue"))
            return -2;
        if (t.startsWithChar('-')) return t.getIntValue();
        if (t.containsOnly("0123456789")) return t.getIntValue();
        if (t.containsOnly("0123456789ABCDEFabcdef ") && t.length() <= 2)
            return t.getHexValue32();
        return -1;
        };

    // Split colon-separated messages (new multi-message)
    StringArray msgs;
    msgs.addTokens(s, ":", "\"\'");
    msgs.trim();
    msgs.removeEmptyStrings();

    for (int i = 0; i < msgs.size(); ++i)
    {
        String msgStr = msgs[i].trim();
        if (msgStr.isEmpty()) continue;

        // split on commas
        StringArray parts;
        parts.addTokens(msgStr, ",", "\"\'");
        parts.trim();
        parts.removeEmptyStrings();

        MultiMessage mm;
        mm.midiType = parts[0].trim();

        // Legacy 6-field format
        if (parts.size() == 6)
        {
            mm.numberToken = parts[3].getIntValue();
            mm.valueToken = parts[4].getIntValue();
            if (mm.midiType.equalsIgnoreCase("SysEx"))
                mm.sysexData = parts[5].trim();
            multiMessages.add(mm);
            continue;
        }

        // SysEx / Custom / Other new-style
        if (mm.midiType.equalsIgnoreCase("SysEx") || mm.midiType.equalsIgnoreCase("Custom"))
        {
            if (parts.size() >= 2)
            {
                String raw;
                for (int p = 1; p < parts.size(); ++p)
                {
                    if (raw.isNotEmpty()) raw += " ";
                    raw += parts[p].trim();
                }
                mm.sysexData = raw;
            }
            multiMessages.add(mm);
            continue;
        }

        // Standard messages
        if (mm.midiType.equalsIgnoreCase("CC") ||
            mm.midiType.equalsIgnoreCase("Aftertouch") ||
            mm.midiType.equalsIgnoreCase("ChannelPressure") ||
            mm.midiType.equalsIgnoreCase("NoteOn") ||
            mm.midiType.equalsIgnoreCase("NoteOff") ||
            mm.midiType.equalsIgnoreCase("PitchWheel"))
        {
            mm.numberToken = (parts.size() > 1) ? tokenToInt(parts[1]) : -1;
            mm.valueToken = (parts.size() > 2) ? tokenToInt(parts[2]) : -1;
            multiMessages.add(mm);
            continue;
        }

        // ProgramChange (single param)
        if (mm.midiType.equalsIgnoreCase("ProgramChange") || mm.midiType.equalsIgnoreCase("Program"))
        {
            mm.numberToken = (parts.size() > 1) ? tokenToInt(parts[1]) : -1;
            mm.valueToken = -1;
            multiMessages.add(mm);
            continue;
        }

        // Unknown / generic numeric
        mm.numberToken = (parts.size() > 1) ? tokenToInt(parts[1]) : -1;
        mm.valueToken = (parts.size() > 2) ? tokenToInt(parts[2]) : -1;
        multiMessages.add(mm);
    }

    // Build actual MIDI messages
    buildMidiMessagesFromMulti();
}

void CtrlrMidiMessage::buildMidiMessagesFromMulti()
{
    // Get panel/global info
    const bool channelOverride = getProperty(Ids::midiMessageChannelOverride);
    const int localChannel = getProperty(Ids::midiMessageChannel);
    const int globalChannel = getChannel(); // panel/global channel

    const int componentNumber = getNumber();
    const int componentValue = getValue();

    messageArray.clear();

    // Helper to resolve tokens (-1 / -2 / numbers)
    auto resolveToken = [&](int token, int defaultValue) -> int {
        if (token == -2) return componentNumber;
        if (token == -1) return componentValue;
        if (token >= 0) return token;
        return defaultValue;
        };

    // Helper to determine channel per message
    auto getChannelForMultiMessage = [&](const MultiMessage& mm, const String& midiData = String()) -> int
        {
            // 1?? Hard-coded channel in first byte of Custom message
            if (midiData.isNotEmpty())
            {
                StringArray bytes;
                bytes.addTokens(midiData, " ", "");
                bytes.trim();
                bytes.removeEmptyStrings();

                if (bytes.size() > 0)
                {
                    String firstByte = bytes[0].trim();
                    if (firstByte.length() == 2)
                    {
                        int statusByte = firstByte.getHexValue32();
                        if (statusByte >= 0x80 && statusByte <= 0xEF)
                        {
                            int hardCodedChannel = (statusByte & 0x0F) + 1;
                            return hardCodedChannel;
                        }
                    }
                }
            }

            // 2?? Channel override requested ? use local channel
            if (channelOverride || mm.numberToken == -2 || mm.valueToken == -2)
                return jmax(localChannel, 1);

            // 3?? Otherwise ? global/panel channel
            return jmax(globalChannel, 1);
        };

    // Build each message
    for (const auto& mm : multiMessages)
    {
        CtrlrMidiMessageEx mex;

        if (mm.midiType.equalsIgnoreCase("CC"))
        {
            int ccNum = resolveToken(mm.numberToken, componentNumber);
            int ccVal = resolveToken(mm.valueToken, componentValue);
            int channel = getChannelForMultiMessage(mm);
            mex.m = MidiMessage::controllerEvent(channel, jmin(ccNum, 127), jmin(ccVal, 127));
            mex.overrideValue = mm.valueToken;
        }
        else if (mm.midiType.equalsIgnoreCase("ProgramChange"))
        {
            int program = resolveToken(mm.numberToken, componentValue);
            int channel = getChannelForMultiMessage(mm);
            mex.m = MidiMessage::programChange(channel, jmin(program, 127));
            mex.overrideValue = mm.numberToken;
        }
        else if (mm.midiType.equalsIgnoreCase("Aftertouch"))
        {
            int note = resolveToken(mm.numberToken, componentNumber);
            int pressure = resolveToken(mm.valueToken, componentValue);
            int channel = getChannelForMultiMessage(mm);
            mex.m = MidiMessage::aftertouchChange(channel, jmin(note, 127), jmin(pressure, 127));
            mex.overrideValue = mm.valueToken;
        }
        else if (mm.midiType.equalsIgnoreCase("ChannelPressure"))
        {
            int pressure = resolveToken(mm.numberToken, componentValue);
            int channel = getChannelForMultiMessage(mm);
            mex.m = MidiMessage::channelPressureChange(channel, jmin(pressure, 127));
            mex.overrideValue = mm.numberToken;
        }
        else if (mm.midiType.equalsIgnoreCase("NoteOn"))
        {
            int note = resolveToken(mm.numberToken, componentNumber);
            int velocity = resolveToken(mm.valueToken, componentValue);
            int channel = getChannelForMultiMessage(mm);
            mex.m = MidiMessage::noteOn(channel, jmin(note, 127), (uint8)jmin(velocity, 127));
            mex.overrideValue = mm.valueToken;
        }
        else if (mm.midiType.equalsIgnoreCase("NoteOff"))
        {
            int note = resolveToken(mm.numberToken, componentNumber);
            int velocity = resolveToken(mm.valueToken, componentValue);
            int channel = getChannelForMultiMessage(mm);
            mex.m = MidiMessage::noteOff(channel, jmin(note, 127), (uint8)jmin(velocity, 127));
            mex.overrideValue = mm.valueToken;
        }
        else if (mm.midiType.equalsIgnoreCase("PitchWheel"))
        {
            int val = resolveToken(mm.numberToken, componentValue);
            int channel = getChannelForMultiMessage(mm);
            mex.m = MidiMessage::pitchWheel(channel, jmin(val, 16383));
            mex.overrideValue = mm.numberToken;
        }
        else if (mm.midiType.equalsIgnoreCase("SysEx") || mm.midiType.equalsIgnoreCase("Custom"))
        {
            if (mm.sysexData.isNotEmpty())
            {
                int channel = getChannelForMultiMessage(mm, mm.sysexData);
                //mex.m = midiMessageExfromString(mm.sysexData, channel, componentNumber, componentValue).m; // don't use this -> needs the old 6 value csv
                mex = CtrlrSysexProcessor::sysexMessageFromString(mm.sysexData, componentValue, channel);
            }
        }

        messageArray.add(mex);
    }
    if (messageArray.size() > 1)
    {
        messageType = Multi;
        setProperty(Ids::midiMessageType, Multi);
    }

    patternChanged();
}

void CtrlrMidiMessage::setNumber(const int number)
{
	multiMasterNumber = number;

	switch (messageType)
	{
		case CC:
		case Aftertouch:
		case ChannelPressure:
		case NoteOn:
		case NoteOff:
		case PitchWheel:
		case ProgramChange:
			setNumberToSingle (0, number);
			break;

		case Multi:
			setNumberToMulti (number);
			break;

		case SysEx:
		case None:
		case MidiClock:
		case MidiClockContinue:
		case MidiClockStop:
		case kMidiMessageType:
		case ActiveSense:
		case MidiClockStart:
			break;
	}
}

void CtrlrMidiMessage::setNumberToSingle (const int index, const int number)
{
	if (index >= messageArray.size())
		return;

	if (messageArray.getReference(index).overrideValue == -2)
	{
		messageArray.getReference(index).setValue (number);
	}
	else
	{
		messageArray.getReference(index).setNumber (number);
	}
}

void CtrlrMidiMessage::setNumberToMulti (const int number)
{
	for (int i=0; i<messageArray.size(); i++)
	{
		setNumberToSingle (i, number);
	}
}

int CtrlrMidiMessage::getNumber() const
{
	if (messageType != Multi && messageType != SysEx)
	{
		if (messageArray.size() == 1)
			return (messageArray.getReference(0).getNumber());
		else
			return (1);
	}
	else if (messageType == Multi)
	{
		return (multiMasterNumber);
	}
	else
	{
		return (1);
	}
}

CtrlrMidiMessageType CtrlrMidiMessage::getMidiMessageType() const
{
	return (messageType);
}

int CtrlrMidiMessage::getChannel() const
{
	if (messageArray.size() >= 1)
	{
		if (!messageArray.getReference(0).m.isSysEx())
		{
			return (jmax <int>(messageArray.getReference(0).m.getChannel(),1));
		}
		else
		{
			return (jmax <int>(getProperty(Ids::midiMessageChannel),1));
		}
	}

	return (jmax <int>(getProperty(Ids::midiMessageChannel),1));
}

void CtrlrMidiMessage::setChannel(const int midiChannel)
{
	for (int i=0; i<messageArray.size(); i++)
	{
		messageArray.getReference(i).setChannel (jmax<int>(midiChannel,1));
	}
}

void CtrlrMidiMessage::setMidiMessageType (const CtrlrMidiMessageType newType)
{
	const int ch = getChannel();

	switch (newType)
	{
		case CC:
			messageArray.clear();
			messageArray.add (MidiMessage::controllerEvent(ch, jmin<int>(getNumber(),127), jmin<int>(getValue(),127)));
			break;

		case Aftertouch:
			messageArray.clear();
			messageArray.add (MidiMessage::aftertouchChange(ch, jmin<int>(getNumber(),127), jmin<int>(getValue(),127)));
			break;

		case ChannelPressure:
			messageArray.clear();
			messageArray.add (MidiMessage::channelPressureChange (ch, jmin<int>(getValue(),127)));
			break;

		case NoteOn:
			messageArray.clear();
			messageArray.add (MidiMessage::noteOn (ch, jmin<int>(getNumber(),127), (uint8)jmin<int>(getValue(),127)));
			break;

		case NoteOff:
			messageArray.clear();
			messageArray.add (MidiMessage::noteOff (ch, jmin<int>(getNumber(),127), (uint8)jmin<int>(getValue(),127)));
			break;

		case SysEx:
			messageArray.clear();

			if (getProperty(Ids::midiMessageSysExFormula).toString().length() > 0)
			{
				messageArray.add (CtrlrSysexProcessor::sysexMessageFromString(getProperty(Ids::midiMessageSysExFormula),getValue(),getChannel()));
				patternChanged();
			}

			break;

		case Multi:
			// CtrlrSysexProcessor::setMultiMessageFromString (*this, getProperty(Ids::midiMessageMultiList));
			setMultiMessageFromString(getProperty(Ids::midiMessageMultiList).toString()); // Updated v5.6.35. For Multi MIDI Message. Thanks to @dnaldoog
            break;

		case PitchWheel:
			messageArray.clear();
			// messageArray.add (MidiMessage::pitchWheel (ch, (uint8)jmin<int>(getValue(),16383)));
            // remove the cast to uint8 - pitchWheel needs the full 14-bit value
			messageArray.add(MidiMessage::pitchWheel(ch, jmin<int>(getValue(), 16383))); // Updated v5.6.35. For Multi MIDI Message. Thanks to @dnaldoog
			break;

		case ProgramChange:
			messageArray.clear();
			messageArray.add (MidiMessage::programChange (ch, (uint8)jmin<int>(getValue(),127)));
			break;

		case MidiClock:
			messageArray.clear();
			messageArray.add (MidiMessage::midiClock());
			break;

		case MidiClockContinue:
			messageArray.clear();
			messageArray.add (MidiMessage::midiContinue());
			break;

		case MidiClockStop:
			messageArray.clear();
			messageArray.add (MidiMessage::midiStop());
			break;

		case MidiClockStart:
			messageArray.clear();
			messageArray.add (MidiMessage::midiStart());
			break;

		case ActiveSense:
			messageArray.clear();
			messageArray.add (MidiMessage());
			break;

		case None:
			messageArray.clear();
			break;

		case kMidiMessageType:
			break;
	}

	messageType = newType;
}

void CtrlrMidiMessage::valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property)
{
	if (property == Ids::midiMessageType)
	{
		setMidiMessageType ((CtrlrMidiMessageType)(int)getProperty(Ids::midiMessageType));
		setNumber ((int)getProperty(Ids::midiMessageCtrlrNumber));
		patternChanged();
	}
	else if(property == Ids::midiMessageSysExFormula)
	{
		if (restoring == false && (int)getProperty(Ids::midiMessageType) == SysEx)
		{
			if (getProperty(property).toString().length() >= 1)
			{
				if (getSysexProcessor())
				{
					CtrlrSysexProcessor::setSysExFormula (*this, getProperty(Ids::midiMessageSysExFormula));
				}
				setValue ((int)getProperty(Ids::midiMessageCtrlrValue));
				patternChanged();
			}
		}
	}
	else if (property == Ids::midiMessageChannel)
	{
		setChannel (getProperty(Ids::midiMessageChannel));
	}
	else if (property == Ids::midiMessageCtrlrValue)
	{
		int newVal = (int)getProperty(Ids::midiMessageCtrlrValue);
		_DBG("CtrlrMidiMessage::valueTreePropertyChanged midiMessageCtrlrValue=" + String(newVal) + " messageArraySize=" + String((int)messageArray.size()));
		if (messageArray.size() > 0)
			_DBG("CtrlrMidiMessage BEFORE raw=" + String::toHexString(messageArray.getReference(0).m.getRawData(), messageArray.getReference(0).m.getRawDataSize()));
		setValue (newVal);
		if (messageArray.size() > 0)
			_DBG("CtrlrMidiMessage AFTER raw=" + String::toHexString(messageArray.getReference(0).m.getRawData(), messageArray.getReference(0).m.getRawDataSize()));
		return;
	}
	else if (property == Ids::midiMessageCtrlrNumber)
	{
		setNumber ((int)getProperty(Ids::midiMessageCtrlrNumber));
	}
	else if (property == Ids::midiMessageMultiList)
	{
		setMultiMessageFromString (getProperty(Ids::midiMessageMultiList));
		
		setNumber ((int)getProperty(Ids::midiMessageCtrlrNumber));
	}

	patternChanged();
}

MidiBuffer CtrlrMidiMessage::getMidiBuffer(const int startSample)
{
	midiBuffer.clear();

	for (int i=0; i<messageArray.size(); i++)
	{
		midiBuffer.addEvent (messageArray.getReference(i).m,startSample+i);
	}

	return (midiBuffer);
}

const LMemoryBlock CtrlrMidiMessage::getData() const
{
	MemoryBlock bl(0);
	for (int i=0; i<messageArray.size(); i++)
	{
		bl.append (messageArray.getReference(i).m.getRawData(), messageArray.getReference(i).m.getRawDataSize());
	}
	return (bl);
}

void CtrlrMidiMessage::patternChanged()
{
	messagePattern.setSize(0);

	for (int i=0; i<messageArray.size(); i++)
	{
		MemoryBlock bl(0);
		if (messageArray.getReference(i).overrideValue == -2)
		{
			bl = MemoryBlock(messageArray.getReference(i).m.getRawData(), messageArray.getReference(i).m.getRawDataSize());
		}
		else
		{
			bl = midiMessagePattern(messageArray.getReference(i), messageArray.getReference(i).getTokenArray(), getGlobalVariables());
		}

		messagePattern.append (bl.getData(), bl.getSize());
	}
}

void CtrlrMidiMessage::addMidiMessage (const MidiMessage &message)
{
	messageArray.add (message);
	patternChanged();
}

void CtrlrMidiMessage::clear()
{
	messageArray.clear();
	patternChanged();
}

CtrlrMidiMessageEx &CtrlrMidiMessage::getReference(const int messageIndex) const
{
	return ((CtrlrMidiMessageEx &)messageArray.getReference(messageIndex));
}

void CtrlrMidiMessage::memoryMerge (const CtrlrMidiMessage &otherMessage)
{
	if (otherMessage.getNumMessages() == getNumMessages())
	{
		for (int i=0; i<messageArray.size(); i++)
		{
			messageArray.getReference(i).m = otherMessage.getMidiMessageEx(i).m;
		}
	}
}

const MemoryBlock &CtrlrMidiMessage::getMidiPattern() const
{
	return (messagePattern);
}

Array <CtrlrMidiMessageEx> &CtrlrMidiMessage::getMidiMessageArray()
{
	return (messageArray);
}

const String CtrlrMidiMessage::toString() const
{
	_DBG("toString() called. Buffer Size: " + String(getData().getSize()) + " Pattern Size: " + String(messagePattern.getSize()));
    return (String::toHexString (getData().getData(), getData().getSize()));
}

int CtrlrMidiMessage::getSize() const
{
	return (getData().getSize());
}

void CtrlrMidiMessage::wrapForLua(lua_State *L)
{
	using namespace luabind;

	module(L)
    [
		class_<CtrlrMidiMessage>("CtrlrMidiMessage")
			// Ensure this is present - it allows CtrlrMidiMessage("F0 01 F7")
			.def(constructor<const String&>())
			
			// This handles the old way where a Lua table might have been passed
			.def(constructor<const CtrlrLuaObjectWrapper&>())
			
			.def(constructor<const MidiMessage&>())
	 
			.def(constructor<const luabind::object&>()) // Added v5.6.35. Direct Table support
			
			// This handles the new JUCE 6 way: CtrlrMidiMessage(MemoryBlock(...))
			.def(constructor<MemoryBlock&>())
			
			.enum_("CtrlrMidiMessageType")
			[
	            value("CC",				0),
				value("Aftertouch",		1),
				value("ChannelPressure",2),
				value("NoteOn",			3),
				value("NoteOff",		4),
				value("SysEx",			5),
				value("Multi",			6),
				value("ProgramChange",	7),
				value("PitchWheel",		8),
				value("None",			9),
				value("MidiClock",		10),
				value("MidiClockContinue", 11),
				value("MidiClockStop", 12),
				value("MidiClockStart", 13)
			]
			.def("setChannel", &CtrlrMidiMessage::setChannel)
			.def("getChannel", &CtrlrMidiMessage::getChannel)
			.def("setNumber", &CtrlrMidiMessage::setNumber)
			.def("getNumber", &CtrlrMidiMessage::getNumber)
			.def("setValue", &CtrlrMidiMessage::setValue)
			.def("getValue", &CtrlrMidiMessage::getValue)
			.def("getSize", &CtrlrMidiMessage::getSize)
			.def("getData", &CtrlrMidiMessage::getData)
			.def("getLuaData", &CtrlrMidiMessage::getData)
			.def("getType", &CtrlrMidiMessage::getMidiMessageType)
            .def("setType", &CtrlrMidiMessage::setMidiMessageType) // Added v5.6.33
			.def("getMidiMessageType", &CtrlrMidiMessage::getMidiMessageType)
            .def("setMidiMessageType", &CtrlrMidiMessage::setMidiMessageType) // Added v5.6.33
			// .def("toString", &CtrlrMidiMessage::toString)
			.def("toString", +[](const CtrlrMidiMessage& self) -> std::string { // Updated v5.6.35. Using Lambda here is safer
				return self.toString().toStdString();
			})
			.def("getInitializationResult", &CtrlrMidiMessage::getInitializationResult)
            .def("getProperty", (const var &(CtrlrMidiMessage::*)(const Identifier &) const) &CtrlrMidiMessage::getProperty) // Added v5.6.31
            .def("setProperty", (void (CtrlrMidiMessage::*)(const Identifier &, const var &, const bool))&CtrlrMidiMessage::setProperty) // Added v5.6.33
	];
}
