#pragma once

#include <JuceHeader.h>

/** Represents a single Lua method from the XML */
struct LuaMethod {
    juce::String name;
    juce::String parameters;
    bool isStatic;
};

/** Define the types for our icons in the SuggestionPopup */
enum SuggestionType {
    TypeClass,      // Icon "C"
    TypeMethod,     // Icon "M"
    TypeProperty,   // Icon "P" (New: for .x, .width, etc)
    TypeGlobal,     // Icon "V"
    TypeUtility,    // Icon "f"
	TypeStatic      // Icon "S"
};

struct SuggestionItem {
    juce::String text;
    SuggestionType type;
};

/** Represents a Lua class and its associated methods, properties, and constructors */
struct LuaClass {
    juce::String name;
	juce::String parentClass;
    juce::Array<LuaMethod> methods;       // Instance methods (accessed via :)
    juce::Array<LuaMethod> staticMethods; // Static methods (accessed via .)
    juce::StringArray properties;         // New: Member variables (accessed via .)
    juce::StringArray constructors;       // New: For bubble tips on Class()
};

class CtrlrLuaMethodAutoCompleteManager {
public:
    CtrlrLuaMethodAutoCompleteManager();
    ~CtrlrLuaMethodAutoCompleteManager() = default;

    void loadDefinitions();

    /** Global suggestions (Classes, Keywords, Globals) */
    std::vector<SuggestionItem> getGlobalSuggestions(const juce::String& prefix);

    /** * Context-Aware Suggestions:
     * If user types ':', includeInstance should be true.
     * If user types '.', includeStatic and includeProperties should be true.
     */
    std::vector<SuggestionItem> getMethodSuggestionsForClass(const juce::String& className,
                                                           const juce::String& prefix,
                                                           bool includeInstance,
                                                           bool includeStatic,
                                                           bool includeProperties);

    /** Returns parameter string for bubble tips, checking methods AND constructors */
    juce::String getMethodParams(const juce::String& className, const juce::String& methodNameOrClass);

	juce::String getClassNameForVariable(const juce::String& varName, const juce::String& currentDocumentText);
	
	/** Predicts the return type of a method to allow for chained autocomplete */
    juce::String resolveReturnType(const juce::String& className, const juce::String& methodName);
	
	/** Returns a list of all known class names in the database */
    juce::StringArray getClassNames() const;

private:
    juce::HashMap<juce::String, LuaClass> classes;
    juce::StringArray classNames;
    juce::StringArray allMethodNames;
    juce::StringArray utilityMethodNames;
	juce::CriticalSection lock; // The master lock for all definition data

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CtrlrLuaMethodAutoCompleteManager)
};
