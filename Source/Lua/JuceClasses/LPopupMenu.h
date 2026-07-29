#ifndef L_POPUP_MENU
#define L_POPUP_MENU

#include "JuceHeader.h"
extern  "C"
{
	#include "lua.h"
}

class LPopupMenu : public PopupMenu
{
	public:
		LPopupMenu();
		int show(int itemIDThatMustBeVisible = 0,
              int minimumWidth = 0,
              int maximumNumColumns = 0,
              int standardItemHeight = 0);
        int show(int itemHeight);
        int showAt(Rectangle<int> &areaToAttachTo, int standardItemHeight);
        int showAt(Component *componentToAttachTo, int standardItemHeight);
        void setLookAndFeel (const luabind::object &_customLookAndFeel); // Added v5.6.36. Thanks to @dnaldoog
		void addSubMenu (const String& subMenuName,
                     const LPopupMenu& subMenu,
                     bool isEnabled = true,
                     const Image& iconToUse = Image(),
                     bool isTicked = false,
                     int itemResultID = 0);

		static void wrapForLua (lua_State *L);
};

#endif
