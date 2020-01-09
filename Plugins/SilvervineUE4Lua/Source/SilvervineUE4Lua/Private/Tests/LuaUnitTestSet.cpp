// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#include "LuaUnitTests.h"

#include "LuaVirtualMachine.h"


//=============================================================================================================================
// FSUE4LuaTestCaseSet
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseSet, "Silvervine.UE4Lua.Set", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseSet::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}

	auto TestUObject = NewObject<USUE4LuaTestUObject>();

	{
		TestUObject->IntSetProperty.Empty();
		TestUObject->IntSetProperty.Add(1);

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		return uobj.IntSetProperty:Num()")
			TEXT("\n	end"));
		TestTrue(TEXT("TSet.Num()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject) == TestUObject->IntSetProperty.Num());
	}
	{
		TestUObject->IntSetProperty.Empty();
		TestUObject->IntSetProperty.Add(1);

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		uobj.IntSetProperty:Empty()")
			TEXT("\n	end"));
		FSUE4LuaFunction::CallGlobal<void>(VM.ToSharedRef(), TEXT("Test"), TestUObject);
		TestTrue(TEXT("TSet.Empty()"), TestUObject->IntSetProperty.Num() == 0);
	}
	{
		int32 TestValue = 123;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.IntSetProperty:Add(value)")
			TEXT("\n		return uobj.IntSetProperty[value] ~= nil")
			TEXT("\n	end"));
		TestTrue(TEXT("TSet<int32>.Add()"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestValue));

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.IntSetProperty:Remove(value)")
			TEXT("\n		return uobj.IntSetProperty[value] == nil")
			TEXT("\n	end"));
		TestTrue(TEXT("TSet<int32>.Remove()"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestValue));
	}
	{
		int32 TestValue = 123;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.IntSetProperty:Add(value)")
			TEXT("\n		uobj.IntSetProperty[value] = nil")
			TEXT("\n		return uobj.IntSetProperty[value] == nil")
			TEXT("\n	end"));
		TestTrue(TEXT("TSet<int32>[key] = nil"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestValue));
	}
	{
		TestUObject->IntSetProperty.Empty();
		int32 TestValue = 0;
		for (int32 i = 1; i <= 10; ++i)
		{
			TestUObject->IntSetProperty.Add(i);
			TestValue += i;
		}

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		local sum = 0")
			TEXT("\n		for _, v in pairs(uobj.IntSetProperty) do")
			TEXT("\n			sum = sum + v")
			TEXT("\n		end")
			TEXT("\n		return sum")
			TEXT("\n	end"));
		TestTrue(TEXT("TSet<int32> pairs()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject) == TestValue);
	}

	return true;
}
