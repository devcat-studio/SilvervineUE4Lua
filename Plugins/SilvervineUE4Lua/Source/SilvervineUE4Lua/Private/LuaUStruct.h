// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "CoreUObject.h"

#include "LuaNativeValue.h"


struct lua_State;

//
// lua와 UStruct를 주고받을 때 사용합니다.
//
namespace SUE4LuaUStruct
{
	// FSUE4LuaVirtualMachine이 생성될 때 호출됩니다.
	void Register(lua_State* L);

	// UStruct 구조체인지 조사
	bool IsUStruct(lua_State* L, int32 StackIndex);

	// lua 스택에서 UStruct 구조체에 대한 참조(메모리 주소)를 가져옵니다.
	void* GetRef(lua_State* L, int32 StackIndex, const UScriptStruct* Struct);

	// lua 스택에 UStruct 구조체의 사본을 푸시합니다.
	// ValueAddress가 null이면 기본값으로 초기화합니다.
	void PushValue(lua_State* L, const UScriptStruct* Struct, const void* ValueAddress = nullptr);
	// lua 스택에 UStruct 구조체의 참조를 푸시합니다.
	void PushRef(lua_State* L, const UScriptStruct* Struct, const void* ValueAddress, FSUE4LuaNativeValue* Owner);

	// lua 스택에 있는 값을 해당 주소로 복사합니다.
	bool CopyToNative(lua_State* L, int32 StackIndex, const UScriptStruct* DestStruct, void* DestValueAddress);
	// 해당 주소의 값을 lua 스택으로 복사합니다. 실패할 경우 조용히 넘어갑니다.
	bool TryCopyToLua(lua_State* L, int32 StackIndex, const UScriptStruct* SrcStruct, const void* SrcValueAddress);
}