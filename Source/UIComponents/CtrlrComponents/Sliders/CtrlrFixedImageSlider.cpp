#include "stdafx.h"
#include "CtrlrFixedImageSlider.h"
#include "CtrlrLuaManager.h"
#include "CtrlrComponents/CtrlrFilmStripPainter.h"
#include "CtrlrSliderInternal.h"
#include "CtrlrPanel/CtrlrPanelEditor.h"
#include "CtrlrPanel/CtrlrPanelResource.h"

CtrlrFixedImageSlider::CtrlrFixedImageSlider (CtrlrModulator &owner)
    : CtrlrComponent(owner),
      ctrlrSlider (0)
{
    valueMap = new CtrlrValueMap();
    lf = new CtrlrImageSliderLF(*this);
    
    /** Tooltip properties */
    setColour (TooltipWindow::textColourId, findColour(Label::textColourId));
    setColour (TooltipWindow::backgroundColourId, findColour(TooltipWindow::backgroundColourId));
    setColour (TooltipWindow::outlineColourId, findColour(TooltipWindow::outlineColourId));
    
    addAndMakeVisible (ctrlrSlider = new CtrlrSliderInternal (*this));
    
    ctrlrSlider->setName ("ctrlrSlider");

    ctrlrSlider->addListener (this);
    ctrlrSlider->setLookAndFeel(lf); // V5.6.28 and before was LookAndFeel_V2, updated to V4
    componentTree.addListener (this);
        
    setProperty (Ids::uiSliderMin, 0);
    setProperty (Ids::uiSliderMax, 1);
    // setProperty (Ids::uiSliderInterval, 1); // Removed v5.6.32. Useless since indexes are stepped 1 by 1
    setProperty (Ids::uiSliderValueSuffix, ""); // Added v5.6.32
    setProperty (Ids::uiSliderSetNotificationOnlyOnRelease, false);
    setProperty (Ids::uiSliderDoubleClickEnabled, true);
    setProperty (Ids::uiSliderDoubleClickValue, 0);
    
    setProperty (Ids::uiSliderVelocitySensitivity, 1.0);
    setProperty (Ids::uiSliderVelocityThreshold, 1);
    setProperty (Ids::uiSliderVelocityOffset, 0.0);
    setProperty (Ids::uiSliderVelocityMode, false);
    setProperty (Ids::uiSliderVelocityModeKeyTrigger, true);
    
    setProperty (Ids::uiSliderSpringMode, false);
    setProperty (Ids::uiSliderSpringValue, 0);
    
    setProperty (Ids::uiSliderMouseWheelInterval, 1);
    
    setProperty (Ids::uiFixedSliderContent, "");

    setProperty (Ids::uiSliderLookAndFeel, "Default");
    setProperty (Ids::uiSliderLookAndFeelIsCustom, false);
    
    setProperty (Ids::uiSliderPopupBubble, false);
    
    setProperty (Ids::uiSliderStyle, "RotaryVerticalDrag");
         
    setProperty (Ids::uiImageSliderResource, COMBO_ITEM_NONE);
    setProperty (Ids::resourceImageWidth, 32);
    setProperty (Ids::resourceImageHeight, 32);
    setProperty (Ids::resourceImagePaintMode, 36);
    setProperty (Ids::resourceImageOrientation, 1);

    setProperty (Ids::uiSliderIncDecTextColour, (String)findColour (Label::textColourId).toString());
        
    setProperty (Ids::uiSliderValuePosition, (int)Slider::TextBoxBelow);
    setProperty (Ids::uiSliderValueWidth, 64);
    setProperty (Ids::uiSliderValueHeight, 12);
    setProperty (Ids::uiSliderValueTextJustification, "centred");
    setProperty (Ids::uiSliderValueFont, FONT2STR (Font(12)));
    setProperty (Ids::uiSliderValueTextColour, (String)findColour (Slider::textBoxTextColourId).toString());
    setProperty (Ids::uiSliderValueHighlightColour, (String)findColour (Slider::textBoxHighlightColourId).toString());
    setProperty (Ids::uiSliderValueBgColour, "0x00ffffff"); // (String)findColour (Slider::textBoxBackgroundColourId).toString());
    setProperty (Ids::uiSliderValueOutlineColour, "0x00ffffff"); //(String)findColour (Slider::textBoxOutlineColourId).toString());
    
    setProperty (Ids::uiSliderLookAndFeelIsCustom, false);
    
    setSize (64, 90);
}

CtrlrFixedImageSlider::~CtrlrFixedImageSlider()
{
    deleteAndZero (ctrlrSlider);
}

void CtrlrFixedImageSlider::paint (Graphics& g)
{
}

void CtrlrFixedImageSlider::resized()
{
    if (restoreStateInProgress)
        return;
    ctrlrSlider->setBounds (getUsableRect());
}

void CtrlrFixedImageSlider::mouseUp (const MouseEvent& e)
{
    if (mouseUpCbk && !mouseUpCbk.wasObjectDeleted())
    {
        if (mouseUpCbk->isValid())
        {
            owner.getOwnerPanel().getCtrlrLuaManager().getMethodManager().call (mouseUpCbk, this, e);
        }
    }
    if ((bool)getProperty(Ids::uiSliderSpringMode) == true)
    {
        ctrlrSlider->setValue ((double)getProperty(Ids::uiSliderSpringValue), sendNotificationSync);
    }
}

double CtrlrFixedImageSlider::getComponentMaxValue()
{
    return (valueMap->getNonMappedMax());
}

double CtrlrFixedImageSlider::getComponentValue()
{
    return ((int)ctrlrSlider->getValue());
}

int CtrlrFixedImageSlider::getComponentMidiValue()
{
    return (valueMap->getMappedValue(ctrlrSlider->getValue()));

}
const String CtrlrFixedImageSlider::getComponentText()
{
    return (valueMap->getTextForIndex (ctrlrSlider->getValue()));
}

void CtrlrFixedImageSlider::setComponentValue (const double newValue, const bool sendChangeMessage)
{
    ctrlrSlider->setValue (newValue, dontSendNotification);
    if (sendChangeMessage)
    {
        owner.getProcessor().setValueGeneric (CtrlrModulatorValue(newValue,CtrlrModulatorValue::changedByGUI));
    }
}

void CtrlrFixedImageSlider::sliderContentChanged()
{
    valueMap->copyFrom (owner.getProcessor().setValueMap (getProperty(Ids::uiFixedSliderContent)));
    ctrlrSlider->setRange (valueMap->getNonMappedMin(), valueMap->getNonMappedMax(), 1);
}


void CtrlrFixedImageSlider::valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property)
{
    if (property == Ids::resourceImagePaintMode)
    {
        lf->setPaintMode ((RectanglePlacement)(int)getProperty(Ids::resourceImagePaintMode));
    }
    else if (property == Ids::uiSliderLookAndFeel)
    {
        String LookAndFeelType = getProperty(property);
        setLookAndFeel(getLookAndFeelFromComponentProperty(LookAndFeelType)); // Updates the current component LookAndFeel
        
        if (LookAndFeelType == "Default")
        {
            setProperty(Ids::uiSliderLookAndFeelIsCustom, false); // Resets the Customized Flag to False to allow Global L&F to apply
        }
        
        if (!getProperty(Ids::uiSliderLookAndFeelIsCustom))
        {
            resetLookAndFeelOverrides(); // Retrieves LookAndFeel colours from selected ColourScheme
        }
    }
    else if (property == Ids::resourceImageWidth)
    {
        lf->setImage (filmStripImage, (int)getProperty(Ids::resourceImageWidth), (int)getProperty(Ids::resourceImageHeight));
    }
    else if (property == Ids::resourceImageHeight)
    {
        lf->setImage (filmStripImage, (int)getProperty(Ids::resourceImageWidth), (int)getProperty(Ids::resourceImageHeight));
    }
    else if (property == Ids::resourceImageOrientation)
    {
        lf->setOrientation((bool)getProperty(Ids::resourceImageOrientation));
    }
    else if(property == Ids::uiImageSliderResource)
    {
        setResource();
    }
    else if (property == Ids::uiSliderStyle)
    {
        ctrlrSlider->setSliderStyle ((Slider::SliderStyle)CtrlrComponentTypeManager::sliderStringToStyle (getProperty (Ids::uiSliderStyle)));
    }
    else if (property == Ids::uiSliderValueTextColour)
    {
        ctrlrSlider->setColour (Slider::textBoxTextColourId, VAR2COLOUR(getProperty (Ids::uiSliderValueTextColour)) );
        setProperty(Ids::uiSliderLookAndFeelIsCustom, true); // Locks the component custom colourScheme
    }
    else if (property == Ids::uiSliderValueHighlightColour)
    {
        ctrlrSlider->setColour (Slider::textBoxHighlightColourId, VAR2COLOUR(getProperty (Ids::uiSliderValueHighlightColour)) );
        setProperty(Ids::uiSliderLookAndFeelIsCustom, true); // Locks the component custom colourScheme
    }
    else if (property == Ids::uiSliderValueBgColour)
    {
        ctrlrSlider->setColour (Slider::textBoxBackgroundColourId, VAR2COLOUR(getProperty (Ids::uiSliderValueBgColour)) );
        setProperty(Ids::uiSliderLookAndFeelIsCustom, true); // Locks the component custom colourScheme
    }
    else if (property == Ids::uiSliderValueOutlineColour)
    {
        ctrlrSlider->setColour (Slider::textBoxOutlineColourId, VAR2COLOUR(getProperty (Ids::uiSliderValueOutlineColour)) );
        setProperty(Ids::uiSliderLookAndFeelIsCustom, true); // Locks the component custom colourScheme
    }
    else if (property == Ids::uiFixedSliderContent)
    {
        sliderContentChanged();
    }
    else if (property == Ids::uiSliderValueSuffix) // Added v5.6.32
    {
        ctrlrSlider->setTextValueSuffix(getProperty(Ids::uiSliderValueSuffix).toString());
        ctrlrSlider->lookAndFeelChanged();
    }
    else if (property == Ids::uiSliderValuePosition || property == Ids::uiSliderValueHeight || property == Ids::uiSliderValueWidth)
    {
        ctrlrSlider->setTextBoxStyle (
            (Slider::TextEntryBoxPosition)(int)getProperty (Ids::uiSliderValuePosition),
            false,
            getProperty (Ids::uiSliderValueWidth, 64),
            getProperty (Ids::uiSliderValueHeight, 12));
    }
    else if (property == Ids::uiSliderSetNotificationOnlyOnRelease)
    {
    ctrlrSlider->setChangeNotificationOnlyOnRelease((bool)getProperty(Ids::uiSliderSetNotificationOnlyOnRelease));
    }
    else if (property == Ids::uiSliderValueFont
        || property == Ids::uiSliderValueTextJustification
        )
    {
        filmStripImage = owner.getOwnerPanel().getResourceManager().getResourceAsImage (getProperty(Ids::uiImageSliderResource));
        lf->setImage (filmStripImage, (int)getProperty(Ids::resourceImageWidth), (int)getProperty(Ids::resourceImageHeight));
        lf->setPaintMode ((RectanglePlacement)(int)getProperty(Ids::resourceImagePaintMode));
        ctrlrSlider->setLookAndFeel(0);
        ctrlrSlider->setLookAndFeel(lf);
    }

    else if (property == Ids::uiSliderVelocityMode
        || property == Ids::uiSliderVelocityModeKeyTrigger
        || property == Ids::uiSliderVelocitySensitivity
        || property == Ids::uiSliderVelocityThreshold
        || property == Ids::uiSliderVelocityOffset
        )
    {
        ctrlrSlider->setVelocityBasedMode((bool)getProperty(Ids::uiSliderVelocityMode));
        ctrlrSlider->setVelocityModeParameters ((double)getProperty(Ids::uiSliderVelocitySensitivity),
                                                (int)getProperty(Ids::uiSliderVelocityThreshold),
                                                (double)getProperty(Ids::uiSliderVelocityOffset),
                                                (bool)getProperty(Ids::uiSliderVelocityModeKeyTrigger));
    }

    else if (property == Ids::uiSliderSpringValue)
    {
        ctrlrSlider->setValue (getProperty(property), dontSendNotification);
    }
    else if (property == Ids::uiSliderDoubleClickValue
            || property == Ids::uiSliderDoubleClickEnabled)
    {
        ctrlrSlider->setDoubleClickReturnValue ((bool)getProperty(Ids::uiSliderDoubleClickEnabled), getProperty(Ids::uiSliderDoubleClickValue));
    }
    else if (property == Ids::uiSliderSpringMode)
    {
        if ((bool)getProperty(property) == true)
        {
            ctrlrSlider->setValue (getProperty(Ids::uiSliderSpringValue), dontSendNotification);
        }
    }

    else if (property == Ids::uiSliderPopupBubble)
    {
        ctrlrSlider->setPopupDisplayEnabled ((bool)getProperty(property), (bool)getProperty(property), owner.getOwnerPanel().getEditor());
    }
    else
    {
        CtrlrComponent::valueTreePropertyChanged(treeWhosePropertyHasChanged, property);
    }

    if (restoreStateInProgress == false)
    {
        resized();
    }
}

const String CtrlrFixedImageSlider::getTextForValue(const double value)
{
    return (valueMap->getTextForIndex(value));
}

void CtrlrFixedImageSlider::sliderValueChanged (Slider* sliderThatWasMoved)
{
    setComponentValue(ctrlrSlider->getValue(), true);
}

void CtrlrFixedImageSlider::setResource()
{
    filmStripImage = owner.getOwnerPanel().getResourceManager().getResourceAsImage (getProperty(Ids::uiImageSliderResource));
    lf->setImage (filmStripImage, (int)getProperty(Ids::resourceImageWidth), (int)getProperty(Ids::resourceImageHeight));
    lookAndFeelChanged();
    repaint();
    resized();
}

void CtrlrFixedImageSlider::reloadResources(Array <CtrlrPanelResource*> resourcesThatChanged)
{
    for (int i=0; i<resourcesThatChanged.size(); i++)
    {
        if (resourcesThatChanged[i]->getName() == getProperty(Ids::uiImageSliderResource).toString())
        {
            setResource();
        }
    }
}

Slider *CtrlrFixedImageSlider::getOwnedSlider()
{
    return (ctrlrSlider);
}

LookAndFeel *CtrlrFixedImageSlider::getLookAndFeelFromComponentProperty(const String &lookAndFeelComponentProperty)
{
    if (lookAndFeelComponentProperty == "Default")
    // Leave empty to get L&F from Global Property
    if (lookAndFeelComponentProperty == "V4 Light")
        return new LookAndFeel_V4(LookAndFeel_V4::getLightColourScheme());
    if (lookAndFeelComponentProperty == "V4 Grey")
        return new LookAndFeel_V4(LookAndFeel_V4::getGreyColourScheme());
    if (lookAndFeelComponentProperty == "V4 Dark")
        return new LookAndFeel_V4(LookAndFeel_V4::getDarkColourScheme());
    if (lookAndFeelComponentProperty == "V4 Midnight")
        return new LookAndFeel_V4(LookAndFeel_V4::getMidnightColourScheme());
    if (lookAndFeelComponentProperty == "V4 JetBlack")
        return new LookAndFeel_V4(LookAndFeel_V4::getJetBlackColourScheme());
    if (lookAndFeelComponentProperty == "V4 YamDX")
        return new LookAndFeel_V4(LookAndFeel_V4::getYamDxColourScheme());
    if (lookAndFeelComponentProperty == "V4 AkAPC")
        return new LookAndFeel_V4(LookAndFeel_V4::getAkApcColourScheme());
    if (lookAndFeelComponentProperty == "V4 AkMPC")
        return new LookAndFeel_V4(LookAndFeel_V4::getAkMpcColourScheme());
    if (lookAndFeelComponentProperty == "V4 LexiBlue")
        return new LookAndFeel_V4(LookAndFeel_V4::getLexiBlueColourScheme());
    if (lookAndFeelComponentProperty == "V4 KurzGreen")
        return new LookAndFeel_V4(LookAndFeel_V4::getKurzGreenColourScheme());
    if (lookAndFeelComponentProperty == "V4 KorGrey")
        return new LookAndFeel_V4(LookAndFeel_V4::getKorGreyColourScheme());
    if (lookAndFeelComponentProperty == "V4 KorGold")
        return new LookAndFeel_V4(LookAndFeel_V4::getKorGoldColourScheme());
    if (lookAndFeelComponentProperty == "V4 ArturOrange")
        return new LookAndFeel_V4(LookAndFeel_V4::getArturOrangeColourScheme());
    if (lookAndFeelComponentProperty == "V4 AiraGreen")
        return new LookAndFeel_V4(LookAndFeel_V4::getAiraGreenColourScheme());
    if (lookAndFeelComponentProperty == "V3")
        return new LookAndFeel_V3();
    if (lookAndFeelComponentProperty == "V2")
        return new LookAndFeel_V2();
    if (lookAndFeelComponentProperty == "V1")
        return new LookAndFeel_V1();
    
    return (nullptr);
}

void CtrlrFixedImageSlider::resetLookAndFeelOverrides()
{
    if (restoreStateInProgress == false) // To prevent the prop lines stacking up from top and keeping their original position
    {
        setProperty (Ids::componentLabelColour, (String)findColour(Label::textColourId).toString());
                
        setProperty (Ids::uiSliderValueTextColour, (String)findColour(Slider::textBoxTextColourId).toString());
        setProperty (Ids::uiSliderValueHighlightColour, (String)findColour(Slider::textBoxHighlightColourId).toString());
        setProperty (Ids::uiSliderValueBgColour, "0x00ffffff"); // (String)findColour (Slider::textBoxBackgroundColourId).toString());
        setProperty (Ids::uiSliderValueOutlineColour, "0x00ffffff"); //(String)findColour (Slider::textBoxOutlineColourId).toString());
        
        setProperty (Ids::uiSliderLookAndFeelIsCustom, false); // Resets the component colourScheme if a new default colourScheme is selected from the menu
        
        updatePropertiesPanel(); // Refreshes property pane
    }
}

void CtrlrFixedImageSlider::updatePropertiesPanel()
{
    CtrlrPanelProperties *props = owner.getCtrlrManagerOwner().getActivePanel()->getEditor(false)->getPropertiesPanel();
    if (props)
    {
        props->refreshAll(); // Needs extra code to prevent scrolling back to top on refresh
    }
}
