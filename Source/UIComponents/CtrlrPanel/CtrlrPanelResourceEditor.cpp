#include "stdafx.h"
#include "CtrlrManager/CtrlrManager.h"
#include "UIComponents/CtrlrPanel/CtrlrPanelEditor.h"
#include "CtrlrProcessor.h"
#include "CtrlrPanel/CtrlrPanelResource.h"
#include "CtrlrInlineUtilitiesGUI.h"
#include "CtrlrPanelResourceEditor.h"

CtrlrPanelResourceEditor::CtrlrPanelResourceEditor (CtrlrPanelEditor &_owner)
    : owner(_owner),
      resourceList(nullptr),
      add(nullptr),
	  remove(nullptr),
	  move(nullptr),
	  reload(nullptr),
	  sortByColumnId(1),
	  sortForward(1)
{
    addAndMakeVisible (resourceList = new TableListBox ("Resource List", this));
    resourceList->setName (L"resourceList");

    addAndMakeVisible (add = new TextButton (L"new button"), -1); // Updated v5.6.33. Z index added. By @dnladoog JG on 4/23/2025
    add->setTooltip (L"Add new resources");
    add->setButtonText (L"Add");
    add->addListener (this);
    add->setColour(TextButton::buttonColourId, Colour(findColour(TextButton::buttonOnColourId))); // Colour (0xffb2b2b2));
    add->setColour(TextButton::buttonOnColourId, Colour(findColour(TextButton::buttonOnColourId)));
    add->setColour(TextButton::textColourOffId, Colour(findColour(TextButton::textColourOffId)));
    add->setColour(TextButton::textColourOnId, Colour(findColour(TextButton::textColourOnId)));

	addAndMakeVisible (remove = new TextButton (""), -1); // Updated v5.6.33. Z index added. By @dnladoog JG on 4/23/2025
    remove->setTooltip (L"Remove selected resources");
    remove->setButtonText (L"Remove");
    remove->addListener (this);
    remove->setColour(TextButton::buttonColourId, Colour(findColour(TextButton::buttonOnColourId))); // Colour (0xffb2b2b2));
    remove->setColour(TextButton::buttonOnColourId, Colour(findColour(TextButton::buttonOnColourId)));
    remove->setColour(TextButton::textColourOffId, Colour(findColour(TextButton::textColourOffId)));
    remove->setColour(TextButton::textColourOnId, Colour(findColour(TextButton::textColourOnId)));

	addAndMakeVisible(move = new TextButton(""), -1); // Updated v5.6.33. Z index added. By @dnladoog JG on 4/23/2025
	move->setTooltip(L"Move resources to panel folder");
	move->setButtonText(L"Move...");
	move->addListener(this);
    move->setColour(TextButton::buttonColourId, Colour(findColour(TextButton::buttonOnColourId))); // Colour (0xffb2b2b2));
    move->setColour(TextButton::buttonOnColourId, Colour(findColour(TextButton::buttonOnColourId)));
    move->setColour(TextButton::textColourOffId, Colour(findColour(TextButton::textColourOffId)));
    move->setColour(TextButton::textColourOnId, Colour(findColour(TextButton::textColourOnId)));

	addAndMakeVisible (reload = new TextButton (""), -1); // Updated v5.6.33. Z index added. By @dnladoog JG on 4/23/2025
    reload->setTooltip (L"Reload all resources");
    reload->setButtonText (L"Reload");
    reload->addListener (this);
    reload->setColour(TextButton::buttonColourId, Colour(findColour(TextButton::buttonOnColourId))); // Colour (0xffb2b2b2));
    reload->setColour(TextButton::buttonOnColourId, Colour(findColour(TextButton::buttonOnColourId)));
    reload->setColour(TextButton::textColourOffId, Colour(findColour(TextButton::textColourOffId)));
    reload->setColour(TextButton::textColourOnId, Colour(findColour(TextButton::textColourOnId)));

	tableFont = Font(Font::getDefaultSansSerifFontName(), 12.0f, Font::plain);
	resourceList->setRowHeight (22);
	resourceList->setHeaderHeight (22);
	resourceList->setMultipleSelectionEnabled (true);
	resourceList->getHeader().setStretchToFitActive (true);
	resourceList->getHeader().addColumn ("Name", 1, 160, 128);
	resourceList->getHeader().addColumn ("Size", 2, 48, 48);
	resourceList->getHeader().addColumn ("Type", 3, 32, 32);
    setSize (216, 340);
	updateTable();
}

CtrlrPanelResourceEditor::~CtrlrPanelResourceEditor()
{
    deleteAndZero (resourceList);
    deleteAndZero (add);
	deleteAndZero (remove);
	deleteAndZero (move);
	deleteAndZero (reload);
}

void CtrlrPanelResourceEditor::paint (Graphics& g)
{
}

void CtrlrPanelResourceEditor::resized() // Updated v5.6.34. Thanks to @dobo365. Updated v5.6.33. FIX issue #86 : unresponsive buttons
{
    int margin = 8;
    int buttonWidth = 72;

    resourceList->setBounds(0, 0, getWidth(), getHeight() - 48);
    add->setBounds(margin + 0, getHeight() - 48 + margin, buttonWidth, 32);
    remove->setBounds(2*margin + buttonWidth, getHeight() - 48 + margin, buttonWidth, 32);
    move->setBounds(3*margin + 2*buttonWidth, getHeight() - 48 + margin, buttonWidth, 32);
    reload->setBounds(getWidth() - buttonWidth - margin, getHeight() - 48 + margin, buttonWidth, 32);
}

void CtrlrPanelResourceEditor::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == add)
    {
		addResourceFromFile();
    }
    else if (buttonThatWasClicked == remove)
    {
		deleteSelectedResources();
    }
	else if (buttonThatWasClicked == move)
	{
		moveResources();
	}
	else if (buttonThatWasClicked == reload)
	{
		reloadAllResourcesFromSourceFiles();
	}
}

int CtrlrPanelResourceEditor::getNumRows()
{
	return (resources.size());
}

void CtrlrPanelResourceEditor::paintRowBackground (Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
	if (rowIsSelected)
	{
        gui::drawSelectionRectangle (g, width, height);
	}
}

void CtrlrPanelResourceEditor::paintCell (Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    // Updated v5.6.34. SEE : https://github.com/damiensellier/CtrlrX/issues/147
    // Draw the background first. The color changes depending on whether the row is selected.
    if (rowIsSelected)
    {
        // For selected rows, use a distinct background color.
        // Use the dedicated highlight color from the file tree component.
        g.setColour (findColour(DirectoryContentsDisplayComponent::highlightColourId));
    }
    else
    {
        // For unselected rows, use the standard background color.
        g.setColour (findColour(ListBox::backgroundColourId));
    }
    g.fillRect (0, 0, width, height);

    // Set the color for the text and other content based on selection.
    if (rowIsSelected)
    {
        // For selected rows, use a text color that contrasts well with the highlight color.
        // `highlightedTextColourId` is often used for this.
        g.setColour(findColour(DirectoryContentsDisplayComponent::highlightedTextColourId));
    }
    else
    {
        // Set the standard text color for unselected rows.
        g.setColour(findColour(ListBox::textColourId));
    }

    // Now, the code to draw the content of each column.

	if (columnId == 1)
	{
		if (resources [rowNumber])
		{
			g.setFont (tableFont.boldened());
			g.drawFittedText (resources [rowNumber]->getName(), 4, 2, width - 8, height - 4, Justification::left, 2);
		}
	}

	if (columnId == 2)
	{
		if (resources [rowNumber])
		{
			g.setFont (tableFont);
			g.drawFittedText (STR (File::descriptionOfSizeInBytes (resources [rowNumber]->getSize())+" ("+STR(resources [rowNumber]->getSize())+")"), 4, 2, width - 8, height - 4, Justification::left, 2);
		}
	}

	if (columnId == 3)
	{
		if (resources [rowNumber])
		{
			g.setFont (tableFont);
			g.drawFittedText (resources [rowNumber]->getTypeDescription(), 4, 2, width - 8, height - 4, Justification::left, 2);
		}
	}
}

void CtrlrPanelResourceEditor::addResourceFromFile()
{
	FileChooser fileChooser ("Resource file",
							File(owner.getOwner().getCtrlrManagerOwner().getProperty(Ids::ctrlrLastBrowsedResourceDir))
							,"*.*"
							,owner.getOwner().getCtrlrManagerOwner().getProperty(Ids::ctrlrNativeFileDialogs));
	if (fileChooser.browseForMultipleFilesToOpen(0))
 	{
		Array <File> filesToOpen = fileChooser.getResults();

		for (int i=0; i<filesToOpen.size(); i++)
		{
			if (i == 0)
				owner.getOwner().getCtrlrManagerOwner().setProperty (Ids::ctrlrLastBrowsedResourceDir, filesToOpen[i].getParentDirectory().getFullPathName());

			if (getResourceManager().resourceExists (filesToOpen[i]))
			{
				if ((bool)owner.getOwner().getCtrlrManagerOwner().getProperty(Ids::ctrlrOverwriteResources) == false)
				{
					if (!SURE("Resource: "+filesToOpen[i].getFileNameWithoutExtension()+" already exists. Overwrite?", this))
					{
						return;
					}
				}

				int index = getResourceManager().getResourceIndex(filesToOpen[i].getFileNameWithoutExtension());

				if (index >= 0)
				{
					getResourceManager().removeResource(index);
					getResourceManager().addResource (filesToOpen[i]);
				}
			}
			else
			{
 				getResourceManager().addResource (filesToOpen[i]);
			}
		}
 	}

	updateTable();
}

void CtrlrPanelResourceEditor::deleteSelectedResources()
{
	_DBG("CtrlrPanelResourceEditor::deleteSelectedResources");

	for (int i=resourceList->getNumSelectedRows()-1; i>=0; i--)
	{
		const int row = resourceList->getSelectedRow(i);

		_DBG("row selected: "+STR(row)+"/"+STR(i));

		if (resources[row])
		{
			getResourceManager().removeResource(resources[row]);
		}
	}

	updateTable();
}

void CtrlrPanelResourceEditor::updateTable()
{
	resources = getResourceManager().getResourcesCopy();

	resourceList->deselectAllRows();
 	resourceList->updateContent();
	if (owner.getPropertiesPanel())
		owner.getPropertiesPanel()->refreshAll();
	resources.sort (*this, false);
}

CtrlrPanelResourceManager &CtrlrPanelResourceEditor::getResourceManager()
{
	return (owner.getOwner().getResourceManager());
}

void CtrlrPanelResourceEditor::visibilityChanged()
{
	resources = getResourceManager().getResourcesCopy();
	resourceList->updateContent();
}

void CtrlrPanelResourceEditor::cellDoubleClicked (int rowNumber, int columnId, const MouseEvent &e)
{
	showResourceInfo(rowNumber);
}

void CtrlrPanelResourceEditor::cellClicked(int rowNumber, int columnId, const MouseEvent &e)
{
	if (e.mods.isPopupMenu())
	{
		PopupMenu m;
		m.addSectionHeader ("Resource");
		m.addItem (1, "More info");
		const int ret = m.show();

		if (ret == 1)
		{
			showResourceInfo(rowNumber);
		}
	}
}

void CtrlrPanelResourceEditor::showResourceInfo(const int resourceIndex)
{
	CtrlrPanelResource *res = resources[resourceIndex];
	if (res == nullptr)
		return;

	String message;
	message << "Load time: " + res->getLoadedTime().toString(true, true, true, true) + "\n";
	message << "Data file: " + res->getFile().getFullPathName() + "\n";
	message << "Source file: " + res->getSourceFile().getFullPathName() + "\n";
	message << "Source hash: " + STR(res->getHashCode()) + "\n";
	AudioFormatReader *afr = res->asAudioFormat();
	if (afr)
	{
		message << "Type: Audio\n";
		message << "Format name: " << afr->getFormatName() << "\n";
		message << "Sample rate: " << afr->sampleRate << "\n";
		message << "Bits per sample: " << ((int)afr->bitsPerSample) << "\n";
		message << "Length in samples: " << afr->lengthInSamples << "\n";
		message << "Number of channels: " << ((int)afr->numChannels) << "\n";
		message << "Metadata:\n";
		message << "\t" << afr->metadataValues.getDescription();
	}


	if (!res->asImage().isNull())
	{
		Image i = res->asImage();
		message << "Type: Image\n";
		message << "Width: " + STR(i.getWidth()) + "\n";
		message << "Height: " + STR(i.getHeight()) + "\n";
		message << "Has alpha: " + STR((int)i.hasAlphaChannel()) + "\n";
	}

	DialogWindow::LaunchOptions lo;
	Label *l = new Label ("", message);
	l->setSize (400, 150);
	l->setJustificationType (Justification::centred);
	l->setFont (Font(12.0f));
	lo.content.set(l, true);
	lo.componentToCentreAround		= this;
	//lo.dialogBackgroundColour		= Colours::whitesmoke;
	lo.dialogTitle					= "Resource information";
	lo.resizable					= true;
	lo.useBottomRightCornerResizer	= false;
	lo.useNativeTitleBar			= true;
	lo.launchAsync();
}

void CtrlrPanelResourceEditor::sortOrderChanged(int newSortColumnId, bool isForwards)
{
	sortByColumnId	= newSortColumnId;
	sortForward		= isForwards ? 1 : -1;

	resources.sort (*this, false);
}

int CtrlrPanelResourceEditor::compareElements(CtrlrPanelResource *first, CtrlrPanelResource *second)
{
	int ret = 0;

	switch (sortByColumnId)
	{
		case 1:
			ret = first->getName().compareNatural(second->getName());
			break;

		case 2:
			if (first->getSize() > second->getSize())
				ret = 1;
			if (first->getSize() < second->getSize())
				ret = -1;
			if (first->getSize() == second->getSize())
				ret = 0;
			break;
		default:
			break;
	}

	return (ret * sortForward);
}

void CtrlrPanelResourceEditor::moveResources()
{
	Array <CtrlrPanelResource*> resourcesReloaded;
	const String location = owner.getOwner().getPanelResourcesDirPath();
	const int confirm = AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon, "Move resources to panel folder", "Do you want to move all resources to the panel folder (location=" + location + ")?", "Yes", "No");
	if (confirm == 1)
	{
		File targetFolder = owner.getOwner().getPanelResourcesDir();
		if (!targetFolder.exists())
		{
			const Result res = targetFolder.createDirectory();
			if (res.failed())
			{
				AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Move resources to panel folder", "Failed to create resources folder '"+ location +"'.\n" + res.getErrorMessage());
				return;
			}
		}
		else if (!targetFolder.isDirectory())
		{
			AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Move resources to panel folder", "Failed to access resources folder '"+ location +"'.");
			return;
		}
		for (int i = 0; i<resources.size(); i++)
		{
			if (resources[i])
			{
				File originalFile = resources[i]->getSourceFile();
				if (!originalFile.exists())
				{	// If the source file is not available, use the data file
					resources[i]->getFile();
				}
				if (!originalFile.isAChildOf(targetFolder))
				{	// Skip resources that are already located in the panel folder
					File targetFile = targetFolder.getChildFile(originalFile.getFileName());
					if (targetFile.exists())
					{
						AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Move resources to panel folder", "Target file '" + targetFile.getFullPathName() + "' already exists, resource will be skiped.");
					}
					else
					{
						if (!originalFile.copyFileTo(targetFile))
						{
							AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Move resources to panel folder", "Could not copy resource to file '" + targetFile.getFullPathName() + "', resource will be skiped.");
						}
						else
						{
							resources[i]->setSourceFile(targetFile);
							resourcesReloaded.add(resources[i]);
							resources[i]->reloadFromSourceFile();
						}
					}
				}
			}
		}
	}
	owner.reloadResources(resourcesReloaded);
}

void CtrlrPanelResourceEditor::reloadAllResourcesFromSourceFiles()
{
	Array <CtrlrPanelResource*> resourcesReloaded;

	if (resourceList->getNumSelectedRows() <= 0)
	{
		for (int i=0; i<resources.size(); i++)
		{
			if (resources[i])
			{
				resourcesReloaded.add (resources[i]);
				resources[i]->reloadFromSourceFile();
			}
		}
	}
	else
	{
		for (int i=0; i<resourceList->getNumSelectedRows(); i++)
		{
			const int row = resourceList->getSelectedRow(i);
			if (resources[row])
			{
				resourcesReloaded.add (resources[row]);
				resources[row]->reloadFromSourceFile();
			}
		}
	}

	owner.reloadResources (resourcesReloaded);
}

void CtrlrPanelResourceEditor::backgroundClicked (const MouseEvent &e)
{
	resourceList->deselectAllRows();
}
