#ifndef LUA_METHOD_EDITOR_COMMAND_IDS_H
#define LUA_METHOD_EDITOR_COMMAND_IDS_H

namespace LuaMethodEditorCommandIDs
{
	enum
	{
        fileSave = 2,
        fileSaveAndCompile = 3,
        fileSaveAndCompileAll = 4,
        fileCloseCurrentTab = 5,
        fileCloseAllTabs = 6,
        fileConvertToFiles = 7,
        fileClose = 1,

        editSearch = 15,
        editFindAndReplace = 16,
        editDebugger = 9,
        editConsole = 8,
        editClearOutput = 17,
        editPreferences = 18,

        editSingleLineComment = 10,
        editMultiLineComment = 11,
        editDuplicateLine = 12,
        editGoToLine = 13,
	};
}

#endif // LUA_METHOD_EDITOR_COMMAND_IDS_H