// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaUnitTests.h"

#include "LuaVirtualMachine.h"


//=============================================================================================================================
// FSUE4LuaTestCaseCaseInsensitiveTable
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseCaseInsensitiveTable, "Silvervine.UE4Lua.CaseInsensitiveTable", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseCaseInsensitiveTable::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}
	
	{
		VM->ExecuteString(
			TEXT("\n	function Test()")
			TEXT("\n		local t = CaseInsensitiveTable.new()")
			TEXT("\n		local value = 123")
			TEXT("\n		t.aBc = value")
			TEXT("\n		return t.Abc == value")
			TEXT("\n	end"));
		TestTrue(TEXT("CaseInsensitiveTable StringKey"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test")));
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test()")
			TEXT("\n		local t = CaseInsensitiveTable.new()")
			TEXT("\n		local value = 123")
			TEXT("\n		t[1] = value")
			TEXT("\n		return t[1] == value")
			TEXT("\n	end"));
		TestTrue(TEXT("CaseInsensitiveTable IntKey"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test")));
	}

	return true;
}