#include "stdafx.h"
#include "CtrlrLuaManager.h"
#include "CtrlrManager/CtrlrManager.h"
#include "CtrlrPanel/CtrlrPanel.h"

#include "CtrlrLuaConsole.h"
#include "CtrlrInlineUtilitiesGUI.h"


const StringArray joinFileArray (const Array<File> ar)
{
	StringArray s;

	for (int i=0; i<ar.size(); i++)
	{
		s.add (ar[i].getFullPathName());
	}
	return (s);
}

//==============================================================================
CtrlrLuaConsole::CtrlrLuaConsole (CtrlrPanel &_owner)
    : owner(_owner),
      luaConsoleOutput (0),
      luaConsoleInput (0),
      resizer (0)
{
    addAndMakeVisible (luaConsoleOutput = new CodeEditorComponent (outputDocument, 0));
    luaConsoleOutput->setName (L"luaConsoleOutput");
    luaConsoleOutput->setScrollbarThickness(owner.getOwner().getProperty(Ids::ctrlrScrollbarThickness));

    addAndMakeVisible (luaConsoleInput = new CodeEditorComponent (inputDocument, 0));
    luaConsoleInput->setName (L"luaConsoleInput");
    luaConsoleInput->setScrollbarThickness(owner.getOwner().getProperty(Ids::ctrlrScrollbarThickness));

    addAndMakeVisible (resizer = new StretchableLayoutResizerBar (&layoutManager, 1, false));

	layoutManager.setItemLayout (0, -0.001, -1.0, -0.69);
 	layoutManager.setItemLayout (1, -0.001, -0.01, -0.01);
 	layoutManager.setItemLayout (2, -0.001, -1.0, -0.30);

	luaConsoleInput->setFont (Font(owner.getCtrlrManagerOwner().getFontManager().getDefaultMonoFontName(), 15, Font::plain));
	luaConsoleOutput->setFont (Font(owner.getCtrlrManagerOwner().getFontManager().getDefaultMonoFontName(), 15, Font::plain));
	luaConsoleInput->setColour (CodeEditorComponent::backgroundColourId, Colour(0xffffffff)); // findColour(CodeEditorComponent::backgroundColourId));
	luaConsoleOutput->setColour (CodeEditorComponent::backgroundColourId, Colour(0xffffffff)); // findColour(CodeEditorComponent::backgroundColourId));
	luaConsoleInput->setColour (CodeEditorComponent::highlightColourId, findColour(CodeEditorComponent::highlightColourId));
	luaConsoleOutput->setColour (CodeEditorComponent::highlightColourId, findColour(CodeEditorComponent::highlightColourId));
	luaConsoleInput->setColour (CodeEditorComponent::defaultTextColourId, Colour(0xff000000)); // findColour(CodeEditorComponent::defaultTextColourId));
	luaConsoleOutput->setColour (CodeEditorComponent::defaultTextColourId, Colour(0xff000000)); // findColour(CodeEditorComponent::defaultTextColourId));
    luaConsoleInput->setColour (CodeEditorComponent::lineNumberBackgroundId, findColour(CodeEditorComponent::lineNumberBackgroundId));
    luaConsoleOutput->setColour (CodeEditorComponent::lineNumberBackgroundId, findColour(CodeEditorComponent::lineNumberBackgroundId));
    luaConsoleInput->setColour (CodeEditorComponent::lineNumberTextId, findColour(CodeEditorComponent::defaultTextColourId));
    luaConsoleOutput->setColour (CodeEditorComponent::lineNumberTextId, findColour(CodeEditorComponent::defaultTextColourId));

	luaConsoleInput->addKeyListener (this);
	owner.getCtrlrManagerOwner().getCtrlrLog().addListener (this);
	nextUpKeyPressWillbeFirst = true;
	lastCommandNumInHistory = -1;
	lastMoveDirection = NONE;
	currentInputString = "";
	
	 // inputHintLabel. Added v5.6.36. Thanks to @dnaldoog
	addAndMakeVisible(inputHintLabel);
	inputHintLabel.setText("RUN CODE WINDOW :: Enter: Run    Ctrl+Enter: New line", dontSendNotification);
	inputHintLabel.setJustificationType(Justification::centredRight);
	inputHintLabel.setFont(Font(12.0f, Font::plain));
	inputHintLabel.setColour(Label::textColourId, Colours::grey.withAlpha(0.8f));
	inputHintLabel.setColour(Label::backgroundColourId, Colours::transparentBlack);
	inputHintLabel.setInterceptsMouseClicks(false, false); // clicks pass through to the editor underneath

    setSize (600, 400);

	snips.addTokens (owner.getProperty(Ids::uiLuaConsoleSnips).toString(), "$", "\'\"");
}

CtrlrLuaConsole::~CtrlrLuaConsole()
{
	owner.getCtrlrManagerOwner().getCtrlrLog().removeListener (this);

    deleteAndZero (luaConsoleOutput);
    deleteAndZero (luaConsoleInput);
    deleteAndZero (resizer);
}

//==============================================================================

void CtrlrLuaConsole::clearConsoleOutput() // Added v5.6.36. Thanks to @dnaldoog
{
    outputDocument.replaceAllContent("");
    luaConsoleOutput->moveCaretToEnd(false);
}

void CtrlrLuaConsole::clearConsoleInput() // Added v5.6.36. Thanks to @dnaldoog
{
    inputDocument.replaceAllContent("");
    luaConsoleInput->moveCaretToEnd(false);
}

void CtrlrLuaConsole::paint (Graphics& g)
{
}

void CtrlrLuaConsole::resized()
{
    luaConsoleOutput->setBounds (0, 0, getWidth() - 0, proportionOfHeight (0.6900f));
    luaConsoleInput->setBounds (0, proportionOfHeight (0.7000f), getWidth() - 0, proportionOfHeight (0.3000f));
    resizer->setBounds (0, proportionOfHeight (0.6900f), getWidth() - 0, proportionOfHeight (0.0100f));
    
    Component* comps[] = { luaConsoleOutput, resizer, luaConsoleInput  };
    layoutManager.layOutComponents (comps, 3, 0, 0, getWidth(), getHeight(), true, true);
    
    // Overlay the hint in the top-right corner of the input editor, on top of it. Added v5.6.36. Thanks to @dnaldoog
    inputHintLabel.setBounds(luaConsoleInput->getRight() - 220, luaConsoleInput->getY() + 2, 216, 16);
    inputHintLabel.toFront(false);
}

bool CtrlrLuaConsole::keyPressed (const KeyPress& key)
{
    return false;  // Return true if your handler uses this key event, or false to allow it to be passed-on.
}

bool CtrlrLuaConsole::keyPressed (const KeyPress& key, Component* originatingComponent)
{
	if (key.getKeyCode() == 13 && originatingComponent == luaConsoleInput && !key.getModifiers().isCtrlDown())
	{
		runCode(inputDocument.getAllContent());

		if ((bool)owner.getProperty(Ids::uiLuaConsoleInputRemoveAfterRun))
		{
			inputDocument.replaceAllContent("");
		}
		return (true);
	}
	else if (key.getKeyCode() == 13 && originatingComponent == luaConsoleInput && key.getModifiers().isCtrlDown())
	{
		luaConsoleInput->insertTextAtCaret ("\n");
		return (true);
	}
	else if (key.getKeyCode() == KeyPress::upKey && key.getModifiers().isCtrlDown() && originatingComponent == luaConsoleInput )
	{
		if(inputHistory.size())
		{
			// Prev command
			if (nextUpKeyPressWillbeFirst) {
				currentInputString = inputDocument.getAllContent();
				nextUpKeyPressWillbeFirst = false;
			}

			luaConsoleInput->loadContent(inputHistory[lastCommandNumInHistory]);  /* Put text at pointer into console */
			lastCommandNumInHistory = ( ((lastCommandNumInHistory - 1) < 0) ? 0 : (lastCommandNumInHistory - 1) );
			lastMoveDirection = UP;
		}
		return (true);
	}
	else if (key.getKeyCode() == KeyPress::downKey && key.getModifiers().isCtrlDown() && originatingComponent == luaConsoleInput)
	{
		if(inputHistory.size())
		{
			// next command
			if (lastCommandNumInHistory == (inputHistory.size() - 1)) // at last command only
			{
				if (!currentInputString.isEmpty()) {
					luaConsoleInput->loadContent(currentInputString);
					nextUpKeyPressWillbeFirst = true;              // if user changes this restored text we need to capture it at up key again
				}
				return true;
			}
			lastCommandNumInHistory += 1;
			luaConsoleInput->loadContent(inputHistory[lastCommandNumInHistory]);  /* Put text at pointer into console */
			lastMoveDirection = DOWN;
		}
		return (true);
	}
	return (false);
}

void CtrlrLuaConsole::runCode(const String &code)
{
	luaConsoleOutput->moveCaretToEnd(false);
	luaConsoleOutput->insertTextAtCaret ("\n");
	luaConsoleOutput->insertTextAtCaret (">>> " + code + "\n");
	// add running code into history
	if (code.isNotEmpty()){
		inputHistory.addIfNotAlreadyThere(code);
		nextUpKeyPressWillbeFirst = true;
		lastCommandNumInHistory = inputHistory.size() - 1;
		lastMoveDirection = NONE;
		currentInputString = "";
	}
	owner.getCtrlrLuaManager().runCode(code);
	// luaConsoleInput->clear();
}

void CtrlrLuaConsole::messageLogged (CtrlrLog::CtrlrLogMessage message)
{
	if (message.level == CtrlrLog::Lua)
	{
		// luaConsoleOutput->setCaretPosition (luaConsoleOutput->getText().length());
		luaConsoleOutput->insertTextAtCaret (message.message + "\n");
	}
	if (message.level == CtrlrLog::LuaError)
	{
		// luaConsoleOutput->setCaretPosition (luaConsoleOutput->getText().length());
		luaConsoleOutput->insertTextAtCaret (message.message + "\n");
	}
}

const PopupMenu CtrlrLuaConsole::getSnipsMenu(const int mask)
{
	PopupMenu m;

	for (int i=0; i<snips.size(); i++)
	{
		m.addItem (mask+i, snips[i]);
	}

	return (m);
}

void CtrlrLuaConsole::snipsItemClicked(Button *b)
{
	PopupMenu m;
	m.addItem (1, "Add input to snips");
	m.addSubMenu ("Run snip", getSnipsMenu(1024));
	m.addSubMenu ("Remove snip", getSnipsMenu(4096));
	m.addItem (2, "Toggle input removal after run", true, (bool)owner.getProperty(Ids::uiLuaConsoleInputRemoveAfterRun));
	// The menu is asynchronous; the code after it runs in the callback, which
	// does nothing if the console is gone.
	Component::SafePointer<CtrlrLuaConsole> safeThis (this);
	PU::showAt (m, b, [this, safeThis] (const int ret)
	{
		if (safeThis == nullptr)
			return;

		if (ret == 1)
		{
			snips.add (inputDocument.getAllContent());
		}
		if (ret >= 1024 && ret < 4096)
		{
			runCode (snips[ret-1024]);
		}
		if (ret >= 4096)
		{
			snips.remove (ret-4096);
		}
		if (ret == 2)
		{
			owner.setProperty (Ids::uiLuaConsoleInputRemoveAfterRun, !owner.getProperty(Ids::uiLuaConsoleInputRemoveAfterRun));
		}
		owner.setProperty (Ids::uiLuaConsoleSnips, snips.joinIntoString("$"));
	});
}

StringArray CtrlrLuaConsole::getMenuBarNames()
{
	const char* const names[] = { "File", "View", nullptr };
	return StringArray (names);
}

PopupMenu CtrlrLuaConsole::getMenuForIndex(int topLevelMenuIndex, const String &menuName)
{
	PopupMenu menu;
	if (topLevelMenuIndex == 0)
	{
		menu.addItem (2, "Add input to snips");
		menu.addSubMenu ("Run snip", getSnipsMenu(1024));
		menu.addSubMenu ("Remove snip", getSnipsMenu(4096));
		// menu.addSeparator(); Updated v5.6.31
		// menu.addItem (1, "Close", false); // Updated v5.6.31
	}
	else if(topLevelMenuIndex == 1)
	{
		menu.addItem (3, "Toggle input removal after run", true, (bool)owner.getProperty(Ids::uiLuaConsoleInputRemoveAfterRun));
	}
	else if (topLevelMenuIndex == 2)
	{
		menu.addItem(10, "Clear Console"); // Added v5.6.36. Thanks to @dnaldoog
		menu.addItem(11, "Clear Input"); // Added v5.6.36. Thanks to @dnaldoog
	}

	return (menu);
}

void CtrlrLuaConsole::menuItemSelected(int menuItemID, int topLevelMenuIndex)
{
	if (topLevelMenuIndex == 0 && menuItemID==1)
	{
        // close handle
        // owner.getWindowManager().toggle (CtrlrPanelWindowManager::LuaConsole, false); // Crashes
	}
	if (menuItemID == 2)
	{
		snips.add (inputDocument.getAllContent());
	}
	if (menuItemID >= 1024 && menuItemID < 4096)
	{
		runCode (snips[menuItemID-1024]);
	}
	if (menuItemID >= 4096)
	{
		snips.remove (menuItemID-4096);
	}
	if (menuItemID == 3)
	{
		owner.setProperty (Ids::uiLuaConsoleInputRemoveAfterRun, !owner.getProperty(Ids::uiLuaConsoleInputRemoveAfterRun));
	}
	if (menuItemID == 10) // Added v5.6.36. Thanks to @dnaldoog
	{
		clearConsoleOutput();
		return; // skip the trailing setProperty(uiLuaConsoleSnips...) call below, it doesn't apply here
	}
	if (menuItemID == 11) // Added v5.6.36. Thanks to @dnaldoog
	{
		clearConsoleInput();
		return;
	}
	
	owner.setProperty (Ids::uiLuaConsoleSnips, snips.joinIntoString("$"));
}

void CtrlrLuaConsole::focusGained(FocusChangeType cause)
{
	luaConsoleInput->grabKeyboardFocus();
}
