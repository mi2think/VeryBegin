#include "luac.h"

class Bar
{
public:
	Bar() { cout << "ctor bar\n"; }
	~Bar() { cout << "dctor bar\n"; }
};

Bar* gBar = nullptr;

static int DestoryBar(lua_State* L)
{
	if (lua_istable(L, -1))
	{
		lua_getfield(L, -1, "obj");

		Bar* bar = (Bar*)lua_touserdata(L, -1);
		if (bar)
		{
			delete bar;
		}
	}

	return 0;
}

static int CreateBar(lua_State* L)
{
	// create meta-table t
	lua_newtable(L);
	lua_newtable(L);
	lua_pushcfunction(L, &DestoryBar);
	// set t["__gc"] = DestoryBar
	lua_setfield(L, -2, "__gc");
	// set meta-table for gBar
	lua_setmetatable(L, -2);

	gBar = new Bar();
	lua_pushlightuserdata(L, gBar);
	lua_setfield(L, -2, "obj");

	return 1;
}


void luac_gc()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	lua_pushcfunction(L, CreateBar);
	lua_setglobal(L, "create_bar");

	luaL_loadfile(L, "scripts/luac_gc.lua");

	int status = lua_pcall(L, 0, 0, 0);
	if (status)
	{
		cout << lua_tostring(L, -1);
	}
}
