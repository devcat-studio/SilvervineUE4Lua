// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#include "LuaVirtualMachine.h"


extern "C"
{
#define LUA_COMPAT_APIINTCASTS 1
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}