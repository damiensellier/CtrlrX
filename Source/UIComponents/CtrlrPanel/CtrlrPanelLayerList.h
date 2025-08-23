#ifndef __JUCER_HEADER_CTRLRPANELLAYERLIST_CTRLRPANELLAYERLIST_4F6BCAF3__
#define __JUCER_HEADER_CTRLRPANELLAYERLIST_CTRLRPANELLAYERLIST_4F6BCAF3__

#include "JuceHeader.h"
#include "CtrlrWindowManagers/CtrlrChildWindowContent.h"
#include "CtrlrWindowManagers/CtrlrPanelWindowManager.h"
#include "CtrlrPanelCanvasLayer.h"

class CtrlrPanel;

class CtrlrPanelLayerList  : public CtrlrChildWindowContent,
                             public ListBoxModel,
                             public DragAndDropContainer,
                             public DragAndDropTarget
{
public:
    CtrlrPanelLayerList (CtrlrPanel &_owner);
    ~CtrlrPanelLayerList();


	int getNumRows();
    void paintListBoxItem (int rowNumber, Graphics& g, int width, int height, bool rowIsSelected);
    Component* refreshComponentForRow (int rowNumber, bool isRowSelected, Component* existingComponentToUpdate);
	void setSelectedRow(const int rowToSelect);
	String getContentName()					{ return ("Panel layers"); }
	uint8 getType()							{ return (CtrlrPanelWindowManager::LayerEditor); }
	void buttonClicked (Button *button);
	CtrlrPanel &getOwner();
	StringArray getMenuBarNames();
	PopupMenu getMenuForIndex(int topLevelMenuIndex, const String &menuName);
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
    void updateAllButtonStates();

    void paint (Graphics& g);
    void resized();
    
    bool isLayerIsolated(int layerIndex) const;

    JUCE_LEAK_DETECTOR(CtrlrPanelLayerList)

private:
	CtrlrPanel &owner;
    
    int dropInsertionIndex;
    // Layer isolation state
    bool layerIsolationActive;
    int isolatedLayerIndex;
    Array<bool> originalLayerVisibility;  // Store original visibility states

    ListBox* layerList;

    CtrlrPanelLayerList (const CtrlrPanelLayerList&);
    const CtrlrPanelLayerList& operator= (const CtrlrPanelLayerList&);
};


#endif   // __JUCER_HEADER_CTRLRPANELLAYERLIST_CTRLRPANELLAYERLIST_4F6BCAF3__
