#ifndef __JUCER_HEADER_CTRLRPANELLAYERLISTITEM_CTRLRPANELLAYERLISTITEM_618D5794__
#define __JUCER_HEADER_CTRLRPANELLAYERLISTITEM_CTRLRPANELLAYERLISTITEM_618D5794__

#include "CtrlrMacros.h"
#include "CtrlrPanelEditorIcons.h"

class CtrlrPanelCanvas;
class CtrlrPanelLayerList;
class CtrlrPanelCanvasLayer;
class CtrlrColourEditorComponent;
//class DragIconComponent; // forward declarations
class CtrlrPanelLayerListItem;

class DragIconComponent : public Component
{
public:
    DragIconComponent(CtrlrPanelLayerListItem* parentItem); // Declaration

    void paint(Graphics& g) override; // Declarations
    void mouseEnter(const MouseEvent&) override;
    void mouseDown(const MouseEvent& e) override;
    void mouseDrag(const MouseEvent& e) override;
    void mouseUp(const MouseEvent& e) override;

private:
    CtrlrPanelLayerListItem* parent;
    //const char* dragDropIcon;
    std::unique_ptr<juce::Drawable> dragIconDrawable;
    const char* dragDropIcon;

};


class CtrlrPanelLayerListItem : public Component,
    public ChangeListener,
    public Label::Listener,
    public Button::Listener
{
public:
    CtrlrPanelLayerListItem(CtrlrPanelLayerList& _owner);
    ~CtrlrPanelLayerListItem();

    void setLayer(CtrlrPanelCanvasLayer* _layer);
    void changeListenerCallback(ChangeBroadcaster* source);
    CtrlrPanelCanvasLayer* getLayer() { return (layer); }

    void handleDragIconMouseDown(const MouseEvent& e);
    void handleDragIconMouseDrag(const MouseEvent& e);
    void handleDragIconMouseUp(const MouseEvent& e);

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
    std::unique_ptr<ToggleIconComponent> layerVisibility;

    JUCE_LEAK_DETECTOR(CtrlrPanelLayerListItem)

private:

    std::unique_ptr<juce::Label> layerName;
    std::unique_ptr<juce::Label> layerIndex;
    std::unique_ptr<CtrlrColourEditorComponent> layerColour; // This should be a CtrlrColourEditorComponent
    std::unique_ptr<juce::TextButton> isolateButton;
    std::unique_ptr<juce::TextButton> restoreButton;
    std::unique_ptr<DragIconComponent> dragIcon;

    CtrlrPanelCanvasLayer* layer;
    CtrlrPanelLayerList& owner;
    int rowIndex;

    bool isDragging;
    Point<int> dragStartPosition;
    //ToggleButton* layerVisibility;
    //CtrlrColourEditorComponent* layerColour;
    bool dragStartedFromIcon;        // Add this line if missing


    CtrlrPanelLayerListItem(const CtrlrPanelLayerListItem&);
    const CtrlrPanelLayerListItem& operator= (const CtrlrPanelLayerListItem&);
};


#endif   // __JUCER_HEADER_CTRLRPANELLAYERLISTITEM_CTRLRPANELLAYERLISTITEM_618D5794__
