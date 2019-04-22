// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaUnitTests.h"

#include "LuaVirtualMachine.h"


//=============================================================================================================================
// FSUE4LuaTestCaseCoroutine
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseCoroutine, "Silvervine.UE4Lua.Coroutine", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseCoroutine::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}
	
	auto TestUObject = NewObject<USUE4LuaTestUObject>();

	{
		int32 TestValue = 123;
		TestUObject->ValueProperty = nullptr;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, arg)")
			TEXT("\n		local co = coroutine.create(function ()")
			TEXT("\n			return uobj:TestCoroutine(arg)")
			TEXT("\n		end)")
			TEXT("\n		uobj.ValueProperty = function (arg) return arg end")
			TEXT("\n		local _, ret =  coroutine.resume(co)")
			TEXT("\n		return ret")
			TEXT("\n	end"));
		TestTrue(TEXT("Call<>() in Coroutine"), FSUE4LuaFunction::CallGlobal<decltype(TestValue)>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestValue) == TestValue);
	}
	{
		int32 TestValue = 123;
		TestUObject->ValueProperty = nullptr;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, arg)")
			TEXT("\n		local co = coroutine.create(function ()")
			TEXT("\n			uobj.ValueProperty = function (arg) return arg end")
			TEXT("\n			return uobj.ValueProperty(arg)")
			TEXT("\n		end)")
			TEXT("\n		local _, ret =  coroutine.resume(co)")
			TEXT("\n		return ret")
			TEXT("\n	end"));
		TestTrue(TEXT("Create LuaFunction in Coroutine"), FSUE4LuaFunction::CallGlobal<decltype(TestValue)>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestValue) == TestValue);
	}

	return true;
}