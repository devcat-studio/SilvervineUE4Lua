// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "Bindings/LuaActorBinding.h"

#include "LuaLog.h"


UClass* USUE4LuaActorBinding::GetBindingClass() const
{
	return AActor::StaticClass();
}

FString USUE4LuaActorBinding::LuaStaticGetDebugName(const AActor* Actor)
{
	return AActor::GetDebugName(Actor);
}