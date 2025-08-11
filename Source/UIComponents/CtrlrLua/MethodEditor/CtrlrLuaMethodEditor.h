#ifndef __CTRLR_LUA_METHOD_EDITOR__
#define __CTRLR_LUA_METHOD_EDITOR__

#include "CtrlrPanel/CtrlrPanelModulatorListTree.h"
#include "CtrlrWindowManagers/CtrlrPanelWindowManager.h"
#include "CtrlrLuaMethodFind.h"
#include "CtrlrLuaMethodEditArea.h"
#include "CtrlrValueTreeEditor.h"
#include "LuaMethodEditorCommandIDs.h"


class CtrlrManager;
class CtrlrLookAndFeel;
class CtrlrLuaMethodManager;
class CtrlrLuaMethodCodeEditor;

#define METHOD_NEW                "Add new method"
#define METHOD_DEL                "Delete method"
#define METHOD_FROM_TEMPLATE    "Replace with template"
#define METHOD_SAVE                "Save method"
#define FIND_AND_REPLACE        "Find and replace"
#define GROUP_NEW                "Add new group"

//namespace LuaMethodEditorCommandIDs // Added v5.6.34. Refactor of LUA method editor menu bar items. Assign index to LUA method editor menu item functions.
//{
//    enum
//    {
//        fileSave                = 2,
//        fileSaveAndCompile      = 3,
//        fileSaveAndCompileAll   = 4,
//        fileCloseCurrentTab     = 5,
//        fileCloseAllTabs        = 6,
//        fileConvertToFiles      = 7,
//        fileClose               = 1,
//
//        editSearch              = 15,
//        editFindAndReplace      = 16,
//        editDebugger            = 9,
//        editConsole             = 8,
//        editClearOutput         = 17,
//        editPreferences         = 18,
//
//        editSingleLineComment   = 10,
//        editMultiLineComment    = 11,
//        editDuplicateLine       = 12,
//        editGoToLine            = 13,
//    };
//}

class ChildSorter
{
    public:
        ChildSorter (const bool _sortByName, CtrlrLuaMethodEditor &_parent);
        int compareElements (ValueTree first, ValueTree second);

    private:
        bool sortByName;
        CtrlrLuaMethodEditor &parent;
};

class CtrlrLuaMethodEditor  : public CtrlrChildWindowContent,
                              public CtrlrValueTreeEditorLookProvider,
                              public KeyListener,
                              public DragAndDropContainer,
                              public ValueTree::Listener
{
    public:
        CtrlrLuaMethodEditor (CtrlrPanel &_owner);
        ~CtrlrLuaMethodEditor();
        enum ToolbarItems
        {
            _none,
            saveMethod,
            saveAndCompileMethod,
            findInMethod
        };

        CtrlrLuaMethod *setEditedMethod (const Uuid &methodUuid);
        CtrlrLuaMethod *setEditedMethod (const String &methodName);
        CtrlrLuaMethodCodeEditor *getEditorForMethod (CtrlrLuaMethod *method);
        void highlightCode (const String &methodName, const int lineNumber);
        void createNewTab(CtrlrLuaMethod* method);
        CtrlrLuaMethodCodeEditor *getCurrentEditor();
        CtrlrLuaMethodEditArea *getMethodEditArea();
        void restoreState(const ValueTree &savedState);
        void setPositionLabelText (const String &text);

        void addNewMethod(ValueTree parentGroup=ValueTree());
        //void addNewClass(ValueTree parentGroup=ValueTree()); JG 1/19/2025
        void addMethodFromFile(ValueTree parentGroup=ValueTree());

        void addNewGroup(ValueTree parentGroup=ValueTree());
        void removeGroup(ValueTree parentGroup=ValueTree());
        void renameGroup(ValueTree parentGroup=ValueTree());

        const String getUniqueName(const ValueTree &item) const;
        Drawable* getIconForItem(const ValueTree &item)    const;
        void itemClicked (const MouseEvent &e, ValueTree &item);
        void itemDoubleClicked (const MouseEvent &e, ValueTree &item);
        const bool renameItem(const ValueTree &item, const String &newName) const;
        const bool canBeRenamed(const ValueTree &item) const;
        const AttributedString    getDisplayString(const ValueTree &item)    const;
        const Font getItemFont(const ValueTree &item) const;
        const bool isInterestedInDragSource(const ValueTree &item, const DragAndDropTarget::SourceDetails &dragSourceDetails) const;
        void itemDropped (ValueTree &item, const DragAndDropTarget::SourceDetails &dragSourceDetails, int insertIndex);
        void handleAsyncUpdate();
        CtrlrLuaMethodManager &getMethodManager();
        String getContentName()                    { return ("LUA Editor"); }
        uint8 getType()                            { return (CtrlrPanelWindowManager::LuaMethodEditor); }
        CtrlrPanel &getOwner();
        void paint (Graphics& g);
        void resized();
        bool keyPressed (const KeyPress& key, Component* originatingComponent);
        void closeCurrentTab();
        void closeAllTabs();
        bool closeTab(const int tabIndex);
        bool canCloseWindow();
        
        void tabChanged(CtrlrLuaMethodCodeEditor *codeEditor, const bool save=false, const bool recompile=false);
        void updateTabs();
        void valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property);
    // Add this public method to CtrlrLuaMethodEditor
    public:
        // ... existing methods ...
        /** Returns the current GenericCodeEditorComponent, or nullptr if not available. */
        GenericCodeEditorComponent* getEditorComponent()
        {
            return editorComponent;
        }
        void valueTreeChildrenChanged (ValueTree &treeWhoseChildHasChanged){}
        void valueTreeParentChanged (ValueTree &treeWhoseParentHasChanged){}
        void valueTreeChildAdded (ValueTree& parentTree, ValueTree& child){}
        void valueTreeChildRemoved (ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int){}
        void valueTreeChildOrderChanged (ValueTree& parentTreeWhoseChildrenHaveMoved, int, int){}
        TabbedComponent *getTabs();
        void updateRootItem();
        void itemChanged (ValueTree &itemTreeThatChanged);
        var getDragSourceDescription(Array <ValueTree> &selectedTreeItems);
        void setCurrentTab (CtrlrLuaMethod *methodToSetAsCurrent);
        void saveSettings();
        ValueTree &getComponentTree();
        StringArray getMenuBarNames();
        PopupMenu getMenuForIndex(int topLevelMenuIndex, const String &menuName);
        void menuItemSelected(int menuItemID, int topLevelMenuIndex);
        void searchResultClicked (const String &methodName, const int lineNumber, const int resultPositionStart, const int resultPositionEnd);
        void saveAndCompilAllMethods();
        void convertToFiles();
        /* Debugger stuff
        */
        void insertRawDebuggerOutput(const String &debuggerOutput);
        //void setRawDebuggerOutput(const String &debuggerOutput); JG 1/19/2025
        void setJsonDebuggerOutput(const String &jsonData);
        int waitForCommand();
        const String getCurrentDebuggerCommand(const bool clearTheReturnedCommand=true);
        void setOpenSearchTabsEnabled(bool shouldOpen); // used for toggling search closed files
        bool getOpenSearchTabsEnabled() const; // getter
        JUCE_LEAK_DETECTOR(CtrlrLuaMethodEditor)

        bool caseCansitive, findDialogActive;
        String lookInString, searchInString, currentSearchString;

private:
        juce::Value sharedSearchTabsValue;
        bool openSearchTabsEnabledState = false; // Add this private member variable
        WeakReference<CtrlrLuaMethodEditor>::Master masterReference;
        friend class WeakReference<CtrlrLuaMethodEditor>;
        File lastBrowsedSourceDir;
        CtrlrValueTreeEditorTree *methodTree;
        StretchableLayoutManager layoutManager;
        StretchableLayoutResizerBar *resizer;
        CtrlrPanel &owner;
        CtrlrLuaMethodEditArea *methodEditArea;
        GenericCodeEditorComponent* editorComponent;
};

class SharedValues
{
public:
    static Value& getSearchTabsValue()
    {
        static Value searchTabsValue(true); // default to true
        return searchTabsValue;
    }
    static const String& getSearchTabsLabel()
    {
        static const String label("Keep methods opened after a match");
        return label;
    }
};
#endif
