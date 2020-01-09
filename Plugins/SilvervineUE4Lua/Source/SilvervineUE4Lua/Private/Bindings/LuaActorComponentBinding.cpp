// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "Bindings/LuaActorComponentBinding.h"

#include "LuaLog.h"


UClass* USUE4LuaActorComponentBinding::GetBindingClass() const
{
	return UActorComponent::StaticClass();
}

void USUE4LuaActorComponentBinding::LuaRegisterComponent(UActorComponent* InSelf)
{
	InSelf->RegisterComponent();
}