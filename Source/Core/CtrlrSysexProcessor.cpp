#include "stdafx.h"
#include "CtrlrMidiMessage.h"
#include "CtrlrSysexProcessor.h"
#include "CtrlrUtilities.h"
#include "CtrlrPanel/CtrlrPanel.h"
#include "CtrlrLog.h"
#include <CtrlrPropertyEditors/CtrlrPropertyComponent.h>


CtrlrSysexProcessor::CtrlrSysexProcessor()
{
}

CtrlrSysexProcessor::~CtrlrSysexProcessor()
{
}

int CtrlrSysexProcessor::getGlobalIndex(const CtrlrSysexToken token)
{
	return (token.getAdditionalData());
}

void CtrlrSysexProcessor::sysExProcessToken (const CtrlrSysexToken token, uint8 *byte, const int value, const int channel)
{
    if (byte == NULL)
        return;
    if (token.getType() >= Nibble16bitLsb0 && token.getType() <= Nibble16bitMsb3)
    {
        _DBG("Processing token type=" + String(token.getType()) + " position=" + String(token.getPosition()) + " value=" + String(value));
    }
    BigInteger bi;

    switch (token.getType())
    {
        case ByteValue:
            *byte = (uint8)value;
            break;

        case ByteChannel:
            *byte = (uint8)jlimit (0,15, channel-1);
            break;

        case LSB7bitValue:
            *byte = (uint8)(value & 127);
            break;

        case MSB7bitValue:
            *byte = (uint8)((value & 0x3fff) >> 7); // Added v5.6.31 from dnaldoog
            break;

        case ByteChannel4Bit:
            bi = BigInteger ((uint8)jlimit (0,15, channel-1));
            bi.setBitRangeAsInt (4, 3, token.getAdditionalData());
            *byte = (uint8)bi.getBitRangeAsInt(0,7);
            break;

        case GlobalVariable:
            if (getGlobalVariables() [token.getAdditionalData()] >= 0)
                *byte = (uint8)getGlobalVariables() [token.getAdditionalData()];
            break;

        case LSB4bitValue:
            *byte = (uint8)(value & 0xf);
            break;

        case MSB4bitValue:
            *byte = (uint8)((value >> 4) & 0xf);
            break;

        case RolandSplitByte1:
            *byte = getRolandSplit (value, 1);
            break;

        case RolandSplitByte2:
            *byte = getRolandSplit (value, 2);
            break;

        case RolandSplitByte3:
            *byte = getRolandSplit (value, 3);
            break;

        case RolandSplitByte4:
            *byte = getRolandSplit (value, 4);
            break;
        case Nibble16bitLsb0:
            *byte = (uint8)(value & 0x0F);
            break;

        case Nibble16bitLsb1:
            *byte = (uint8)((value >> 4) & 0x0F);
            _DBG("Nibble16bitLsb1: wrote " + String::toHexString(byte, 1) + " at position");
            break;

        case Nibble16bitLsb2:
            *byte = (uint8)((value >> 8) & 0x0F);
            _DBG("Nibble16bitLsb1: wrote " + String::toHexString(byte, 1) + " at position");
            break;

        case Nibble16bitLsb3:
            *byte = (uint8)((value >> 12) & 0x0F);
            //_DBG("Nibble16bitLsb1: wrote " + String::toHexString(byte, 1) + " at position");
            break;

        case Nibble16bitMsb0:
            *byte = (uint8)((value >> 12) & 0x0F);
            //_DBG("Nibble16bitLsb1: wrote " + String::toHexString(byte, 1) + " at position");
            break;

        case Nibble16bitMsb1:
            *byte = (uint8)((value >> 8) & 0x0F);
            //_DBG("Nibble16bitLsb1: wrote " + String::toHexString(byte, 1) + " at position");
            break;

        case Nibble16bitMsb2:
            *byte = (uint8)((value >> 4) & 0x0F);
            //_DBG("Nibble16bitLsb1: wrote " + String::toHexString(byte, 1) + " at position");
            break;

        case Nibble16bitMsb3:
            *byte = (uint8)(value & 0x0F);
            //_DBG("Nibble16bitLsb1: wrote " + String::toHexString(byte, 1) + " at position");
            break;
        case CurrentProgram:
        case CurrentBank:
		case Ignore:
		case ChecksumRolandJP8080:
		case ChecksumXor:
		case ChecksumTechnics:
		case ChecksumOnesComplement:
		case ChecksumSummingSimple:
		case FormulaToken:
		case LUAToken:
		case NoToken:
			break;
	}
}

void CtrlrSysexProcessor::sysExProcess (const Array<CtrlrSysexToken> &tokens, MidiMessage &m, const int value, const int channel)
{
	if (tokens.size() != m.getRawDataSize())
		return;

	uint8 *dataPtr = (uint8 *)m.getRawData();

	for (int i=0; i<m.getRawDataSize(); i++)
	{
		sysExProcessToken (tokens.getReference(i), dataPtr+i, value, channel);
	}

	sysexProcessPrograms(tokens, m);
	sysexProcessChecksums(tokens, m);
}

void CtrlrSysexProcessor::sysexProcessChecksums(const Array<CtrlrSysexToken> &tokens, MidiMessage &m)
{
    if (tokens.size() != m.getRawDataSize())
        return;
    
    for (int i=0; i<m.getRawDataSize(); i++)
    {
        if (tokens.getReference(i).getType() == ChecksumRolandJP8080)
        {
            checksumRolandJp8080 (tokens.getReference(i), m);
        }
        if (tokens.getReference(i).getType() == ChecksumXor)
        {
			checksumXor(tokens.getReference(i), m);
        }
        if (tokens.getReference(i).getType() == ChecksumTechnics)
        {
            checksumTechnics (tokens.getReference(i), m);
        }
		if (tokens.getReference(i).getType() == ChecksumOnesComplement)
        {
            checksumOnesComplement (tokens.getReference(i), m);
        }
        if (tokens.getReference(i).getType() == ChecksumSummingSimple)
        {
			checksumSummingSimple(tokens.getReference(i), m);
        }
    }
}

void CtrlrSysexProcessor::sysexProcessPrograms(const Array<CtrlrSysexToken> &tokens, MidiMessage &m)
{
	return;
}


CtrlrMidiMessageEx CtrlrSysexProcessor::sysexMessageFromString (const String &formula, const int value, const int channel)
{
	MidiMessage m;
	MemoryBlock bl(0,true);
	StringArray tokens;
	tokens.addTokens (formula, " ;:", "\"");

	for (int i=0; i<tokens.size(); i++)
	{
		const int d = jmin<int> (tokens[i].getHexValue32(),255);
		bl.append (&d, 1);
	}

	Array <CtrlrSysexToken> tokenArray = sysExToTokenArray(formula);

	CtrlrMidiMessageEx mex;
	mex.m  = MidiMessage (bl.getData(), (int)bl.getSize());
	mex.setTokenArray(tokenArray);
	return (mex);
}

double CtrlrSysexProcessor::getValueFromSysExData (const Array<CtrlrSysexToken> &tokens, const CtrlrMidiMessageEx &message)
{
	double v = 0;

	for (int i=0; i<tokens.size(); i++)
	{
		if (tokens[i].isHoldingValue())
		{
			v += indirectReverseOperation (*(message.m.getRawData()+i), tokens[i].getType());
		}
	}

	return (v);
}

uint8 CtrlrSysexProcessor::getRolandSplit (const int value, const int byteNum)
{
	const double result4   = value/16;
	const double reminder4 = fmod((double)value, 16.0);

	const double result3   = result4/16;
	const double reminder3 = fmod(result4, 16.0);

	const double result2   = result3/16;
	const double reminder2 = fmod(result3, 16.0);
	const double reminder1 = fmod(result2, 16.0);

	switch (byteNum)
	{
		case 1:
			return (jlimit<uint8>(0,127,(uint8)reminder1));
		case 2:
			return (jlimit<uint8>(0,127,(uint8)reminder2));
		case 3:
			return (jlimit<uint8>(0,127,(uint8)reminder3));
		case 4:
			return (jlimit<uint8>(0,127,(uint8)reminder4));
		default:
			return (0);
	}
}

const StringArray CtrlrSysexProcessor::templatesPrepare()
{
	StringArray templates;

	for (int i=0; i<kMidiMessageType; i++)
	{
		templates.add (midiMessageTypeToString((CtrlrMidiMessageType)i));
	}

	return (templates);
}

/** Static functions
*/

double CtrlrSysexProcessor::getValue(const Array<CtrlrSysexToken> &tokens, const CtrlrMidiMessageEx &message)
{
	double v = -1;

	for (int i=0; i<tokens.size(); i++)
	{
		if (tokens[i].isHoldingValue())
		{
			v += indirectReverseOperation (*(message.m.getRawData()+i), tokens[i].getType());
		}
	}

	return (v);
}

void CtrlrSysexProcessor::setSysExFormula(CtrlrMidiMessage &message, const String &formulaString)
{
	/* parse the formula to byte format */
	message.getMidiMessageArray().clear();
	message.getMidiMessageArray().add (sysexMessageFromString(formulaString, message.getValue(), message.getChannel()));
}

void CtrlrSysexProcessor::setMultiMessageFromString(CtrlrMidiMessage &message, const String &savedState)
{
	if (savedState.trim() == "")
		return;

	StringArray messages;
	const int ch = message.getChannel();
	const int v  = message.getValue();
	const int n  = message.getNumber();

	message.getMidiMessageArray().clear();
	messages.addTokens (savedState.trim(), ":", "\"\'");

	for (int i=0; i<messages.size(); i++)
	{
		message.getMidiMessageArray().add (midiMessageExfromString (messages[i], ch, n, v));
	}
}

Array<CtrlrSysexToken> CtrlrSysexProcessor::sysExToTokenArray (const String &formula)
{
	Array <CtrlrSysexToken> tokensToReturn;

	StringArray ar;
	ar.addTokens (formula, " :;", "\"\'");

	for (int i=0; i<ar.size(); i++)
	{
		CtrlrSysexToken tokenToAdd (i, *ar[i].substring(0,1).toUTF8(), sysExIdentifyToken (ar[i]));

		if (tokenToAdd.getType() == ByteChannel4Bit)
		{
			tokenToAdd.setAdditionalData (ar[i].substring(0,1).getHexValue32());
		}
		if (tokenToAdd.getType() == GlobalVariable
			|| tokenToAdd.getType() == ChecksumRolandJP8080
			|| tokenToAdd.getType() == ChecksumTechnics // Added v5.6.34.
			|| tokenToAdd.getType() == ChecksumOnesComplement
			|| tokenToAdd.getType() == ChecksumSummingSimple
			|| tokenToAdd.getType() == ChecksumXor)
		{
			// tokenToAdd.setAdditionalData (ar[i].substring(1,2).getHexValue32());
			/*
			This old code was passing a hexString which works up to 9 but if you pass 10 ie z10
			it will be passed through as 16, so getter is changed to getIntValue()
			*/
			tokenToAdd.setAdditionalData(ar[i].substring(1).trim().getIntValue()); // Updated v5.6.34. Thanks to @dnaldoog
		}

		tokensToReturn.add (tokenToAdd);
	}

	return (tokensToReturn);
}

CtrlrSysExFormulaToken CtrlrSysexProcessor::sysExIdentifyToken(const String &s)
{
	if (s == "xx")
	{
		return (ByteValue);
	}
	if (s.endsWith("y") && !s.startsWith("y"))
	{
		return (ByteChannel4Bit);
	}
	if (s.startsWith("z"))
	{
		return (ChecksumRolandJP8080);
	}
	if (s.startsWith("O") && CharacterFunctions::isDigit(s[1]))
	{
		return (ChecksumOnesComplement);
	}
	if (s.startsWith("w"))
	{
		return (ChecksumSummingSimple);
	}
	if (s.startsWith("X") && CharacterFunctions::isDigit(s[1]))
	{
		_DBG("CtrlrSysexProcessor::sysExIdentifyToken - X token found, returning CurrentProgram");
		return (ChecksumXor);
	}
	if (s.startsWith("u"))
	{
		return (LUAToken);
	}
	if (s.startsWith ("v"))
	{
		return (FormulaToken);
	}
	if (s == "yy")
	{
		return (ByteChannel);
	}
	if (s == "LS")
	{
		return (LSB7bitValue);
	}
	if (s == "MS")
	{
		return (MSB7bitValue);
	}
	if (s == "ii")
	{
		return (Ignore);
	}
	if (s == "ls")
	{
		return (LSB4bitValue);
	}
	if (s == "ms")
	{
		return (MSB4bitValue);
	}

    if (s == "r1")
    {
        return (RolandSplitByte1);
    }
    if (s == "r2")
    {
        return (RolandSplitByte2);
    }
    if (s == "r3")
    {
        return (RolandSplitByte3);
    }
    if (s == "r4")
    {
        return (RolandSplitByte4);
    }
    if (s == "tp")
    {
        return (CurrentProgram);
    }
    if (s == "tb")
    {
        return (CurrentBank);
    }
    if (s == "tc")
    {
        return (ChecksumTechnics);
    }
    if (s == "q0")
    {
        return (Nibble16bitLsb0);
    }
    if (s == "q1")
    {
        return (Nibble16bitLsb1);
    }
    if (s == "q2")
    {
        return (Nibble16bitLsb2);
    }
    if (s == "q3")
    {
        return (Nibble16bitLsb3);
    }
    if (s == "Q0")
    {
        return (Nibble16bitMsb0);
    }
    if (s == "Q1")
    {
        return (Nibble16bitMsb1);
    }
    if (s == "Q2")
    {
        return (Nibble16bitMsb2);
    }
    if (s == "Q3")
    {
        return (Nibble16bitMsb3);
    }
    if (s.startsWith("k") || s.startsWith("p") || s.startsWith("n") || s.startsWith("o"))
    {
        return (GlobalVariable);
    }
    return (NoToken);
}

/** Checksum processors
*/

/*
 * Calculates a Technics-style checksum using a chained XOR operation.
 *
 * This function calculates the checksum over a specified range of bytes
 * within a SysEx message. The range starts after the F0 header.
 *
 * SM:
 * Checksum for checking data errors.
 * Based on EXCLUSIVE-OR operation from
 * IDC to CN. Where IDC = 0x50 MATSUSHITA ELECTRIC INDUSTRIAL CO LTD
 * CN = Subsequent Data up to the checksum byte.
 */

void CtrlrSysexProcessor::checksumTechnics(const CtrlrSysexToken token, MidiMessage& m)
{
    const int messageLength = m.getRawDataSize();
    const int tokenPos = token.getPosition();

    // For Technics, always start from byte 1 (0x50) up to but not including the tc token
    const int startByte = 1; // Start at manufacturer ID (0x50)

    // Bounds checking
    if (startByte >= tokenPos || tokenPos >= messageLength) {
        return;
    }

    uint8* ptr = (uint8*)m.getRawData();

    // Start with the first byte (0x50)
    uint8 chTotal = *(ptr + startByte);

    // XOR with subsequent bytes up to (but not including) the tc position
    for (int i = startByte + 1; i < tokenPos; i++)
    {
        chTotal ^= *(ptr + i);
    }

    // Store the checksum at the token position
    *(ptr + tokenPos) = chTotal;
}

void CtrlrSysexProcessor::checksumRolandJp8080(const CtrlrSysexToken token, MidiMessage &m) // Update v5.6.34. Thanks to @dnaldoog
{
	_DBG("I am Roland");
	const int startByte = token.getPosition() - token.getAdditionalData();
	uint8 chTotal		= 0;
	uint8 *ptr	= (uint8 *)m.getRawData();

	for (int i=startByte; i<token.getPosition(); i++)
	{
		chTotal = chTotal + *(ptr+i); // From v5.6.31
	}
	chTotal = (~chTotal + 1) & 0x7f; // Two's complement with mask
	*(ptr + token.getPosition()) = chTotal;
}

void CtrlrSysexProcessor::checksumOnesComplement(const CtrlrSysexToken token, MidiMessage& m)
{
	const int startByte = token.getPosition() - token.getAdditionalData();
	uint8 chTotal = 0;
	uint8* ptr = (uint8*)m.getRawData();

	for (int i = startByte; i < token.getPosition(); i++)
	{
		chTotal = chTotal + *(ptr + i);
	}
	chTotal = ~chTotal & 0x7f; // One's Complement
	*(ptr + token.getPosition()) = chTotal;
}

void CtrlrSysexProcessor::checksumSummingSimple(const CtrlrSysexToken token, MidiMessage &m)
{
	const int startByte = token.getPosition() - token.getAdditionalData();
	uint8 chTotal = 0;
	uint8 *ptr = (uint8 *)m.getRawData();
	for (int i=startByte; i<token.getPosition(); i++)
	{
		chTotal = chTotal + *(ptr + i); // Some implementations just add up the data, but still need to mask it to 7 bits
	}
	chTotal = chTotal & 0x7f; // Mask to 7 bits
	*(ptr+token.getPosition()) = chTotal;
}

void CtrlrSysexProcessor::checksumXor(const CtrlrSysexToken token, MidiMessage& m)
{
    _DBG("token I am checksumXor()");
    const int startByte = token.getPosition() - token.getAdditionalData();
    uint8 chTotal = 0;
    uint8* ptr = (uint8*)m.getRawData();
    for (int i = startByte; i < token.getPosition(); i++)
    {
        chTotal ^= *(ptr + i);
    }
    *(ptr + token.getPosition()) = chTotal & 0x7f;
}

String CtrlrSysexProcessor::openAdvancedMessageEditor() // Updated v5.6.35. For Multi MIDI Message. Thanks to @dnaldoog . Updated by dam for juce 6 & Juce 8
{
    MultiMidiAlert alert;
    
    // Optional: Center it relative to the main editor if available,
    // or just center on screen.
    alert.centreAroundComponent(nullptr, alert.getWidth(), alert.getHeight());

    if (alert.runModalLoop() == 1)
    {
        const String newMsg = alert.getValue();
        if (newMsg.isNotEmpty())
            return newMsg;
    }

    return String();
}

void CtrlrSysexProcessor::showMidiHelp()
{
    const String helpText =
        "MIDI Message Conventions:\n\n"
        "-1       = Parent component value\n"
        "-2       = Parent component number\n\n"
        "SysEx Messages:\n"
        "Use the same formula as in the SysEx editor (F0 .. F7)\n\n"
        "Use the Custom ... option to add custom MIDI.\n\n"
        "NRPN/RPN templates are pre-defined for common controller mappings.";

#if JUCE_MAJOR_VERSION >= 8
    // --- JUCE 8 Logic (Custom Layout) ---
    auto* alert = new AlertWindow("MIDI Message Help", String(), AlertWindow::InfoIcon);

    auto* messageLabel = new Label(String(), helpText);
    messageLabel->setFont(Font(15.0f));
    messageLabel->setColour(Label::textColourId, alert->findColour(AlertWindow::textColourId));
    messageLabel->setSize(460, 180);
    
    alert->addCustomComponent(messageLabel);
    alert->addButton("OK", 1, KeyPress(KeyPress::returnKey, 0, 0));
    alert->setSize(560, 330);

    // This is the part that fails in JUCE 6
    if (auto* okButton = alert->getButton("OK"))
    {
        const int bW = 80;
        const int bH = 40;
        okButton->setBounds((alert->getWidth() - bW) / 2,
                             alert->getHeight() - bH - 30,
                             bW, bH);
    }

#else
    // --- JUCE 6 Logic (Standard Layout) ---
    #if JUCE_LINUX
        auto* alert = new AlertWindow("MIDI Message Help", helpText, AlertWindow::InfoIcon);
        alert->addButton("OK", 1);
    #else
        // Use a stack-based or pointer-based window for Windows/macOS modal loop
        auto* alert = new AlertWindow("MIDI Message Help", helpText, AlertWindow::InfoIcon);
        alert->addButton("OK", 1);
    #endif
#endif

    // --- Unified Execution/Cleanup ---
#if JUCE_LINUX
    // Always async on Linux to avoid window manager deadlocks
    alert->enterModalState(true, ModalCallbackFunction::create([alert](int) {
        delete alert;
    }), true);
#else
    // For Windows/macOS
    alert->runModalLoop();
    delete alert;
#endif
}
