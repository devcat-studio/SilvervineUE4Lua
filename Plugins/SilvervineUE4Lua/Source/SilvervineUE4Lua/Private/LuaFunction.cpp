// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#include "LuaFunction.h"

#include "LuaAPI.h"
#include "LuaCall.h"
#include "LuaLog.h"


FSUE4LuaFunction::FSUE4LuaFunction(TSharedRef<FSUE4LuaThread> InLuaMainThread)
	: LuaMainThread(InLuaMainThread)
{
}

FSUE4LuaFunction::~FSUE4LuaFunction()
{
}

TSharedPtr<FSUE4LuaFunction> FSUE4LuaFunction::CreateFromGlobal(TSharedRef<FSUE4LuaVirtualMachine> LuaVM, const TCHAR* FunctionName)
{
	if (LuaVM->IsDisposed())
	{
		UE_LOG(LogSUE4L, Error, TEXT("VM was disposed. [%s]"), __SUE4LUA_FUNCTION__);
		return nullptr;
	}
	if (FunctionName == nullptr)
	{
		UE_LOG(LogSUE4L, Error, TEXT("Invalid function name. [%s]"), __SUE4LUA_FUNCTION__);
		return nullptr;
	}

	auto RetLuaFunction = TSharedPtr<FSUE4LuaFunction>(new FSUE4LuaFunction(LuaVM->GetLuaMainThread().ToSharedRef()));
	auto L = LuaVM->GetCurrentLuaState();

	lua_getglobal(L, TCHAR_TO_UTF8(FunctionName));
	// Stack: TargetFunc

	if (lua_isfunction(L, -1))
	{
		RetLuaFunction->FunctionReferencer.Reset(new FSUE4LuaValueReferencer(L, -1));
	}

	lua_pop(L, 1);
	// Stack: (empty)

	if (!RetLuaFunction->FunctionReferencer.IsValid())
	{
		UE_LOG(LogSUE4L, Error, TEXT("Global lua function '%s' doesn't exist. [%s]"), FunctionName, __SUE4LUA_FUNCTION__);
		return nullptr;
	}

	return RetLuaFunction;
}

TSharedPtr<FSUE4LuaFunction> FSUE4LuaFunction::CreateFromStack(lua_State* L, int32 Index)
{
	if (lua_isnil(L, Index))
	{
		return nullptr;
	}

	if (!lua_isfunction(L, Index))
	{
		SUE4LVM_ERROR(L, TEXT("Target lua value is not a function"));
		return nullptr;
	}

	if (auto VM = FSUE4LuaVirtualMachine::PinVM(L))
	{
		auto RetLuaFunction = TSharedPtr<FSUE4LuaFunction>(new FSUE4LuaFunction(VM->GetLuaMainThread().ToSharedRef()));

		RetLuaFunction->FunctionReferencer.Reset(new FSUE4LuaValueReferencer(L, Index));

		return RetLuaFunction;
	}

	return nullptr;
}

bool FSUE4LuaFunction::IsSame(TSharedPtr<FSUE4LuaFunction> Function) const
{
	if (!Function.IsValid())
	{
		return false;
	}
	if (LuaMainThread != Function->LuaMainThread)
	{
		return false;
	}

	if (auto VM = LuaMainThread->PinVM())
	{
		auto L = VM->GetCurrentLuaState();

		FunctionReferencer->Push(L);
		Function->FunctionReferencer->Push(L);
		// Stack: Function1, Function2

		bool bSame = !!lua_compare(L, -1, -2, LUA_OPEQ);
		lua_pop(L, 2);
		// Stack: (empty)

		return bSame;
	}

	return false;
}

TSharedPtr<FSUE4LuaVirtualMachine> FSUE4LuaFunction::PinVM() const
{
	return LuaMainThread->PinVM();
}

void FSUE4LuaFunction::Push(lua_State* L)
{
	FunctionReferencer->Push(L);
}

bool FSUE4LuaFunction::CallInternal(lua_State* L, int32 ArgCount, int32 RetCount)
{
	return SUE4LuaCall::PCall(L, ArgCount, RetCount);
}

