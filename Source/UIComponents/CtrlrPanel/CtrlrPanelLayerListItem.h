#ifndef __JUCER_HEADER_CTRLRPANELLAYERLISTITEM_CTRLRPANELLAYERLISTITEM_618D5794__
#define __JUCER_HEADER_CTRLRPANELLAYERLISTITEM_CTRLRPANELLAYERLISTITEM_618D5794__

#include "CtrlrMacros.h"

class CtrlrPanelCanvas;
class CtrlrPanelLayerList;
class CtrlrPanelCanvasLayer;
class CtrlrColourEditorComponent;
//class DragIconComponent; // forward delclarations

class DragIconComponent : public Component
{
public:
    DragIconComponent()
    {
        setMouseCursor(MouseCursor::DraggingHandCursor);
    }

    void paint(Graphics& g) override
    {
        // Parse and draw the SVG icon
        std::unique_ptr<Drawable> icon = Drawable::createFromImageData(dragDropIcon, strlen(dragDropIcon));
        if (icon)
        {
            icon->drawWithin(g, getLocalBounds().toFloat(), RectanglePlacement::centred, 1.0f);
        }
    }

    void mouseEnter(const MouseEvent&) override
    {
        setMouseCursor(MouseCursor::DraggingHandCursor);
    }
private:
    const char* dragDropIcon = R"(
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 640 640"><!--!Font Awesome Free v7.0.0 by @FontAwesome - https://fontawesome.com License - https://fontawesome.com/license/free Copyright 2025 Fonticons, Inc.--><path d="M470.6 566.6L566.6 470.6C575.8 461.4 578.5 447.7 573.5 435.7C568.5 423.7 556.9 416 544 416L480 416L480 96C480 78.3 465.7 64 448 64C430.3 64 416 78.3 416 96L416 416L352 416C339.1 416 327.4 423.8 322.4 435.8C317.4 447.8 320.2 461.5 329.3 470.7L425.3 566.7C437.8 579.2 458.1 579.2 470.6 566.7zM214.6 73.4C202.1 60.9 181.8 60.9 169.3 73.4L73.3 169.4C64.1 178.6 61.4 192.3 66.4 204.3C71.4 216.3 83.1 224 96 224L160 224L160 544C160 561.7 174.3 576 192 576C209.7 576 224 561.7 224 544L224 224L288 224C300.9 224 312.6 216.2 317.6 204.2C322.6 192.2 319.8 178.5 310.7 169.3L214.7 73.3z"/></svg>
    )";
};

class CtrlrPanelLayerListItem : public Component,
    public ChangeListener,
    public Label::Listener,
    public Button::Listener
{
public:
    //==============================================================================
    CtrlrPanelLayerListItem(CtrlrPanelLayerList& _owner);
    ~CtrlrPanelLayerListItem();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void setLayer(CtrlrPanelCanvasLayer* _layer);
    void changeListenerCallback(ChangeBroadcaster* source);
    CtrlrPanelCanvasLayer* getLayer() { return (layer); }
    void setRow(const int _rowIndex);
    const int getRow() { return (rowIndex); }
    void updateButtonStates(); 
    void paint(Graphics& g);
    void resized();
    void labelTextChanged(Label* labelThatHasChanged);
    void buttonClicked(Button* buttonThatWasClicked);
    void mouseDown(const MouseEvent& e);
    void mouseDrag(const MouseEvent& e);
    void mouseUp(const MouseEvent& e);

    //==============================================================================
    JUCE_LEAK_DETECTOR(CtrlrPanelLayerListItem)

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    CtrlrPanelCanvasLayer* layer;
    CtrlrPanelLayerList& owner;
    int rowIndex;
    bool isDragging;
    Point<int> dragStartPosition;
    //[/UserVariables]

    //==============================================================================
    Label* layerName;
    ToggleButton* layerVisibility;
    CtrlrColourEditorComponent* layerColour;
    Label* layerIndex;

    // New buttons for isolation
    TextButton* isolateButton;
    TextButton* restoreButton;
    DragIconComponent* dragIcon;
    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    CtrlrPanelLayerListItem(const CtrlrPanelLayerListItem&);
    const CtrlrPanelLayerListItem& operator= (const CtrlrPanelLayerListItem&);
};


#endif   // __JUCER_HEADER_CTRLRPANELLAYERLISTITEM_CTRLRPANELLAYERLISTITEM_618D5794__