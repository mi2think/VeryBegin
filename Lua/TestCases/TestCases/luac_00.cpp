#include "luac.h"

void luac_00()
{
	char buff[256] = { 0 };
	int error;
	lua_State* L = luaL_newstate(); // open Lua
	luaL_openlibs(L);

	{
		lua_pushfstring(L, "hello, lua");
		lua_pushboolean(L, true);
		lua_pushnumber(L, 100);
		lua_pushnil(L);
		print_stack(L);
	}

	{
		int n = lua_tonumber(L, 3);
		cout << "n:" << n << "\n";
	}

	{
		cout << "lua push 2\n";
		// push element of given index of stack to top of stack
		lua_pushvalue(L, 2);
		print_stack(L);
	}

	{
		cout << "lua remove 1\n";
		// remove element of given index of stack
		// then move elements down, like erase of vector in C++
		lua_remove(L, 1);
		print_stack(L);
	}

	{
		cout << "lua insert 2\n";
		// shift up elements upper of given index
		// move top element of stack to given index+1
		lua_insert(L, 3);
		print_stack(L);
	}

	{
		cout << "lua replace 1\n";
		// pop top element of stack, assign it to given index
		lua_replace(L, 1);
		print_stack(L);
	}

	{
		cout << "lua set top 10\n";
		// set stack to given index, if it's high than before,
		// fill stack with nil, else drop elements
		// lua_settop(L, 0) will clear stack
		lua_settop(L, 10);
		print_stack(L);
	}

	cout << "input lua code for run\n";
	while (fgets(buff, sizeof(buff), stdin) != nullptr)
	{
		error = luaL_loadbuffer(L, buff, strlen(buff), "line") || lua_pcall(L, 0, 0, 0);
		if (error)
		{
			fprintf(stderr, "%s", lua_tostring(L, -1));
			lua_pop(L, 1); // pop err msg from stack
		}
	}

	lua_close(L);
}