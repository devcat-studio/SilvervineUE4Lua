// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "Bindings/LuaClassBinding.h"

#include "LuaLog.h"


UClass* USUE4LuaClassBinding::GetBindingClass() const
{
	return UClass::StaticClass();
}

UObject* USUE4LuaClassBinding::LuaGetDefaultObject(UClass* InSelf)
{
	return InSelf->GetDefaultObject();
}