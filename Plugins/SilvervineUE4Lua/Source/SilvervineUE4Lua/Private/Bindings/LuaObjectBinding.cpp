// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "Bindings/LuaObjectBinding.h"

#include "LuaLog.h"


UClass* USUE4LuaObjectBinding::GetBindingClass() const
{
	return UObject::StaticClass();
}

FString USUE4LuaObjectBinding::LuaGetName(UObject* InSelf)
{
	return InSelf->GetName();
}

UClass* USUE4LuaObjectBinding::LuaGetClass(UObject* InSelf)
{
	return InSelf->GetClass();
}

bool USUE4LuaObjectBinding::LuaIsA(UObject* InSelf, FName ClassName)
{
	UClass* Class = FindObject<UClass>(ANY_PACKAGE, *ClassName.ToString());
	if (Class != nullptr)
	{
		return InSelf->IsA(Class);
	}
	else
	{
		return false;
	}
}

UWorld* USUE4LuaObjectBinding::LuaGetWorld(UObject* InSelf)
{
	return InSelf->GetWorld();
}