// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "CoreMinimal.h"

struct lua_State;


//
// lua에서 사용할 c++ 라이브러리 구현
//
namespace SUE4LuaLibrary
{
	void RegisterSUE4LuaLibrary(lua_State* L);
	void RegisterUE4Library(lua_State* L);
	void RegisterPlatformTimeLibrary(lua_State* L);
	void RegisterMathLibrary(lua_State* L);
	void RegisterVectorLibrary(lua_State* L);
	void RegisterVector2DLibrary(lua_State* L);
	void RegisterTransformLibrary(lua_State* L);
	void RegisterRotatorLibrary(lua_State* L);
	void RegisterQuatLibrary(lua_State* L);
	void RegisterColorLibrary(lua_State* L);
	void RegisterLinearColorLibrary(lua_State* L);
	void RegisterTextFormatterLibrary(lua_State* L);
	void RegisterDateTimeLibrary(lua_State* L);
	void RegisterTimespanLibrary(lua_State* L);
	void RegisterQualifiedFrameTimeLibrary(lua_State* L);
	void RegisterPointerEventLibrary(lua_State* L);
};