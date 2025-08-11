#include "stdafx.h"
#include "CtrlrLuaCodeTokeniser.h"
#include "CtrlrLuaCodeTokeniserFunctions.h"

CtrlrLuaCodeTokeniser::CtrlrLuaCodeTokeniser() {}
CtrlrLuaCodeTokeniser::~CtrlrLuaCodeTokeniser() {}

int CtrlrLuaCodeTokeniser::readNextToken(CodeDocument::Iterator& source)
{
    return CtrlrLuaCodeTokeniserFunctions::readNextToken(source);
}

CodeEditorComponent::ColourScheme CtrlrLuaCodeTokeniser::getDefaultColourScheme()
{
    struct Type
    {
        const char* name;
        uint32 colour;
    };

    const Type types[] =
    {
        { "Error",              0xffcc0000 },
        { "Comment",            0xff008000 },
        { "Keyword",            0xff0000cc },
        { "Operator",           0xff225500 },
        { "Identifier",         0xff000000 },
        { "Integer",            0xff880000 },
        { "Float",              0xff885500 },
        { "String",             0xff990099 },
        { "Bracket",            0xff000055 },
        { "Punctuation",        0xff004400 }
    };

    CodeEditorComponent::ColourScheme cs;

    for (unsigned int i = 0; i < sizeof(types) / sizeof(types[0]); ++i)
        cs.set(types[i].name, Colour(types[i].colour));

    return cs;
}

CodeEditorComponent::ColourScheme CtrlrLuaCodeTokeniser::getCustomColourScheme(const HashMap<String, Colour>& customColors)
{
    // Create default scheme manually since this is a static method
    struct Type
    {
        const char* name;
        uint32 colour;
    };

    const Type types[] =
    {
        { "Error",              0xffcc0000 },
        { "Comment",            0xff008000 },
        { "Keyword",            0xff0000cc },
        { "Operator",           0xff225500 },
        { "Identifier",         0xff000000 },
        { "Integer",            0xff880000 },
        { "Float",              0xff885500 },
        { "String",             0xff990099 },
        { "Bracket",            0xff000055 },
        { "Punctuation",        0xff004400 }
    };

    CodeEditorComponent::ColourScheme cs;

    // Set default colors
    for (unsigned int i = 0; i < sizeof(types) / sizeof(types[0]); ++i)
        cs.set(types[i].name, Colour(types[i].colour));

    DBG("Applied default colors to scheme");

    // Override with custom colors
    HashMap<String, Colour>::Iterator it(customColors);
    while (it.next())
    {
        DBG("Overriding " + it.getKey() + " with " + it.getValue().toString());
        cs.set(it.getKey(), it.getValue());
    }

    return cs;
}

StringArray CtrlrLuaCodeTokeniser::getTokenTypeNames()
{
    StringArray tokenTypes;
    tokenTypes.add("Error");
    tokenTypes.add("Comment");
    tokenTypes.add("Keyword");
    tokenTypes.add("Operator");
    tokenTypes.add("Identifier");
    tokenTypes.add("Integer");
    tokenTypes.add("Float");
    tokenTypes.add("String");
    tokenTypes.add("Bracket");
    tokenTypes.add("Punctuation");
    return tokenTypes;
}

bool CtrlrLuaCodeTokeniser::isReservedKeyword(const String& token) noexcept
{
    return CtrlrLuaCodeTokeniserFunctions::isReservedKeyword(token.getCharPointer(), token.length());
}