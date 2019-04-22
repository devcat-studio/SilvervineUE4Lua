// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "CoreMinimal.h"


struct lua_State;

//
// lua와 UObject를 주고받을 때 사용합니다.
//
namespace SUE4LuaUObject
{
	// FSUE4LuaVirtualMachine이 생성될 때 호출됩니다.
	void Register(lua_State* L);

	// UObject를 lua 스택에 푸시합니다.
	void PushRef(lua_State* L, UObject* TargetUObject);
	// UObject를 lua 스택에서 팝합니다.
	UObject* PopRef(lua_State* L);
	// UObject를 lua 스택에서 팝 하지 않고 가져옵니다.
	UObject* ToRef(lua_State* L, int32 Index);
	// ToRef()와 비슷하지만 실패해도 조용히 넘어갑니다.
	UObject* TryToRef(lua_State* L, int32 Index);
}