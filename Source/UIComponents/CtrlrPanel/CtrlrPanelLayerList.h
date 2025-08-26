#ifndef __JUCER_HEADER_CTRLRPANELLAYERLIST_CTRLRPANELLAYERLIST_4F6BCAF3__
#define __JUCER_HEADER_CTRLRPANELLAYERLIST_CTRLRPANELLAYERLIST_4F6BCAF3__

#include "JuceHeader.h"
#include "CtrlrWindowManagers/CtrlrChildWindowContent.h"
#include "CtrlrWindowManagers/CtrlrPanelWindowManager.h"
#include "CtrlrPanelCanvasLayer.h"

// Forward declarations
class CtrlrPanel;
class CtrlrPanelLayerListItem; // You are using this in refreshComponentForRow


// Define the header component class here
class CtrlrPanelLayerListHeader : public juce::Component
{
public:
    CtrlrPanelLayerListHeader();
    ~CtrlrPanelLayerListHeader() override;
    
    void resized() override;
    void paint (juce::Graphics& g) override;
    
private:
    juce::Label indexHeader;
    juce::Label nameHeader;
    juce::Label visibilityHeader;
    juce::Label colourHeader;
    juce::Label editHeader;
    
    juce::Label reorderHeader;
    
    juce::Array<int> separatorPositions;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CtrlrPanelLayerListHeader)
};

class CtrlrPanelLayerList : public juce::Component,
                             public juce::ListBoxModel,
                             public juce::DragAndDropContainer,
                             public juce::DragAndDropTarget,
                             public juce::Button::Listener,
                             public CtrlrChildWindowContent
{
public:
    CtrlrPanelLayerList (CtrlrPanel &_owner);
    ~CtrlrPanelLayerList() override; // Add 'override' for clarity

    // Your existing public methods
    int getNumRows() override;
    void paintListBoxItem (int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    juce::Component* refreshComponentForRow (int rowNumber, bool isRowSelected, juce::Component* existingComponentToUpdate) override;
    void setSelectedRow(const int rowToSelect);
    juce::String getContentName() override { return "Panel layers"; }
    uint8 getType() override { return CtrlrPanelWindowManager::LayerEditor; }
    void buttonClicked (juce::Button* button) override;
    CtrlrPanel& getOwner();
    juce::StringArray getMenuBarNames() override;
    juce::PopupMenu getMenuForIndex(int topLevelMenuIndex, const juce::String& menuName) override;
    void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;
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

    void paint (juce::Graphics& g) override;
    void resized() override;
    
    bool isLayerIsolated(int layerIndex) const;

 private:
    CtrlrPanel& owner;
    bool isBeingDestroyed = false;
    int dropInsertionIndex;
    bool layerIsolationActive = false;
    int isolatedLayerIndex = -1;
    juce::Array<bool> originalLayerVisibility;

    std::unique_ptr<juce::ListBox> layerList;
    std::unique_ptr<CtrlrPanelLayerListHeader> headerComponent;
    
    // Private copy constructor and assignment operator to prevent copies
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CtrlrPanelLayerList)
};

#endif // __JUCER_HEADER_CTRLRPANELLAYERLIST_CTRLRPANELLAYERLIST_4F6BCAF3__
