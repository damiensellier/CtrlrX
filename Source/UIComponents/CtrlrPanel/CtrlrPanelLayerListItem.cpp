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
    isDragging(false)
{
    // Your existing component creation code...
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

    // Add the new buttons
    addAndMakeVisible(isolateButton = new TextButton("Isolate"));
    isolateButton->setButtonText("Isolate");
    isolateButton->addListener(this);
    isolateButton->setColour(TextButton::buttonColourId, Colours::lightblue);
    isolateButton->setColour(TextButton::textColourOffId, Colours::black);

    addAndMakeVisible(restoreButton = new TextButton("Restore"));
    restoreButton->setButtonText("Restore");
    restoreButton->addListener(this);
    restoreButton->setColour(TextButton::buttonColourId, Colours::lightgreen);
    restoreButton->setColour(TextButton::textColourOffId, Colours::black);

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
}

//==============================================================================
void CtrlrPanelLayerListItem::paint (Graphics& g)
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
    layerName->setBounds(48, 4, proportionOfWidth(0.35f), 12);  // Made narrower to fit buttons
    layerVisibility->setBounds(8, 4, 32, 32);
    layerColour->setBounds(48, 16, getWidth() - 200, 16);  // Made narrower to fit buttons
    layerIndex->setBounds(getWidth() - 150, getHeight() - 16, 14, 16);

    // Position the buttons on the right
    restoreButton->setBounds(getWidth() - 130, 4, 60, 14);
    isolateButton->setBounds(getWidth() - 130, 20, 60, 14);
}

void CtrlrPanelLayerListItem::labelTextChanged (Label* labelThatHasChanged)
{
    //[UserlabelTextChanged_Pre]
    //[/UserlabelTextChanged_Pre]

    if (labelThatHasChanged == layerName)
    {
        //[UserLabelCode_layerName] -- add your label text handling code here..
		if (layer)
		{
			layer->setProperty (Ids::uiPanelCanvasLayerName, layerName->getText());
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
            owner.isolateLayer(rowIndex);
            updateButtonStates();
        }
    }
    else if (buttonThatWasClicked == restoreButton)
    {
        owner.restoreLayerVisibility();
        updateButtonStates();
    }
}

void CtrlrPanelLayerListItem::updateButtonStates()
{
    bool hasBackup = owner.getOwner().hasLayerVisibilityStates();

    isolateButton->setEnabled(!hasBackup);  // Can only isolate if no backup exists
    restoreButton->setEnabled(hasBackup);   // Can only restore if backup exists

    // Visual feedback
    if (hasBackup)
    {
        isolateButton->setColour(TextButton::buttonColourId, Colours::grey);
        restoreButton->setColour(TextButton::buttonColourId, Colours::orange);
    }
    else
    {
        isolateButton->setColour(TextButton::buttonColourId, Colours::lightblue);
        restoreButton->setColour(TextButton::buttonColourId, Colours::grey);
    }
}

void CtrlrPanelLayerListItem::mouseDown(const MouseEvent& e)
{
    if (layer)
    {
        owner.setSelectedRow(rowIndex);

        // Store the drag start position for drag and drop
        dragStartPosition = e.getPosition();
        isDragging = false;
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

void CtrlrPanelLayerListItem::changeListenerCallback (ChangeBroadcaster* source)
{
	if (layer)
	{
		layer->setProperty (Ids::uiPanelCanvasLayerColour, layerColour->getColour().toString());
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
    if (!layer)
        return;

    // Start dragging if we've moved far enough from the initial click
    if (!isDragging && e.getDistanceFromDragStart() > 5)
    {
        isDragging = true;

        // Create a drag image of this component
        Image dragImage = createComponentSnapshot(getLocalBounds());

        // Start the drag operation with a description containing the row index
        String dragDescription = "layer_item_" + String(rowIndex);

        // Find the drag container (should be the CtrlrPanelLayerList)
        DragAndDropContainer* dragContainer = DragAndDropContainer::findParentDragContainerFor(this);
        if (dragContainer)
        {
            dragContainer->startDragging(dragDescription, this, dragImage, true);
        }
    }
}

void CtrlrPanelLayerListItem::mouseUp(const MouseEvent& e)
{
    isDragging = false;
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
