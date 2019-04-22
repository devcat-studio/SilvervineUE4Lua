// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "CoreUObject.h"


struct lua_State;

//
// lua_pcall()을 간편하게 쓰기 위한 유틸리티
//
namespace SUE4LuaCall
{
	// lua_pcall()을 사용해서 푸시된 lua 함수를 실행합니다. lua 스택에 함수와 인자를 푸시한 상태로 PCall()을 호출하세요.
	// 함수 호출이 성공하면 true, 실패하면 false를 반환합니다.
	bool PCall(lua_State* L, int32 ArgCount, int32 RetCount);
}