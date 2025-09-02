#ifndef __JUCER_HEADER_CTRLRPANELLAYERLISTITEM_CTRLRPANELLAYERLISTITEM_618D5794__
#define __JUCER_HEADER_CTRLRPANELLAYERLISTITEM_CTRLRPANELLAYERLISTITEM_618D5794__

#include "CtrlrMacros.h"
#include "CtrlrPanelEditorIcons.h" // Added v5.6.34. Handles SVG icons for the panelEditor and childWindows. Thanks to @dnaldoog
class CtrlrPanelCanvas;
class CtrlrPanelLayerList;
class CtrlrPanelCanvasLayer;
class CtrlrColourEditorComponent;

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
    std::unique_ptr<juce::Drawable> dragIconDrawable;
    juce::String dragDropIcon;
};


class CtrlrPanelLayerListItem  : public Component,
                                 public ChangeListener,
								 public Label::Listener,
								 public Button::Listener
{
public:
    CtrlrPanelLayerListItem (CtrlrPanelLayerList &_owner);
    ~CtrlrPanelLayerListItem();

	void setLayer(CtrlrPanelCanvasLayer *_layer);
	void changeListenerCallback (ChangeBroadcaster* source);
	CtrlrPanelCanvasLayer *getLayer() { return (layer); }
	
    void handleDragIconMouseDown(const MouseEvent& e);
    void handleDragIconMouseDrag(const MouseEvent& e);
    void handleDragIconMouseUp(const MouseEvent& e);

	void setRow(const int _rowIndex);
	const int getRow() { return (rowIndex); }

	void updateButtonStates();

    void paint (Graphics& g);
    void resized();
    void labelTextChanged (Label* labelThatHasChanged);
    void buttonClicked (Button* buttonThatWasClicked);
    void mouseDown (const MouseEvent& e);
    // void mouseDrag(const MouseEvent& e); // Useless. Handled by handleDragIconMouseUp
    void mouseUp(const MouseEvent& e);
	
    std::unique_ptr<ToggleIconComponent> layerVisibility;

    JUCE_LEAK_DETECTOR(CtrlrPanelLayerListItem)

private:
	CtrlrPanelCanvasLayer *layer;
	CtrlrPanelLayerList &owner;
	int rowIndex;
	
	bool isDragging;
    Point<int> dragStartPosition;

    Label* layerName;
    CtrlrColourEditorComponent* layerColour;
    Label* layerIndex;

    TextButton* isolateButton;
    TextButton* restoreButton;
    bool dragStartedFromIcon;
    DragIconComponent* dragIcon;
	
    CtrlrPanelLayerListItem (const CtrlrPanelLayerListItem&);
    const CtrlrPanelLayerListItem& operator= (const CtrlrPanelLayerListItem&);
};


#endif   // __JUCER_HEADER_CTRLRPANELLAYERLISTITEM_CTRLRPANELLAYERLISTITEM_618D5794__
