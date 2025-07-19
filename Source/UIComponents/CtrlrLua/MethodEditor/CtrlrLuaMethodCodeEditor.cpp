#include "stdafx.h"
#include "CtrlrManager/CtrlrManager.h"
#include "CtrlrLuaMethodCodeEditor.h"
#include "CtrlrLuaMethodEditor.h"
#include "CtrlrPanel/CtrlrPanel.h"
#include "Methods/CtrlrLuaMethodManager.h"
#include "CtrlrLuaDebugger.h"
#include "CtrlrLuaManager.h"

CtrlrLuaMethodCodeEditor::CtrlrLuaMethodCodeEditor(CtrlrLuaMethodEditor &_owner, CtrlrLuaMethod *_method)
	:	editorComponent(nullptr),
		owner(_owner),
		method(_method),
		lastFoundPosition(-1)
{
	addAndMakeVisible (editorComponent	= new GenericCodeEditorComponent (*this,
															document, codeTokeniser = new CtrlrLuaCodeTokeniser()));

	editorComponent->setScrollbarThickness(owner.getOwner().getOwner().getProperty(Ids::ctrlrScrollbarThickness));
	document.replaceAllContent(method->getCode());
	document.setSavePoint();

	document.addListener (this);
	editorComponent->addMouseListener (this, true);
	editorComponent->addKeyListener (this);
    
    editorComponent->setColour(CodeEditorComponent::backgroundColourId, Colours::white); // findColour(CodeEditorComponent::backgroundColourId));
    editorComponent->setColour(CodeEditorComponent::defaultTextColourId, Colours::black); // findColour(CodeEditorComponent::defaultTextColourId));
    editorComponent->setColour(CodeEditorComponent::highlightColourId, findColour(CodeEditorComponent::highlightColourId));
    editorComponent->setColour(CodeEditorComponent::lineNumberTextId, findColour(CodeEditorComponent::lineNumberTextId));
    editorComponent->setColour(CodeEditorComponent::lineNumberBackgroundId, findColour(CodeEditorComponent::lineNumberBackgroundId));
    
	if (method != nullptr)
		method->setCodeEditor (this);

	if (owner.getComponentTree().hasProperty (Ids::luaMethodEditorFont))
	{
		setFontAndColour (owner.getOwner().getCtrlrManagerOwner().getFontManager().getFontFromString (owner.getComponentTree().getProperty (Ids::luaMethodEditorFont)),
        VAR2COLOUR(owner.getComponentTree().getProperty (Ids::luaMethodEditorBgColour, Colours::white.toString()))); // (String) findColour(CodeEditorComponent::backgroundColourId).toString())));

        editorComponent->setColour(CodeEditorComponent::lineNumberTextId,
        VAR2COLOUR(owner.getComponentTree().getProperty (Ids::luaMethodEditorLineNumbersColour, Colours::grey.toString()))); // Added v5.6.31
        
        editorComponent->setColour(CodeEditorComponent::lineNumberBackgroundId,
        VAR2COLOUR(owner.getComponentTree().getProperty (Ids::luaMethodEditorLineNumbersBgColour, Colours::cyan.toString()))); // Added v5.6.31
	}
	else
	{
		setFontAndColour (Font (Font::getDefaultMonospacedFontName(), 14.0f, Font::plain), Colours::white); // findColour(CodeEditorComponent::backgroundColourId));
	}
	//editorComponent->grabKeyboardFocus();
}

CtrlrLuaMethodCodeEditor::~CtrlrLuaMethodCodeEditor()
{
	masterReference.clear();

	if (method)
		method->setCodeEditor (nullptr);

	document.removeListener (this);
	deleteAndZero(editorComponent);
	deleteAndZero (codeTokeniser);
}

void CtrlrLuaMethodCodeEditor::resized()
{
	editorComponent->setBounds (0,0,getWidth(),getHeight());
}

void CtrlrLuaMethodCodeEditor::mouseDown (const MouseEvent &e)
{
	CodeDocument::Position pos = editorComponent->getCaretPos();
	String url;
	if (isMouseOverUrl (pos, &url))
	{
		URL(url).launchInDefaultBrowser();
	}
	owner.setPositionLabelText ("Line:  " + String(pos.getLineNumber()+1) + " Column: " + String(pos.getIndexInLine()));
}

void CtrlrLuaMethodCodeEditor::mouseMove (const MouseEvent &e)
{
	if (e.eventComponent == editorComponent)
	{
		CodeDocument::Position pos = editorComponent->getPositionAt (e.x, e.y);
		if (isMouseOverUrl (pos))
		{
			editorComponent->setMouseCursor (MouseCursor::PointingHandCursor);
			return;
		}
		else if (editorComponent->getMouseCursor() == MouseCursor::PointingHandCursor)
		{
			editorComponent->setMouseCursor (MouseCursor::IBeamCursor);
		}
	}
}

bool CtrlrLuaMethodCodeEditor::keyStateChanged (bool isKeyDown, Component *originatingComponent)
{
	CodeDocument::Position pos = editorComponent->getCaretPos();
	owner.setPositionLabelText ("Line:  " + String(pos.getLineNumber()+1) + " Column: " + String(pos.getIndexInLine()));
	return (false);
}

bool CtrlrLuaMethodCodeEditor::keyPressed(const KeyPress& key, Component* originatingComponent)
{
	if (key.getModifiers().isCommandDown())
	{
		if (key.getKeyCode() == 68) // CTRL + D
		{
			duplicateCurrentLine();
			return true;
		}
		if (key.getKeyCode() == 81) // CTRL + Q
		{
			toggleLongLineComment();
			return true;
		}
		//// Check for CTRL + SHIFT + / first (more specific condition)
		//if (key.getModifiers().isShiftDown() && (key.getKeyCode() == 191 || key.getKeyCode() == 47))
		//{
		//	toggleLongLineComment();
		//	return true;
		//}

		// Then check for CTRL + / (but NOT with Shift)
		if (!key.getModifiers().isShiftDown() && (key.getKeyCode() == 191 || key.getKeyCode() == 47))
		{
			toggleLineComment();
			return true;
		}
		if(key.getKeyCode() == 9)
		{
			owner.keyPressed (key, originatingComponent);
			return (true);
		}
		if (key.getKeyCode() == 83) // CTRL + S
		{
			saveDocument();
			return (true);
		}
		if (CharacterFunctions::toUpperCase ((juce_wchar) (key.getKeyCode())) == 70) // CTRL + F
		{
			// Show search Dialog
			editorComponent->showFindPanel();
			return (true);
		}
		if (CharacterFunctions::toUpperCase ((juce_wchar) (key.getKeyCode())) == 71) // CTRL + G
		{
			// Show Go To Dialog
			editorComponent->showGoTOPanel();
			return (true);
		}
		if (CharacterFunctions::toUpperCase ((juce_wchar) (key.getKeyCode())) == 72) // CTRL + H
		{
			// Show search Dialog
			editorComponent->showFindPanel(true);
			return (true);
		}

		if (key.getKeyCode() == KeyPress::deleteKey) // CTRL + Delete
		{
			owner.keyPressed(key, originatingComponent);
			return (true);
		}

		// search selected previous in current
		if (key.getModifiers().isShiftDown()
			&& key.getKeyCode() == KeyPress::F3Key) // CTRL + SHIFT + F3
		{
			editorComponent->findSelection(false);
			return (true);
		}
	}

	// search selected next in current
	if (key.getModifiers().isShiftDown() && key.getKeyCode() == KeyPress::F3Key) // SHIFT + F3
	{
		editorComponent->findSelection(true);
		return (true);
	}

	if (key.getKeyCode() == KeyPress::F7Key)
	{
		saveAndCompileDocument();
		return (true);
	}

	if (key.getKeyCode() == KeyPress::F8Key)
	{
		owner.saveAndCompilAllMethods();
		return (true);
	}

	CodeDocument::Position pos = editorComponent->getCaretPos();
	owner.setPositionLabelText ("Line:  " + String(pos.getLineNumber()+1) + " Column: " + String(pos.getIndexInLine()));
	return (false);
}

void CtrlrLuaMethodCodeEditor::codeDocumentTextInserted (const String& newText, int insertIndex)
{
	document.newTransaction();
	documentChanged(false);
}

void CtrlrLuaMethodCodeEditor::codeDocumentTextDeleted (int startIndex, int endIndex)
{
	document.newTransaction();
	documentChanged(false);
}

void CtrlrLuaMethodCodeEditor::documentChanged(const bool save, const bool recompile)
{
	owner.tabChanged (this, save, recompile);
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
		{	// This method is stored within the panel => set the panel dirty
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
		{	// This method is stored within the panel => set the panel dirty
			owner.getOwner().luaManagerChanged();
		}
	}

	documentChanged(true, true);
}

const bool CtrlrLuaMethodCodeEditor::isMouseOverUrl(CodeDocument::Position &position, String *url)
{
	if (position.getPosition() >= document.getNumCharacters())
	{
		return (false);
	}

	int moveLeft=0;

	while (!CharacterFunctions::isWhitespace(position.getCharacter()))
	{
		if (position.getPosition() <= 0)
			break;

		position.moveBy(-1);
		moveLeft++;
	}

	int start = position.getPosition();
	position.setPosition (position.getPosition()+moveLeft);

	while (!CharacterFunctions::isWhitespace(position.getCharacter()))
	{
		if (position.getPosition() >= document.getNumCharacters())
			break;

		position.moveBy(1);
	}

	int end = position.getPosition();

	const String word = document.getTextBetween (CodeDocument::Position(document, start), CodeDocument::Position(document, end)).trim();

	if (word.startsWith ("http://"))
	{
		if (url)
		{
			*url = word;
		}
		return (URL::isProbablyAWebsiteURL (word));
	}

	return (false);
}

void CtrlrLuaMethodCodeEditor::handleAsyncUpdate()
{
	owner.tabChanged (this, false, false);
}

void CtrlrLuaMethodCodeEditor::setErrorLine (const int lineNumber)
{
	editorComponent->scrollToLine (lineNumber-1);
	CodeDocument::Position start(document, lineNumber-1,0);
	CodeDocument::Position end(document,lineNumber-1,document.getLine (lineNumber-1).length());
	editorComponent->setHighlightedRegion(Range<int>(start.getPosition(), end.getPosition()));
}

void CtrlrLuaMethodCodeEditor::setFontAndColour (const Font newFont, const Colour newColour)
{
	editorComponent->setColour (CodeEditorComponent::backgroundColourId, newColour);
	editorComponent->setFont (newFont);
}

void CtrlrLuaMethodCodeEditor::findNextMatch(const String & search, bool bMatchCase)
{
	if (owner.getCurrentEditor() == nullptr)
	{
		return;
	}

	CodeDocument &doc = owner.getCurrentEditor()->getCodeDocument();
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

void CtrlrLuaMethodCodeEditor::replaceNextMatch(const String &search, const String &replace, bool bMatchCase)
{
	if (owner.getCurrentEditor() == nullptr)
	{
		return;
	}

	CodeDocument &doc		= owner.getCurrentEditor()->getCodeDocument();
	findNextMatch(search, bMatchCase);
	if (lastFoundPosition >= 0)
	{
		doc.newTransaction();
		doc.deleteSection (lastFoundPosition, lastFoundPosition+search.length());
		doc.insertText (lastFoundPosition, replace);
	}
}

void CtrlrLuaMethodCodeEditor::replaceAllMatches(const String &search, const String &replace,bool bMatchCase)
{
	lastFoundPosition = -1;
	do
	{
		replaceNextMatch(search, replace, bMatchCase);
	}
	while (lastFoundPosition >= 0);
}

void CtrlrLuaMethodCodeEditor::findInOpened(const String &search)
{
	if (owner.getTabs() == nullptr)
		return;

	StringArray names = owner.getTabs()->getTabNames();

	owner.getMethodEditArea()->insertOutput("\n\nSearching for: \""+search+"\" in all opened methods (double click line to jump)\n", Colours::darkblue);

	for (int i=0; i<owner.getTabs()->getNumTabs(); i++)
	{
		CtrlrLuaMethodCodeEditor *codeEditor = dynamic_cast<CtrlrLuaMethodCodeEditor*>(owner.getTabs()->getTabContentComponent(i));

		if (codeEditor != nullptr )
		{
			CodeDocument &doc	= codeEditor->getCodeDocument() ;

			Array<Range<int> > results = searchForMatchesInDocument (doc, search);

			for (int j=0; j<results.size(); j++)
			{
				reportFoundMatch (doc, names[i], results[j]);
			}
		}
	}

	owner.getMethodEditArea()->getLowerTabs()->setCurrentTabIndex(0,true);
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

void CtrlrLuaMethodCodeEditor::findInAll(const String &search)
{
	// Validate search string first
	if (!isValidSearchString(search))
	{
		owner.getMethodEditArea()->insertOutput("\n\nInvalid search term: \"" + search +
			"\". Please enter a meaningful search term (at least 3 characters, not just whitespace or common single characters).\n",
			Colours::red);
		return;
	}
	owner.getMethodEditArea()->insertOutput("\n\nSearching for: \""+search+"\" in all methods (double click line to jump)\n", Colours::darkblue);
	StringArray names;

	for (int i=0; i<owner.getMethodManager().getNumMethods(); i++)
	{
		CtrlrLuaMethod *m = owner.getMethodManager().getMethodByIndex (i);

		if (m)
		{
			names.add (m->getName());

			if (m->getCodeEditor())
			{
				/* it has an editor so it's open */
				CodeDocument &doc		= m->getCodeEditor()->getCodeDocument();

				Array<Range<int> > results = searchForMatchesInDocument (doc, search);

				for (int j=0; j<results.size(); j++)
				{
					reportFoundMatch (doc, names[i], results[j]);
				}
			}
			else // Added 5.6.34 by goodweather. Search in not yet opened methods
			{
				/* Open method */
				owner.createNewTab(m);
				owner.setCurrentTab(m);

				/* Perform search and report result */
				CodeDocument& doc = m->getCodeEditor()->getCodeDocument();

				Array<Range<int> > results = searchForMatchesInDocument(doc, search);

				for (int j = 0; j < results.size(); j++)
				{
					reportFoundMatch(doc, names[i], results[j]);
				}

				/* If no result then close method; if any result then keep method open */
				/*Dnaldoog disable this because I think it's better for ser to open file at bottom list, 
				especially if the search results in dozens of hits therefore opening dozens of windows*/
				//if (results.size() == 0)
				//{
					owner.closeCurrentTab();
				//}
			}
		}
	}

	owner.getMethodEditArea()->getLowerTabs()->setCurrentTabIndex(0,true);
}

const Array<Range<int> > CtrlrLuaMethodCodeEditor::searchForMatchesInDocument(CodeDocument &doc, const String &search)
{
	Array<Range<int> > results;
	int position			= -1;
	lastFoundPosition		= -1;
	do
	{
		String documentContent = doc.getAllContent();
		if (documentContent.isNotEmpty())
		{
			if (editorComponent->isCaseSensitiveSearch())
			{
				position = documentContent.indexOfIgnoreCase (lastFoundPosition+1, search);
			}
			else
			{
				position = documentContent.indexOf (lastFoundPosition+1, search);
			}
		}


		if (position >= 0)
		{
			lastFoundPosition		= position;
			results.add (Range<int> (lastFoundPosition, lastFoundPosition+search.length()));
		}
		else
		{
			lastFoundPosition = -1;
		}
	}
	while (lastFoundPosition >= 0);

	return (results);
}

void CtrlrLuaMethodCodeEditor::reportFoundMatch (CodeDocument &doc, const String &methodName, const Range<int> range)
{
	CodeDocument::Position pos (doc, range.getStart());
	AttributedString as;
    as.append ("Method: ", Colours::black);
	as.append (methodName, Colours::blue);
 
    as.append ("\tline: ", Colours::black);
	as.append (String(pos.getLineNumber()+1), Colours::darkgreen);

    as.append ("\tstart: ", Colours::black);
	as.append (String(range.getStart()), Colours::darkgreen);

    as.append ("\tend: ", Colours::black);
	as.append (String(range.getEnd()), Colours::darkgreen);

	owner.getMethodEditArea()->insertOutput (as);
}

void CtrlrLuaMethodCodeEditor::gotoLine(int position, const bool selectLine)
{
	editorComponent->scrollToLine(position-3);
	editorComponent->moveCaretTo(CodeDocument::Position(document, position - 1, 0), false);
	if (selectLine)
    {
        editorComponent->selectRegion (CodeDocument::Position (document, position - 1, 0), CodeDocument::Position (document, position - 1, 1024));
    }
	//editorComponent->selectRegion(CodeDocument::Position(document, position - 1, 0), CodeDocument::Position(document, position - 1, 1));
	editorComponent->grabKeyboardFocus();
	editorComponent->hideGoTOPanel();
}

CtrlrLuaMethodEditor &CtrlrLuaMethodCodeEditor::getOwner()
{
    return  (owner);
}

//==============================================================================
class GenericCodeEditorComponent::GoToPanel  : public Component,
	private TextEditor::Listener,
	private Button::Listener
{
public:
	GoToPanel() : goToButton("", 0.0, Colours::white)
	{
		editor.setColour (CaretComponent::caretColourId, Colours::black);
		editor.setInputRestrictions(5, ("1234567890"));
		editor.addListener (this);
		addAndMakeVisible (editor);
		goToButton.addListener(this);
		addAndMakeVisible (goToButton);

		label.setText ("Go To:", dontSendNotification);
		label.setColour (Label::textColourId, Colours::white);
		label.attachToComponent (&editor, true);

		setWantsKeyboardFocus (false);
		setFocusContainer (true);
	}

	void paint (Graphics& g) override
	{
		Path outline;
		outline.addRoundedRectangle (1.0f, 1.0f, getWidth() - 2.0f, getHeight() - 2.0f, 8.0f);

		g.setColour (Colours::black.withAlpha (0.6f));
		g.fillPath (outline);
		g.setColour (Colours::white.withAlpha (0.8f));
		g.strokePath (outline, PathStrokeType (1.0f));
	}

	void resized() override
	{
		int y = 5;
		editor.setBounds (45, y, getWidth() - 90, 24);
		goToButton.setBounds(editor.getRight() + 5, y+1, 30, 24);
	}
	void buttonClicked (Button* button) override
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

	void textEditorTextChanged (TextEditor&) override	{}

	void textEditorFocusLost (TextEditor&) override {}

	void textEditorReturnKeyPressed (TextEditor& textEditor) override
	{
		GenericCodeEditorComponent* ed = getOwner();
		if (ed)
		{
			ed->gotoLine(editor.getText().getIntValue());
		}
	}

	void textEditorEscapeKeyPressed (TextEditor&) override
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
class GenericCodeEditorComponent::FindPanel  : public Component,
	private TextEditor::Listener,
	private Button::Listener,
	private ComboBox::Listener
{
public:
	FindPanel()
		: caseButton ("Match Case"),
		findPrev ("<", "Search Previous"),
		findNext (">", "Search Next"),
		searchButton("", 0.0, Colours::white)
	{
		editor.setColour (CaretComponent::caretColourId, Colours::black);
		editor.addListener (this);
		addAndMakeVisible (editor);
		searchButton.addListener(this);
		addAndMakeVisible (searchButton);

		label.setText ("Find:", dontSendNotification);
		label.setColour (Label::textColourId, Colours::white);
		label.attachToComponent (&editor, true);

		searchInComboBox.setEditableText (false);
		searchInComboBox.setJustificationType (Justification::centredLeft);
		searchInComboBox.addItem (TRANS("Editor"), 1);
		searchInComboBox.addItem (TRANS("Output"), 2);
		searchInComboBox.addItem (TRANS("Methods"), 3);
		searchInComboBox.setSelectedItemIndex(0, dontSendNotification);
		searchInComboBox.addListener(this);
		searchInComboBox.setEnabled(false);
		addAndMakeVisible(searchInComboBox);

		addAndMakeVisible (caseButton);
		caseButton.setColour (ToggleButton::textColourId, Colours::white);
		caseButton.setToggleState (false, dontSendNotification);
		caseButton.addListener (this);

		lookInComboBox.setEditableText (false);
		lookInComboBox.setJustificationType (Justification::centredLeft);
		lookInComboBox.addItem (TRANS("Current"), 1);
		lookInComboBox.addItem (TRANS("All Open"), 2);
		lookInComboBox.addItem (TRANS("All"), 3);
		lookInComboBox.setSelectedItemIndex(0, dontSendNotification);
		lookInComboBox.addListener(this);
		addAndMakeVisible(lookInComboBox);

		findPrev.setConnectedEdges (Button::ConnectedOnRight);
		findPrev.addListener(this);
		findNext.setConnectedEdges (Button::ConnectedOnLeft);
		findNext.addListener(this);
		addAndMakeVisible (findPrev);
		addAndMakeVisible (findNext);

		setWantsKeyboardFocus (false);
		setFocusContainer (true);
		findPrev.setWantsKeyboardFocus (true);
		findNext.setWantsKeyboardFocus (true);
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
		if(GenericCodeEditorComponent* ed = getOwner())
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
		replaceEditor->setColour (CaretComponent::caretColourId, Colours::black);
		addAndMakeVisible (*replaceEditor);

		replaceLabel = new Label();
		replaceLabel->setText ("Replace:", dontSendNotification);
		replaceLabel->setFont(Font (13.00f, Font::plain));
		replaceLabel->setColour (Label::textColourId, Colours::white);
		replaceLabel->attachToComponent (replaceEditor, true);

		replaceButton = new TextButton(">", "Replace Next");
		replaceButton->addListener(this);
		addAndMakeVisible (*replaceButton);

		replaceAllButton = new TextButton("A", "Replace all in current");
		replaceAllButton->addListener(this);
		addAndMakeVisible (*replaceAllButton);

		replaceButton->setConnectedEdges (Button::ConnectedOnRight);
		replaceAllButton->setConnectedEdges (Button::ConnectedOnLeft);

		replaceEditor->addListener (this);
		resized();
	}

	void paint (Graphics& g) override
	{
		Path outline;
		outline.addRoundedRectangle (1.0f, 1.0f, getWidth() - 2.0f, getHeight() - 2.0f, 8.0f);

		g.setColour (Colours::black.withAlpha (0.6f));
		g.fillPath (outline);
		g.setColour (Colours::white.withAlpha (0.8f));
		g.strokePath (outline, PathStrokeType (1.0f));
	}

	void resized() override
	{
		int y = 5;
		editor.setBounds (45, y, getWidth() - 90, 24);
		searchButton.setBounds(editor.getRight() + 5, y+1, 30, 24);
		y += 30;
		if (replaceEditor != nullptr)
		{
			replaceEditor->setBounds (45, y, getWidth() - 90, 24);
			replaceButton->setBounds(replaceEditor->getRight(), y, 20, 24);
			replaceAllButton->setBounds(replaceButton->getRight(), y, 20, 24);
			y += 30;
		}
		caseButton.setBounds (0, y, 75, 22);
		searchInComboBox.setBounds(caseButton.getRight(), y, 70, 24);
		lookInComboBox.setBounds(searchInComboBox.getRight() + 5, y, 70, 24);
		findPrev.setBounds (lookInComboBox.getRight(), y, 15, 24);
		findNext.setBounds (findPrev.getRight(), y, 15, 24);
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
			if(GenericCodeEditorComponent* ed = getOwner())
				ed->getCtrlrLuaMethodCodeEditor().getCtrlrLuaMethodEditor().searchInString=
				searchInComboBox.getText();
		}
		else if (combo == &lookInComboBox)
		{
			if(GenericCodeEditorComponent* ed = getOwner())
				ed->getCtrlrLuaMethodCodeEditor().getCtrlrLuaMethodEditor().lookInString =
				lookInComboBox.getText();
		}
	}

	void buttonClicked (Button* button) override
	{
		GenericCodeEditorComponent* ed = getOwner();
		if (ed)
		{
			if (button == &findNext )
			{
				ed->findNext (true, true);
			}
			else if (button == &findPrev )
			{
				ed->findNext (false, false);
			}
			else if (button == replaceButton)
			{
				ed->replaceNextMatch (editor.getText(), replaceEditor->getText(), caseButton.getToggleState());
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

	void textEditorTextChanged (TextEditor& te) override
	{
		switch(searchInComboBox.getSelectedItemIndex())
		{
		case 0:
			if (GenericCodeEditorComponent* ed = getOwner())
			{
				ed->findNext (true, false);
				ed->getCtrlrLuaMethodCodeEditor().getCtrlrLuaMethodEditor().currentSearchString = editor.getText();
			}
			break;
		default:
			break;
		}
	}

	void textEditorFocusLost (TextEditor&) override {}

	void textEditorReturnKeyPressed (TextEditor& textEditor) override
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
			switch(lookInComboBox.getSelectedItemIndex())
			{
			case 0: // code editor current
				switch(searchInComboBox.getSelectedItemIndex())
				{
				case 0: // code editor
					ed->findNext (true, true);
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

	void textEditorEscapeKeyPressed (TextEditor&) override
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
GenericCodeEditorComponent::GenericCodeEditorComponent (CtrlrLuaMethodCodeEditor &_owner, CodeDocument &codeDocument,
	CodeTokeniser* tokeniser)	: CodeEditorComponent (codeDocument, tokeniser), owner(_owner), bSensitive(false),
	lookUpString("")
{
    setColour (CodeEditorComponent::lineNumberTextId, Colours::black);
}

GenericCodeEditorComponent::~GenericCodeEditorComponent()
{
}

void GenericCodeEditorComponent::resized()
{
	CodeEditorComponent::resized();

	if (findPanel != nullptr)
	{
		findPanel->setSize (jmin (260, getWidth() - 32), 100);
		findPanel->setTopRightPosition (getWidth() - 6, 4);
	}
	if (goToPanel != nullptr)
	{
		goToPanel->setSize (jmin (160, getWidth() - 32), 40);
		goToPanel->setTopRightPosition (getWidth() - 6, getHeight() -40);
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
		addAndMakeVisible (findPanel);
		resized();
	}

	if (findPanel != nullptr)
	{
		findPanel->setSearchText(getTextInRange (getHighlightedRegion()));
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
		addAndMakeVisible (goToPanel);
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
			findNext (true, true);
		}
		else
		{
			findNext (false, false);
		}
	}
}

void GenericCodeEditorComponent::findNext (bool forwards, bool skipCurrentSelection)
{
	const Range<int> highlight (getHighlightedRegion());
	const CodeDocument::Position startPos (getDocument(), skipCurrentSelection ? highlight.getEnd()
		: highlight.getStart());
	int lineNum = startPos.getLineNumber();
	int linePos = startPos.getIndexInLine();

	const int totalLines = getDocument().getNumLines();
	const String searchText (getSearchString());
	const bool caseSensitive = isCaseSensitiveSearch();

	for (int linesToSearch = totalLines; --linesToSearch >= 0;)
	{
		String line (getDocument().getLine (lineNum));
		int index;

		if (forwards)
		{
			index = caseSensitive ? line.indexOf (linePos, searchText)
				: line.indexOfIgnoreCase (linePos, searchText);
		}
		else
		{
			if (linePos >= 0)
				line = line.substring (0, linePos);

			index = caseSensitive ? line.lastIndexOf (searchText)
				: line.lastIndexOfIgnoreCase (searchText);
		}

		if (index >= 0)
		{
			const CodeDocument::Position p (getDocument(), lineNum, index);
			selectRegion (p, p.movedBy (searchText.length()));
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
		searchString = getTextInRange (getHighlightedRegion());
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

void GenericCodeEditorComponent::replaceAllMatches(const String &search, const String &replace, bool bMatchCase)
{
	owner.replaceAllMatches(search,replace,bMatchCase);
}
void GenericCodeEditorComponent::replaceNextMatch(const String &search, const String &replace, bool bMatchCase)
{
	owner.replaceNextMatch(search, replace,bMatchCase);
}
void GenericCodeEditorComponent::findInAll(const String &search)
{
	owner.findInAll(search);
}
void GenericCodeEditorComponent::findInOpened(const String &search)
{
	owner.findInOpened(search);
}

CtrlrLuaDebugger &GenericCodeEditorComponent::getDebugger()
{
    return (owner.getOwner().getOwner().getCtrlrLuaManager().getDebugger());
}

void GenericCodeEditorComponent::markedLinesChanged(int lineNumber, bool isNowSelected)
{
    getDebugger().setBreakpoint (lineNumber, owner.getMethod() ? owner.getMethod()->getName() : "ctrlr", isNowSelected);
}
/***************************************************************************************/

	void CtrlrLuaMethodCodeEditor::duplicateCurrentLine()
	{
		if (!editorComponent)
			return;

		CodeDocument::Position caretPos = editorComponent->getCaretPos();
		int lineNumber = caretPos.getLineNumber();

		// Get the current line content
		String currentLine = document.getLine(lineNumber);

		// Move to end of current line
		CodeDocument::Position endOfLine(document, lineNumber, currentLine.length());

		// Insert newline and duplicate the line
		document.newTransaction();
		document.insertText(endOfLine.getPosition(), "\n" + currentLine);

		// Move caret to the same position on the new line
		int newLineNumber = lineNumber + 1;
		CodeDocument::Position newCaretPos(document, newLineNumber, caretPos.getIndexInLine());
		editorComponent->moveCaretTo(newCaretPos, false);

		documentChanged(false, false);
	}


	void CtrlrLuaMethodCodeEditor::toggleLineComment()
	{
		if (!editorComponent)
			return;

		Range<int> selection = editorComponent->getHighlightedRegion();
		CodeDocument::Position startPos(document, selection.getStart());
		CodeDocument::Position endPos(document, selection.getEnd());

		int startLine = startPos.getLineNumber();
		int endLine = endPos.getLineNumber();

		// If selection ends at start of line, don't include that line
		if (endPos.getIndexInLine() == 0 && endLine > startLine)
			endLine--;

		document.newTransaction();

		// Check if all lines are commented (to decide whether to comment or uncomment)
		bool allLinesCommented = true;
		for (int lineNum = startLine; lineNum <= endLine; lineNum++)
		{
			String line = document.getLine(lineNum);
			String trimmed = line.trimStart();
			if (trimmed.isNotEmpty() && !trimmed.startsWith("--"))
			{
				allLinesCommented = false;
				break;
			}
		}

		// Store the original caret position
		CodeDocument::Position originalCaret = editorComponent->getCaretPos();
		int caretOffset = 0;

		if (allLinesCommented)
		{
			// Uncomment lines
			for (int lineNum = startLine; lineNum <= endLine; lineNum++)
			{
				String line = document.getLine(lineNum);

				// Find the first occurrence of "--" and remove it
				int commentPos = line.indexOf("--");
				if (commentPos >= 0)
				{
					CodeDocument::Position lineStart(document, lineNum, 0);
					CodeDocument::Position commentStart(document, lineNum, commentPos);
					CodeDocument::Position commentEnd(document, lineNum, commentPos + 2);

					// Remove the "--"
					document.deleteSection(commentStart.getPosition(), commentEnd.getPosition());

					// Adjust caret position if it's on this line and after the comment
					if (lineNum == originalCaret.getLineNumber() && originalCaret.getIndexInLine() > commentPos)
					{
						caretOffset = -2; // "--" was removed
					}
				}
			}
		}
		else
		{
			// Comment lines
			for (int lineNum = startLine; lineNum <= endLine; lineNum++)
			{
				String line = document.getLine(lineNum);
				String trimmed = line.trimStart();

				// Skip empty lines
				if (trimmed.isEmpty())
					continue;

				// Find first non-whitespace character
				int firstNonSpace = line.indexOfAnyOf(" \t", 0, false);
				if (firstNonSpace < 0)
					firstNonSpace = 0;
				else
					firstNonSpace = line.length() - line.trimStart().length();

				CodeDocument::Position insertPos(document, lineNum, firstNonSpace);
				document.insertText(insertPos.getPosition(), "--");

				// Adjust caret position if it's on this line and at/after the insertion point
				if (lineNum == originalCaret.getLineNumber() && originalCaret.getIndexInLine() >= firstNonSpace)
				{
					caretOffset = 2; // "--" was added
				}
			}
		}

		// Restore caret position with adjustment
		if (caretOffset != 0)
		{
			int newCaretIndex = jmax(0, originalCaret.getIndexInLine() + caretOffset);
			CodeDocument::Position newCaret(document, originalCaret.getLineNumber(), newCaretIndex);
			editorComponent->moveCaretTo(newCaret, false);
		}
		else
		{
			editorComponent->moveCaretTo(originalCaret, false);
		}

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