#include "luac.h"

void luac_01()
{
	lua_State* L = luaL_newstate();
	luaL_loadfile(L, "scripts/luac_01.lua");
	lua_pcall(L, 0, 0, 0);

	{
		// push value of global name to stack
		// return type of value
		lua_getglobal(L, "width");
		lua_getglobal(L, "height");

		int width = lua_tointeger(L, -2);
		int height = lua_tointeger(L, -1);
		cout << "width:" << width << ", height:" << height << "\n";
	}

	{
		lua_getglobal(L, "background");
		// push t[k] to stack, t is value at given index.
		lua_getfield(L, -1, "r");
		float r = lua_tonumber(L, -1);
		cout << "r:" << r << "\n";
	}

	{
		// push float number to stack
		lua_pushnumber(L, 0.55);
		lua_setfield(L, -3, "r");
	}

	{
		// create empty table and push to stack
		lua_newtable(L);
		// push string to stack, Lua will copy or reuse it internal
		lua_pushstring(L, "http://www.jellythink.com");
		lua_setfield(L, -2, "website");
	}

	{
		lua_getglobal(L, "add");
		lua_pushnumber(L, 10);
		lua_pushnumber(L, 20);
		print_stack(L);
		// call a function in protected mode
		// Lua will pop function and params after it called, and push results to stack
		lua_pcall(L, 2, 3, 0);
		float sum = lua_tonumber(L, -1);
		cout << "sum:" << sum << "\n";
	}

	print_stack(L);
	lua_close(L);
}
