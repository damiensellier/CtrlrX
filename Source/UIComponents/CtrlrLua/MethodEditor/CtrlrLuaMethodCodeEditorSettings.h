#ifndef __JUCER_HEADER_CTRLRLUAMETHODCODEEDITORSETTINGS_CTRLRLUAMETHODCODEEDITORSETTINGS_FC2CDFB3__
#define __JUCER_HEADER_CTRLRLUAMETHODCODEEDITORSETTINGS_CTRLRLUAMETHODCODEEDITORSETTINGS_FC2CDFB3__

#include "Methods/CtrlrLuaMethod.h"
#include "CtrlrTextEditor.h"
#include "CtrlrWindowManagers/CtrlrChildWindowContent.h"
#include "CtrlrWindowManagers/CtrlrPanelWindowManager.h"
#include "CtrlrLuaCodeTokeniser.h"
#include "CtrlrPropertyEditors/CtrlrPropertyComponent.h"

class CtrlrLuaMethodEditor;

class CtrlrLuaMethodCodeEditorSettings  : public Component,
                                          public ChangeListener,
                                          public ComboBox::Listener,
                                          public Button::Listener,
                                          public Slider::Listener
{
public:

    CtrlrLuaMethodCodeEditorSettings (CtrlrLuaMethodEditor &_owner, juce::Value& sharedSearchTabsValue_);
    ~CtrlrLuaMethodCodeEditorSettings();

    void changeListenerCallback(ChangeBroadcaster* source);
    const Font getFont();
    const Colour getBgColour();
    const Colour getLineNumbersBgColour();
    const Colour getLineNumbersColour();

    // Moved these from local functions inside the constructor
    void populateColourCombo(ComboBox* combo);
    int findColourIndex(const Colour& colour);
    Colour getColourFromCombo(ComboBox* combo);

    void paint(Graphics& g);
    void resized();
    void comboBoxChanged(ComboBox* comboBoxThatHasChanged);
    void buttonClicked(Button* buttonThatWasClicked);
    void sliderValueChanged(Slider* sliderThatWasMoved);


private:
    struct ColourItem {
        String name;
        Colour colour;
    };
    static const ColourItem availableColours[];

    CtrlrLuaCodeTokeniser luaTokeniser;
    CodeDocument codeDocument;
    CtrlrLuaMethodEditor& owner;
    Font codeFont;
    Font previousFont;
    int marginLeft;
    int marginTop;
    int sampleWidth;
    int sampleHeight;

    Label* label0;
    ComboBox* fontTypeface;
    ComboBox* bgColour;
    ComboBox* lineNumbersBgColour;
    ComboBox* lineNumbersColour;
    ToggleButton* fontBold;
    ToggleButton* fontItalic;
    ToggleButton* openSearchTabs;
    TextButton* resetButton;
    TextButton* resetToPreviousButton;
    Slider* fontSize;
    Label* label1;
    Label* label2;
    Label* label3;
    CodeEditorComponent* fontTest;
    juce::Value& sharedSearchTabsValue;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CtrlrLuaMethodCodeEditorSettings);
};


#endif   // __JUCER_HEADER_CTRLRLUAMETHODCODEEDITORSETTINGS_CTRLRLUAMETHODCODEEDITORSETTINGS_FC2CDFB3__
