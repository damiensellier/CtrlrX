#include "stdafx.h"
#include "CtrlrManager/CtrlrManager.h"
#include "UIComponents/CtrlrPanel/CtrlrPanelEditor.h"
#include "CtrlrProcessor.h"
#include "CtrlrPanel/CtrlrPanelResource.h"
#include "CtrlrInlineUtilitiesGUI.h"
#include "CtrlrPanelResourceEditor.h"

// Definition of ImageInfoComponent
class ImageInfoComponent : public Component
{
public:
    ImageInfoComponent(const String& infoText, const Image& thumbnailImg)
        : infoMessage(infoText), thumbnail(thumbnailImg)
    {
    }

    void paint(Graphics& g) override
    {
        g.fillAll(Colour(0xfff0f0f0));

        // Draw the info text at the top
        g.setColour(Colours::black);
        g.setFont(Font(12.0f));

        int textX = 10;
        int textY = 10;
        int textWidth = getWidth() - 20;
        int textHeight = thumbnail.isNull() ? (getHeight() - 20) : 120; // Reserve space for image if present

        g.drawMultiLineText(infoMessage, textX, textY + 15, textWidth);

        // Draw the thumbnail image underneath the text
        if (!thumbnail.isNull())
        {
            int imageY = textHeight + 20; // Position below text with some margin
            int imageX = (getWidth() - thumbnail.getWidth()) / 2; // Center the image horizontally

            // Ensure imageX is not negative (in case thumbnail is wider than dialog)
            if (imageX < 10) imageX = 10;

            g.drawImage(thumbnail, imageX, imageY, thumbnail.getWidth(), thumbnail.getHeight(),
                        0, 0, thumbnail.getWidth(), thumbnail.getHeight());

            // Draw a border around the image
            g.setColour(Colours::grey);
            g.drawRect(imageX - 1, imageY - 1, thumbnail.getWidth() + 2, thumbnail.getHeight() + 2);
        }
    }

    void resized() override
    {
        // Component will be resized by the dialog window
    }
private:
    String infoMessage;
    Image thumbnail;
};

// Definition of CtrlrPanelResourceEditor
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

	addAndMakeVisible (remove = new TextButton (""), -1); // Updated v5.6.33. Z index added. By @dnladoog JG on 4/23/2025
    remove->setTooltip (L"Remove selected resources");
    remove->setButtonText (L"Remove");
    remove->addListener (this);

	addAndMakeVisible(move = new TextButton(""), -1); // Updated v5.6.33. Z index added. By @dnladoog JG on 4/23/2025
	move->setTooltip(L"Move resources to panel folder");
	move->setButtonText(L"Move...");
	move->addListener(this);

	addAndMakeVisible (reload = new TextButton (""), -1); // Updated v5.6.33. Z index added. By @dnladoog JG on 4/23/2025
    reload->setTooltip (L"Reload all resources");
    reload->setButtonText (L"Reload");
    reload->addListener (this);

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
	
	Image originalImage = res->asImage();
	Image thumbnailImage;
	bool hasImage = false;
	bool hasFont = false;
	Font previewFont;
	
	// Check if it's a regular image format
	if (!originalImage.isNull())
	{
		hasImage = true;
		message << "Type: Image\n";
		message << "Width: " + STR(originalImage.getWidth()) + "\n";
		message << "Height: " + STR(originalImage.getHeight()) + "\n";
		message << "Has alpha: " + STR((int)originalImage.hasAlphaChannel()) + "\n";
		
		thumbnailImage = createThumbnail(originalImage, 150);
		message << "Thumbnail size: " + STR(thumbnailImage.getWidth()) + "x" + STR(thumbnailImage.getHeight()) + "\n";
	}
	// Check if it might be a font file
	else if (isFontFile(res->getFile()))
	{
		try
		{
			// Try to load the font from the file directly
			File fontFile = res->getFile();
			
			if (fontFile.exists())
			{
				// Try different methods to create typeface based on JUCE version
				Typeface::Ptr typeface = nullptr;
				
				// Method 1: Try the most common JUCE 6.x method
				MemoryBlock fontData;
				if (fontFile.loadFileAsData(fontData))
				{
					typeface = Typeface::createSystemTypefaceFor(fontData.getData(), fontData.getSize());
				}
				
				// Method 2: If that doesn't work, try alternative methods
				if (typeface == nullptr)
				{
					// Try with InputStream
					FileInputStream fontStream(fontFile);
					if (fontStream.openedOk())
					{
						// Different possible method names in JUCE 6.x
						// typeface = Typeface::createSystemTypefaceFor(fontStream);  // Already failed
						// typeface = Typeface::createFromInputStream(fontStream);
						// typeface = Typeface::createFromData(fontStream);
					}
				}
				
				if (typeface != nullptr)
				{
					hasFont = true;
					message << "Type: Font\n";
					message << "Font name: " + typeface->getName() + "\n";
					message << "Font style: " + typeface->getStyle() + "\n";
					
					// Create a preview font
					previewFont = Font(typeface);
					previewFont.setHeight(24.0f);
					
					// Create a font preview image
					const int previewWidth = 300;
					const int previewHeight = 120;
					
					thumbnailImage = Image(Image::ARGB, previewWidth, previewHeight, true);
					Graphics g(thumbnailImage);
					g.fillAll(Colours::lightgrey);
					
					// Draw sample text at different sizes
					g.setColour(Colours::black);
					
					// Large sample text
					Font largeFont(typeface);
					largeFont.setHeight(28.0f);
					g.setFont(largeFont);
					g.drawText("Sample Text", 10, 10, previewWidth - 20, 35, Justification::left);
					
					// Medium sample text
					Font mediumFont(typeface);
					mediumFont.setHeight(18.0f);
					g.setFont(mediumFont);
					g.drawText("The quick brown fox jumps", 10, 45, previewWidth - 20, 25, Justification::left);
					
					// Small sample text with numbers and symbols
					Font smallFont(typeface);
					smallFont.setHeight(14.0f);
					g.setFont(smallFont);
					g.drawText("ABCDEFGHIJKLMNOPQRSTUVWXYZ", 10, 70, previewWidth - 20, 20, Justification::left);
					g.drawText("abcdefghijklmnopqrstuvwxyz", 10, 85, previewWidth - 20, 20, Justification::left);
					g.drawText("0123456789 !@#$%^&*()", 10, 100, previewWidth - 20, 20, Justification::left);
					
					// Draw a border
					g.setColour(Colours::lightgrey);
					g.drawRect(0, 0, previewWidth, previewHeight);
					
					message << "Font preview generated\n";
				}
				else
				{
					// Method 3: Simple fallback - just show it's a font without preview
					hasFont = true;
					message << "Type: Font\n";
					message << "Font file: " + fontFile.getFileName() + "\n";
					message << "Size: " + File::descriptionOfSizeInBytes(fontFile.getSize()) + "\n";
					message << "Note: Preview not available (typeface loading not supported)\n";
					
					// Create a simple "Font" placeholder image
					const int previewWidth = 200;
					const int previewHeight = 80;
					
					thumbnailImage = Image(Image::ARGB, previewWidth, previewHeight, true);
					Graphics g(thumbnailImage);
					g.fillAll(Colour(0xfff0f0f0));
					
					// Draw a simple font icon/text
					g.setColour(Colours::darkgrey);
					g.setFont(Font(24.0f, Font::bold));
					g.drawText("FONT", 0, 0, previewWidth, previewHeight, Justification::centred);
					
					g.setColour(Colours::grey);
					g.setFont(Font(12.0f));
					g.drawText(fontFile.getFileNameWithoutExtension(), 5, previewHeight - 20, previewWidth - 10, 15, Justification::centred);
					
					// Draw a border
					g.setColour(Colours::lightgrey);
					g.drawRect(0, 0, previewWidth, previewHeight);
					
					message << "Font placeholder generated\n";
				}
			}
			else
			{
				message << "Type: Font (file not found)\n";
				message << "Font file does not exist: " + fontFile.getFullPathName() + "\n";
			}
		}
		catch (...)
		{
			message << "Type: Font (error loading)\n";
			message << "Error creating font preview\n";
		}
	}
	// Check if it might be an SVG file
	else if (res->getFile().getFileExtension().toLowerCase() == ".svg")
	{
		try
		{
			String svgContent = res->getFile().loadFileAsString();
			std::unique_ptr<XmlElement> svgElement = XmlDocument::parse(svgContent);
			
			if (svgElement != nullptr && svgElement->hasTagName("svg"))
			{
				hasImage = true;
				message << "Type: SVG Vector Image\n";
				
				// Try to parse SVG dimensions
				String widthStr = svgElement->getStringAttribute("width");
				String heightStr = svgElement->getStringAttribute("height");
				
				if (widthStr.isNotEmpty() && heightStr.isNotEmpty())
				{
					message << "SVG dimensions: " + widthStr + " x " + heightStr + "\n";
				}
				
				// Create a thumbnail from SVG
				std::unique_ptr<Drawable> svgDrawable = Drawable::createFromSVG(*svgElement);
				if (svgDrawable != nullptr)
				{
					Rectangle<float> svgBounds = svgDrawable->getDrawableBounds();
					
					// Handle case where bounds might be empty or invalid
					if (svgBounds.isEmpty() || svgBounds.getWidth() <= 0 || svgBounds.getHeight() <= 0)
					{
						// Use default size if bounds are invalid
						svgBounds = Rectangle<float>(0, 0, 100, 100);
					}
					
					// Calculate thumbnail size maintaining aspect ratio
					const int maxThumbnailSize = 150;
					float aspectRatio = svgBounds.getWidth() / svgBounds.getHeight();
					int thumbWidth, thumbHeight;
					
					if (aspectRatio > 1.0f)
					{
						thumbWidth = maxThumbnailSize;
						thumbHeight = jmax(1, (int)(maxThumbnailSize / aspectRatio));
					}
					else
					{
						thumbWidth = jmax(1, (int)(maxThumbnailSize * aspectRatio));
						thumbHeight = maxThumbnailSize;
					}
					
					// Render SVG to thumbnail image
					thumbnailImage = Image(Image::ARGB, thumbWidth, thumbHeight, true);
					Graphics g(thumbnailImage);
					g.fillAll(Colours::lightgrey);
					
					// Set transform and draw
					AffineTransform transform = RectanglePlacement(RectanglePlacement::centred)
					.getTransformToFit(svgBounds, Rectangle<float>(0, 0, (float)thumbWidth, (float)thumbHeight));
					g.addTransform(transform);
					
					svgDrawable->draw(g, 1.0f);
					
					message << "SVG rendered to thumbnail: " + STR(thumbWidth) + "x" + STR(thumbHeight) + "\n";
				}
				else
				{
					message << "Failed to create SVG drawable\n";
				}
			}
			else
			{
				message << "Invalid SVG format\n";
			}
		}
		catch (...)
		{
			message << "Error processing SVG file\n";
		}
	}
	
	DialogWindow::LaunchOptions lo;
	
	if ((hasImage || hasFont) && !thumbnailImage.isNull())
	{
		// Create a custom component that contains both the label and image/font preview
		Component* contentComponent = new ImageInfoComponent(message, thumbnailImage);
		// Adjust height to accommodate text at top + preview at bottom + margins
		int dialogHeight = 160 + thumbnailImage.getHeight() + 30; // text area + preview + margins
		contentComponent->setSize(450, dialogHeight);
		lo.content.set(contentComponent, true);
	}
	else
	{
		// No image, just show the text label as before
		Label* l = new Label("", message);
		l->setSize(400, 150);
		l->setJustificationType(Justification::centred);
		l->setFont(Font(12.0f));
		lo.content.set(l, true);
	}
	
	lo.componentToCentreAround = this;
	lo.dialogTitle = "Resource information";
	lo.resizable = true;
	lo.useBottomRightCornerResizer = false;
	lo.useNativeTitleBar = true;
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

void CtrlrPanelResourceEditor::lookAndFeelChanged()
{
    // Update the 'add' button's colours
    add->setColour(TextButton::buttonColourId, findColour(TextButton::buttonColourId));
    add->setColour(TextButton::buttonOnColourId, findColour(TextButton::buttonOnColourId));
    add->setColour(TextButton::textColourOffId, findColour(TextButton::textColourOffId));
    add->setColour(TextButton::textColourOnId, findColour(TextButton::textColourOnId));

    // Update the 'remove' button's colours
    remove->setColour(TextButton::buttonColourId, findColour(TextButton::buttonColourId));
    remove->setColour(TextButton::buttonOnColourId, findColour(TextButton::buttonOnColourId));
    remove->setColour(TextButton::textColourOffId, findColour(TextButton::textColourOffId));
    remove->setColour(TextButton::textColourOnId, findColour(TextButton::textColourOnId));

    // Update the 'move' button's colours
    move->setColour(TextButton::buttonColourId, findColour(TextButton::buttonColourId));
    move->setColour(TextButton::buttonOnColourId, findColour(TextButton::buttonOnColourId));
    move->setColour(TextButton::textColourOffId, findColour(TextButton::textColourOffId));
    move->setColour(TextButton::textColourOnId, findColour(TextButton::textColourOnId));

    // Update the 'reload' button's colours
    reload->setColour(TextButton::buttonColourId, findColour(TextButton::buttonColourId));
    reload->setColour(TextButton::buttonOnColourId, findColour(TextButton::buttonOnColourId));
    reload->setColour(TextButton::textColourOffId, findColour(TextButton::textColourOffId));
    reload->setColour(TextButton::textColourOnId, findColour(TextButton::textColourOnId));
}

// Helper method to detect font files
bool CtrlrPanelResourceEditor::isFontFile(const File& file)
{
	String extension = file.getFileExtension().toLowerCase();
	return extension == ".ttf" || extension == ".otf" || extension == ".woff" || extension == ".woff2";
}

// Helper method to create thumbnails
Image CtrlrPanelResourceEditor::createThumbnail(const Image& originalImage, int maxSize) // Added v5.6.34. Thanks to @dnaldoog
{
	int originalWidth = originalImage.getWidth();
	int originalHeight = originalImage.getHeight();
	
	if (originalWidth <= maxSize && originalHeight <= maxSize)
	{
		return originalImage; // Already small enough
	}
	
	// Calculate scaling factor to maintain aspect ratio
	float scaleFactor = jmin((float)maxSize / originalWidth,
							 (float)maxSize / originalHeight);
	
	int thumbnailWidth = (int)(originalWidth * scaleFactor);
	int thumbnailHeight = (int)(originalHeight * scaleFactor);
	
	// Create scaled thumbnail with high quality resampling
	return originalImage.rescaled(thumbnailWidth, thumbnailHeight,
								  Graphics::ResamplingQuality::highResamplingQuality);
}
