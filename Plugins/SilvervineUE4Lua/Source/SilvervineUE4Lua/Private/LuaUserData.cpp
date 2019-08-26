// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaUserData.h"

#include "LuaAPI.h"
#include "LuaLog.h"
#include "LuaThread.h"
#include "LuaVirtualMachine.h"


FSUE4LuaUserData::FSUE4LuaUserData()
{	
}

FSUE4LuaUserData::~FSUE4LuaUserData()
{
	if (!bDisposed)
	{
		if (OwnerVM != nullptr)
		{
			OwnerVM->RemoveUserData(this);
		}

		bDisposed = true;
	}
}

void FSUE4LuaUserData::Initialize(lua_State* L)
{
	if (auto VM = FSUE4LuaVirtualMachine::PinVM(L))
	{
		OwnerVM = VM.Get();
		OwnerVM->AddUserData(this);
	}

	UE_CLOG(OwnerVM == nullptr, LogSUE4L, Error, TEXT("Invalid OwnerVM. [%s]"), __SUE4LUA_FUNCTION__);
}

void FSUE4LuaUserData::Dispose()
{
	if (bDisposed)
	{
		return;
	}

	this->~FSUE4LuaUserData();
}

void* FSUE4LuaUserData::LuaNewUserData(lua_State* L, int32 Size)
{
	return lua_newuserdata(L, Size);
}

void* FSUE4LuaUserData::LuaToUserData(lua_State* L, int32 StackIndex)
{
	return lua_touserdata(L, StackIndex);
}