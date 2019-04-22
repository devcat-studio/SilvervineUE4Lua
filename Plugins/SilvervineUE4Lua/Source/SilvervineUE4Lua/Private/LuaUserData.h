// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "UObject/GCObject.h"


class FSUE4LuaVirtualMachine;
struct lua_State;

//
// lua의 fulluserdata를 생성하고 gc관리자에 등록하는 클래스
//
class FSUE4LuaUserData
{
public:
	FSUE4LuaUserData();
	virtual ~FSUE4LuaUserData();

	template<typename UserDataType, typename ...ArgTypes>
	static UserDataType* NewUserData(lua_State* L, ArgTypes&&... Args);

	template<typename UserDataType>
	static UserDataType* ToUserData(lua_State* L, int32 StackIndex);

	// userdata가 lua에서 gc될 때 반드시 호출해줘야 합니다.
	void Dispose();

private:
	static void* LuaNewUserData(lua_State* L, int32 Size);
	static void* LuaToUserData(lua_State* L, int32 StackIndex);

	void Initialize(lua_State* L);

	bool bDisposed = false;
	FSUE4LuaVirtualMachine* OwnerVM = nullptr;
};

template<typename UserDataType, typename ...ArgTypes>
UserDataType* FSUE4LuaUserData::NewUserData(lua_State* L, ArgTypes&&... Args)
{
	// 여기서 컴파일 에러가 난다면 UserDataType이 FSUE4LuaUserData의 파생 클래스인지 확인할 것
	FSUE4LuaUserData* UserData = new (LuaNewUserData(L, sizeof(UserDataType))) UserDataType(Forward<ArgTypes>(Args)...);

	UserData->Initialize(L);

	return static_cast<UserDataType*>(UserData);
}

template<typename UserDataType>
UserDataType* FSUE4LuaUserData::ToUserData(lua_State* L, int32 StackIndex)
{
	// 여기서 컴파일 에러가 난다면 UserDataType이 FSUE4LuaUserData의 파생 클래스인지 확인할 것
	return static_cast<UserDataType*>(reinterpret_cast<FSUE4LuaUserData*>(LuaToUserData(L, StackIndex)));
}