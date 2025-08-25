#include "stdafx.h"
#include "CtrlrPanelLayerListItem.h"
#include "CtrlrPropertyEditors/CtrlrPropertyComponent.h"
#include "CtrlrPanelCanvasLayer.h"
#include "CtrlrPanelLayerList.h"
#include "CtrlrInlineUtilitiesGUI.h"

CtrlrPanelLayerListItem::CtrlrPanelLayerListItem (CtrlrPanelLayerList &_owner)
    : layer(0), owner(_owner),
      layerName (0),
      layerVisibility (0),
      layerColour (0),
      layerIndex (0),
      isolateButton(0),
      restoreButton(0),
      dragStartedFromIcon(false),
      dragIcon(0),
      isDragging(false)
{
	addAndMakeVisible (layerIndex = new Label (L"layerIndex",
                                               L"2"));
    layerIndex->setFont (Font (12.0000f, Font::plain));
    layerIndex->setJustificationType (Justification::centred);
    layerIndex->setEditable (false, false, false);
    layerIndex->setColour (TextEditor::textColourId, Colours::black);
    layerIndex->setColour (TextEditor::backgroundColourId, Colour (0x0));
	
	addAndMakeVisible(dragIcon = new DragIconComponent(this));
	
    addAndMakeVisible (layerVisibility = new ToggleButton(""));
    layerVisibility->addListener (this);
	
	addAndMakeVisible (layerName = new Label ("",
                                              L"Layer Name"));
    layerName->setFont (Font (12.0000f, Font::plain));
    layerName->setJustificationType (Justification::centredLeft);
    layerName->setEditable (true, true, false);
    layerName->setColour (TextEditor::textColourId, Colours::black);
    layerName->setColour (TextEditor::backgroundColourId, Colour (0x0));
    layerName->addListener (this);

    addAndMakeVisible (layerColour = new CtrlrColourEditorComponent (this));
    
    addAndMakeVisible(isolateButton = new TextButton("Edit"));
    isolateButton->setButtonText("Edit");
    isolateButton->addListener(this);
    isolateButton->setColour(TextButton::buttonColourId, findColour(juce::TextButton::buttonOnColourId));
    isolateButton->setColour(TextButton::textColourOffId, findColour(juce::TextButton::textColourOffId));

    addAndMakeVisible(restoreButton = new TextButton("Restore"));
    restoreButton->setButtonText("Restore");
    restoreButton->addListener(this);
    restoreButton->setColour(TextButton::buttonColourId, Colours::green);
    restoreButton->setColour(TextButton::textColourOffId, Colours::white);
    restoreButton->setVisible(false);

    // Add mouse listeners for existing components
	layerIndex->addMouseListener (this, true);
	layerVisibility->addMouseListener (this, true);
	layerName->addMouseListener (this, true);
	layerColour->addMouseListener (this, true);

	layerVisibility->setMouseCursor (MouseCursor::PointingHandCursor);

    setSize (355, 40);

}

CtrlrPanelLayerListItem::~CtrlrPanelLayerListItem()
{
    deleteAndZero (layerName);
    deleteAndZero (layerVisibility);
    deleteAndZero (layerColour);
    deleteAndZero (layerIndex);
	deleteAndZero (isolateButton);
    deleteAndZero (restoreButton);
    deleteAndZero (dragIcon);
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

void CtrlrPanelLayerListItem::labelTextChanged (Label* labelThatHasChanged)
{
    if (labelThatHasChanged == layerName)
    {
		if (layer)
		{
			layer->setProperty (Ids::uiPanelCanvasLayerName, layerName->getText());
		}
    }
}

void CtrlrPanelLayerListItem::buttonClicked (Button* buttonThatWasClicked)
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

void CtrlrPanelLayerListItem::setLayer (CtrlrPanelCanvasLayer *_layer)
{
    if (_layer == nullptr)
        return;

    layer = _layer;

    layerName->setText (layer->getProperty(Ids::uiPanelCanvasLayerName), dontSendNotification);
    layerVisibility->setToggleState (layer->getProperty(Ids::uiPanelCanvasLayerVisibility), sendNotification);
    
    // Set the colour, and then call updateLabel() to apply it visually.
    // Set the colour on the component. The 'false' parameter prevents it from
    // sending a change message, which is correct for an initialization step.
    layerColour->setColour (VAR2COLOUR(layer->getProperty(Ids::uiPanelCanvasLayerColour)), false);
    
    // This is the crucial line to ensure the component's internal UI is updated
    // with the color from the layer when the list item is first created or assigned.
    layerColour->updateLabel();
    
    layerIndex->setText (layer->getProperty(Ids::uiPanelCanvasLayerIndex).toString(), dontSendNotification);
    
    // Update button states when layer is set
    updateButtonStates();
}

void CtrlrPanelLayerListItem::changeListenerCallback (juce::ChangeBroadcaster* source)
{
    // First, check that the change message came from the colour editor
    if (source == layerColour)
    {
        if (layer)
        {
            // 1. Get the new color from the component and set it in the layer's property.
            layer->setProperty (Ids::uiPanelCanvasLayerColour, layerColour->getColour().toString());

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
    setMouseCursor(juce::MouseCursor::DraggingHandCursor);

    dragDropIcon = R"(
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512">
	<path fill="#000000" d="M409,103.2c0,2.5,0,4,0,5.5c0,114,0,227.9,0,341.9c0,14.1-11.5,25.6-25.6,25.6c-14.1,0-25.6-11.6-25.6-25.7c0-113.6,0-227.2,0-340.7c0-1.8,0-3.6,0-6.3c-1.5,1.4-2.5,2.2-3.4,3.1c-18.3,18.3-36.5,36.6-54.8,54.8c-7.2,7.2-16,9.5-25.7,6.7c-9.7-2.8-15.6-9.7-17.8-19.3c-2.3-9.9,1.7-18,8.6-24.9c28.2-28.2,56.5-56.4,84.7-84.7c5.1-5.1,10.2-10,15-15.3c10.1-11.2,28.2-10.3,38.6,0.3c32.5,33.3,65.7,66,98.5,99c1.8,1.8,3.6,3.7,5,5.8c7.8,10.9,6,25.4-4.2,34.2c-9.9,8.5-25.2,7.9-34.6-1.4c-18.4-18.3-36.7-36.7-55.1-55.1C411.7,105.7,410.7,104.8,409,103.2z"/>
	<path fill="#000000" d="M102,388.7c0-2,0-3.2,0-4.4c0-114.1,0-228.2,0-342.3c0-14.7,11.4-26.4,25.8-26.2c14.3,0.1,25.5,11.7,25.5,26.2c0,114,0,227.9,0,341.9c0,1.2,0,2.5,0,4.8c1.6-1.5,2.6-2.4,3.5-3.3c18.3-18.3,36.5-36.6,54.8-54.8c7.1-7.1,15.8-9.4,25.4-6.7c9.5,2.7,15.5,9.3,17.9,18.8c2.6,10-1.3,18.3-8.3,25.3c-19,19-38,38-57,57c-14.5,14.5-28.9,29-43.3,43.5c-10.9,11-27,10.5-37.9-0.6c-32.8-33.1-65.8-65.9-98.6-98.9c-2-2-3.9-4.1-5.5-6.4c-7.6-10.8-5.5-25.3,4.6-33.9c9.8-8.3,25.1-7.8,34.3,1.3c18.5,18.4,36.9,36.9,55.4,55.3C99.4,386.3,100.4,387.2,102,388.7z"/></svg>
    )";
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
