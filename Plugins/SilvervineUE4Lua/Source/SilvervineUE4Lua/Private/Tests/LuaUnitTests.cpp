// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaUnitTests.h"

#include "SilvervineUE4LuaBridge.h"


//=============================================================================================================================
// USUE4LuaTestUObject
//=============================================================================================================================
int32 USUE4LuaTestUObject::TestArgs(int32 InIntArg1, int32 InIntArg2, const int32& InIntArg3, int32& OutIntArg1, int32& OutIntArg2)
{
	OutIntArg1 = InIntArg1 + InIntArg2;
	OutIntArg2 = InIntArg3;

	return OutIntArg1 + OutIntArg2;
}

int32 USUE4LuaTestUObject::TestDefaultArgs(int32 InIntArg1, int32 InIntArg2)
{
	return InIntArg1 + InIntArg2;
}

FVector USUE4LuaTestUObject::TestVector(const FVector& InArg)
{
	return InArg;
}

ESUE4LuaTestEnum USUE4LuaTestUObject::TestEnum(ESUE4LuaTestEnum InArg)
{
	return InArg;
}

FTransform USUE4LuaTestUObject::TestTransform(const FTransform& InArg)
{
	return InArg;
}

void USUE4LuaTestUObject::TestStruct(FSUE4LuaTestStruct InArg1, const FSUE4LuaTestStruct& InArg2, FSUE4LuaTestStruct& OutArg1)
{
	OutArg1.IntProperty = InArg1.IntProperty + InArg2.IntProperty;
}

int32 USUE4LuaTestUObject::TestLuaValueArg(USUE4LuaValue* InArg)
{
	if (InArg != nullptr)
	{
		return InArg->GetInteger();
	}

	return 0;
}

void USUE4LuaTestUObject::TestArrayArg(TArray<UObject*>& OutArg, UObject* InArg)
{
	OutArg.Add(InArg);
}

void USUE4LuaTestUObject::TestNativeDispatch(FSUE4LuaTestNestedStruct InParam1, const FSUE4LuaTestNestedStruct& InParam2, FSUE4LuaTestNestedStruct& OutParam3)
{
	SUE4LUA_DISPATCH(LuaBridge, InParam1, InParam2, OutParam3);
}

int32 USUE4LuaTestUObject::TestCoroutine(int32 InArg)
{
	if (ValueProperty != nullptr && ValueProperty->IsFunctionType())
	{
		return ValueProperty->Call<int32>(InArg);
	}

	return 0;
}

void USUE4LuaTestUObject::TestTimer()
{
}

//=============================================================================================================================
// USUE4TestBinding
//=============================================================================================================================
UClass* USUE4TestBinding::GetBindingClass() const
{
	return BindingClass;
}

void USUE4TestBinding::SetBindingClass(UClass* InClass)
{
	BindingClass = InClass;
}