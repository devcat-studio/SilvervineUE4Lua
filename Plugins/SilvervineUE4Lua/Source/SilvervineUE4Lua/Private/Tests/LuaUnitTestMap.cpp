// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaUnitTests.h"

#include "LuaVirtualMachine.h"


//=============================================================================================================================
// FSUE4LuaTestCaseMap
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseMap, "Silvervine.UE4Lua.Map", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseMap::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}

	auto TestUObject = NewObject<USUE4LuaTestUObject>();

	{
		TestUObject->IntMapProperty.Empty();
		TestUObject->IntMapProperty.Add(1, 1);

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		return uobj.IntMapProperty:Num()")
			TEXT("\n	end"));
		TestTrue(TEXT("TMap.Num()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject) == TestUObject->IntMapProperty.Num());
	}
	{
		TestUObject->IntMapProperty.Empty();
		TestUObject->IntMapProperty.Add(1, 1);

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		uobj.IntMapProperty:Empty()")
			TEXT("\n	end"));
		FSUE4LuaFunction::CallGlobal<void>(VM.ToSharedRef(), TEXT("Test"), TestUObject);
		TestTrue(TEXT("TMap.Empty()"), TestUObject->IntMapProperty.Num() == 0);
	}
	{
		TestUObject->IntMapProperty.Empty();

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		uobj.IntMapProperty:Add(123)")
			TEXT("\n		return uobj.IntMapProperty[123]")
			TEXT("\n	end"));
		TestTrue(TEXT("TMap<int32,int32>.Add()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject) == 0);
	}
	{
		int32 TestKey = 3;
		int32 TestValue = 123;
		TestUObject->IntMapProperty.Add(TestKey);

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, key, value)")
			TEXT("\n		uobj.IntMapProperty[key] = value")
			TEXT("\n		return uobj.IntMapProperty[key]")
			TEXT("\n	end"));
		TestTrue(TEXT("TMap<int32,int32>.Get()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestKey, TestValue) == TestValue);
		TestTrue(TEXT("TMap<int32,int32>.Set()"), TestUObject->IntMapProperty[TestKey] == TestValue);
	}
	{
		int32 TestKey = 3;
		FSUE4LuaTestStruct TestValue;
		TestValue.IntProperty = 123;
		TestUObject->StructMapProperty.Add(TestKey);

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, key, value)")
			TEXT("\n		uobj.StructMapProperty:Add(key, value)")
			TEXT("\n		return uobj.StructMapProperty:FindCopy(key)")
			TEXT("\n	end"));
		TestTrue(TEXT("TMap<int32,UStruct>.Add() and FindCopy()"), FSUE4LuaFunction::CallGlobal<decltype(TestValue)>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestKey, TestValue).IntProperty == TestValue.IntProperty);
	}
	{
		FString TestKey(TEXT("TestKey"));
		int32 TestValue = 123;
		TestUObject->StringKeyMapProperty.Add(TestKey);

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, key, value)")
			TEXT("\n		uobj.StringKeyMapProperty[key] = value")
			TEXT("\n		return uobj.StringKeyMapProperty[key]")
			TEXT("\n	end"));
		TestTrue(TEXT("TMap<UString,int32>.Get()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestKey, TestValue) == TestValue);
		TestTrue(TEXT("TMap<UString,int32>.Set()"), TestUObject->StringKeyMapProperty[TestKey] == TestValue);
	}
	{
		int32 TestKey = 4;
		int32 TestValue = 456;
		TestUObject->IntMapProperty.Add(TestKey, TestValue);

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, key)")
			TEXT("\n		uobj.IntMapProperty:Remove(key)")
			TEXT("\n		return uobj.IntMapProperty[key]")
			TEXT("\n	end"));
		TestTrue(TEXT("TMap<int32,int32>.Remove()"),
			FSUE4LuaFunction::CallGlobal<FSUE4LuaStack::FNil>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestKey) == FSUE4LuaStack::Nil &&
			TestUObject->IntMapProperty.Find(TestKey) == nullptr);
	}
	{
		int32 TestKey = 4;
		int32 TestValue = 456;
		TestUObject->IntMapProperty.Add(TestKey, TestValue);

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, key)")
			TEXT("\n		uobj.IntMapProperty[key] = nil")
			TEXT("\n		return uobj.IntMapProperty[key]")
			TEXT("\n	end"));
		TestTrue(TEXT("TMap<int32,int32>[key] = nil"), 
			FSUE4LuaFunction::CallGlobal<FSUE4LuaStack::FNil>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestKey) == FSUE4LuaStack::Nil &&
			TestUObject->IntMapProperty.Find(TestKey) == nullptr);
	}
	{
		TestUObject->IntMapProperty.Empty();
		int32 TestValue = 0;
		for (int32 i = 1; i <= 10; ++i)
		{
			TestUObject->IntMapProperty.Add(i, i);
			TestValue += i * i;
		}

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		local sum = 0")
			TEXT("\n		for k, v in pairs(uobj.IntMapProperty) do")
			TEXT("\n			sum = sum + k * v")
			TEXT("\n		end")
			TEXT("\n		return sum")
			TEXT("\n	end"));
		TestTrue(TEXT("TMap<int32,int32> pairs()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject) == TestValue);
	}
	{
		int32 TestKey = 3;
		FSUE4LuaTestStruct TestValue;
		TestValue.IntProperty = 123;
		TestUObject->StructMapProperty.Add(TestKey, TestValue);

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, key)")
			TEXT("\n		local value = uobj.StructMapProperty:FindCopy(key)")
			TEXT("\n		uobj.StructMapProperty:Add(key)")
			TEXT("\n		return value.IntProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("TMap<int32,UStruct>.FindCopy()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestKey) == TestValue.IntProperty);
	}
	{
		int32 TestKey = 3;
		FSUE4LuaTestStruct TestValue;
		TestValue.IntProperty = 123;
		TestUObject->StructMapProperty.Add(TestKey, TestValue);

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, key)")
			TEXT("\n		local ref = uobj.StructMapProperty:FindRef(key)")
			TEXT("\n		uobj.StructMapProperty:Add(key)")
			TEXT("\n		return ref.IntProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("TMap<int32, UStruct>.FindRef()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestKey) != TestValue.IntProperty);
	}

	return true;
}
