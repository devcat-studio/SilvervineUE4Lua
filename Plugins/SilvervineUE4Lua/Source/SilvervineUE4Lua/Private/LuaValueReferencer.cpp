// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaValueReferencer.h"

#include "LuaAPI.h"
#include "LuaLog.h"
#include "LuaVirtualMachine.h"


FSUE4LuaValueReferencer::FSUE4LuaValueReferencer(lua_State* L, int32 StackIndex)
{
	if (auto VM = FSUE4LuaVirtualMachine::PinVM(L))
	{
		LuaMainThread = VM->GetLuaMainThread();

		// 음수 StackIndex를 처리하기 위해서 일단 스택에 푸시
		lua_pushvalue(L, StackIndex);
		// Stack: Value

		// SUE4LuaBinding.AddReference(this, Value)
		{
			// Stack: func
			lua_getglobal(L, "SUE4LuaBinding");
			lua_getfield(L, -1, "AddReference");
			lua_remove(L, -2);
			// Stack: Value, AddReference

			lua_pushlightuserdata(L, this);
			lua_pushvalue(L, -3);
			// Stack: Value, AddReference, this, Value

			lua_pcall(L, 2, 0, 0);
			// Stack: Value
		}

		// Stack: Value
		lua_pop(L, 1);
		// Stack: (empty)
	}
}

FSUE4LuaValueReferencer::~FSUE4LuaValueReferencer()
{
	if (auto VM = PinVM())
	{
		auto L = VM->GetCurrentLuaState();

		// SUE4LuaBinding.RemoveReference(this)
		{
			lua_getglobal(L, "SUE4LuaBinding");
			lua_getfield(L, -1, "RemoveReference");
			lua_remove(L, -2);
			// Stack: RemoveReference

			lua_pushlightuserdata(L, this);
			// Stack: RemoveReference, this
			lua_pcall(L, 1, 0, 0);
			// Stack: (empty)
		}
	}
}

TSharedPtr<FSUE4LuaVirtualMachine> FSUE4LuaValueReferencer::PinVM() const
{
	if (LuaMainThread.IsValid())
	{
		return LuaMainThread->PinVM();
	}

	return nullptr;
}

void FSUE4LuaValueReferencer::Push(lua_State* L)
{
	// SUE4LuaBinding.GetlReferencedObj(LuaFunction*)
	{
		lua_getglobal(L, "SUE4LuaBinding");
		lua_getfield(L, -1, "GetReferenced");
		lua_remove(L, -2);

		lua_pushlightuserdata(L, this);

		lua_pcall(L, 1, 1, 0);
	}
}
