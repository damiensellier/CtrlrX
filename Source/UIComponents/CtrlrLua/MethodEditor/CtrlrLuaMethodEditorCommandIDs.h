// LuaMethodEditorCommandIDs.h

#ifndef LUA_METHOD_EDITOR_COMMAND_IDS_H
#define LUA_METHOD_EDITOR_COMMAND_IDS_H

namespace LuaMethodEditorCommandIDs
{
	enum
	{
		fileSave = 0x7000,
		fileSaveAndCompile,
		fileSaveAndCompileAll,
		fileCloseCurrentTab,
		fileCloseAllTabs,
		fileConvertToFiles,
		fileClose,
		editSearch,
		editFindAndReplace,
		editDebugger,
		editConsole,
		editClearOutput,
		editPreferences,
		editSingleLineComment,
		editMultiLineComment,
		editDuplicateLine,
		editGoToLine
	};
}

#endif // LUA_METHOD_EDITOR_COMMAND_IDS_H
