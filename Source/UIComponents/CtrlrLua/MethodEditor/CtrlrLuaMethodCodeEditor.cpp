#include "stdafx.h"
#include "CtrlrManager/CtrlrManager.h"
#include "CtrlrLuaMethodCodeEditor.h"
#include "CtrlrLuaMethodEditor.h"
#include "CtrlrPanel/CtrlrPanel.h"
#include "Methods/CtrlrLuaMethodManager.h"
#include "CtrlrLuaDebugger.h"
#include "CtrlrLuaManager.h"

// A simple popup window to show the Lua suggestions, matching the editor's LookAndFeel
class LuaSuggestionPopup : public juce::Component, public juce::ListBoxModel
{
public:
    // Updated the function signature to pass the whole SuggestionItem
    LuaSuggestionPopup(std::function<void(const SuggestionItem&)> onChosen) : onSelection(onChosen)
    {
        setAlwaysOnTop(true);
        setWantsKeyboardFocus(false);
        listBox.setColour(juce::ListBox::backgroundColourId, juce::Colours::transparentBlack);
        listBox.setOutlineThickness(0);
		// API CORRECT: Makes the list react to mouse moves by selecting the row
        listBox.setMouseMoveSelectsRows(true);
        
        // API CORRECT: Ensure clicking selects immediately
        listBox.setRowSelectedOnMouseDown(true);
        addAndMakeVisible(listBox);
        listBox.setModel(this);
        listBox.setRowHeight(22);
        listBox.setWantsKeyboardFocus(false);
    }

    void setSuggestions(const std::vector<SuggestionItem>& newSuggestions)
    {
        activeItems = newSuggestions;
        listBox.updateContent();
        if (activeItems.size() > 0) listBox.selectRow(0);
        
        int totalHeight = juce::jmin(10, (int)activeItems.size()) * 22;
        setSize(400, totalHeight); // was 300
        repaint();
    }

    SuggestionItem getSelectedItem()
    {
        int row = listBox.getSelectedRow();
        if (row >= 0 && row < (int)activeItems.size())
            return activeItems[row];
        
        return SuggestionItem();
    }

    void paint(juce::Graphics& g) override
    {
        auto bg = getLookAndFeel().findColour(juce::TextEditor::backgroundColourId);
        g.fillAll(bg);
        auto border = getLookAndFeel().findColour(juce::TextEditor::outlineColourId);
        g.setColour(border.withAlpha(0.6f));
        g.drawRect(getLocalBounds().toFloat(), 1.0f);
    }

    void resized() override { listBox.setBounds(getLocalBounds().reduced(1)); }

	int getNumRows() override { return (int)activeItems.size(); }
	
	void paintListBoxItem (int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override
	{
		if (rowNumber >= (int)activeItems.size()) return;
		
		// 1. Get Theme Colours
		auto highlightBg = getLookAndFeel().findColour(juce::TextEditor::highlightColourId);
		auto mainText = getLookAndFeel().findColour(juce::TextEditor::textColourId);
		auto highlightText = getLookAndFeel().findColour(juce::TextEditor::highlightedTextColourId);
		
		// 2. Draw Background
		if (rowIsSelected) {
			g.setColour(highlightBg); // Full opacity for the selection bar
			g.fillRect(0, 0, width, height);
		}
		
		auto& item = activeItems[rowNumber];
		auto iconArea = juce::Rectangle<float> (6.0f, 4.0f, (float)height - 8.0f, (float)height - 8.0f);
		
		// 3. Logic for Icon C/M/V/f
		juce::StringArray libraries = { "math", "table", "string", "utils" };
		SuggestionType displayType = item.type;
		if (libraries.contains(item.text)) displayType = TypeClass;
		
		juce::Colour iconColor;
		juce::String iconLetter;
		
		switch (displayType) {
			case TypeClass:   iconColor = juce::Colours::darkorange; iconLetter = "C"; break;
			case TypeMethod:  iconColor = juce::Colours::darkcyan;   iconLetter = "M"; break;
			case TypeStatic:  iconColor = juce::Colours::darkblue;   iconLetter = "S"; break;
			case TypeGlobal:  iconColor = juce::Colours::darkgreen; iconLetter = "V"; break;
			case TypeUtility: iconColor = juce::Colours::purple;   iconLetter = "f"; break;
			case TypeProperty: iconColor = juce::Colours::darkgoldenrod; iconLetter = "P"; break;
			default:          iconColor = mainText.withAlpha(0.5f); iconLetter = "?"; break;
		}
		
		// Draw Icon Box
		g.setColour(iconColor.withAlpha(0.4f));
		g.drawRoundedRectangle(iconArea, 3.0f, 1.0f);
		g.setColour(iconColor);
		g.setFont(juce::Font(height * 0.5f, juce::Font::bold));
		g.drawText(iconLetter, iconArea.getSmallestIntegerContainer(), juce::Justification::centred);
		
		// 4. Split Text for Multi-Style Rendering
		juce::String fullText = item.text;
		juce::String methodName = fullText.upToFirstOccurrenceOf(" (", false, false);
		juce::String params = fullText.fromFirstOccurrenceOf(" (", true, false);
		
		int textX = (int)iconArea.getRight() + 8;
		
		// --- DRAW METHOD NAME (High Contrast) ---
		// If selected, use highlight text color. If not, use 100% main text color.
		g.setColour(rowIsSelected ? highlightText : mainText);
		g.setFont(juce::Font(height * 0.65f, juce::Font::bold));
		
		int methodWidth = g.getCurrentFont().getStringWidth(methodName);
		g.drawText(methodName, textX, 0, methodWidth, height, juce::Justification::centredLeft);
		
		// --- DRAW PARAMETERS (Lower Contrast) ---
		if (params.isNotEmpty()) {
			// We use 0.6f alpha here to make it look "lighter" or "dimmed" compared to the name
			g.setColour(rowIsSelected ? highlightText.withAlpha(0.7f) : mainText.withAlpha(0.5f));
			g.setFont(juce::Font(height * 0.55f, juce::Font::plain));
			
			g.drawText(params, textX + methodWidth + 4, 0,
					   width - textX - methodWidth - 10, height,
					   juce::Justification::centredLeft);
		}
	}
	
    // Fixed the error: currentMatches -> activeItems
    void listBoxItemClicked (int row, const juce::MouseEvent&) override
    {
        commit(row);
    }

    void moveSelection(int delta) {
        int nextRow = juce::jlimit(0, (int)activeItems.size() - 1, listBox.getSelectedRow() + delta);
        listBox.selectRow(nextRow);
        listBox.scrollToEnsureRowIsOnscreen(nextRow);
    }

    void commitCurrent() { commit(listBox.getSelectedRow()); }

private:
    void commit(int row) {
        if (row >= 0 && row < (int)activeItems.size())
            onSelection(activeItems[row]); // Pass the whole SuggestionItem
    }

    juce::ListBox listBox;
    std::vector<SuggestionItem> activeItems;
    std::function<void(const SuggestionItem&)> onSelection;
};


// A call-tip for the suggestion popup
class LuaCallTip : public juce::Component
{
public:
    LuaCallTip() {
        setAlwaysOnTop(true);
        setInterceptsMouseClicks(false, false); // Don't block the mouse
    }
	
	void setTipText(const juce::String& newText) {
        if (tipText != newText) {
            tipText = newText;
            repaint();
        }
    }

    // ADD THIS GETTER:
    juce::String getTipText() const { return tipText; }
    
    void showTip(const juce::String& text, juce::Point<int> pos) {
        tipText = text;
        setVisible(true);
        
        juce::Font font(13.0f);
        int width = font.getStringWidth(tipText) + 20;
        setSize(width, 24);
        
        setTopLeftPosition(pos.x, pos.y - 30);
        toFront(false);
        repaint();
    }

    void paint(juce::Graphics& g) override {
        g.setColour(juce::Colours::darkgrey.withAlpha(0.9f));
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 4.0f);
        g.setColour(juce::Colours::white);
        g.setFont(13.0f);
        g.drawText(tipText, getLocalBounds(), juce::Justification::centred);
    }

private:
    juce::String tipText;
};


CtrlrLuaMethodCodeEditor::CtrlrLuaMethodCodeEditor(CtrlrLuaMethodEditor& _owner, CtrlrLuaMethod* _method, juce::Value& sharedSearchTabsValue_)
    : owner(_owner),
      method(_method),
      sharedSearchTabsValue(sharedSearchTabsValue_),
      lastFoundPosition(0),
      isReplacingText(false)
{
    // 1. Initialize the Editor Component
    // Note: ensure GenericCodeEditorComponent::keyPressed is overridden in its .cpp
    // to call this->keyPressed.
    addAndMakeVisible(editorComponent = new GenericCodeEditorComponent(*this,
        document, codeTokeniser = new CtrlrLuaCodeTokeniser()));

    // 2. Autocomplete & CallTip Initialization
    callTip = std::make_unique<LuaCallTip>();
    addChildComponent(callTip.get());

	// Change (juce::String chosen) to (const SuggestionItem& item)
	suggestionPopup = std::make_unique<LuaSuggestionPopup>([this](const SuggestionItem& item) {
		handleSuggestionChosen(item);
	});
	
    addChildComponent(suggestionPopup.get());

    // 3. Search Tabs Logic
    hiddenSearchTabsToggle = new ToggleButton("Hidden Search Tabs");
    hiddenSearchTabsToggle->setVisible(false);
    addAndMakeVisible(hiddenSearchTabsToggle);

    // Connect the hidden toggle to the shared value
    hiddenSearchTabsToggle->getToggleStateValue().referTo(SharedValues::getSearchTabsValue());
    SharedValues::getSearchTabsValue().addListener(this);

    // 4. Editor Styling & Content
    editorComponent->setScrollbarThickness(owner.getOwner().getOwner().getProperty(Ids::ctrlrScrollbarThickness));
    document.replaceAllContent(method->getCode());
    document.setSavePoint();

    // 5. EVENT ROUTING (Only for Autocomplete. Crucial Section)
    document.addListener(this);
    
    // Mouse listener is fine for clicks
    editorComponent->addMouseListener(this, true);
	editorComponent->addKeyListener(this);
	
	// editorComponent->setWantsKeyboardFocus(true); // Not sure if actually required
	// editorComponent->grabKeyboardFocus(); // Not sure if actually required
	
    editorComponent->setColour(CodeEditorComponent::backgroundColourId, Colours::white); // findColour(CodeEditorComponent::backgroundColourId));
    editorComponent->setColour(CodeEditorComponent::defaultTextColourId, Colours::black); // findColour(CodeEditorComponent::defaultTextColourId));
    editorComponent->setColour(CodeEditorComponent::highlightColourId, findColour(CodeEditorComponent::highlightColourId));
    editorComponent->setColour(CodeEditorComponent::lineNumberTextId, findColour(CodeEditorComponent::lineNumberTextId));
    editorComponent->setColour(CodeEditorComponent::lineNumberBackgroundId, findColour(CodeEditorComponent::lineNumberBackgroundId));

    if (method != nullptr)
        method->setCodeEditor(this);

    if (owner.getComponentTree().hasProperty(Ids::luaMethodEditorFont))
    {
        setFontAndColour(owner.getOwner().getCtrlrManagerOwner().getFontManager().getFontFromString(owner.getComponentTree().getProperty(Ids::luaMethodEditorFont)),
            VAR2COLOUR(owner.getComponentTree().getProperty(Ids::luaMethodEditorBgColour, Colours::white.toString()))); // (String) findColour(CodeEditorComponent::backgroundColourId).toString())));

        editorComponent->setColour(CodeEditorComponent::lineNumberTextId,
            VAR2COLOUR(owner.getComponentTree().getProperty(Ids::luaMethodEditorLineNumbersColour, Colours::grey.toString()))); // Added v5.6.31

        editorComponent->setColour(CodeEditorComponent::lineNumberBackgroundId,
            VAR2COLOUR(owner.getComponentTree().getProperty(Ids::luaMethodEditorLineNumbersBgColour, Colours::cyan.toString()))); // Added v5.6.31
    }
    else
    {
        setFontAndColour(Font(Font::getDefaultMonospacedFontName(), 14.0f, Font::plain), Colours::white); // findColour(CodeEditorComponent::backgroundColourId));
    }
}

CtrlrLuaMethodCodeEditor::~CtrlrLuaMethodCodeEditor()
{
    masterReference.clear();

    if (method)
        method->setCodeEditor(nullptr);

    // Remove the value listener FIRST before other cleanup
    //sharedSearchTabsValue.removeListener(this);
    SharedValues::getSearchTabsValue().removeListener(this);
    document.removeListener(this);
    deleteAndZero(editorComponent);
    deleteAndZero(codeTokeniser);

    // hiddenSearchTabsToggle will be automatically deleted by Component destructor
}

void CtrlrLuaMethodCodeEditor::resized()
{
    editorComponent->setBounds(0, 0, getWidth(), getHeight());
}

void CtrlrLuaMethodCodeEditor::mouseDown(const juce::MouseEvent& e)
{
    juce::CodeDocument::Position pos = editorComponent->getCaretPos();
    juce::String url;
    
    // --- AUTOCOMPLETE CLEANUP ---
    // 1. Hide the autocomplete popup if the user clicks the editor
    if (suggestionPopup)
        suggestionPopup->setVisible(false);

    // 2. Hide the call tip if the user clicks elsewhere
    if (callTip)
        callTip->setVisible(false);

    // 3. IMPORTANT: Reset the replacement lock.
    // If the user clicks manually, they are taking control back from the
    // automation, so we must ensure isReplacingText is false.
    isReplacingText = false;
    // ----------------------------

    if (isMouseOverUrl(pos, &url))
    {
        juce::URL(url).launchInDefaultBrowser();
    }
    
    owner.setPositionLabelText("Line:  " + juce::String(pos.getLineNumber() + 1) +
                               " Column: " + juce::String(pos.getIndexInLine()));
}

void CtrlrLuaMethodCodeEditor::mouseMove(const MouseEvent& e)
{
    if (e.eventComponent == editorComponent)
    {
        CodeDocument::Position pos = editorComponent->getPositionAt(e.x, e.y);
        if (isMouseOverUrl(pos))
        {
            editorComponent->setMouseCursor(MouseCursor::PointingHandCursor);
            return;
        }
        else if (editorComponent->getMouseCursor() == MouseCursor::PointingHandCursor)
        {
            editorComponent->setMouseCursor(MouseCursor::IBeamCursor);
        }
    }
}
bool CtrlrLuaMethodCodeEditor::keyStateChanged(bool isKeyDown, Component* originatingComponent)
{
    // This is the correct place to update the position label, as it is called
    // continuously as the caret moves via key repeats.
    if (isKeyDown)
    {
        CodeDocument::Position pos = editorComponent->getCaretPos();
        owner.setPositionLabelText("Line:  " + String(pos.getLineNumber() + 1) + " Column: " + String(pos.getIndexInLine()));
        
        // --- ADDED: Update highlight when moving with arrows or typing ---
        updateCallTipHighlight();
    }
    
    return false;
}

bool CtrlrLuaMethodCodeEditor::keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent)
{
    if (editorComponent == nullptr) return false;

    const int currentPos = editorComponent->getCaretPos().getPosition();
    juce::String allText = document.getAllContent();

	// --- 1. TAB JUMPING (For Argument Bubble / Commas) ---
	if (key == juce::KeyPress::tabKey)
	{
		// Check if the suggestion list is actually being shown to the user.
		// If it's NOT visible, or if we have a jump pending, the jump takes priority.
		bool popupIsShowing = (suggestionPopup != nullptr && suggestionPopup->isVisible());
		
		if (!popupIsShowing && nextTabJumpPosition != -1)
		{
			int targetPos = -1;
			juce::String allText = document.getAllContent();
			const int currentPos = editorComponent->getCaretPos().getPosition();
			
			// Scan from current caret forward
			for (int i = currentPos; i < allText.length(); ++i)
			{
				// Case A: Found a comma (jump to next argument)
				if (allText[i] == ',') {
					targetPos = i + 1;
					// If there's a space after the comma, land after the space
					if (i + 1 < allText.length() && allText[i + 1] == ' ')
						targetPos = i + 2;
					break;
				}
				
				// Case B: Found the closing bracket (jump out of function)
				if (allText[i] == ')') {
					targetPos = i + 1;
					nextTabJumpPosition = -1; // We are officially out
					break;
				}
				
				// Safety: if we hit a newline or semicolon before a comma or bracket,
				// we've probably left the function context.
				if (allText[i] == '\n' || allText[i] == ';') break;
			}
			
			if (targetPos != -1) {
				editorComponent->moveCaretTo(juce::CodeDocument::Position(document, targetPos), false);
				
				juce::MessageManager::callAsync([this]() {
					updateCallTipHighlight();
				});
				
				return true; // Stop the editor from inserting a '\t'
			}
		}
	}

    // --- 2. SUGGESTION LIST NAVIGATION ---
    if (suggestionPopup && suggestionPopup->isVisible())
    {
        if (key == juce::KeyPress::upKey)    { suggestionPopup->moveSelection(-1); return true; }
        if (key == juce::KeyPress::downKey)  { suggestionPopup->moveSelection(1);  return true; }
        
        if (key == juce::KeyPress::returnKey || key == juce::KeyPress::tabKey)
        {
            suggestionPopup->commitCurrent();
            return true;
        }
    }

    // --- 3. DISMISSAL (ESCAPE) ---
    if (key == juce::KeyPress::escapeKey)
    {
        bool handled = false;
        if (suggestionPopup && suggestionPopup->isVisible()) { suggestionPopup->setVisible(false); handled = true; }
        if (callTip && callTip->isVisible())                 { callTip->setVisible(false); handled = true; }
        if (handled) return true;
    }

    return false; // Let every other key (brackets, quotes, letters) behave normally
}

// --- The logic for the highlighting ---
void CtrlrLuaMethodCodeEditor::updateCallTipHighlight()
{
    if (callTip == nullptr)
        return;

    int caretPos = editorComponent->getCaretPos().getPosition();
    juce::String allText = document.getAllContent();

    // 1. Find the opening bracket for the CURRENT method
    int openBracket = allText.substring(0, caretPos).lastIndexOfChar('(');
    
    // If we just inserted a snippet, the caret might be RIGHT after or ON the '('
    if (openBracket == -1 && allText.length() > caretPos && allText[caretPos] == '(')
        openBracket = caretPos;

    // If no bracket found, or if there's a closing bracket before our caret, hide tip
    int closeBracket = allText.indexOfChar(openBracket, ')');
    if (openBracket == -1 || (closeBracket != -1 && caretPos > closeBracket))
    {
        callTip->setVisible(false);
        return;
    }

    // --- IDENTIFY METHOD NAME FROM TEXT (The "Source of Truth") ---
    int nameEnd = openBracket;
    while (nameEnd > 0 && juce::CharacterFunctions::isWhitespace(allText[nameEnd - 1]))
        nameEnd--;

    int methodStart = nameEnd;
    while (methodStart > 0 && (juce::CharacterFunctions::isLetterOrDigit(allText[methodStart - 1]) || allText[methodStart - 1] == '_'))
        methodStart--;

    juce::String actualMethodInText = allText.substring(methodStart, nameEnd);

    // FIX: Context Verification.
    // If we are typing a NEW method (like 'setHeight') but the bubble is
    // still trying to show the OLD method ('setBounds'), hide it.
    if (lastAutocompletedMethod.isNotEmpty() && actualMethodInText != lastAutocompletedMethod)
    {
        // If the user is manually typing and just hit '(', sync the name
        // Otherwise, if it's a mismatch during autocomplete navigation, hide it.
        if (actualMethodInText.isNotEmpty())
            lastAutocompletedMethod = actualMethodInText;
        else
        {
            callTip->setVisible(false);
            return;
        }
    }
    
    // Safety fallback
    if (actualMethodInText.isEmpty())
        actualMethodInText = lastAutocompletedMethod;

    if (actualMethodInText.isEmpty())
    {
        callTip->setVisible(false);
        return;
    }

    // 2. Get Parameters (Check Hardcoded Fallbacks in Manager)
    // We pass the lastAutocompletedClass for context resolution
    juce::String params = owner.getAutocompleteManager().getMethodParams(lastAutocompletedClass, actualMethodInText);
    
    if (params.isEmpty())
    {
        callTip->setVisible(false);
        return;
    }

    // 3. Count commas to determine parameter index
    int parameterIndex = 0;
    int nestingLevel = 0;
    for (int i = openBracket + 1; i < caretPos; ++i)
    {
        juce::juce_wchar c = allText[i];
        if (c == '(') nestingLevel++;
        else if (c == ')') nestingLevel--;
        else if (c == ',' && nestingLevel == 0) parameterIndex++;
    }

    // 4. Formatting logic
    // Grab the first line/overload if multiple exist
    juce::String firstOverload = params.contains("\n") ? params.upToFirstOccurrenceOf("\n", false, false) : params;
    juce::StringArray pArray;
    pArray.addTokens(firstOverload, ",", "");

    if (pArray.size() > 0)
    {
        if (parameterIndex >= pArray.size())
            parameterIndex = pArray.size() - 1;

        juce::String formattedTip = "(";
        for (int i = 0; i < pArray.size(); ++i)
        {
            juce::String p = pArray[i].trim();
            if (i == parameterIndex)
                formattedTip += "[" + p + "]";
            else
                formattedTip += p;

            if (i < pArray.size() - 1)
                formattedTip += ", ";
        }
        formattedTip += ")";

        // 5. Update UI
        callTip->setTipText(formattedTip);
        callTip->setVisible(true);
        
        juce::Font font(13.0f);
        int newWidth = font.getStringWidth(formattedTip) + 20;
        callTip->setSize(newWidth, 24);
        
        auto caretRect = editorComponent->getCaretRectangle();
        
        // Position slightly above the current line
        callTip->setTopLeftPosition(caretRect.getX() - (newWidth / 4), caretRect.getY() - 28);
        callTip->toFront(false);
    }
    else
    {
        callTip->setVisible(false);
    }
}

void CtrlrLuaMethodCodeEditor::hideCallTip()
{
    if (callTip != nullptr)
        callTip->setVisible(false);
    
    nextTabJumpPosition = -1;
}
void CtrlrLuaMethodCodeEditor::codeDocumentTextInserted(const juce::String& newText, int insertIndex)
{
	 if (!SharedValues::getAutoCompleteValue().getValue())
        return; // if disabled in preferences bail out
    // --- 1. AGGRESSIVE GUARD ---
    if (isReplacingText)
    {
        if (newText != ":" && newText != ".")
            return;

        _DBG("AUTOCOMPLETE: Bypassing lock for manual flow re-trigger [" + newText + "]");
    }

    if (callTip && nextTabJumpPosition == -1)
        callTip->setVisible(false);

    document.newTransaction();
    documentChanged(false);

    if (!suggestionPopup) return;

    auto& manager = owner.getAutocompleteManager();
    std::vector<SuggestionItem> matches;

    // --- 2. WORD AND SEPARATOR RESOLUTION ---
    int caretPosInt = editorComponent->getCaretPos().getPosition();
    int wordStart = 0;
    juce::String currentWord = getWordBeforeCaret(wordStart);

    juce::juce_wchar separator = ' ';
    if (wordStart > 0)
    {
        separator = juce::CodeDocument::Position(document, wordStart - 1).getCharacter();
    }
    
    if (newText == ":" || newText == ".")
        separator = newText[0];

    _DBG("AUTOCOMPLETE: Triggered. Word: [" + currentWord + "] | Separator: [" + juce::String::charToString(separator) + "]");
    
    bool contextResolved = false;

    // --- 3. CONTEXTUAL SEARCH (Triggered by : or .) ---
    if (separator == ':' || separator == '.')
    {
        bool isInstance = (separator == ':');
        juce::String className = "";
        juce::String expressionToResolve = "";

        int searchOrigin = wordStart > 0 ? wordStart - 1 : caretPosInt - 1;
        int bracketStack = 0;
        int searchPos = searchOrigin;

        while (searchPos > 0)
        {
            juce::juce_wchar c = juce::CodeDocument::Position(document, searchPos - 1).getCharacter();

            if (c == ')') bracketStack++;
            else if (c == '(') bracketStack--;

            if (bracketStack == 0)
            {
                if (juce::CharacterFunctions::isWhitespace(c) || c == '=' || c == ',' || c == ';' || c == '\n' || c == '\r')
                    break;
            }
            searchPos--;
        }

        expressionToResolve = document.getTextBetween(juce::CodeDocument::Position(document, searchPos),
                                                     juce::CodeDocument::Position(document, searchOrigin)).trim();

        _DBG("AUTOCOMPLETE: Expression found to resolve: [" + expressionToResolve + "]");

        if (expressionToResolve.isNotEmpty())
        {
            // Resolve variable to Class (e.g., "myVar" -> "AffineTransform")
            className = manager.getClassNameForVariable(expressionToResolve, document.getAllContent());

            // --- UPDATED: Class Name Check ---
            // If it's not a variable, check if the expression IS the class name (e.g., "AffineTransform.")
            if (className.isEmpty() && manager.getClassNames().contains(expressionToResolve, true))
            {
                className = expressionToResolve;
            }

            // Fallback for hardcoded shortcuts
            if (className.isEmpty())
            {
                if (expressionToResolve.equalsIgnoreCase("panel") || expressionToResolve == "pan") className = "CtrlrPanel";
                else if (expressionToResolve.startsWithIgnoreCase("mod")) className = "CtrlrModulator";
                else if (expressionToResolve == "utils") className = "utils";
            }
        }

        if (className.isNotEmpty())
        {
            contextResolved = true;
            _DBG("AUTOCOMPLETE: Context Resolved: [" + expressionToResolve + "] -> [" + className + "]");

            // Logic: ':' is for instances, '.' is for static methods/constructors
            bool includeInstance   = isInstance;
            bool includeStatic     = !isInstance;
            bool includeProperties = !isInstance;
            
            matches = manager.getMethodSuggestionsForClass(className, currentWord, includeInstance, includeStatic, includeProperties);
            _DBG("AUTOCOMPLETE: Found " + juce::String((int)matches.size()) + " class-specific matches.");
        }
    }

    // --- 4. GLOBAL FALLBACK ---
    if (matches.empty() && currentWord.length() >= 1)
    {
        if (separator != ':' && separator != '.')
        {
            _DBG("AUTOCOMPLETE: Fetching Global Suggestions for [" + currentWord + "]");
            matches = manager.getGlobalSuggestions(currentWord);
            _DBG("AUTOCOMPLETE: Found " + juce::String((int)matches.size()) + " global matches.");
        }
        else if (contextResolved)
        {
            _DBG("AUTOCOMPLETE: Class context matched 0 results. Suppressing global fallback.");
        }
    }
    
    // --- 5. UI DISPLAY ---
    if (!matches.empty())
    {
        suggestionPopup->setSuggestions(matches);
        juce::Rectangle<int> caretRect = editorComponent->getCharacterBounds(editorComponent->getCaretPos());
        auto popupPos = getLocalPoint(editorComponent, caretRect.getBottomLeft());
        
        int rowHeight = 22;
        int displayCount = juce::jmin(10, (int)matches.size());
        suggestionPopup->setBounds(popupPos.getX(), popupPos.getY() + 2, 350, displayCount * rowHeight);
        
        suggestionPopup->setVisible(true);
        suggestionPopup->toFront(false);
    }
    else if (suggestionPopup)
    {
        suggestionPopup->setVisible(false);
    }
}

void CtrlrLuaMethodCodeEditor::codeDocumentTextDeleted(int startIndex, int endIndex)
{
    // If we are programmatically replacing text, don't start new transactions
    if (isReplacingText)
        return;

    document.newTransaction(); // Standard manual deletion (backspace) gets a transaction
    documentChanged(false);
}

void CtrlrLuaMethodCodeEditor::documentChanged(const bool save, const bool recompile)
{
    owner.tabChanged(this, save, recompile);
}

void CtrlrLuaMethodCodeEditor::saveDocument()
{
    bool hasChanged = document.hasChangedSinceSavePoint();
    document.setSavePoint();

    if (method)
    {
        method->triggerSourceChangeFromEditor(false);
        if (owner.getMethodEditArea())
        {
            owner.getMethodEditArea()->setActiveOutputTab();
        }
        if (hasChanged && !method->isSourceInFile())
        {    // This method is stored within the panel => set the panel dirty
            owner.getOwner().luaManagerChanged();
        }
    }

    documentChanged(true, false);
}

void CtrlrLuaMethodCodeEditor::saveAndCompileDocument()
{
    bool hasChanged = document.hasChangedSinceSavePoint();
    document.setSavePoint();

    if (method)
    {
        method->triggerSourceChangeFromEditor(true);
        if (owner.getMethodEditArea())
        {
            owner.getMethodEditArea()->setActiveOutputTab();
        }
        if (hasChanged && !method->isSourceInFile())
        {    // This method is stored within the panel => set the panel dirty
            owner.getOwner().luaManagerChanged();
        }
    }

    documentChanged(true, true);
}

const bool CtrlrLuaMethodCodeEditor::isMouseOverUrl(CodeDocument::Position& position, String* url)
{
    if (position.getPosition() >= document.getNumCharacters())
    {
        return (false);
    }

    int moveLeft = 0;

    while (!CharacterFunctions::isWhitespace(position.getCharacter()))
    {
        if (position.getPosition() <= 0)
            break;

        position.moveBy(-1);
        moveLeft++;
    }

    int start = position.getPosition();
    position.setPosition(position.getPosition() + moveLeft);

    while (!CharacterFunctions::isWhitespace(position.getCharacter()))
    {
        if (position.getPosition() >= document.getNumCharacters())
            break;

        position.moveBy(1);
    }

    int end = position.getPosition();

    const String word = document.getTextBetween(CodeDocument::Position(document, start), CodeDocument::Position(document, end)).trim();

    if (word.startsWith("http://"))
    {
        if (url)
        {
            *url = word;
        }
        return (URL::isProbablyAWebsiteURL(word));
    }

    return (false);
}

void CtrlrLuaMethodCodeEditor::handleAsyncUpdate()
{
    owner.tabChanged(this, false, false);
}

void CtrlrLuaMethodCodeEditor::setErrorLine(const int lineNumber)
{
    editorComponent->scrollToLine(lineNumber - 1);
    CodeDocument::Position start(document, lineNumber - 1, 0);
    CodeDocument::Position end(document, lineNumber - 1, document.getLine(lineNumber - 1).length());
    editorComponent->setHighlightedRegion(Range<int>(start.getPosition(), end.getPosition()));
}

void CtrlrLuaMethodCodeEditor::setFontAndColour(const Font newFont, const Colour newColour)
{
    editorComponent->setColour(CodeEditorComponent::backgroundColourId, newColour);
    editorComponent->setFont(newFont);
}

void CtrlrLuaMethodCodeEditor::findNextMatch(const String& search, bool bMatchCase)
{
    if (owner.getCurrentEditor() == nullptr)
    {
        return;
    }

    CodeDocument& doc = owner.getCurrentEditor()->getCodeDocument();
    int position = -1;

    if (bMatchCase)
    {
        position = document.getAllContent().indexOfIgnoreCase(lastFoundPosition + 1, search);
    }
    else
    {
        position = document.getAllContent().indexOf(lastFoundPosition + 1, search);
    }

    if (position >= 0)
    {
        lastFoundPosition = position;
        if (editorComponent)
        {
            editorComponent->selectRegion(CodeDocument::Position(document, lastFoundPosition), CodeDocument::Position(doc, lastFoundPosition + search.length()));
        }
    }
    else
    {
        lastFoundPosition = -1;
    }
}

void CtrlrLuaMethodCodeEditor::replaceNextMatch(const String& search, const String& replace, bool bMatchCase)
{
    if (owner.getCurrentEditor() == nullptr)
    {
        return;
    }

    CodeDocument& doc = owner.getCurrentEditor()->getCodeDocument();
    findNextMatch(search, bMatchCase);
    if (lastFoundPosition >= 0)
    {
        doc.newTransaction();
        doc.deleteSection(lastFoundPosition, lastFoundPosition + search.length());
        doc.insertText(lastFoundPosition, replace);
    }
}

void CtrlrLuaMethodCodeEditor::replaceAllMatches(const String& search, const String& replace, bool bMatchCase)
{
    lastFoundPosition = -1;
    do
    {
        replaceNextMatch(search, replace, bMatchCase);
    } while (lastFoundPosition >= 0);
}

void CtrlrLuaMethodCodeEditor::findInOpened(const String& search)
{
    if (owner.getTabs() == nullptr)
        return;

    StringArray names = owner.getTabs()->getTabNames();

    owner.getMethodEditArea()->insertOutput("\n\nSearching for: \"" + search + "\" in all opened methods (double click line to jump)\n", Colours::darkblue);

    for (int i = 0; i < owner.getTabs()->getNumTabs(); i++)
    {
        CtrlrLuaMethodCodeEditor* codeEditor = dynamic_cast<CtrlrLuaMethodCodeEditor*>(owner.getTabs()->getTabContentComponent(i));

        if (codeEditor != nullptr)
        {
            CodeDocument& doc = codeEditor->getCodeDocument();

            Array<Range<int> > results = searchForMatchesInDocument(doc, search);

            for (int j = 0; j < results.size(); j++)
            {
                reportFoundMatch(doc, names[i], results[j]);
            }
        }
    }

    owner.getMethodEditArea()->getLowerTabs()->setCurrentTabIndex(0, true);
}
bool isValidSearchString(const String& search)
{
    String trimmed = search.trim();

    // Minimum length check
    if (trimmed.length() < 3)
        return false;

    // Check for only whitespace
    if (trimmed.isEmpty() || trimmed.containsOnly(" \t\n\r"))
        return false;

    // Check for repetitive single characters (like "aaa" or "111")
    if (trimmed.length() >= 3)
    {
        bool allSame = true;
        juce_wchar firstChar = trimmed[0];
        for (int i = 1; i < trimmed.length(); i++)
        {
            if (trimmed[i] != firstChar)
            {
                allSame = false;
                break;
            }
        }
        if (allSame)
            return false;
    }

    // Check for very common single characters or short sequences
    StringArray commonInvalidSearches = {
        "a", "an", "and", "the", "of", "to", "in", "it", "is", "be", "as", "at", "by", "for", "with", "on",
        "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
        "aa", "bb", "cc", "dd", "ee", "ff", "gg", "hh", "ii", "jj", "kk", "ll", "mm", "nn", "oo", "pp",
        "11", "22", "33", "44", "55", "66", "77", "88", "99", "00"
    };

    for (const String& invalid : commonInvalidSearches)
    {
        if (trimmed.equalsIgnoreCase(invalid))
            return false;
    }

    return true;
}
void CtrlrLuaMethodCodeEditor::findInAll(const String& search)
{
    // Validate search string first
    if (!isValidSearchString(search))
    {
        owner.getMethodEditArea()->insertOutput("\n\nInvalid search term: \"" + search +
            "\". Please enter a meaningful search term (at least 3 characters, not just whitespace or common single characters).\n",
            Colours::red);
        return;
    }

    owner.getMethodEditArea()->insertOutput("\n\nSearching for: \"" + search + "\" in all methods (double click line to jump)\n", Colours::darkblue);
    StringArray names;

    // Get the toggle state from the HIDDEN toggle instead of the problematic reference
    const bool shouldOpenTabs = hiddenSearchTabsToggle->getToggleState();
    DBG("findInAll: shouldOpenTabs = " + String(shouldOpenTabs ? "true" : "false"));
    DBG("findInAll: sharedSearchTabsValue = " + sharedSearchTabsValue.toString());
    for (int i = 0; i < owner.getMethodManager().getNumMethods(); i++)
    {
        CtrlrLuaMethod* m = owner.getMethodManager().getMethodByIndex(i);

        if (m)
        {
            names.add(m->getName());

            if (m->getCodeEditor())
            {
                /* it has an editor so it's open */
                CodeDocument& doc = m->getCodeEditor()->getCodeDocument();

                Array<Range<int> > results = searchForMatchesInDocument(doc, search);

                for (int j = 0; j < results.size(); j++)
                {
                    reportFoundMatch(doc, names[i], results[j]);
                }
            }
            else // Method is not yet opened
            {
                /* Open method temporarily to search */
                owner.createNewTab(m);
                owner.setCurrentTab(m);

                /* Perform search and report result */
                CodeDocument& doc = m->getCodeEditor()->getCodeDocument();

                Array<Range<int> > results = searchForMatchesInDocument(doc, search);

                for (int j = 0; j < results.size(); j++)
                {
                    reportFoundMatch(doc, names[i], results[j]);
                }

                // Decide whether to keep the tab open based on toggle state AND whether there were results
                if (!shouldOpenTabs || results.size() == 0)
                {

                    DBG("Closing tab for method: " + names[i] +
                        " (shouldOpenTabs=" + String(shouldOpenTabs ? "true" : "false") +
                        ", results=" + String(results.size()) + ")");
                    // Close the tab if:
                    // 1. The user doesn't want tabs opened automatically, OR
                    // 2. No search results were found (regardless of toggle state)
                    owner.closeCurrentTab();
                }
                else
                {
                    DBG("Keeping tab open for method: " + names[i] +
                        " (shouldOpenTabs=" + String(shouldOpenTabs ? "true" : "false") +
                        ", results=" + String(results.size()) + ")");
                }
                // If shouldOpenTabs is true AND results.size() > 0, keep the tab open
            }
        }
    }

    owner.getMethodEditArea()->getLowerTabs()->setCurrentTabIndex(0, true);
}


//void CtrlrLuaMethodCodeEditor::findInAll(const String &search)
//{
//    // Validate search string first
//    if (!isValidSearchString(search))
//    {
//        owner.getMethodEditArea()->insertOutput("\n\nInvalid search term: \"" + search +
//            "\". Please enter a meaningful search term (at least 3 characters, not just whitespace or common single characters).\n",
//            Colours::red);
//        return;
//    }
//    owner.getMethodEditArea()->insertOutput("\n\nSearching for: \""+search+"\" in all methods (double click line to jump)\n", Colours::darkblue);
//    StringArray names;
//
//    for (int i=0; i<owner.getMethodManager().getNumMethods(); i++)
//    {
//        CtrlrLuaMethod *m = owner.getMethodManager().getMethodByIndex (i);
//
//        if (m)
//        {
//            names.add (m->getName());
//
//            if (m->getCodeEditor())
//            {
//                /* it has an editor so it's open */
//                CodeDocument &doc        = m->getCodeEditor()->getCodeDocument();
//
//                Array<Range<int> > results = searchForMatchesInDocument (doc, search);
//
//                for (int j=0; j<results.size(); j++)
//                {
//                    reportFoundMatch (doc, names[i], results[j]);
//                }
//            }
//            else // Added 5.6.34 by goodweather. Search in not yet opened methods
//            {
//                /* Open method */
//                owner.createNewTab(m);
//                owner.setCurrentTab(m);
//
//                /* Perform search and report result */
//                CodeDocument& doc = m->getCodeEditor()->getCodeDocument();
//
//                Array<Range<int> > results = searchForMatchesInDocument(doc, search);
//
//                for (int j = 0; j < results.size(); j++)
//                {
//                    reportFoundMatch(doc, names[i], results[j]);
//                }
//
//                /* If no result then close method; if any result then keep method open */
//                /*Dnaldoog disable this because I think it's better for ser to open file at bottom list,
//                especially if the search results in dozens of hits therefore opening dozens of windows*/
//                //if (results.size() == 0)
//                //{
//                    owner.closeCurrentTab();
//                //}
//            }
//        }
//    }
//
//    owner.getMethodEditArea()->getLowerTabs()->setCurrentTabIndex(0,true);
//}

const Array<Range<int> > CtrlrLuaMethodCodeEditor::searchForMatchesInDocument(CodeDocument& doc, const String& search)
{
    Array<Range<int> > results;
    int position = -1;
    lastFoundPosition = -1;
    do
    {
        String documentContent = doc.getAllContent();
        if (documentContent.isNotEmpty())
        {
            if (editorComponent->isCaseSensitiveSearch())
            {
                position = documentContent.indexOfIgnoreCase(lastFoundPosition + 1, search);
            }
            else
            {
                position = documentContent.indexOf(lastFoundPosition + 1, search);
            }
        }


        if (position >= 0)
        {
            lastFoundPosition = position;
            results.add(Range<int>(lastFoundPosition, lastFoundPosition + search.length()));
        }
        else
        {
            lastFoundPosition = -1;
        }
    } while (lastFoundPosition >= 0);

    return (results);
}

void CtrlrLuaMethodCodeEditor::reportFoundMatch(CodeDocument& doc, const String& methodName, const Range<int> range)
{
    CodeDocument::Position pos(doc, range.getStart());
    AttributedString as;
    as.append("Method: ", Colours::black);
    as.append(methodName, Colours::blue);

    as.append("\tline: ", Colours::black);
    as.append(String(pos.getLineNumber() + 1), Colours::darkgreen);

    as.append("\tstart: ", Colours::black);
    as.append(String(range.getStart()), Colours::darkgreen);

    as.append("\tend: ", Colours::black);
    as.append(String(range.getEnd()), Colours::darkgreen);

    owner.getMethodEditArea()->insertOutput(as);
}

void CtrlrLuaMethodCodeEditor::gotoLine(int position, const bool selectLine)
{
    editorComponent->scrollToLine(position - 3);
    editorComponent->moveCaretTo(CodeDocument::Position(document, position - 1, 0), false);
    if (selectLine)
    {
        editorComponent->selectRegion(CodeDocument::Position(document, position - 1, 0), CodeDocument::Position(document, position - 1, 1024));
    }
    //editorComponent->selectRegion(CodeDocument::Position(document, position - 1, 0), CodeDocument::Position(document, position - 1, 1));
    editorComponent->grabKeyboardFocus();
    editorComponent->hideGoTOPanel();
}

CtrlrLuaMethodEditor& CtrlrLuaMethodCodeEditor::getOwner()
{
    return  (owner);
}

//==============================================================================
class GenericCodeEditorComponent::GoToPanel : public Component,
    private TextEditor::Listener,
    private Button::Listener
{
public:
    GoToPanel() : goToButton("", 0.0, Colours::white)
    {
        editor.setColour(CaretComponent::caretColourId, Colours::black);
        editor.setInputRestrictions(5, ("1234567890"));
        editor.addListener(this);
        addAndMakeVisible(editor);
        goToButton.addListener(this);
        addAndMakeVisible(goToButton);

        label.setText("Go To:", dontSendNotification);
        label.setColour(Label::textColourId, Colours::white);
        label.attachToComponent(&editor, true);

        setWantsKeyboardFocus(false);
        setFocusContainer(true);
    }

    void paint(Graphics& g) override
    {
        Path outline;
        outline.addRoundedRectangle(1.0f, 1.0f, getWidth() - 2.0f, getHeight() - 2.0f, 8.0f);

        g.setColour(Colours::black.withAlpha(0.6f));
        g.fillPath(outline);
        g.setColour(Colours::white.withAlpha(0.8f));
        g.strokePath(outline, PathStrokeType(1.0f));
    }

    void resized() override
    {
        int y = 5;
        editor.setBounds(45, y, getWidth() - 90, 24);
        goToButton.setBounds(editor.getRight() + 5, y + 1, 30, 24);
    }
    void buttonClicked(Button* button) override
    {
        GenericCodeEditorComponent* ed = getOwner();
        if (ed)
        {
            if (button == &goToButton)
            {
                ed->gotoLine(editor.getText().getIntValue());
            }
        }
    }

    void textEditorTextChanged(TextEditor&) override {}

    void textEditorFocusLost(TextEditor&) override {}

    void textEditorReturnKeyPressed(TextEditor& textEditor) override
    {
        GenericCodeEditorComponent* ed = getOwner();
        if (ed)
        {
            ed->gotoLine(editor.getText().getIntValue());
        }
    }

    void textEditorEscapeKeyPressed(TextEditor&) override
    {
        if (GenericCodeEditorComponent* ed = getOwner())
            ed->hideGoTOPanel();
    }

    GenericCodeEditorComponent* getOwner() const
    {
        return findParentComponentOfClass <GenericCodeEditorComponent>();
    }

    TextEditor editor;
    Label label;
    ArrowButton goToButton;
};

//==============================================================================
class GenericCodeEditorComponent::FindPanel : public Component,
    private TextEditor::Listener,
    private Button::Listener,
    private ComboBox::Listener
{
public:
    FindPanel()
        : caseButton("Match Case"),
        findPrev("<", "Search Previous"),
        findNext(">", "Search Next"),
        searchButton("", 0.0, Colours::white)
    {
        editor.setColour(CaretComponent::caretColourId, Colours::black);
        editor.addListener(this);
        addAndMakeVisible(editor);
        searchButton.addListener(this);
        addAndMakeVisible(searchButton);

        label.setText("Find:", dontSendNotification);
        label.setColour(Label::textColourId, Colours::white);
        label.attachToComponent(&editor, true);

        searchInComboBox.setEditableText(false);
        searchInComboBox.setJustificationType(Justification::centredLeft);
        searchInComboBox.addItem(TRANS("Editor"), 1);
        searchInComboBox.addItem(TRANS("Output"), 2);
        searchInComboBox.addItem(TRANS("Methods"), 3);
        searchInComboBox.setSelectedItemIndex(0, dontSendNotification);
        searchInComboBox.addListener(this);
        searchInComboBox.setEnabled(false);
        //addAndMakeVisible(searchInComboBox);

        //addAndMakeVisible (caseButton);
        //caseButton.setColour (ToggleButton::textColourId, Colours::white);
        //caseButton.setToggleState (false, dontSendNotification);
        //caseButton.addListener (this);
        addAndMakeVisible(caseButton);
        caseButton.setColour(juce::ToggleButton::textColourId, juce::Colours::yellow);
        caseButton.setColour(juce::ToggleButton::tickColourId, juce::Colours::yellow);
        caseButton.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::yellow);
        caseButton.setToggleState(false, dontSendNotification);
        caseButton.addListener(this);


        lookInComboBox.setEditableText(false);
        lookInComboBox.setJustificationType(Justification::centredLeft);
        lookInComboBox.addItem(TRANS("Current"), 1);
        lookInComboBox.addItem(TRANS("All Open"), 2);
        lookInComboBox.addItem(TRANS("All"), 3);
        lookInComboBox.setSelectedItemIndex(0, dontSendNotification);
        lookInComboBox.addListener(this);
        addAndMakeVisible(lookInComboBox);

        findPrev.setConnectedEdges(Button::ConnectedOnRight);
        findPrev.addListener(this);
        findNext.setConnectedEdges(Button::ConnectedOnLeft);
        findNext.addListener(this);
        addAndMakeVisible(findPrev);
        addAndMakeVisible(findNext);

        setWantsKeyboardFocus(false);
        setFocusContainer(true);
        findPrev.setWantsKeyboardFocus(true);
        findNext.setWantsKeyboardFocus(true);
    }

    ~FindPanel()
    {
        replaceEditor = nullptr;
        replaceLabel = nullptr;
        replaceButton = nullptr;
        replaceAllButton = nullptr;
    }

    void applyCurrentSetting()
    {
        if (GenericCodeEditorComponent* ed = getOwner())
        {
            caseButton.setToggleState(ed->getCtrlrLuaMethodCodeEditor().getCtrlrLuaMethodEditor().caseCansitive, dontSendNotification);
            lookInComboBox.setText(ed->getCtrlrLuaMethodCodeEditor().getCtrlrLuaMethodEditor().lookInString, dontSendNotification);
            searchInComboBox.setText(ed->getCtrlrLuaMethodCodeEditor().getCtrlrLuaMethodEditor().searchInString, dontSendNotification);
            editor.setText(ed->getCtrlrLuaMethodCodeEditor().getCtrlrLuaMethodEditor().currentSearchString, true);
        }
    }

    void setSearchText(const String& s)
    {
        editor.setText(s);
    }

    void addReplaceComponents()
    {
        replaceEditor = new TextEditor();
        replaceEditor->setColour(CaretComponent::caretColourId, Colours::black);
        addAndMakeVisible(*replaceEditor);

        replaceLabel = new Label();
        replaceLabel->setText("Replace:", dontSendNotification);
        replaceLabel->setFont(Font(13.00f, Font::plain));
        replaceLabel->setColour(Label::textColourId, Colours::white);
        replaceLabel->attachToComponent(replaceEditor, true);

        replaceButton = new TextButton(">", "Replace Next");
        replaceButton->addListener(this);
        addAndMakeVisible(*replaceButton);

        replaceAllButton = new TextButton("A", "Replace all in current");
        replaceAllButton->addListener(this);
        addAndMakeVisible(*replaceAllButton);

        replaceButton->setConnectedEdges(Button::ConnectedOnRight);
        replaceAllButton->setConnectedEdges(Button::ConnectedOnLeft);

        replaceEditor->addListener(this);
        resized();
    }

    void paint(Graphics& g) override
    {
        Path outline;
        outline.addRoundedRectangle(1.0f, 1.0f, getWidth() - 2.0f, getHeight() - 2.0f, 8.0f);

        g.setColour(Colours::black.withAlpha(0.6f));
        g.fillPath(outline);
        g.setColour(Colours::white.withAlpha(0.8f));
        g.strokePath(outline, PathStrokeType(1.0f));
    }

    void resized() override
    {
        int y = 5;
        editor.setBounds(45, y, getWidth() - 90, 24);
        searchButton.setBounds(editor.getRight() + 5, y + 1, 30, 24);
        y += 30;
        if (replaceEditor != nullptr)
        {
            replaceEditor->setBounds(45, y, getWidth() - 90, 24);
            replaceButton->setBounds(replaceEditor->getRight(), y, 20, 24);
            replaceAllButton->setBounds(replaceButton->getRight(), y, 20, 24);
            y += 30;
        }
        //caseButton.setBounds (0, y, 75, 22);
        //searchInComboBox.setBounds(caseButton.getRight(), y, 70, 24);
        //lookInComboBox.setBounds(searchInComboBox.getRight() + 5, y, 70, 24);
        caseButton.setBounds(5, y, 75, 22);
        lookInComboBox.setBounds(searchInComboBox.getRight() + 75, y, 90, 24);
        findPrev.setBounds(lookInComboBox.getRight(), y, 15, 24);
        findNext.setBounds(findPrev.getRight(), y, 15, 24);
    }

    void comboBoxChanged(ComboBox* combo) override
    {
        if (combo == &searchInComboBox)
        {
            switch (searchInComboBox.getSelectedItemIndex())
            {
            case 0:
                lookInComboBox.setEnabled(true);
                if (replaceEditor != nullptr)
                {
                    replaceEditor->setEnabled(true);
                    replaceLabel->setEnabled(true);
                    replaceButton->setEnabled(true);
                    replaceAllButton->setEnabled(true);
                }
                break;
            default:
                lookInComboBox.setEnabled(false);
                if (replaceEditor != nullptr)
                {
                    replaceEditor->setEnabled(false);
                    replaceLabel->setEnabled(false);
                    replaceButton->setEnabled(false);
                    replaceAllButton->setEnabled(false);
                }
                break;
            }
            if (GenericCodeEditorComponent* ed = getOwner())
                ed->getCtrlrLuaMethodCodeEditor().getCtrlrLuaMethodEditor().searchInString =
                searchInComboBox.getText();
        }
        else if (combo == &lookInComboBox)
        {
            if (GenericCodeEditorComponent* ed = getOwner())
                ed->getCtrlrLuaMethodCodeEditor().getCtrlrLuaMethodEditor().lookInString =
                lookInComboBox.getText();
        }
    }

    void buttonClicked(Button* button) override
    {
        GenericCodeEditorComponent* ed = getOwner();
        if (ed)
        {
            if (button == &findNext)
            {
                ed->findNext(true, true);
            }
            else if (button == &findPrev)
            {
                ed->findNext(false, false);
            }
            else if (button == replaceButton)
            {
                ed->replaceNextMatch(editor.getText(), replaceEditor->getText(), caseButton.getToggleState());
            }
            else if (button == replaceAllButton)
            {
                ed->replaceAllMatches(editor.getText(), replaceEditor->getText(), caseButton.getToggleState());
            }
            else if (button == &searchButton)
            {
                lookInSearch();
            }
            else if (button == &caseButton)
            {
                ed->getCtrlrLuaMethodCodeEditor().getCtrlrLuaMethodEditor().caseCansitive =
                    caseButton.getToggleState();
            }
        }
    }

    void textEditorTextChanged(TextEditor& te) override
    {
        switch (searchInComboBox.getSelectedItemIndex())
        {
        case 0:
            if (GenericCodeEditorComponent* ed = getOwner())
            {
                ed->findNext(true, false);
                ed->getCtrlrLuaMethodCodeEditor().getCtrlrLuaMethodEditor().currentSearchString = editor.getText();
            }
            break;
        default:
            break;
        }
    }

    void textEditorFocusLost(TextEditor&) override {}

    void textEditorReturnKeyPressed(TextEditor& textEditor) override
    {
        if (&textEditor == &editor)
        {
            lookInSearch();
        }
        else if (&textEditor == replaceEditor)
        {
            if (GenericCodeEditorComponent* ed = getOwner())
            {
                ed->replaceNextMatch(editor.getText(), replaceEditor->getText(), caseButton.getToggleState());
            }
        }
    }

    void lookInSearch()
    {
        if (GenericCodeEditorComponent* ed = getOwner())
        {
            switch (lookInComboBox.getSelectedItemIndex())
            {
            case 0: // code editor current
                switch (searchInComboBox.getSelectedItemIndex())
                {
                case 0: // code editor
                    ed->findNext(true, true);
                    break;
                case 1: // output window
                    break;
                case 2: // Lua method tree
                    break;
                default:
                    jassertfalse;
                }
                break;
            case 1: // code editor all open
                ed->findInOpened(editor.getText());
                break;
            case 2: // All methods
                ed->findInAll(editor.getText());
                break;
            default:
                jassertfalse;
            }
        }
    }

    void textEditorEscapeKeyPressed(TextEditor&) override
    {
        if (GenericCodeEditorComponent* ed = getOwner())
            ed->hideFindPanel();
    }

    GenericCodeEditorComponent* getOwner() const
    {
        return findParentComponentOfClass <GenericCodeEditorComponent>();
    }

    TextEditor editor;
    Label label;
    ToggleButton caseButton;
    TextButton findPrev, findNext;
    ArrowButton searchButton;
    ComboBox    lookInComboBox, searchInComboBox;
    // For replace
    ScopedPointer<TextEditor> replaceEditor;
    ScopedPointer<Label> replaceLabel;
    ScopedPointer<TextButton> replaceButton, replaceAllButton;
};

//==============================================================================
GenericCodeEditorComponent::GenericCodeEditorComponent(CtrlrLuaMethodCodeEditor& _owner, CodeDocument& codeDocument,
    CodeTokeniser* tokeniser) : CodeEditorComponent(codeDocument, tokeniser), owner(_owner), bSensitive(false),
    lookUpString("")
{
    setColour(CodeEditorComponent::lineNumberTextId, Colours::black);
}

GenericCodeEditorComponent::~GenericCodeEditorComponent()
{
}

void GenericCodeEditorComponent::resized()
{
    CodeEditorComponent::resized();

    if (findPanel != nullptr)
    {
        findPanel->setSize(jmin(260, getWidth() - 32), 100);
        findPanel->setTopRightPosition(getWidth() - 6, 4);
    }
    if (goToPanel != nullptr)
    {
        goToPanel->setSize(jmin(160, getWidth() - 32), 40);
        goToPanel->setTopRightPosition(getWidth() - 6, getHeight() - 40);
    }
}

bool GenericCodeEditorComponent::isFindActive()
{
    return (findPanel == nullptr);
}

void GenericCodeEditorComponent::showFindPanel(bool bForReplace)
{
    if (findPanel == nullptr)
    {
        findPanel = new FindPanel();
        addAndMakeVisible(findPanel);
        resized();
    }

    if (findPanel != nullptr)
    {
        findPanel->setSearchText(getTextInRange(getHighlightedRegion()));
        findPanel->applyCurrentSetting();
        if (bForReplace)
            findPanel->addReplaceComponents();
        findPanel->editor.grabKeyboardFocus();
        findPanel->editor.selectAll();
    }
    owner.getCtrlrLuaMethodEditor().findDialogActive = true;
}

void GenericCodeEditorComponent::hideFindPanel()
{
    owner.getCtrlrLuaMethodEditor().findDialogActive = false;
    findPanel = nullptr;
}

void GenericCodeEditorComponent::showGoTOPanel()
{
    if (goToPanel == nullptr)
    {
        goToPanel = new GoToPanel();
        addAndMakeVisible(goToPanel);
        resized();
    }
    if (goToPanel != nullptr)
    {
        goToPanel->editor.grabKeyboardFocus();
        goToPanel->editor.selectAll();
    }
}

void GenericCodeEditorComponent::hideGoTOPanel()
{
    goToPanel = nullptr;
}

void GenericCodeEditorComponent::findSelection(bool forward)
{
    const String selected = getSearchString();

    if (selected.isNotEmpty())
    {
        if (forward)
        {
            findNext(true, true);
        }
        else
        {
            findNext(false, false);
        }
    }
}

void GenericCodeEditorComponent::findNext(bool forwards, bool skipCurrentSelection)
{
    const Range<int> highlight(getHighlightedRegion());
    const CodeDocument::Position startPos(getDocument(), skipCurrentSelection ? highlight.getEnd()
        : highlight.getStart());
    int lineNum = startPos.getLineNumber();
    int linePos = startPos.getIndexInLine();

    const int totalLines = getDocument().getNumLines();
    const String searchText(getSearchString());
    const bool caseSensitive = isCaseSensitiveSearch();

    for (int linesToSearch = totalLines; --linesToSearch >= 0;)
    {
        String line(getDocument().getLine(lineNum));
        int index;

        if (forwards)
        {
            index = caseSensitive ? line.indexOf(linePos, searchText)
                : line.indexOfIgnoreCase(linePos, searchText);
        }
        else
        {
            if (linePos >= 0)
                line = line.substring(0, linePos);

            index = caseSensitive ? line.lastIndexOf(searchText)
                : line.lastIndexOfIgnoreCase(searchText);
        }

        if (index >= 0)
        {
            const CodeDocument::Position p(getDocument(), lineNum, index);
            selectRegion(p, p.movedBy(searchText.length()));
            break;
        }

        if (forwards)
        {
            linePos = 0;
            lineNum = (lineNum + 1) % totalLines;
        }
        else
        {
            if (--lineNum < 0)
                lineNum = totalLines - 1;

            linePos = -1;
        }
    }
}

void GenericCodeEditorComponent::handleEscapeKey()
{
    CodeEditorComponent::handleEscapeKey();
    hideFindPanel();
}

String GenericCodeEditorComponent::getSearchString()
{
    String searchString = "";
    if (findPanel != nullptr)
        searchString = findPanel->editor.getText();
    else
        searchString = getTextInRange(getHighlightedRegion());
    return searchString;
}

bool GenericCodeEditorComponent::isCaseSensitiveSearch()
{
    if (findPanel != nullptr)
        bSensitive = !findPanel->caseButton.getToggleState();
    // this needed to be reversed for Match Case toggle J.G
    return bSensitive;
}

void GenericCodeEditorComponent::gotoLine(int position)
{
    owner.gotoLine(position);
}

void GenericCodeEditorComponent::replaceAllMatches(const String& search, const String& replace, bool bMatchCase)
{
    owner.replaceAllMatches(search, replace, bMatchCase);
}
void GenericCodeEditorComponent::replaceNextMatch(const String& search, const String& replace, bool bMatchCase)
{
    owner.replaceNextMatch(search, replace, bMatchCase);
}
void GenericCodeEditorComponent::findInAll(const String& search)
{
    owner.findInAll(search);
}
void GenericCodeEditorComponent::findInOpened(const String& search)
{
    owner.findInOpened(search);
}

CtrlrLuaDebugger& GenericCodeEditorComponent::getDebugger()
{
    return (owner.getOwner().getOwner().getCtrlrLuaManager().getDebugger());
}

void GenericCodeEditorComponent::markedLinesChanged(int lineNumber, bool isNowSelected)
{
    getDebugger().setBreakpoint(lineNumber, owner.getMethod() ? owner.getMethod()->getName() : "ctrlr", isNowSelected);
}

bool GenericCodeEditorComponent::keyPressed(const juce::KeyPress& key)
{
    const int currentPos = getCaretPos().getPosition();
    const juce::juce_wchar character = key.getTextCharacter();
    juce::String allText = getDocument().getAllContent();

    // --- 1. HANDLE PAIR INSERTION ---
    if (character == '(' || character == '"')
    {
        // Quote specific logic: if we are already on a quote, just step over (Type-over)
        if (character == '"' && currentPos < allText.length() && allText[currentPos] == '"')
        {
            moveCaretTo(juce::CodeDocument::Position(getDocument(), currentPos + 1), false);
            return true;
        }

        juce::String pair = (character == '(') ? "()" : "\"\"";
        
        // Atomically insert the pair
        getDocument().insertText(currentPos, pair);
        
        // Move caret into the middle immediately
        moveCaretTo(juce::CodeDocument::Position(getDocument(), currentPos + 1), false);
        
        return true; // STOP JUCE from adding a 3rd character
    }

    // --- 2. HANDLE CLOSING PARENTHESIS TYPE-OVER ---
    if (character == ')' && currentPos < allText.length() && allText[currentPos] == ')')
    {
        moveCaretTo(juce::CodeDocument::Position(getDocument(), currentPos + 1), false);
        return true;
    }

    // --- 3. HAND OVER TO PARENT ---
    // This calls your CtrlrLuaMethodCodeEditor::keyPressed for Tabs, Esc, and Autocomplete
    if (owner.keyPressed(key, this))
        return true;

    // --- 4. DEFAULT JUCE TYPING ---
    return juce::CodeEditorComponent::keyPressed(key);
}

/***************************************************************************************/

void CtrlrLuaMethodCodeEditor::duplicateCurrentLine()
{
    DBG("Executing duplicate line function...");

    if (!editorComponent)
        return;

    CodeDocument::Position caretPos = editorComponent->getCaretPos();
    int lineNumber = caretPos.getLineNumber();

    CodeDocument::Position startPos(document, lineNumber, 0);
    CodeDocument::Position endPos(document, lineNumber + 1, 0);
    
    String lineToDuplicate = document.getTextBetween(startPos, endPos);
    
    document.newTransaction();
    document.insertText(endPos.getPosition(), lineToDuplicate);
    
    CodeDocument::Position newCaretPos(document, lineNumber + 1, caretPos.getIndexInLine());
    editorComponent->moveCaretTo(newCaretPos, false);
}


void CtrlrLuaMethodCodeEditor::toggleLineComment() // Updated v5.6.34
{
    if (!editorComponent)
        return;

    Range<int> selection = editorComponent->getHighlightedRegion();
    CodeDocument::Position startPos(document, selection.getStart());
    CodeDocument::Position endPos(document, selection.getEnd());

    // If there is no selection, use the current line
    if (selection.isEmpty())
    {
        startPos = CodeDocument::Position(document, startPos.getLineNumber(), 0);
        endPos = CodeDocument::Position(document, startPos.getLineNumber() + 1, 0);
    }
    else
    {
        // Adjust selection to span full lines
        startPos = CodeDocument::Position(document, startPos.getLineNumber(), 0);
        
        // Correctly get the start position of the line after the selection ends
        endPos = CodeDocument::Position(document, endPos.getLineNumber(), 0);
        if (endPos.getIndexInLine() != 0)
        {
            endPos = CodeDocument::Position(document, endPos.getLineNumber() + 1, 0);
        }
    }
    
    int startLine = startPos.getLineNumber();
    int endLine = endPos.getLineNumber();

    document.newTransaction();

    // Check if we should comment or uncomment
    bool allLinesCommented = true;
    for (int lineNum = startLine; lineNum <= endLine; ++lineNum)
    {
        String line = document.getLine(lineNum);
        if (line.trimStart().isEmpty() || !line.trimStart().startsWith("--"))
        {
            allLinesCommented = false;
            break;
        }
    }

    // Comment or uncomment
    for (int lineNum = startLine; lineNum <= endLine; ++lineNum)
    {
        CodeDocument::Position lineStart(document, lineNum, 0);
        String line = document.getLine(lineNum);
        
        if (allLinesCommented)
        {
            // Uncomment: Find and remove the first "--"
            int commentPos = line.trimStart().indexOf("--");
            if (commentPos >= 0)
            {
                int actualCommentPos = line.indexOf("--");
                document.deleteSection(lineStart.getPosition() + actualCommentPos,
                                      lineStart.getPosition() + actualCommentPos + 2);
            }
        }
        else
        {
            // Comment: Find the first non-whitespace character and insert "--"
            int firstNonWhitespace = 0;
            while (firstNonWhitespace < line.length() && iswspace (line[firstNonWhitespace]))
                ++firstNonWhitespace;
                
            document.insertText(lineStart.getPosition() + firstNonWhitespace, "--");
        }
    }

    // Restore caret position and selection
    editorComponent->setHighlightedRegion(Range<int>(startPos.getPosition(), endPos.getPosition()));
    documentChanged(false, false);
}

void CtrlrLuaMethodCodeEditor::toggleLongLineComment()
{
    if (!editorComponent)
        return;

    Range<int> selection = editorComponent->getHighlightedRegion();
    CodeDocument::Position startPos(document, selection.getStart());
    CodeDocument::Position endPos(document, selection.getEnd());

    // If no selection, select the current line
    if (selection.isEmpty())
    {
        int currentLine = startPos.getLineNumber();
        startPos = CodeDocument::Position(document, currentLine, 0);
        endPos = CodeDocument::Position(document, currentLine + 1, 0);
        selection = Range<int>(startPos.getPosition(), endPos.getPosition());
    }

    document.newTransaction();

    // Get the selected text
    String selectedText = document.getTextBetween(startPos, endPos);

    // Check if the selection is already block commented
    String trimmedStart = selectedText.trimStart();
    String trimmedEnd = selectedText.trimEnd();

    bool isBlockCommented = trimmedStart.startsWith("--[[") && trimmedEnd.endsWith("--]]");

    // Store the original caret position
    CodeDocument::Position originalCaret = editorComponent->getCaretPos();

    if (isBlockCommented)
    {
        // Uncomment: Remove --[[ from start and --]] from end
        String uncommentedText = selectedText;

        // Remove --[[ from the beginning
        int startCommentPos = uncommentedText.indexOf("--[[");
        if (startCommentPos >= 0)
        {
            uncommentedText = uncommentedText.substring(0, startCommentPos) +
                uncommentedText.substring(startCommentPos + 4);
        }

        // Remove --]] from the end
        int endCommentPos = uncommentedText.lastIndexOf("--]]");
        if (endCommentPos >= 0)
        {
            uncommentedText = uncommentedText.substring(0, endCommentPos) +
                uncommentedText.substring(endCommentPos + 4);
        }

        // Replace the selected text
        document.replaceSection(startPos.getPosition(), endPos.getPosition(), uncommentedText);

        // Adjust caret position (moved back by the length of removed comment markers)
        int caretAdjustment = -8; // Length of "--[[" + "--]]"
        if (originalCaret.getPosition() > startPos.getPosition())
        {
            int newCaretPos = jmax(startPos.getPosition(), originalCaret.getPosition() + caretAdjustment);
            CodeDocument::Position newCaret(document, newCaretPos);
            editorComponent->moveCaretTo(newCaret, false);
        }
    }
    else
    {
        // Comment: Add --[[ at start and --]] at end
        String commentedText = "--[[\n" + selectedText + "\n--]]";

        // Replace the selected text
        document.replaceSection(startPos.getPosition(), endPos.getPosition(), commentedText);

        // Adjust caret position (moved forward by the length of added comment markers)
        int caretAdjustment = 6; // Length of "--[[\n"
        if (originalCaret.getPosition() >= startPos.getPosition())
        {
            int newCaretPos = originalCaret.getPosition() + caretAdjustment;
            CodeDocument::Position newCaret(document, newCaretPos);
            editorComponent->moveCaretTo(newCaret, false);
        }
    }

    documentChanged(false, false);
}
void CtrlrLuaMethodCodeEditor::valueChanged(Value& value)
{
    if (value.refersToSameSourceAs(sharedSearchTabsValue))
    {
        // The shared search tabs value has changed
        bool newState = sharedSearchTabsValue.getValue();
        DBG("CtrlrLuaMethodCodeEditor::valueChanged - new state: " + String(newState ? "true" : "false"));
        // Also check if the hidden toggle updated correctly
        DBG("Hidden toggle state: " + String(hiddenSearchTabsToggle->getToggleState() ? "true" : "false"));
        if (value.refersToSameSourceAs(sharedSearchTabsValue))
    {
        bool newState = sharedSearchTabsValue.getValue();
        DBG("CtrlrLuaMethodCodeEditor::valueChanged - new state: " + String(newState ? "true" : "false"));
        
        // Also check if the hidden toggle updated correctly
        DBG("Hidden toggle state: " + String(hiddenSearchTabsToggle->getToggleState() ? "true" : "false"));
    }
        // Add your custom logic here that should happen when the toggle state changes
        if (newState)
        {
            // Handle when search tabs are enabled
            DBG("Search tabs enabled in CtrlrLuaMethodCodeEditor");
        }
        else
        {
            // Handle when search tabs are disabled
            DBG("Search tabs disabled in CtrlrLuaMethodCodeEditor");
        }

        // The hiddenSearchTabsToggle will automatically update because it's referencing the same value
        // You can also trigger any additional UI updates here if needed
    }
}

juce::String CtrlrLuaMethodCodeEditor::getWordBeforeCaret(int& startOfWord, int offset) // Added v5.6.35. Autocomplete typing feature
{
    int caretPos = editorComponent->getCaretPos().getPosition() + offset;
    
    if (caretPos <= 0) return "";

    juce::CodeDocument::Position pos (document, caretPos);

    while (pos.getPosition() > 0)
    {
        juce::CodeDocument::Position prevPos = pos.movedBy(-1);
        juce::juce_wchar c = prevPos.getCharacter();

        if (!juce::CharacterFunctions::isLetterOrDigit(c) && c != '_')
            break;

        pos = prevPos;
    }

    startOfWord = pos.getPosition();
    
    juce::CodeDocument::Position start(document, startOfWord);
    juce::CodeDocument::Position end(document, caretPos);

    return document.getTextBetween(start, end).trim();
}

void CtrlrLuaMethodCodeEditor::handleSuggestionChosen(const SuggestionItem& item)
{
    if (suggestionPopup)
        suggestionPopup->setVisible(false);

    juce::String chosenText = item.text;
    SuggestionType chosenType = item.type; // Extract the type from the struct

    // Strip for clean replacement
    if (chosenText.contains(" "))
        chosenText = chosenText.upToFirstOccurrenceOf(" ", false, false);
    
    while (chosenText.endsWith(".") || chosenText.endsWith(":"))
        chosenText = chosenText.dropLastCharacters(1);

    // This now matches the 3-argument signature
    performReplacement(chosenText, true, chosenType);
    
    if (editorComponent)
        editorComponent->grabKeyboardFocus();
}

bool CtrlrLuaMethodCodeEditor::isLuaObjectInstance(const juce::String& s, SuggestionType type)
{
    if (type == TypeClass)
        return true;

    static const char* objectNames[] = { "panel", "mod", "owner", "source", "instance" };

    for (auto* name : objectNames)
    {
        if (s.equalsIgnoreCase(name))
            return true;
    }

    return false;
}

void CtrlrLuaMethodCodeEditor::performReplacement(const juce::String& suggestion, bool triggerMethods, SuggestionType type)
{
    if (callTip != nullptr) { callTip->setVisible(false); callTip->setTipText(""); }
    
    auto& manager = owner.getAutocompleteManager();
    // Split the suggestion (e.g., "translation (float x)" -> "translation")
    juce::String cleanSuggestion = suggestion.upToFirstOccurrenceOf(" ", false, false);
    
    int wordStart = 0;
    juce::String currentWord = getWordBeforeCaret(wordStart);
    int actualLen = currentWord.length();

    juce::String methodParams = "";
    juce::String className = "";
    juce::String allText = document.getAllContent();
    
    nextTabJumpPosition = -1;
    int validParamCount = 0;

    // 1. CONTEXT RESOLUTION
    bool isFollowingDotOrColon = false;
    if (wordStart > 0) {
        juce::juce_wchar separator = allText[wordStart - 1];
        if (separator == '.' || separator == ':') {
            isFollowingDotOrColon = true;
            int varStart = wordStart - 1;
            while (varStart > 0 && (juce::CharacterFunctions::isLetterOrDigit(allText[varStart - 1]) || allText[varStart - 1] == '_'))
                varStart--;
            
            juce::String varName = allText.substring(varStart, wordStart - 1);
            className = manager.getClassNameForVariable(varName, allText);
            
            if (className.isEmpty() && manager.getClassNames().contains(varName, true))
                className = varName;
        }
    }

    isReplacingText = true;
    this->triggerSuggestionsAfterReplacement = triggerMethods;
    document.newTransaction();

    if (actualLen > 0)
        document.deleteSection(wordStart, wordStart + actualLen);

    juce::String textToInsert = cleanSuggestion;
    int caretOffsetFromEnd = 0;
    juce::String forcedSeparator = "";
	
    // 2. LOGIC BY TYPE
    methodParams = manager.getMethodParams(className.isEmpty() ? cleanSuggestion : className, cleanSuggestion);

    bool shouldAddParentheses = false;
    bool shouldAddDot = false;

    if (type == TypeMethod || type == TypeUtility) {
        shouldAddParentheses = true;
    }
    else if (type == TypeStatic) {
        if (isFollowingDotOrColon) {
            // We are inside a class (e.g., AffineTransform.selection) -> Method Call
            shouldAddParentheses = true;
        } else {
            // We are at the global level (e.g., AffineTransform) -> Class selection, add dot
            shouldAddDot = true;
        }
    }
    else if (type == TypeClass) {
        shouldAddDot = true;
    }

    // --- HANDLE PARENTHESES ---
    if (shouldAddParentheses || methodParams.isNotEmpty())
    {
        this->triggerSuggestionsAfterReplacement = false;
        if (methodParams.trim().isNotEmpty() && methodParams.trim() != "()") {
            juce::StringArray overloads;
            overloads.addLines(methodParams);
            juce::String simplestLine = overloads[0];
            juce::String cleaned = simplestLine.replace("void ", "").replace("size_t ", "").replace("luabind::object ", "table ", false);
            juce::StringArray params;
            params.addTokens(cleaned, ",", "");
            juce::String suffix = "(";
            bool firstParamIsString = false;
            for (int i = 0; i < params.size(); ++i) {
                juce::String p = params[i].trim();
                if (p.isNotEmpty()) {
                    if (validParamCount > 0) suffix += ", ";
                    if (p.containsIgnoreCase("String") || p.containsIgnoreCase("name") || p.containsIgnoreCase("text")) {
                        suffix += "\"\"";
                        if (validParamCount == 0) firstParamIsString = true;
                    }
                    validParamCount++;
                }
            }
            suffix += ")";
            textToInsert += suffix;
            caretOffsetFromEnd = firstParamIsString ? (suffix.length() - 2) : (suffix.length() - 1);
        }
		else
        {
            // FALLBACK / EMPTY XML ARGS:
            textToInsert += "()";
            
            // --- STANDARDIZED CARET PLACEMENT ---
            
            // 1. Check if it's a "Getter"
            bool isGetter = cleanSuggestion.startsWithIgnoreCase("get") && !cleanSuggestion.containsIgnoreCase("ByName");
            
            // 2. Check if the suggestion is actually a Class Name (Constructor)
            bool isConstructor = false;
            
            // 3. Check if the method has an empty overload "()" in the API
            bool hasEmptyOverload = false;

            auto& autocompleteManager = owner.getAutocompleteManager();
            
            // Check Constructor
            if (autocompleteManager.getClassNames().contains(cleanSuggestion, true))
            {
                isConstructor = true;
            }
            
            // Check for Empty Overload (e.g., repaint, addBubble)
            // We use the last resolved class name to look up this specific method's params
            juce::String params = autocompleteManager.getMethodParams(lastAutocompletedClass, cleanSuggestion);
            if (params.contains("()") || params.isEmpty())
            {
                hasEmptyOverload = true;
            }

            // If it's a getter, a constructor, or has a no-arg version: caret at end ()|
            if (isGetter || isConstructor || hasEmptyOverload)
            {
                caretOffsetFromEnd = 0;
				
				// IMPORTANT: If this is a class that also has static methods,
                // we must EXPLICITLY disable the automatic dot trigger here.
                this->triggerSuggestionsAfterReplacement = false;
            }
            else
            {
                // It's a method that REQUIRES arguments: caret inside (|)
                caretOffsetFromEnd = 1;
            }
        }
	}
    // --- HANDLE DOTS ---
    else if (shouldAddDot)
    {
        if (!isFollowingDotOrColon) {
            forcedSeparator = ".";
            this->triggerSuggestionsAfterReplacement = true;
        }
    }
    // --- HANDLE GLOBAL HELPERS ---
    else if (type == TypeGlobal)
    {
        if (cleanSuggestion == "panel" || cleanSuggestion == "mod" || cleanSuggestion == "comp") {
            forcedSeparator = ":";
            this->triggerSuggestionsAfterReplacement = true;
        }
    }

    // 3. EXECUTION
    document.insertText(wordStart, textToInsert);
    int finalPos = wordStart + textToInsert.length() - caretOffsetFromEnd;
    editorComponent->moveCaretTo(juce::CodeDocument::Position(document, finalPos), false);
    document.newTransaction();

    if (validParamCount > 0) nextTabJumpPosition = finalPos;

	// 5. TRIGGERING (Auto-inserting . or :)
	if (this->triggerSuggestionsAfterReplacement && forcedSeparator.isNotEmpty())
	{
		// FIX: Use the 'type' parameter from the function signature,
		// not 'suggestion.type' (since suggestion is a String).
		if (type == TypeClass)
		{
			forcedSeparator = "";
			this->triggerSuggestionsAfterReplacement = false;
		}
		
		if (this->triggerSuggestionsAfterReplacement && forcedSeparator.isNotEmpty())
		{
			juce::Timer::callAfterDelay(100, [this, forcedSeparator]() {
				this->triggerSuggestionsAfterReplacement = false;
				int caretPos = editorComponent->getCaretPos().getPosition();
				if (caretPos > 0 && document.getAllContent().substring(caretPos - 1, caretPos) != forcedSeparator)
				{
					this->isReplacingText = true;
					this->document.insertText(caretPos, forcedSeparator);
					editorComponent->moveCaretTo(juce::CodeDocument::Position(document, caretPos + 1), false);
					this->isReplacingText = false;
					this->codeDocumentTextInserted(forcedSeparator, caretPos);
				} else {
					this->isReplacingText = false;
				}
			});
		}
		else
		{
			isReplacingText = false;
		}
	}
	else
	{
		isReplacingText = false;
	}
	
	// 6. CALLTIP / CONTEXT SYNC
    if (type == TypeClass)
    {
        // For Constructors: Store the class name so the next ":" works
        lastAutocompletedClass = cleanSuggestion;
        lastAutocompletedMethod = ""; // Clear method since we just made an object
    }
    else if (methodParams.trim().isNotEmpty() && methodParams.trim() != "()")
    {
        // For Methods with arguments: show the yellow call-tip box
        lastAutocompletedMethod = cleanSuggestion;
        lastAutocompletedClass = className.isEmpty() ? cleanSuggestion : className;
        
        juce::MessageManager::callAsync([this]() {
            if (callTip) {
                updateCallTipHighlight();
                callTip->setVisible(true);
            }
        });
    }
}
