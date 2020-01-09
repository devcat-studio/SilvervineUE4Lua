// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#include "LuaStaticBinding.h"

#include "LuaAPI.h"
#include "LuaLog.h"
#include "LuaUFunction.h"


FSUE4LuaFunctionStaticBindingContext::FSUE4LuaFunctionStaticBindingContext(lua_State* InL, int32 InArgStackBaseIndex, const TCHAR* InFunctionName)
	: L(InL)
	, ArgStackBaseIndex(InArgStackBaseIndex)
	, FunctionName(InFunctionName)
{
	bPassByName = SUE4LuaUFunction::IsParameterTable(L, ArgStackBaseIndex);

	if (bPassByName)
	{
		// pass-by-name 방식에서는 인자 수를 확인하지 않습니다.
		ArgCount = 0;
	}
	else
	{
		ArgCount = lua_gettop(L) - ArgStackBaseIndex + 1;
	}
}

void FSUE4LuaFunctionStaticBindingContext::LuaPushValue(int32 Index)
{
	lua_pushvalue(L, Index);
}

bool FSUE4LuaFunctionStaticBindingContext::LuaGetField(const char* Name)
{
	return lua_getfield(L, ArgStackBaseIndex, Name) != LUA_TNIL;
}

void FSUE4LuaFunctionStaticBindingContext::LuaSetField(const char* Name)
{
	lua_setfield(L, ArgStackBaseIndex, Name);
}

void FSUE4LuaFunctionStaticBindingContext::HandleMissingParameter(const char* ArgName)
{
	SUE4LVM_WARNING(L, TEXT("Found missing parameter '%s' while calling function '%s'."), UTF8_TO_TCHAR(ArgName), FunctionName);
}