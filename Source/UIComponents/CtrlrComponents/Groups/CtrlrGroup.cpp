#include "stdafx.h"
#include "CtrlrGroup.h"
#include "CtrlrModulator/CtrlrModulator.h"
#include "CtrlrIDs.h"
#include "CtrlrPanel/CtrlrPanel.h"
#include "CtrlrUtilitiesGUI.h"
#include "CtrlrPanel/CtrlrPanelEditor.h"
#include "CtrlrPanel/CtrlrPanelResource.h"

CtrlrGroupContentComponent::CtrlrGroupContentComponent(CtrlrGroup &_owner) : owner(_owner)
{
	setColour (GroupComponent::outlineColourId, Colours::transparentBlack);
}

CtrlrGroupContentComponent::~CtrlrGroupContentComponent()
{
}

void CtrlrGroupContentComponent::customLookAndFeelChanged(LookAndFeelBase *customLookAndFeel)
{
    for (int i=0; i<getNumChildComponents(); i++)
    {
        CtrlrComponent *c = dynamic_cast<CtrlrComponent*>(getChildComponent(i));

        if (c!=nullptr)
        {
            c->setCustomLookAndFeel (customLookAndFeel);
        }
    }
}
//[/MiscUserDefs]

//==============================================================================
CtrlrGroup::CtrlrGroup (CtrlrModulator &owner)
    : CtrlrComponent(owner), content(*this),
      label (0)
{
    addAndMakeVisible (label = new Label ("new label",
                                          "Group Text"));
    label->setFont (Font (14.0000f, Font::plain)); // Was bold
    label->setJustificationType (Justification::centred);
    label->setEditable (false, false, false);
    label->setColour (TextEditor::textColourId, findColour(Label::textColourId)); // Colours::black
    label->setColour (TextEditor::backgroundColourId, Colour (0x0)); // Colour (0x0)

    //[UserPreSize]
	addAndMakeVisible(&content);
	componentTree.addListener (this);

	owner.setProperty (Ids::modulatorIsStatic, true);
	owner.setProperty (Ids::modulatorVstExported, false);

	setProperty (Ids::uiGroupText, "Group Text");
    setProperty (Ids::uiGroupTextPlacement, "top");
    setProperty (Ids::uiGroupTextFont, FONT2STR (Font(14)));
    setProperty (Ids::uiGroupTextMargin, 18);
    setProperty (Ids::componentLabelVisible, true);
    
    setProperty (Ids::uiButtonLookAndFeel, "Default");
    setProperty (Ids::uiButtonLookAndFeelIsCustom, false);
    
    if ( owner.getOwnerPanel().getEditor()->getProperty(Ids::uiPanelLookAndFeel) == "V3"
        || owner.getOwnerPanel().getEditor()->getProperty(Ids::uiPanelLookAndFeel) == "V2"
        || owner.getOwnerPanel().getEditor()->getProperty(Ids::uiPanelLookAndFeel) == "V1" )
    {
        setProperty (Ids::uiGroupTextColour, "0xff000000");
        
        setProperty (Ids::uiGroupBackgroundGradientType, 1); // 0 for solidcolour, 1 for vertical etc
        setProperty (Ids::uiGroupBackgroundColour1, "0xffa3a3a3");
        setProperty (Ids::uiGroupBackgroundColour2, "0xffffffff");
        
        setProperty (Ids::uiGroupOutlineGradientType, "Vertical");
        setProperty (Ids::uiGroupOutlineColour1, "0xffa3a3a3");
        setProperty (Ids::uiGroupOutlineColour2, "0xffffffff");
    }
    else
    {
        setProperty (Ids::uiGroupTextColour, (String)findColour(Label::textColourId).toString());
        
        setProperty (Ids::uiGroupBackgroundGradientType, 0); // 0 for solidcolour, 1 for vertical etc
        setProperty (Ids::uiGroupBackgroundColour1, (String)findColour(DocumentWindow::backgroundColourId).darker(0.1f).toString());
        setProperty (Ids::uiGroupBackgroundColour2, (String)findColour(DocumentWindow::backgroundColourId).toString());
        
        setProperty (Ids::uiGroupOutlineGradientType, "SolidColour");
        setProperty (Ids::uiGroupOutlineColour1, (String)findColour(DocumentWindow::textColourId).darker(0.2f).toString());
        setProperty (Ids::uiGroupOutlineColour2, (String)findColour(DocumentWindow::textColourId).toString());
    }
    
    setProperty (Ids::uiGroupOutlineThickness, 2.0); // 2.0
    setProperty (Ids::uiGroupOutlineRoundAngle, 5.0); // 8.0
    
    setProperty (Ids::uiGroupBackgroundImage, "");
    
    setProperty (Ids::uiGroupBackgroundImageLayout, 36);
    setProperty (Ids::uiGroupBackgroundImageAlpha, 255);
	setProperty (Ids::uiGroupBackgroundGradientType, 1);

    setProperty (Ids::uiButtonLookAndFeelIsCustom, false); // Resets the component colourScheme if a new default colourScheme is selected from the menu
    
	owner.getModulatorTree().addListener (this);
    //[/UserPreSize]

    setSize (128, 128);

    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

CtrlrGroup::~CtrlrGroup()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
	owner.getModulatorTree().removeListener (this);
	componentTree.removeListener (this);
    //[/Destructor_pre]

    deleteAndZero (label);

    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void CtrlrGroup::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
	Rectangle<int> r = getUsableRect();

	gradientFromProperty(g, getBounds(), getObjectTree(), Ids::uiGroupOutlineGradientType, Ids::uiGroupOutlineColour1, Ids::uiGroupOutlineColour2);
	g.drawRoundedRectangle (r.toFloat().reduced((float)getProperty(Ids::uiGroupOutlineThickness)/2.0f), getProperty(Ids::uiGroupOutlineRoundAngle), getProperty(Ids::uiGroupOutlineThickness));

	gradientFromProperty(g, getBounds(), getObjectTree(), Ids::uiGroupBackgroundGradientType, Ids::uiGroupBackgroundColour1, Ids::uiGroupBackgroundColour2);
	g.fillRoundedRectangle (r.toFloat().reduced((float)getProperty(Ids::uiGroupOutlineThickness)/2.0f), getProperty(Ids::uiGroupOutlineRoundAngle));

	if (groupBackgroundImage.isValid())
	{
		if ((int)getProperty (Ids::uiGroupBackgroundImageLayout) == 8192)
		{
			g.setTiledImageFill (groupBackgroundImage, 0, 0, (float)getProperty (Ids::uiGroupBackgroundImageAlpha)/255.0f);
			g.fillRect(r);
		}
		else
		{
			g.setColour (Colours::black.withAlpha ((float)getProperty (Ids::uiGroupBackgroundImageAlpha)/255.0f));
			g.drawImageWithin (groupBackgroundImage,
								r.getX(),
								r.getY(),
								r.getWidth(),
								r.getHeight(),
								RectanglePlacement(getProperty (Ids::uiGroupBackgroundImageLayout)));
		}
	}
    //[/UserPaint]
}

void CtrlrGroup::resized()
{
    //label->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    //[UserResized] Add your own custom resize handling here..
	label->setBounds (textMargin, textMargin, getWidth() - (textMargin*2), getHeight() - (textMargin*2));
	content.setBounds(getUsableRect());
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void CtrlrGroup::setComponentValue (const double newValue, const bool sendChangeMessage)
{
}

const Array<Font> CtrlrGroup::getFontList()
{
	Array <Font> ret;
	Font f = STR2FONT(getProperty(Ids::uiGroupTextFont));
	if (f.getTypefaceName() != Font::getDefaultSerifFontName()
		&& f.getTypefaceName() != Font::getDefaultSansSerifFontName()
		&& f.getTypefaceName() != Font::getDefaultMonospacedFontName()
		&& f.getTypefaceName() != "<Sans-Serif>")
	{
		ret.add (f);
	}
	return (ret);
}

const String CtrlrGroup::getComponentText()
{
	return (label->getText());
}

void CtrlrGroup::setComponentText (const String &componentText)
{
	setProperty (Ids::uiGroupText, componentText, true);
}

double CtrlrGroup::getComponentMaxValue()
{
	return (1);
}

double CtrlrGroup::getComponentValue()
{
	return (1);
}

int CtrlrGroup::getComponentMidiValue()
{
	return (1);
}

void CtrlrGroup::valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property)
{
	if (property == Ids::uiGroupOutlineColour1 || property == Ids::uiGroupOutlineColour2
			|| property == Ids::uiGroupBackgroundColour1 || property == Ids::uiGroupBackgroundColour2
			|| property == Ids::uiGroupBackgroundGradientType || property == Ids::uiGroupOutlineGradientType
			|| property == Ids::uiGroupOutlineRoundAngle || property == Ids::uiGroupOutlineThickness)
	{
		repaint();
	}
    else if (property == Ids::uiButtonLookAndFeel)
    {
        String LookAndFeelType = getProperty(property);
        setLookAndFeel(getLookAndFeelFromComponentProperty(LookAndFeelType)); // Updates the current component LookAndFeel
        
        if (LookAndFeelType == "Default")
        {
            setProperty(Ids::uiButtonLookAndFeelIsCustom, false); // Resets the Customized Flag to False to allow Global L&F to apply
        }
        
        if (!getProperty(Ids::uiButtonLookAndFeelIsCustom))
        {
            resetLookAndFeelOverrides(); // Retrieves LookAndFeel colours from selected ColourScheme
        }
    }
	else if (property == Ids::uiGroupTextColour)
	{
		label->setColour (Label::textColourId, VAR2COLOUR(getProperty(Ids::uiGroupTextColour)));
	}
	else if (property == Ids::uiGroupText)
	{
		label->setText (getProperty(Ids::uiGroupText), dontSendNotification);
	}
	else if (property == Ids::uiGroupTextFont)
	{
		label->setFont (STR2FONT(getProperty(Ids::uiGroupTextFont)));
	}
	else if (property == Ids::uiGroupTextPlacement)
	{
		label->setJustificationType (justificationFromProperty(getProperty(Ids::uiGroupTextPlacement)));
	}
	else if (property == Ids::uiGroupTextMargin)
	{
		textMargin			= getProperty(Ids::uiGroupTextMargin);
	}

	else if (property == Ids::uiGroupBackgroundImage
		|| property == Ids::uiGroupBackgroundImageAlpha
		|| property == Ids::uiGroupBackgroundImageLayout
		)
	{
		setResource();
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

const CtrlrGroup::GradientType CtrlrGroup::gradientFromString(const String &str)
{
	if (str == "None")
		return CtrlrGroup::None;

	if (str == "Vertical")
		return CtrlrGroup::Vertical;

	if (str == "Horizontal")
		return CtrlrGroup::Horizontal;

	if (str == "Radial")
		return CtrlrGroup::Radial;

	return CtrlrGroup::None;
}

bool CtrlrGroup::isOwned(CtrlrComponent *componentToCheck)
{
	for (int i=0; i<content.getNumChildComponents(); i++)
	{
		CtrlrComponent *c = dynamic_cast<CtrlrComponent*>(content.getChildComponent(i));
		if (c != nullptr)
		{
			if (c == componentToCheck)
				return (true);
		}
	}

	return (false);
}

Array <CtrlrComponent*> CtrlrGroup::getOwnedChildren()
{
	Array <CtrlrComponent*> ret;

	for (int i=0; i<content.getNumChildComponents(); i++)
	{
		CtrlrComponent *c = dynamic_cast<CtrlrComponent*>(content.getChildComponent(i));
		if (c != nullptr)
		{
			ret.add (c);
		}
	}

	return (ret);
}

void CtrlrGroup::setOwned (CtrlrComponent *componentToOwn, const int subIndexInGroup, const bool shouldOwnThisComponent)
{
	if (shouldOwnThisComponent)
	{
		content.addAndMakeVisible (componentToOwn);
		componentToOwn->setProperty (Ids::componentGroupName, owner.getName(), true);
		componentToOwn->setProperty (Ids::componentGroupped, true, true);
	}
	else
	{
		owner.getOwnerPanel().getEditor()->getCanvas()->addAndMakeVisibleNg(componentToOwn);
		componentToOwn->setProperty (Ids::componentGroupped, false, true);
        if (!owner.getOwnerPanel().isSchemeAtLeast(1))
            componentToOwn->setProperty (Ids::componentGroupName, "", false);
	}
}

void CtrlrGroup::canvasStateRestored()
{
	Array <CtrlrModulator*> children = owner.getOwnerPanel().getModulatorsWithProperty(Ids::componentGroupName, owner.getName());

	for (int i=0; i<children.size(); i++)
	{
		if (children[i]->getComponent())
		{
			if (owner.getOwnerPanel().isSchemeAtLeast(1))
			{
				if (children[i]->getComponent()->getProperty(Ids::componentGroupped))
				{
					setOwned (children[i]->getComponent(), 0, true);

					if (children[i]->getComponent()->getProperty(Ids::componentSentBack))
					{
						children[i]->getComponent()->toBack ();
					}
				}
			}
			else
			{
				setOwned (children[i]->getComponent(), 0, true);

				if (children[i]->getComponent()->getProperty(Ids::componentSentBack))
				{
					children[i]->getComponent()->toBack ();
				}
			}
		}
	}
}

void CtrlrGroup::modulatorNameChanged (const String &newName)
{
	for (int i=0; i<content.getNumChildComponents(); i++)
	{
		CtrlrComponent *c = dynamic_cast<CtrlrComponent*>(content.getChildComponent(i));
		if (c!=0)
		{
			c->setProperty (Ids::componentGroupName, newName, true);
		}
	}
}

bool CtrlrGroup::isInterestedInDragSource (const SourceDetails &dragSourceDetails)
{
	if (dragSourceDetails.description == "__ctrlr_component_selection")
	{
		return (true);
	}

	return (false);
}

void CtrlrGroup::itemDropped (const SourceDetails &dragSourceDetails)
{
	if (dragSourceDetails.description == "__ctrlr_component_selection")
	{
		if (owner.getOwnerPanel().getEditor() && owner.getOwnerPanel().getEditor()->getSelection())
		{
			AffineTransform trans = owner.getOwnerPanel().getEditor()->moveSelectionToPosition(dragSourceDetails.localPosition.getX(), dragSourceDetails.localPosition.getY());

			for (int i=0; i<owner.getOwnerPanel().getEditor()->getSelection()->getNumSelected(); i++)
			{
				CtrlrComponent *c = owner.getOwnerPanel().getEditor()->getSelection()->getSelectedItem(i);

				if (c == this || isOwned(c) || (bool)c->getProperty(Ids::componentIsLocked) == true)
					continue;

				setOwned (c, 0, true);

				c->setBounds (c->getBounds().transformedBy(trans));
			}
		}
	}
}

void CtrlrGroup::itemDragExit (const SourceDetails &dragSourceDetails)
{
}

void CtrlrGroup::itemDragEnter (const SourceDetails &dragSourceDetails)
{
}

void CtrlrGroup::setResource()
{
	groupBackgroundImage = owner.getOwnerPanel().getResourceManager().getResourceAsImage(getProperty(Ids::uiGroupBackgroundImage));
	repaint();
	resized();
}

void CtrlrGroup::reloadResources(Array <CtrlrPanelResource*> resourcesThatChanged)
{
	for (int i=0; i<resourcesThatChanged.size(); i++)
	{
		if (resourcesThatChanged[i]->getName() == getProperty(Ids::uiGroupBackgroundImage).toString())
		{
			setResource();
		}
	}
}



void CtrlrGroup::customLookAndFeelChanged(LookAndFeelBase *customLookAndFeel)
{
    content.customLookAndFeelChanged (customLookAndFeel);
}

LookAndFeel *CtrlrGroup::getLookAndFeelFromComponentProperty(const String &lookAndFeelComponentProperty) // Updated v5.6.34
{
    if (lookAndFeelComponentProperty == "Default")
    {
        // This case still means "use the default LookAndFeel (which might be the global one)"
        // so returning nullptr is appropriate if that's the desired behavior.
        return nullptr;
    }

    // Call your new generic factory function
    // We pass 'false' for the second argument here, as 'Default' is handled separately
    // and an unknown string should likely result in nullptr to fall back to the global L&F.
    return gui::createLookAndFeelFromDescription(lookAndFeelComponentProperty, false);
}

void CtrlrGroup::resetLookAndFeelOverrides()
{
    if (restoreStateInProgress == false) // To prevent the prop lines stacking up from top and keeping their original position
    {
        setProperty (Ids::componentLabelColour, (String)findColour(Label::textColourId).toString());

        setProperty (Ids::uiGroupTextColour, (String)findColour(Label::textColourId).toString());
        
        setProperty (Ids::uiGroupBackgroundGradientType, 0); // 0 for solidcolour, 1 for vertical etc
        setProperty (Ids::uiGroupBackgroundColour1, (String)findColour(DocumentWindow::backgroundColourId).darker(0.1f).toString());
        setProperty (Ids::uiGroupBackgroundColour2, (String)findColour(DocumentWindow::backgroundColourId).toString());
        
        setProperty (Ids::uiGroupOutlineGradientType, "SolidColour");
        setProperty (Ids::uiGroupOutlineColour1, (String)findColour(DocumentWindow::backgroundColourId).darker(0.2f).toString());
        setProperty (Ids::uiGroupOutlineColour2, (String)findColour(DocumentWindow::backgroundColourId).toString());
        
        setProperty (Ids::uiButtonLookAndFeelIsCustom, false); // Resets the component colourScheme if a new default colourScheme is selected from the menu
        
        updatePropertiesPanel(); // Refreshes property pane
    }
}

void CtrlrGroup::updatePropertiesPanel()
{
    CtrlrPanelProperties *props = owner.getCtrlrManagerOwner().getActivePanel()->getEditor(false)->getPropertiesPanel();
    if (props)
    {
        props->refreshAll(); // Needs extra code to prevent scrolling back to top on refresh
    }
}




//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="CtrlrGroup" componentName=""
                 parentClasses="public CtrlrComponent" constructorParams="CtrlrModulator &amp;owner"
                 variableInitialisers="CtrlrComponent(owner), content(*this)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330000013"
                 fixedSize="1" initialWidth="128" initialHeight="128">
  <BACKGROUND backgroundColour="ffffff"/>
  <LABEL name="new label" id="20a4cb0ec13b8efc" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="0 0 0M 0M" edTextCol="ff000000" edBkgCol="0"
         labelText="Group Text" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="14"
         bold="1" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
