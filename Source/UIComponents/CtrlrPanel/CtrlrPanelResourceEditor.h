#ifndef CTRLR_PANEL_RESOURCE_EDITOR_H
#define CTRLR_PANEL_RESOURCE_EDITOR_H

#include "CtrlrMacros.h"
class CtrlrPanelEditor;
class CtrlrPanelResourceManager;

class CtrlrPanelResourceEditor  : public Component,
                                  public TableListBoxModel,
								  public Button::Listener
{
	public:
		CtrlrPanelResourceEditor (CtrlrPanelEditor &_owner);
		~CtrlrPanelResourceEditor();
		int getNumRows();
		void paintRowBackground (Graphics& g, int rowNumber, int width, int height, bool rowIsSelected);
		void paintCell (Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected);
		void cellClicked(int rowNumber, int columnId, const MouseEvent &e);
		void cellDoubleClicked (int rowNumber, int columnId, const MouseEvent &e);
		void addResourceFromFile();
		void deleteSelectedResources();
		CtrlrPanelResourceManager &getResourceManager();
		void visibilityChanged();
		void sortOrderChanged(int newSortColumnId, bool isForwards);
		int compareElements(CtrlrPanelResource *first, CtrlrPanelResource *second);
		void paint (Graphics& g);
		void resized();
		void buttonClicked (Button* buttonThatWasClicked);
		void updateTable();
		void reloadAllResourcesFromSourceFiles();
		void moveResources();
		void backgroundClicked (const MouseEvent &e);
		void showResourceInfo(const int resourceIndex);
	private:
		Array <CtrlrPanelResource*> resources;
		CtrlrPanelEditor &owner;
		int sortByColumnId;
		int sortForward;
		Font tableFont;
	    TableListBox* resourceList;
	    TextButton* add;
		TextButton* move;
		TextButton* remove, *reload;
		Image createThumbnail(const Image& originalImage, int maxSize);
        String infoMessage;
        Image thumbnail;
		bool isFontFile(const File& file);
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

	    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CtrlrPanelResourceEditor);
};


#endif
