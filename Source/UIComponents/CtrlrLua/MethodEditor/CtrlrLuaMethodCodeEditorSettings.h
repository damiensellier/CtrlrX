#ifndef __JUCER_HEADER_CTRLRLUAMETHODCODEEDITORSETTINGS_CTRLRLUAMETHODCODEEDITORSETTINGS_FC2CDFB3__
#define __JUCER_HEADER_CTRLRLUAMETHODCODEEDITORSETTINGS_CTRLRLUAMETHODCODEEDITORSETTINGS_FC2CDFB3__

#include "stdafx.h"
#include "CtrlrLuaCodeTokeniser.h"
#include "Methods/CtrlrLuaMethod.h"
#include "CtrlrTextEditor.h"
#include "CtrlrWindowManagers/CtrlrChildWindowContent.h"
#include "CtrlrWindowManagers/CtrlrPanelWindowManager.h"
#include "CtrlrPropertyEditors/CtrlrPropertyComponent.h"
#include "CtrlrLuaMethodCodeEditorSettingsColourLnF.h"

class CtrlrLuaMethodEditor;

class CtrlrLuaMethodCodeEditorSettings : public Component,
    public ChangeListener,
    public ComboBox::Listener,
    public Button::Listener,
    public Slider::Listener
{
public:

    CtrlrLuaMethodCodeEditorSettings(CtrlrLuaMethodEditor& _owner, juce::Value& sharedSearchTabsValue_);
    ~CtrlrLuaMethodCodeEditorSettings();

    void changeListenerCallback(ChangeBroadcaster* source);
    const Font getFont();
    const Colour getBgColour();
    const Colour getLineNumbersBgColour();
    const Colour getLineNumbersColour();

    // Moved these from local functions inside the constructor
    void populateColourCombo(ColourComboBox* combo);
    int findColourIndex(const Colour& colour);
    Colour getColourFromCombo(ComboBox* combo);

    void paint(Graphics& g);
    void resized();
    void comboBoxChanged(ComboBox* comboBoxThatHasChanged);
    void buttonClicked(Button* buttonThatWasClicked);
    void sliderValueChanged(Slider* sliderThatWasMoved);
    void loadSyntaxColorsFromSettings();
    void saveSyntaxColorsToSettings();
    void populateSyntaxTokenCombo();
    void updateSyntaxColors();
    String getCurrentSelectedTokenType();
    void populateColourComboWithThumbnails(ColourComboBox* combo);
    void updateTokenColorDisplay(const String& tokenType);
    void clearSyntaxColorSettings();

    bool hasUnsavedChanges() const;
    void markAsChanged();
    void markAsSaved();
    bool promptToSaveChanges();
    void applySettings();
    void closeWindow();

    const char* getDefaultFont() const {
        return defaultFont;
    };


private:
    bool hasChanges;
    static constexpr const char* defaultFont = "<Monospaced>";

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

    ScopedPointer<Label> label0;
    ScopedPointer<Label> label1;
    ScopedPointer<Label> label2;
    ScopedPointer<Label> label3;
    ScopedPointer<Label> syntaxLabel;
    ComboBox* fontTypeface;
    ColourComboBox* bgColour;
    ColourComboBox* lineNumbersBgColour;
    ColourComboBox* lineNumbersColour;
    ComboBox* syntaxTokenType;
    ColourComboBox* syntaxTokenColor;
    ToggleButton* fontBold;
    ToggleButton* fontItalic;
    ToggleButton* openSearchTabs;
    TextButton* applyButton;
    TextButton* cancelButton;
    TextButton* resetButton;
    TextButton* resetToPreviousButton;
    Slider* fontSize;

    CodeEditorComponent* fontTest;

    static CodeEditorComponent::ColourScheme& getSharedScheme();
    HashMap<String, Colour> customSyntaxColors;

    Font originalFont;
    Colour originalBgColour;
    Colour originalLineNumbersBgColour;
    Colour originalLineNumbersColour;
    HashMap<String, Colour> originalSyntaxColors;
    bool originalOpenSearchTabs;


    juce::Value& sharedSearchTabsValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CtrlrLuaMethodCodeEditorSettings);
};


#endif   // __JUCER_HEADER_CTRLRLUAMETHODCODEEDITORSETTINGS_CTRLRLUAMETHODCODEEDITORSETTINGS_FC2CDFB3__