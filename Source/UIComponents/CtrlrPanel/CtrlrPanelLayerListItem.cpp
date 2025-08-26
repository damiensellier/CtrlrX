#include "stdafx.h"
#include "CtrlrPanelLayerListItem.h"
#include "CtrlrPropertyEditors/CtrlrPropertyComponent.h"
#include "CtrlrPanelCanvasLayer.h"
#include "CtrlrPanelLayerList.h"
#include "CtrlrInlineUtilitiesGUI.h"

// CtrlrPanelLayerListItem.cpp
CtrlrPanelLayerListItem::CtrlrPanelLayerListItem(CtrlrPanelLayerList& _owner)
    : layer(0), owner(_owner), dragStartedFromIcon(false), isDragging(false),
    layerName(std::make_unique<Label>()),
    layerIndex(std::make_unique<Label>()),
    layerVisibility(std::make_unique<ToggleIconComponent>(IconType::EyeSlash, IconType::Eye)),
    layerColour(std::make_unique<CtrlrColourEditorComponent>(this)),
    dragIcon(std::make_unique<DragIconComponent>(this)),
    isolateButton(std::make_unique<TextButton>("Edit")),
    restoreButton(std::make_unique<TextButton>("Restore"))
{
    SvgIconManager::initialise();

    addAndMakeVisible(layerIndex.get());
    layerIndex->setText(L"2", dontSendNotification);
    layerIndex->setFont(Font(12.0000f, Font::plain));
    layerIndex->setJustificationType(Justification::centred);
    layerIndex->setEditable(false, false, false);
    layerIndex->setColour(TextEditor::textColourId, Colours::black);
    layerIndex->setColour(TextEditor::backgroundColourId, Colour(0x0));
    layerIndex->addMouseListener(this, true);

    addAndMakeVisible(dragIcon.get());

    addAndMakeVisible(layerVisibility.get());
    layerVisibility->addListener(this);
    layerVisibility->addMouseListener(this, true);
    layerVisibility->setMouseCursor(MouseCursor::PointingHandCursor);

    addAndMakeVisible(layerName.get());
    layerName->setText(L"Layer Name", dontSendNotification);
    layerName->setFont(Font(12.0000f, Font::plain));
    layerName->setJustificationType(Justification::centredLeft);
    layerName->setEditable(true, true, false);
    layerName->setColour(TextEditor::textColourId, Colours::black);
    layerName->setColour(TextEditor::backgroundColourId, Colour(0x0));
    layerName->addListener(this);
    layerName->addMouseListener(this, true);

    addAndMakeVisible(layerColour.get());
    layerColour->addMouseListener(this, true);

    addAndMakeVisible(isolateButton.get());
    isolateButton->setButtonText("Edit");
    isolateButton->addListener(this);
    isolateButton->setColour(TextButton::buttonColourId, findColour(juce::TextButton::buttonOnColourId));
    isolateButton->setColour(TextButton::textColourOffId, findColour(juce::TextButton::textColourOffId));

    addAndMakeVisible(restoreButton.get());
    restoreButton->setButtonText("Restore");
    restoreButton->addListener(this);
    restoreButton->setColour(TextButton::buttonColourId, Colours::green);
    restoreButton->setColour(TextButton::textColourOffId, Colours::white);
    restoreButton->setVisible(false);

    setSize(355, 40);
}

CtrlrPanelLayerListItem::~CtrlrPanelLayerListItem()
{
    //deleteAndZero(layerName);
    ////deleteAndZero(layerVisibility);
    //deleteAndZero(layerColour);
    //deleteAndZero(layerIndex);
    //deleteAndZero(isolateButton);
    //deleteAndZero(restoreButton);
    //deleteAndZero(dragIcon);
}

//==============================================================================
void CtrlrPanelLayerListItem::paint(juce::Graphics& g)
{
    // Get the background color from the current LookAndFeel
    auto backgroundColour = getLookAndFeel().findColour(juce::ListBox::backgroundColourId);

    // Get the text colour from the current LookAndFeel
    auto textColour = getLookAndFeel().findColour(juce::ListBox::textColourId);

    // Fill the background with the dynamic color
    g.setColour(backgroundColour);
    g.fillRect(getLocalBounds());

    // The rest of your existing paint code follows
    g.setColour(textColour);
    g.drawLine(0, getHeight(), getWidth(), getHeight(), 1.0f);

    // Show if this layer is part of an isolation
    if (owner.isLayerIsolationActive())
    {
        // Highlight the item differently when isolation is active
        g.setColour(juce::Colours::orange.withAlpha(0.1f));
        g.fillRect(getLocalBounds().reduced(1));
    }

    // Optional: Add visual feedback when dragging
    if (isDragging)
    {
        g.setColour(getLookAndFeel().findColour(juce::TextButton::buttonOnColourId).withAlpha(0.3f));
        g.fillRect(getLocalBounds());
    }
}

void CtrlrPanelLayerListItem::resized()
{
    const int padding = 10;
    const int componentHeight = getHeight() - (padding * 2);
    const int totalHeight = getHeight();

    // These proportions are taken directly from the header to ensure alignment
    const float layerIndexProportion = 0.07f;
    const float dragIconProportion = 0.07f;
    const float visibilityProportion = 0.07f;
    const float layerNameProportion = 0.33f;
    const float layerColourProportion = 0.23f;
    const float buttonProportion = 0.23f;

    const float totalWidth = getWidth();
    float x = 0.0f; // Start at 0, no padding

    // 1. Layer Index
    float layerIndexWidth = totalWidth * layerIndexProportion;
    layerIndex->setBounds(x, (getHeight() - componentHeight) / 2, layerIndexWidth, componentHeight);
    x += layerIndexWidth;

    // 2. Drag Icon (Reorder)
    float dragIconWidth = totalWidth * dragIconProportion;
    if (dragIcon) {
        dragIcon->setBounds(x, (getHeight() - componentHeight) / 2, dragIconWidth, componentHeight);
        x += dragIconWidth;
    }

    // 3. Visibility Toggle
    float visibilityWidth = totalWidth * visibilityProportion;
    // float checkboxSize = jmin(visibilityWidth, (float)componentHeight); // don't know why but the checkbox is cropped on the right side ???
    int checkboxSize = 32;
    float checkboxX = x + (visibilityWidth - checkboxSize) / 2.0f;
    float checkboxY = (totalHeight - checkboxSize) / 2.0f;
    layerVisibility->setBounds((int)checkboxX, (int)checkboxY, (int)checkboxSize, (int)checkboxSize);
    x += visibilityWidth;

    // 4. Layer Name (with padding)
    const int layerNamePadding = 5;
    float layerNameWidth = totalWidth * layerNameProportion;
    layerName->setBounds(x + layerNamePadding, (totalHeight - componentHeight) / 2, layerNameWidth - layerNamePadding, componentHeight);
    x += layerNameWidth;

    // 5. Layer Colour Chooser (with padding)
    float colourChooserWidth = totalWidth * layerColourProportion;
    const int colourPadding = 5;
    float paddedColourWidth = colourChooserWidth - (2 * colourPadding);
    float paddedColourX = x + colourPadding;
    layerColour->setBounds((int)paddedColourX, (totalHeight - componentHeight) / 2, (int)paddedColourWidth, componentHeight);
    x += colourChooserWidth;

    // 6. Action Buttons (occupying the same position with padding)
    float buttonColumnWidth = totalWidth * buttonProportion;
    if (isolateButton && restoreButton) {
        const int buttonPadding = 5;
        float paddedButtonWidth = buttonColumnWidth - (2 * buttonPadding);
        float paddedButtonX = x + buttonPadding;
        const int buttonTop = (totalHeight - componentHeight) / 2;

        // Both buttons are given the exact same bounds
        isolateButton->setBounds((int)paddedButtonX, buttonTop, (int)paddedButtonWidth, componentHeight);
        restoreButton->setBounds((int)paddedButtonX, buttonTop, (int)paddedButtonWidth, componentHeight);
    }
    x += buttonColumnWidth;
}

void CtrlrPanelLayerListItem::labelTextChanged(Label* labelThatHasChanged)
{
    if (labelThatHasChanged == layerName.get())
    {
        if (layer)
        {
            layer->setProperty(Ids::uiPanelCanvasLayerName, layerName->getText());
        }
    }
}

void CtrlrPanelLayerListItem::buttonClicked(Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == layerVisibility.get())
    {
        if (layer)
        {
            layer->setProperty(Ids::uiPanelCanvasLayerVisibility, layerVisibility->getToggleState(), 0);
        }
    }
    else if (buttonThatWasClicked == isolateButton.get())
    {
        if (layer)
        {
            // Set the state in the data model
            layer->setProperty(Ids::uiPanelCanvasLayerIsIsolated, true, 0);

            // The rest of the logic can remain the same
            isolateButton->setColour(TextButton::buttonColourId, Colours::red);
            isolateButton->setColour(TextButton::textColourOffId, Colours::white);
            owner.isolateLayer(rowIndex);
            updateButtonStates();
        }
    }
    else if (buttonThatWasClicked == restoreButton.get())
    {
        // Set the state in the data model
        layer->setProperty(Ids::uiPanelCanvasLayerIsIsolated, false, 0);

        owner.restoreLayerVisibility();
        updateButtonStates();
    }
}

void CtrlrPanelLayerListItem::updateButtonStates()
{
    if (!layer)
        return;

    // Read the state from the data model
    bool isThisLayerIsolated = layer->getProperty(Ids::uiPanelCanvasLayerIsIsolated);

    if (isThisLayerIsolated)
    {
        isolateButton->setVisible(false);
        restoreButton->setVisible(true);
    }
    else
    {
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

    // Set the colour, and then call updateLabel() to apply it visually.
    // Set the colour on the component. The 'false' parameter prevents it from
    // sending a change message, which is correct for an initialization step.
    layerColour->setColour(VAR2COLOUR(layer->getProperty(Ids::uiPanelCanvasLayerColour)), false);

    // This is the crucial line to ensure the component's internal UI is updated
    // with the color from the layer when the list item is first created or assigned.
    layerColour->updateLabel();

    layerIndex->setText(layer->getProperty(Ids::uiPanelCanvasLayerIndex).toString(), dontSendNotification);

    // Update button states when layer is set
    updateButtonStates();
}

void CtrlrPanelLayerListItem::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    // First, check that the change message came from the colour editor
    if (source == layerColour.get())
    {
        if (layer)
        {
            // 1. Get the new color from the component and set it in the layer's property.
            layer->setProperty(Ids::uiPanelCanvasLayerColour, layerColour->getColour().toString());

            // 2. Tell the colour editor component to update its internal text box
            //    with the new color value and background.
            layerColour->updateLabel();

            // 3. Force the parent list item to repaint, which ensures all children
            //    (including the colour editor) are redrawn correctly.
            repaint();
        }
    }
}

void CtrlrPanelLayerListItem::setRow(const int _rowIndex)
{
    rowIndex = _rowIndex;
}

void CtrlrPanelLayerListItem::mouseDrag(const MouseEvent& e)
{
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
            // Correct the startDragging call with the hotspot parameter
            // The hotspot is the top-left corner of the drag image,
            // which will align to the mouse cursor's position.
            Point<int> hotspot(20, 20);
            dragContainer->startDragging(dragDescription, this, dragImage, true, &hotspot, nullptr);
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
    dragDropIcon = SvgIconManager::getSvgString(IconType::DragDrop);
}

void DragIconComponent::paint(juce::Graphics& g)
{
    std::unique_ptr<juce::Drawable> icon = juce::Drawable::createFromImageData(dragDropIcon, strlen(dragDropIcon));

    if (icon)
    {
        auto iconColour = getLookAndFeel().findColour(juce::Label::textColourId);

        // This is the key change: we replace the black fill colour with the correct one. I spent half an hour on this one :(
        icon->replaceColour(juce::Colours::black, iconColour);

        icon->drawWithin(g, getLocalBounds().toFloat(), juce::RectanglePlacement::centred, 1.0f);
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
