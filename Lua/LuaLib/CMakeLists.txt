cmake_minimum_required(VERSION 2.6.2)

project(Lua)

set(Header_Files_Headers
./src/lua.h
./src/luaconf.h
./src/lualib.h
./src/lauxlib.h
./src/lua.h
./src/lua.hpp
)

set(Source_Files_Sources
./src/lapi.c
./src/lapi.h
./src/lauxlib.c
./src/lauxlib.h
./src/lbaselib.c
./src/lbitlib.c
./src/lcode.c
./src/lcode.c
./src/lcorolib.c
./src/lctype.c
./src/lctype.h
./src/ldblib.c
./src/ldebug.c
./src/ldebug.h
./src/ldo.c
./src/ldo.h
./src/ldump.c
./src/lfunc.c
./src/lfunc.h
./src/lgc.c
./src/lgc.h
./src/linit.c
./src/liolib.c
./src/llex.c
./src/llex.h
./src/llimits.h
./src/lmathlib.c
./src/lmem.c
./src/lmem.h
./src/loadlib.c
./src/lobject.c
./src/lobject.h
./src/lopcodes.c
./src/lopcodes.h
./src/loslib.c
./src/lparser.c
./src/lparser.h
./src/lprefix.h
./src/lstate.c
./src/lstate.h
./src/lstring.c
./src/lstring.h
./src/lstrlib.c
./src/ltable.c
./src/ltable.h
./src/ltablib.c
./src/ltm.c
./src/ltm.h
./src/lua.h
./src/lua.hpp
./src/luaconf.h
./src/lualib.h
./src/lundump.c
./src/lundump.h
./src/lutf8lib.c
./src/lvm.c
./src/lvm.h
./src/lzio.c
./src/lzio.h
)

set(Lua_Sources
${Header_Files_Headers}
${Source_Files_Sources}
)

set(Lua_Header_Files
	${Header_Files_Headers}
)

add_library(Lua STATIC ${Lua_Sources})

add_executable(LuaVM ./src/lua.c ${Lua_Sources})

add_executable(LuaC ./src/luac.c ${Lua_Sources})

add_definitions(-DLUA_COMPAT_5_2 -D_LIB)

include_directories(./src)
