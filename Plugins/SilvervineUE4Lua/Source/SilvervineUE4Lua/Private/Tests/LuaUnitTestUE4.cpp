// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaUnitTests.h"

#include "Kismet/KismetSystemLibrary.h"

#include "LuaVirtualMachine.h"


//=============================================================================================================================
// FSUE4LuaTestCaseUE4
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseUE4, "Silvervine.UE4Lua.UE4", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseUE4::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}

	auto TestUObject = NewObject<USUE4LuaTestUObject>();

	{
		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		return UE4.IsValid(uobj)")
			TEXT("\n	end"));
		TestTrue(TEXT("UE4.IsValid() == true"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), TestUObject));
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test()")
			TEXT("\n		return UE4.IsValid(nil)")
			TEXT("\n	end"));
		TestTrue(TEXT("UE4.IsValid() == false"), !FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test")));
	}
	{

		VM->ExecuteString(
			TEXT("\n	function Test()")
			TEXT("\n		return UE4.FindClass('SUE4LuaTestUObject') ~= nil")
			TEXT("\n	end"));
		TestTrue(TEXT("UE.FindClass()"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test")));
	}
	{
		VM->ExecuteString(
			TEXT("\n	local UKismetSystemLibrary = UE4.FindClass('KismetSystemLibrary')")
			TEXT("\n	function Test(uobj) return UKismetSystemLibrary.GetObjectName(uobj) end"));

		auto Arg = NewObject<USUE4LuaTestUObject>();
		TestTrue(TEXT("UKismetSystemLibrary.GetObjectName()"), FSUE4LuaFunction::CallGlobal<FString>(VM.ToSharedRef(), TEXT("Test"), Arg) == UKismetSystemLibrary::GetObjectName(Arg));
	}

	return true;
}
