// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaSceneComponentBinding.h"

#include "LuaLog.h"


UClass* USUE4LuaSceneComponentBinding::GetBindingClass() const
{
	return USceneComponent::StaticClass();
}

void USUE4LuaSceneComponentBinding::LuaSetupAttachment(USceneComponent* InSelf, USceneComponent* InParent, FName InSocketName)
{
	InSelf->SetupAttachment(InParent, InSocketName);
}