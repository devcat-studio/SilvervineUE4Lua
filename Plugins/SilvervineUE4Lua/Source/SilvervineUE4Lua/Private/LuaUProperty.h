// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "CoreUObject.h"

#include "LuaFunction.h"
#include "LuaNativeValue.h"


struct lua_State;

//
// lua에서 UProperty를 통해 c++의 객체에 접근할 때 사용합니다.
//
namespace SUE4LuaUProperty
{
	// FSUE4LuaVirtualMachine이 생성될 때 호출됩니다.
	void Register(lua_State* L);

	// 고정 크기 배열(int32[] 등)인지 조사
	bool IsFixedArray(lua_State* L, int32 StackIndex);
	// TArray 인지 조사
	bool IsArray(lua_State* L, int32 StackIndex);
	// TMap 인지 조사
	bool IsMap(lua_State* L, int32 StackIndex);
	// TSet 인지 조사
	bool IsSet(lua_State* L, int32 StackIndex);

	// 값타입(int32, FVector, ...)은 항상 사본이 푸시됩니다.
	// UObject는 포인터로 처리되므로 값타입과 같이 사본이 푸시됩니다.
	// 그 외의 타입은 Owner가 null이 아니면 참조를, null이면 사본값이 생성된 후에 그 참조가 푸시됩니다.
	void Push(lua_State* L, const UProperty* Property, const void* ValueAddress, FSUE4LuaNativeValue* Owner);

	// lua 스택에 있는 값을 해당 주소로 복사합니다.	
	bool CopyToNative(lua_State* L, int32 StackIndex, const UProperty* DestProperty, void* DestValueAddress);
	// 해당 주소의 값을 lua 스택으로 복사합니다. 실패할 경우 조용히 넘어갑니다.
	bool TryCopyToLua(lua_State* L, int32 StackIndex, const UProperty* SrcProperty, const void* SrcValueAddress);
}