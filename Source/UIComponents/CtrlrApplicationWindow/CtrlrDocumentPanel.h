#ifndef __CTRLR_DOCUMENT_PANEL__
#define __CTRLR_DOCUMENT_PANEL__

#include "CtrlrMacros.h"

class CtrlrEditor;
class CtrlrManager;

class CtrlrDocumentPanel  : public MultiDocumentPanel, public Button::Listener
{
	public:
		CtrlrDocumentPanel (CtrlrManager &_owner);
		~CtrlrDocumentPanel();
		bool tryToCloseDocument (Component* component);
		void activeDocumentChanged();
		void setEditor (CtrlrEditor *_editorToSet);
		void resized();
        void buttonClicked (Button *button);
        //void lookAndFeelChanged();
		JUCE_LEAK_DETECTOR(CtrlrDocumentPanel)

	private:
		CtrlrEditor *ctrlrEditor;
		CtrlrManager &owner;
};


/** Added v5.6.30. Brings back the panel tab close button */
class CtrlrDocumentPanelCloseButton  : public Button
{
public:
    CtrlrDocumentPanelCloseButton (const String& buttonName);
    ~CtrlrDocumentPanelCloseButton ();


    void resized();
    void paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown);
    
    JUCE_LEAK_DETECTOR(CtrlrDocumentPanelCloseButton)

    juce_UseDebuggingNewOperator

private:
    Path internalPath1;
    Path internalPath2;
    Path internalPath3;
    Path internalPath4;
    Path internalPath5;
    Path internalPath6;


    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    CtrlrDocumentPanelCloseButton (const CtrlrDocumentPanelCloseButton&);
    const CtrlrDocumentPanelCloseButton& operator= (const CtrlrDocumentPanelCloseButton&);
};

#endif
