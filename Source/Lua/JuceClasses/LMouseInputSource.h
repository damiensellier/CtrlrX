#ifndef L_MOUSE_INPUT_SOURCE
#define L_MOUSE_INPUT_SOURCE

extern  "C"
{
	#include "lua.h"
}

class LMouseInputSource
{
	public:
		static void wrapForLua (lua_State *L);
};

#endif
