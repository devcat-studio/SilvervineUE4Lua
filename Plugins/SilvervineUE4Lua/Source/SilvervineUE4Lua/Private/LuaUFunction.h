// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "CoreUObject.h"


struct lua_State;

//
// lua에서 UFunction을 호출할 때 사용.
//
namespace SUE4LuaUFunction
{
	// pass-by-name 방식으로 함수를 호출할 때 사용하는 파라메터 테이블인지 조사합니다.
	bool IsParameterTable(lua_State* L, int32 StackIndex);

	// UFunction을 호출하는 lua 클로저를 푸시합니다.
	void PushClosure(lua_State* L, const UFunction* Function);

	// lua 스택의 파라메터를 사용해서 UFunction을 호출합니다.
	//
	// 함수 전달 방식은 pass-by-positions과 pass-by-names를 지원합니다.
	// 예:	-- void Foo(int32 P1, int32 P2) {}
	//		Foo(1, 2)				-- pass-by-positions
	//		Foo({ P1 = 1, P2 = 2})	-- pass-by-names 
	//
	// 함수의 반환값과 출력 파라메터(non const referenced type)가 lua 스택에 반환됩니다.
	// 예:	-- int32 Foo(const int32& P1, int32& Out1, int32& Out2)
	//		local Ret, Out1, Out2 = Foo(1)
	int32 CallFunction(lua_State* L, const UFunction* Function);
}