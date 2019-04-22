// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaNativeValue.h"


//=============================================================================================================================
// FSUE4LuaNativeValue
//=============================================================================================================================
int32 FSUE4LuaNativeValue::InstanceCount = 0;

FSUE4LuaNativeValue::FSUE4LuaNativeValue()
{
	++InstanceCount;
}

FSUE4LuaNativeValue::~FSUE4LuaNativeValue()
{
	--InstanceCount;
}

FSUE4LuaUObjectValue::FSUE4LuaUObjectValue(UObject* InTargetUObject)
{
	TargetUObject = InTargetUObject;
}

void* FSUE4LuaUObjectValue::GetValueAddress()
{
	if (auto TargetUObjectPtr = TargetUObject.Get())
	{
		return TargetUObjectPtr;
	}

	return nullptr;
}

//=============================================================================================================================
// FSUE4LuaUStructValue
//=============================================================================================================================
FSUE4LuaUStructValue::FSUE4LuaUStructValue(const UStruct* InStruct)
	: StructInstance(InStruct)
{
}

void* FSUE4LuaUStructValue::GetValueAddress()
{
	if (StructInstance.IsValid())
	{
		return StructInstance.GetStructMemory();
	}

	return nullptr;
}

//=============================================================================================================================
// FSUE4LuaUPropertyValue
//=============================================================================================================================
FSUE4LuaUPropertyValue::FSUE4LuaUPropertyValue(const UProperty* InProperty)
	: Property(InProperty)
{
	Buffer = FMemory::Malloc(Property->GetSize(), Property->GetMinAlignment());
	Property->InitializeValue(Buffer);
}

FSUE4LuaUPropertyValue::~FSUE4LuaUPropertyValue()
{
	Property->DestroyValue(Buffer);
	FMemory::Free(Buffer);
	Buffer = nullptr;
}

void* FSUE4LuaUPropertyValue::GetValueAddress()
{
	return Buffer;
}