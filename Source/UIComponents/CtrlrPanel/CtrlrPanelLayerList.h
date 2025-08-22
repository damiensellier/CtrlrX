/*
  ==============================================================================
  This is an automatically generated file created by the Jucer!
  Creation date:  14 Nov 2011 8:14:25pm
  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.
  Jucer version: 1.12
  ------------------------------------------------------------------------------
  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.
  ==============================================================================
*/
#ifndef __JUCER_HEADER_CTRLRPANELLAYERLIST_CTRLRPANELLAYERLIST_4F6BCAF3__
#define __JUCER_HEADER_CTRLRPANELLAYERLIST_CTRLRPANELLAYERLIST_4F6BCAF3__
//[Headers]     -- You can add your own extra header files here --
#include "JuceHeader.h"
#include "CtrlrWindowManagers/CtrlrChildWindowContent.h"
#include "CtrlrWindowManagers/CtrlrPanelWindowManager.h"
#include "CtrlrPanelCanvasLayer.h"

class CtrlrPanel;
//[/Headers]
//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.
    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class CtrlrPanelLayerList : public CtrlrChildWindowContent,
    public ListBoxModel,
    public DragAndDropContainer,
    public DragAndDropTarget
{
public:
    //==============================================================================
    CtrlrPanelLayerList(CtrlrPanel& _owner);
    ~CtrlrPanelLayerList();
    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    int getNumRows();
    void paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected);
    Component* refreshComponentForRow(int rowNumber, bool isRowSelected, Component* existingComponentToUpdate);
    void setSelectedRow(const int rowToSelect);
    String getContentName() { return ("Panel layers"); }
    uint8 getType() { return (CtrlrPanelWindowManager::LayerEditor); }
    void buttonClicked(Button* button);
    CtrlrPanel& getOwner();
    StringArray getMenuBarNames();
    PopupMenu getMenuForIndex(int topLevelMenuIndex, const String& menuName);
    void menuItemSelected(int menuItemID, int topLevelMenuIndex);
    void addLayer();
    void removeLayer();
    void moveLayerUp();
    void moveLayerDown();
    void refresh();

    // Drag and drop methods
    bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override;
    void itemDropped(const SourceDetails& dragSourceDetails) override;
    void itemDragEnter(const SourceDetails& dragSourceDetails) override;
    void itemDragExit(const SourceDetails& dragSourceDetails) override;
    void itemDragMove(const SourceDetails& dragSourceDetails) override;
    void moveLayerToPosition(int sourceIndex, int targetIndex);
    void isolateLayer(int layerIndex);
    void restoreLayerVisibility();
    bool isLayerIsolationActive() const { return layerIsolationActive; }
    //[/UserMethods]
    void updateAllButtonStates();
    void paint(Graphics& g);
    void resized();

    //==============================================================================
    JUCE_LEAK_DETECTOR(CtrlrPanelLayerList)

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    CtrlrPanel& owner;
    int dropInsertionIndex;
    // Layer isolation state
    bool layerIsolationActive;
    Array<bool> originalLayerVisibility;  // Store original visibility states
    //[/UserVariables]

    //==============================================================================
    ListBox* layerList;

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    CtrlrPanelLayerList(const CtrlrPanelLayerList&);
    const CtrlrPanelLayerList& operator= (const CtrlrPanelLayerList&);
};

#endif   // __JUCER_HEADER_CTRLRPANELLAYERLIST_CTRLRPANELLAYERLIST_4F6BCAF3__