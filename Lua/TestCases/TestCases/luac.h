#ifndef __LUAC_H__
#define __LUAC_H__

#include "lua.hpp"

#include <string>
#include <cstdio>
#include <iostream>
#include <sstream>
using namespace std;

inline const char* lua_type2str(int t)
{
	if (t == -1)
		return "none";

	const char* typeName[] =
	{
		"nil",
		"boolean",
		"lightuserdata",
		"number",
		"string",
		"table",
		"function",
		"userdata",
		"thread"
	};
	return typeName[t];
}

inline string lua_value2str(lua_State* L, int t, int idx)
{
	ostringstream oss;
	switch (t)
	{
	case LUA_TNONE:		oss << "none";	break;
	case LUA_TNIL:		oss << "nil";	break;
	case LUA_TBOOLEAN:	oss << (lua_toboolean(L, idx) ? "true" : "false"); break;
	case LUA_TLIGHTUSERDATA:	oss << "lightuserdata";	break;
	case LUA_TNUMBER:	oss << lua_tonumber(L, idx);	break;
	case LUA_TSTRING:
		{
			size_t len = 0;
			const char* p = lua_tostring(L, idx);
			if (p)
			{
				oss << "'" << p << "'"; 
			} 
		}
		break;
	case LUA_TTABLE:	oss << "table";		break;
	case LUA_TUSERDATA:	oss << "userdata";	break;
	case LUA_TTHREAD:	oss << "thread";	break;
	}
	return oss.str();
}

inline void print_stack(lua_State* L)
{
	// get num of elements in stack
	int n = lua_gettop(L);
	cout << "----------------" << "\n";
	for (int i = n; i >= 1; --i)
	{
		int type = lua_type(L, i);
		cout << "[" << i << "] " << lua_type2str(type) << " " << lua_value2str(L, type, i) << "\n";
	}
	cout << "----------------" << "\n";
}


void luac_00();
void luac_01();
void luac_02();
void luac_gc();

#endif