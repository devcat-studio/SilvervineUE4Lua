// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#include "SilvervineUE4LuaVirtualMachine.h"

#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/World.h"
#include "Misc/CoreDelegates.h"

#include "LuaAPI.h"
#include "LuaFunction.h"
#include "LuaLog.h"


FSUE4LuaVMInstance* USUE4LuaVirtualMachine::GetEditorVMInstance()
{
#if WITH_EDITOR
	if (!EditorVMInstance.VM.IsValid())
	{
		EditorVMInstance.VM = FSUE4LuaVirtualMachine::Create();
		if (ensure(EditorVMInstance.VM.IsValid()))
		{
			EditorVMInstance.VM->SetWorldContextObject(this);

			if (!StartupFilePath.IsEmpty())
			{
				EditorVMInstance.VM->ExecuteFile(*StartupFilePath);
			}

			FCoreDelegates::OnPreExit.AddUObject(this, &USUE4LuaVirtualMachine::OnPreExit);
		}
	}

	return &EditorVMInstance;
#else
	UE_LOG(LogSUE4L, Error, TEXT("EditorVM is not valid in non editor build. [%s]"), __SUE4LUA_FUNCTION__);
	return nullptr;
#endif
}

void USUE4LuaVirtualMachine::OnPreExit()
{
	if (EditorVMInstance.VM.IsValid())
	{
		EditorVMInstance.VM->Dispose();
		EditorVMInstance.VM.Reset();
	}
}

TSharedPtr<FSUE4LuaVirtualMachine> USUE4LuaVirtualMachine::RegisterGameInstance(UGameInstance* GameInstance)
{
	if (GameInstance == nullptr)
	{
		UE_LOG(LogSUE4L, Error, TEXT("Invalid GameInstance. [%s]"), __SUE4LUA_FUNCTION__);
		return nullptr;
	}

	for (auto& GameVMInstance : GameVMInstances)
	{
		if (GameVMInstance.GameInstance == GameInstance)
		{
			UE_LOG(LogSUE4L, Warning, TEXT("Already registered GameInstance: %s. [%s]"), *GameInstance->GetName(), __SUE4LUA_FUNCTION__);
			return GameVMInstance.VM;
		}
	}

	FSUE4LuaVMInstance NewGameVMInstance;
	NewGameVMInstance.GameInstance = GameInstance;
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	NewGameVMInstance.VM = FSUE4LuaVirtualMachine::Create(true);
#else
	NewGameVMInstance.VM = FSUE4LuaVirtualMachine::Create();
#endif
	if (!NewGameVMInstance.VM.IsValid())
	{
		UE_LOG(LogSUE4L, Error, TEXT("GameInstance VM was not properly initialized. [%s]"), __SUE4LUA_FUNCTION__);
		return nullptr;
	}

	NewGameVMInstance.VM->SetWorldContextObject(GameInstance);

	if (!StartupFilePath.IsEmpty())
	{
		NewGameVMInstance.VM->ExecuteFile(*StartupFilePath);
	}

	GameVMInstances.Add(NewGameVMInstance);

	return NewGameVMInstance.VM;
}

bool USUE4LuaVirtualMachine::UnregisterGameInstance(UGameInstance* GameInstance)
{
	if (GameInstance == nullptr)
	{
		UE_LOG(LogSUE4L, Error, TEXT("Invalid GameInstance. [%s]"), __SUE4LUA_FUNCTION__);
		return false;
	}

	for (int32 Index = 0; Index < GameVMInstances.Num(); ++Index)
	{
		auto& GameVMInstance = GameVMInstances[Index];

		if (GameVMInstance.GameInstance == GameInstance)
		{
			GameVMInstance.VM->Dispose();
			GameVMInstances.RemoveAt(Index);

			return true;
		}
	}

	UE_LOG(LogSUE4L, Error, TEXT("Unregistered GameInstance: %s. [%s]"), *GameInstance->GetName(), __SUE4LUA_FUNCTION__);
	
	return false;
}

FSUE4LuaVirtualMachine* USUE4LuaVirtualMachine::FindVMFromWorldContext(UObject* WorldContext)
{
	auto VMInstance = FindVMInstanceFromWorldContext(WorldContext);

	return VMInstance != nullptr ? VMInstance->VM.Get() : nullptr;
}

FSUE4LuaVirtualMachine* USUE4LuaVirtualMachine::FindVMFromGameInstance(UGameInstance* GameInstance)
{
	auto VMInstance = FindVMInstanceFromGameInstance(GameInstance);

	return VMInstance != nullptr ? VMInstance->VM.Get() : nullptr;
}

FSUE4LuaVMInstance* USUE4LuaVirtualMachine::FindVMInstanceFromWorldContext(UObject* WorldContext)
{
	if (WorldContext != nullptr)
	{
		if (auto World = WorldContext->GetWorld())
		{
			if (auto GameInstance = World->GetGameInstance())
			{
				return FindVMInstanceFromGameInstance(GameInstance);
			}

			return GetEditorVMInstance();
		}
	}

	return nullptr;
}

FSUE4LuaVMInstance* USUE4LuaVirtualMachine::FindVMInstanceFromGameInstance(UGameInstance* GameInstance)
{
	if (GameInstance != nullptr)
	{
		for (auto& GameVMInstance : GameVMInstances)
		{
			if (GameVMInstance.GameInstance == GameInstance)
			{
				return &GameVMInstance;
			}
		}
	}

	return nullptr;
}

bool USUE4LuaVirtualMachine::BlueprintRegisterGameInstance(UGameInstance* GameInstance)
{
	return RegisterGameInstance(GameInstance).IsValid();
}

bool USUE4LuaVirtualMachine::BlueprintUnregisterGameInstance(UGameInstance* GameInstance)
{
	return UnregisterGameInstance(GameInstance);
}