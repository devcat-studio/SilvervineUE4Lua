// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#include "LuaThread.h"

#include "LuaAPI.h"
#include "LuaLog.h"
#include "LuaVirtualMachine.h"


FSUE4LuaThread::FSUE4LuaThread(TSharedRef<FSUE4LuaVirtualMachine> InVM, lua_State* InLuaState)
	: WeakVM(InVM)
	, LuaState(InLuaState)
{
	check(LuaState != nullptr);
}

FSUE4LuaThread::~FSUE4LuaThread()
{
	LuaState = nullptr;
}

TSharedPtr<FSUE4LuaThread> FSUE4LuaThread::Create(TSharedRef<FSUE4LuaVirtualMachine> VM, lua_State* LuaState)
{
	if (VM->IsDisposed())
	{
		UE_LOG(LogSUE4L, Warning, TEXT("Disposed VM is passed. [%s]"), __SUE4LUA_FUNCTION__);
		return nullptr;
	}
	if (LuaState == nullptr)
	{
		UE_LOG(LogSUE4L, Warning, TEXT("Invalid lua state. [%s]"), __SUE4LUA_FUNCTION__);
		return nullptr;
	}

	auto LuaThread = TSharedPtr<FSUE4LuaThread>(new FSUE4LuaThread(VM, LuaState));

	return LuaThread;
}

void FSUE4LuaThread::SetDisposed()
{
	if (LuaState == nullptr)
	{
		UE_LOG(LogSUE4L, Warning, TEXT("Already disposed. [%s]"), __SUE4LUA_FUNCTION__);
		return;
	}

	LuaState = nullptr;
}

TSharedPtr<FSUE4LuaVirtualMachine> FSUE4LuaThread::PinVM() const
{
	auto VM = WeakVM.Pin();

	if (VM.IsValid() && !VM->IsDisposed())
	{
		return VM;
	}

	return nullptr;
}