#include "luac.h"
#include <cmath>

static int l_sin(lua_State* L)
{
	float val = lua_tonumber(L, 1);
	lua_pushnumber(L, sin(val));
	return 1; // number of results
}

void luac_02()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	luaL_loadfile(L, "scripts/luac_02.lua");

	lua_pushcfunction(L, l_sin);
	lua_setglobal(L, "mysin");

	int status = lua_pcall(L, 0, 0, 0);
	if (status)
	{
		cout << lua_tostring(L, -1);
	}

	lua_close(L);
}