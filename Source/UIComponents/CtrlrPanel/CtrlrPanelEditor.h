#pragma once

#ifdef _WIN32
#pragma warning(disable:4355)
#endif // _WIN32

#include "CtrlrComponentSelection.h"
#include "CtrlrPanelCanvas.h"
#include "CtrlrUtilities.h"
#include "CtrlrPanel/CtrlrPanel.h"
#include "CtrlrLuaObject.h"
#include "CtrlrPanelProperties.h"
#include "CtrlrPanelViewport.h"
#include "CtrlrPanel/CtrlrPanelUndoManager.h"
#include "CtrlrPanelFindProperty.h"
#include "CtrlrPanelEditorIcons.h"

class CtrlrLuaMethod;

class CtrlrPanelEditor;



class CtrlrPanelNotifier : public Component
{
public:
	CtrlrPanelNotifier(CtrlrPanelEditor& _owner);
	~CtrlrPanelNotifier();
	void paint(Graphics& g);
	void resized();
	void setNotification(const String& notification, const CtrlrNotificationType ctrlrNotificationType);
	Colour getBackgroundColourForNotification(const CtrlrNotificationType ctrlrNotificationType);
	void mouseDown(const MouseEvent& e);
	JUCE_LEAK_DETECTOR(CtrlrPanelNotifier)
private:
	Colour background;
	ScopedPointer<Label> text;
	CtrlrPanelEditor& owner;
};


class CtrlrPanelEditor : public Component,
	public ValueTree::Listener,
	public CtrlrLuaObject,
	public LookAndFeel_V4 // is this redundant?
{
public:
	CtrlrPanelEditor(CtrlrPanel& _owner, CtrlrManager& _ctrlrManager, const String& panelName);
	~CtrlrPanelEditor();
	enum BackgroundImageLayout
	{
		Stretched,
		Center,
		Tile
	};
	void restoreState(const ValueTree& savedSate);
	void setBackgroundImage(const File& imageFile);
	CtrlrPanelCanvas* getCanvas();
	CtrlrComponentSelection* getSelection();
	void editModeChanged();
	CtrlrPanelProperties* getPanelProperties() { return (ctrlrPanelProperties); }
	CtrlrPanelViewport* getPanelViewport() { return (ctrlrPanelViewport); }
	CtrlrComponent* getSelected(const Identifier& uiType);
	void valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property);
	void valueTreeChildrenChanged(ValueTree& treeWhoseChildHasChanged) {}
	void valueTreeParentChanged(ValueTree& treeWhoseParentHasChanged) {}
	void valueTreeChildAdded(ValueTree& parentTree, ValueTree& childWhichHasBeenAdded) {}
	void valueTreeChildRemoved(ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int) {}
	void valueTreeChildOrderChanged(ValueTree& parentTreeWhoseChildrenHaveMoved, int, int) {}

	CtrlrPanel& getOwner() { return (owner); }
	void setProperty(const Identifier& name, const var& newValue, const bool isUndoable = false);
	const var& getProperty(const Identifier& name) const;
	const var getProperty(const Identifier& name, const var& defaultReturnValue) const;
	ValueTree& getPanelEditorTree() { return (panelEditorTree); }

	void layoutItems();
	CtrlrPanelProperties* getPropertiesPanel() { return (ctrlrPanelProperties); }
	const bool getMode();
	AffineTransform moveSelectionToPosition(const int newX, const int newY);
	static void wrapForLua(lua_State* L);
	ValueTree& getObjectTree() { return (panelEditorTree); }
	void initSingleInstance();
	void resized();
	void visibilityChanged();

	void notify(const String& notification, CtrlrNotificationCallback* callback, const CtrlrNotificationType ctrlrNotificationType = NotifyInformation);  // Added back v5.6.31 for file management bottom notification bar
	void notificationClicked(const MouseEvent e); // Added back v5.6.31 for file management bottom notification bar
	void changeListenerCallback(ChangeBroadcaster* source); // Added back v5.6.31 for file management bottom notification bar

	void setAllCombosDisabled();
	void setAllCombosEnabled();
	void saveLayout();
	bool getRestoreState() { return (currentRestoreState); }
	void setRestoreState(const bool _state) { currentRestoreState = _state; }
	void reloadResources(Array <CtrlrPanelResource*> resourcesThatChanged);
	//void showComponentRuntimeConfig(CtrlrComponent *componentToConfigure);
	void searchForProperty();
	static LookAndFeel* getLookAndFeelFromDescription(const String& lookAndFeelDesc);
	void editModeChanged(const bool isEditMode);

	bool luaEditorExistsAndIsFocused(); // Added v5.6.34. Required to pass keypress to the LUA method manager for menu items. Handles the focus gain/loss.

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CtrlrPanelEditor)

private:
	ScopedPointer <CtrlrPanelNotifier> ctrlrPanelNotifier;  // Added back v5.6.31 for file management bottom notification bar
	ComponentAnimator componentAnimator;
	CtrlrPanel& owner;
	double canvasHeight;
	double canvasWidth;
	double canvasAspectRatio;
	ScopedPointer <CtrlrComponentSelection> ctrlrComponentSelection;
	StretchableLayoutManager layoutManager;
	bool lastEditMode, currentRestoreState;
	CtrlrManager& ctrlrManager;
	ValueTree panelEditorTree;
	friend class WeakReference<CtrlrPanelEditor>;
	WeakReference <CtrlrPanelEditor>::Master masterReference;
	WeakReference <CtrlrLuaMethod>
		resizedEditorCbk,
		resizedCbk;
	CtrlrPanelProperties* ctrlrPanelProperties;
	StretchableLayoutResizerBar* spacerComponent;
	CtrlrPanelViewport* ctrlrPanelViewport;
	WeakReference<CtrlrNotificationCallback> notificationCallback;
	Component* editorComponentsInEditMode[3];
	Component* editorComponents[2];
	std::unique_ptr<juce::LookAndFeel> lookAndFeel;
	juce::LookAndFeel* previousGlobalLookAndFeel;
};