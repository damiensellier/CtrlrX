#include "stdafx.h"
#include "CtrlrPanelLayerListItem.h"
#include "CtrlrPropertyEditors/CtrlrPropertyComponent.h"
#include "CtrlrPanelCanvasLayer.h"
#include "CtrlrPanelLayerList.h"
#include "CtrlrInlineUtilitiesGUI.h"

CtrlrPanelLayerListItem::CtrlrPanelLayerListItem(CtrlrPanelLayerList& _owner)
    : layer(0), owner(_owner),
    layerName(0),
    layerVisibility(0),
    layerColour(0),
    layerIndex(0),
    isolateButton(0),
    restoreButton(0),
    dragStartedFromIcon(false),
    dragIcon(0),
    isDragging(false)
{
    addAndMakeVisible(dragIcon = new DragIconComponent(this));

    addAndMakeVisible(layerName = new Label("", L"Layer Name"));
    layerName->setFont(Font(12.0000f, Font::plain));
    layerName->setJustificationType(Justification::centredLeft);
    layerName->setEditable(true, true, false);
    layerName->setColour(TextEditor::textColourId, Colours::black);
    layerName->setColour(TextEditor::backgroundColourId, Colour(0x0));
    layerName->addListener(this);

    addAndMakeVisible(layerVisibility = new ToggleButton(""));
    layerVisibility->addListener(this);

    addAndMakeVisible(layerColour = new CtrlrColourEditorComponent(this));
    addAndMakeVisible(layerIndex = new Label(L"layerIndex", L"2"));
    layerIndex->setFont(Font(12.0000f, Font::plain));
    layerIndex->setJustificationType(Justification::centred);
    layerIndex->setEditable(false, false, false);
    layerIndex->setColour(TextEditor::textColourId, Colours::black);
    layerIndex->setColour(TextEditor::backgroundColourId, Colour(0x0));

    addAndMakeVisible(isolateButton = new TextButton("Edit"));
    isolateButton->setButtonText("Edit");
    isolateButton->addListener(this);
    isolateButton->setColour(TextButton::buttonColourId, Colours::lightblue);  // Initial light blue
    isolateButton->setColour(TextButton::textColourOffId, Colours::black);

    addAndMakeVisible(restoreButton = new TextButton("Restore"));
    restoreButton->setButtonText("Restore");
    restoreButton->addListener(this);
    restoreButton->setColour(TextButton::buttonColourId, Colours::green);
    restoreButton->setColour(TextButton::textColourOffId, Colours::white);
    restoreButton->setVisible(false);

    // Add mouse listeners for existing components
    layerName->addMouseListener(this, true);
    layerVisibility->addMouseListener(this, true);
    layerColour->addMouseListener(this, true);
    layerIndex->addMouseListener(this, true);

    layerVisibility->setMouseCursor(MouseCursor::PointingHandCursor);

    setSize(355, 40);
}

CtrlrPanelLayerListItem::~CtrlrPanelLayerListItem()
{
    deleteAndZero(layerName);
    deleteAndZero(layerVisibility);
    deleteAndZero(layerColour);
    deleteAndZero(layerIndex);
    deleteAndZero(isolateButton);
    deleteAndZero(restoreButton);
    deleteAndZero(dragIcon);
}

//==============================================================================
void CtrlrPanelLayerListItem::paint(Graphics& g)
{
    g.setColour(Colours::black);
    g.drawLine(0, getHeight(), getWidth(), getHeight(), 1.0f);

    // Show if this layer is part of an isolation
    if (owner.isLayerIsolationActive())
    {
        // Highlight the item differently when isolation is active
        g.setColour(Colours::orange.withAlpha(0.1f));
        g.fillRect(getLocalBounds().reduced(1));
    }

    // Optional: Add visual feedback when dragging
    if (isDragging)
    {
        g.setColour(Colours::blue.withAlpha(0.3f));
        g.fillRect(getLocalBounds());
    }
}

void CtrlrPanelLayerListItem::resized()
{
    const int buttonWidth = 60;
    const int buttonHeight = 16;
    const int colourChooserWidth = 80;
    const int padding = 4;
    const int pushLeft = 40;
    const int dragIconWidth = 16;
    const int dragIconHeight = 16;

    // Position the drag icon correctly
    if (dragIcon)
    {
        dragIcon->setBounds(padding, (getHeight() - dragIconHeight) / 2, dragIconWidth, dragIconHeight);
    }
    else {
        _DBG("dragIcon is null");
    }

    // Adjust other components
    int leftOffset = padding;
    if (dragIcon) {
        leftOffset = dragIcon->getRight();
    }

    // Position the visibility toggle button
    layerVisibility->setBounds(leftOffset+dragIconWidth, padding, 32, 32);

    // Position the layer name label
    layerName->setBounds(layerVisibility->getRight() + padding, padding, proportionOfWidth(0.35f), 12);

    // Position the color chooser
    layerColour->setBounds(layerName->getRight() + padding, 16, colourChooserWidth, 16);

    // Position the layer index label
    layerIndex->setBounds(getWidth() - (padding + 14), getHeight() - 16, 14, 16);

    // Position BOTH buttons in the SAME location (they swap visibility)
    const int buttonLeft = getWidth() - buttonWidth - pushLeft;
    const int buttonTop = (getHeight() - buttonHeight) / 2;
    
    isolateButton->setBounds(buttonLeft, buttonTop, buttonWidth, buttonHeight);
    restoreButton->setBounds(buttonLeft, buttonTop, buttonWidth, buttonHeight);  // Same position!
}

void CtrlrPanelLayerListItem::labelTextChanged(Label* labelThatHasChanged)
{
    //[UserlabelTextChanged_Pre]
    //[/UserlabelTextChanged_Pre]

    if (labelThatHasChanged == layerName)
    {
        //[UserLabelCode_layerName] -- add your label text handling code here..
        if (layer)
        {
            layer->setProperty(Ids::uiPanelCanvasLayerName, layerName->getText());
        }
        //[/UserLabelCode_layerName]
    }

    //[UserlabelTextChanged_Post]
    //[/UserlabelTextChanged_Post]
}

void CtrlrPanelLayerListItem::buttonClicked(Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == layerVisibility)
    {
        if (layer)
        {
            layer->setProperty(Ids::uiPanelCanvasLayerVisibility, layerVisibility->getToggleState());
        }
    }
    else if (buttonThatWasClicked == isolateButton)
    {
        if (layer)
        {
            // When Edit is clicked, turn it red briefly, then isolate
            isolateButton->setColour(TextButton::buttonColourId, Colours::red);
            isolateButton->setColour(TextButton::textColourOffId, Colours::white);

            // Perform the isolation
            owner.isolateLayer(rowIndex);

            // Update button states (this will hide Edit and show Restore)
            updateButtonStates();
        }
    }
    else if (buttonThatWasClicked == restoreButton)
    {
        // Restore visibility and update button states
        owner.restoreLayerVisibility();
        updateButtonStates();
    }
}

void CtrlrPanelLayerListItem::updateButtonStates()
{
    // Check if THIS specific layer is the one that was isolated
    bool isThisLayerIsolated = owner.isLayerIsolated(rowIndex);

    if (isThisLayerIsolated)
    {
        // Only THIS layer shows Restore button
        isolateButton->setVisible(false);
        restoreButton->setVisible(true);
    }
    else
    {
        // All other layers show Edit button (light blue)
        isolateButton->setVisible(true);
        isolateButton->setButtonText("Edit");
        isolateButton->setColour(TextButton::buttonColourId, Colours::lightblue);
        isolateButton->setColour(TextButton::textColourOffId, Colours::black);
        restoreButton->setVisible(false);
    }
}
void CtrlrPanelLayerListItem::mouseDown(const MouseEvent& e)
{
    if (layer)
    {
        // Only handle selection if NOT clicking on drag icon
        // (drag icon handles its own events now)
        if (!dragIcon || !dragIcon->getBounds().contains(e.getPosition()))
        {
            int totalLayers = owner.getNumRows();
            int visualRow = totalLayers - 1 - rowIndex;
            owner.setSelectedRow(visualRow);
        }

        // Reset drag flags for non-drag-icon interactions
        dragStartedFromIcon = false;
    }
}

void CtrlrPanelLayerListItem::setLayer(CtrlrPanelCanvasLayer* _layer)
{
    if (_layer == nullptr)
        return;

    layer = _layer;

    layerName->setText(layer->getProperty(Ids::uiPanelCanvasLayerName), dontSendNotification);
    layerVisibility->setToggleState(layer->getProperty(Ids::uiPanelCanvasLayerVisibility), sendNotification);
    layerColour->setColour(VAR2COLOUR(layer->getProperty(Ids::uiPanelCanvasLayerColour)), false);
    layerIndex->setText(layer->getProperty(Ids::uiPanelCanvasLayerIndex).toString(), dontSendNotification);

    // Update button states when layer is set
    updateButtonStates();
}

void CtrlrPanelLayerListItem::changeListenerCallback(ChangeBroadcaster* source)
{
    if (layer)
    {
        layer->setProperty(Ids::uiPanelCanvasLayerColour, layerColour->getColour().toString());
        layerColour->updateLabel();
    }
}

void CtrlrPanelLayerListItem::setRow(const int _rowIndex)
{
    rowIndex = _rowIndex;
}
//[/MiscUserCode]
void CtrlrPanelLayerListItem::mouseDrag(const MouseEvent& e)
{
    //if (!layer || !dragStartedFromIcon)  // Only drag if started from icon
    //    return;

    //// Start dragging if we've moved far enough from the initial click
    //if (!isDragging && e.getDistanceFromDragStart() > 5)
    //{
    //    isDragging = true;

    //    // Create a drag image of this component
    //    Image dragImage = createComponentSnapshot(getLocalBounds());

    //    // Use the visual row index for drag description
    //    int totalLayers = owner.getNumRows();
    //    int visualRow = totalLayers - 1 - rowIndex;

    //    String dragDescription = "layer_item_" + String(visualRow);

    //    // Find the drag container
    //    DragAndDropContainer* dragContainer = DragAndDropContainer::findParentDragContainerFor(this);
    //    if (dragContainer)
    //    {
    //        dragContainer->startDragging(dragDescription, this, dragImage, true);
    //    }
    //}
}

void CtrlrPanelLayerListItem::mouseUp(const MouseEvent& e)
{
    isDragging = false;
    dragStartedFromIcon = false;  // Reset the flag
}

void CtrlrPanelLayerListItem::handleDragIconMouseDown(const MouseEvent& e)
{
    if (layer)
    {
        // Convert actual layer index to visual row for selection
        int totalLayers = owner.getNumRows();
        int visualRow = totalLayers - 1 - rowIndex;
        owner.setSelectedRow(visualRow);

        // Set up for dragging
        dragStartPosition = e.getPosition();
        isDragging = false;
        dragStartedFromIcon = true;
    }
}

void CtrlrPanelLayerListItem::handleDragIconMouseDrag(const MouseEvent& e)
{
    if (!layer || !dragStartedFromIcon)
        return;

    // Start dragging if we've moved far enough from the initial click
    if (!isDragging && e.getDistanceFromDragStart() > 5)
    {
        isDragging = true;

        // Create a drag image of this component
        Image dragImage = createComponentSnapshot(getLocalBounds());

        // Use the visual row index for drag description
        int totalLayers = owner.getNumRows();
        int visualRow = totalLayers - 1 - rowIndex;

        String dragDescription = "layer_item_" + String(visualRow);

        // Find the drag container
        DragAndDropContainer* dragContainer = DragAndDropContainer::findParentDragContainerFor(this);
        if (dragContainer)
        {
            dragContainer->startDragging(dragDescription, this, dragImage, true);
        }
    }
}

void CtrlrPanelLayerListItem::handleDragIconMouseUp(const MouseEvent& e)
{
    isDragging = false;
    dragStartedFromIcon = false;
}

DragIconComponent::DragIconComponent(CtrlrPanelLayerListItem* parentItem) : parent(parentItem)
{
    setMouseCursor(MouseCursor::DraggingHandCursor);

    dragDropIcon = R"(
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 640 640">
<path d="M470.6 566.6L566.6 470.6C575.8 461.4 578.5 447.7 573.5 435.7C568.5 423.7 556.9 416 544 416L480 416L480 96C480 78.3 465.7 64 448 64C430.3 64 416 78.3 416 96L416 416L352 416C339.1 416 327.4 423.8 322.4 435.8C317.4 447.8 320.2 461.5 329.3 470.7L425.3 566.7C437.8 579.2 458.1 579.2 470.6 566.7zM214.6 73.4C202.1 60.9 181.8 60.9 169.3 73.4L73.3 169.4C64.1 178.6 61.4 192.3 66.4 204.3C71.4 216.3 83.1 224 96 224L160 224L160 544C160 561.7 174.3 576 192 576C209.7 576 224 561.7 224 544L224 224L288 224C300.9 224 312.6 216.2 317.6 204.2C322.6 192.2 319.8 178.5 310.7 169.3L214.7 73.3z"/></svg>
    )";
}

void DragIconComponent::paint(Graphics& g)
{
    std::unique_ptr<Drawable> icon = Drawable::createFromImageData(dragDropIcon, strlen(dragDropIcon));
    if (icon)
    {
        icon->drawWithin(g, getLocalBounds().toFloat(), RectanglePlacement::centred, 1.0f);
    }
}

void DragIconComponent::mouseEnter(const MouseEvent&)
{
    setMouseCursor(MouseCursor::DraggingHandCursor);
}

void DragIconComponent::mouseDown(const MouseEvent& e)
{
    if (parent)
    {
        parent->handleDragIconMouseDown(e.getEventRelativeTo(parent));
    }
}

void DragIconComponent::mouseDrag(const MouseEvent& e)
{
    if (parent)
    {
        parent->handleDragIconMouseDrag(e.getEventRelativeTo(parent));
    }
}

void DragIconComponent::mouseUp(const MouseEvent& e)
{
    if (parent)
    {
        parent->handleDragIconMouseUp(e.getEventRelativeTo(parent));
    }
}
//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="CtrlrPanelLayerListItem"
                 componentName="" parentClasses="public Component, public ChangeListener"
                 constructorParams="CtrlrPanelLayerList &amp;_owner" variableInitialisers="layer(0), owner(_owner)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330000013"
                 fixedSize="1" initialWidth="355" initialHeight="40">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="ffffff"/>
  <LABEL name="" id="a96af5bb0d837023" memberName="layerName" virtualName=""
         explicitFocusOrder="0" pos="48 4 51.831% 12" edTextCol="ff000000"
         edBkgCol="0" labelText="Layer Name" editableSingleClick="1" editableDoubleClick="1"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12"
         bold="0" italic="0" justification="33"/>
  <IMAGEBUTTON name="" id="2965d7960a5cfd60" memberName="layerVisibility" virtualName=""
               explicitFocusOrder="0" pos="8 4 32 32" tooltip="Layer visibility"
               buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
               keepProportions="1" resourceNormal="" opacityNormal="1" colourNormal="0"
               resourceOver="" opacityOver="0.5" colourOver="0" resourceDown=""
               opacityDown="1" colourDown="0"/>
  <GENERICCOMPONENT name="" id="b66f09cccadeeec4" memberName="layerColour" virtualName=""
                    explicitFocusOrder="0" pos="48 16 144M 16" class="CtrlrColourEditorComponent"
                    params="this"/>
  <LABEL name="layerIndex" id="5dd9a815809be664" memberName="layerIndex"
         virtualName="" explicitFocusOrder="0" pos="83R 8R 14 8" edTextCol="ff000000"
         edBkgCol="0" labelText="2" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="8"
         bold="0" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif