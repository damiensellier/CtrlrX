#ifndef __CTRLR_LUA_CODE_TOKENISER__
#define __CTRLR_LUA_CODE_TOKENISER__

#include "CtrlrMacros.h"

class CtrlrLuaCodeTokeniser : public LuaTokeniser
{
public:
    CtrlrLuaCodeTokeniser();
    ~CtrlrLuaCodeTokeniser();
    int readNextToken(CodeDocument::Iterator& source);

    // Modified to accept custom colors
	// static CodeEditorComponent::ColourScheme getDefaultColourScheme(); // XCODE ERROR REPORTED
    CodeEditorComponent::ColourScheme getDefaultColourScheme(); // Fixed 5.6.34. Non static
    static CodeEditorComponent::ColourScheme getCustomColourScheme(const HashMap<String, Colour>& customColors);
    static bool isReservedKeyword(const String& token) noexcept;
    static StringArray getTokenTypeNames();

    enum TokenType
    {
        tokenType_error = 0,
        tokenType_comment,
        tokenType_keyword,
        tokenType_operator,
        tokenType_identifier,
        tokenType_integer,
        tokenType_float,
        tokenType_string,
        tokenType_bracket,
        tokenType_punctuation
    };
    JUCE_LEAK_DETECTOR(CtrlrLuaCodeTokeniser);
};

#endif
