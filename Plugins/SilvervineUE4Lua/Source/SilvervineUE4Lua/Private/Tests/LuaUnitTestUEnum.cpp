// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaUnitTests.h"

#include "LuaVirtualMachine.h"


//=============================================================================================================================
// FSUE4LuaTestCaseEnum
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseEnum, "Silvervine.UE4Lua.Enum", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseEnum::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}

	VM->ExecuteString(
		TEXT("\n	ESUE4LuaTestEnum = SUE4Lua.GetEnumTable('ESUE4LuaTestEnum')"));

	{
		VM->ExecuteString(
			TEXT("\n	function Test()")
			TEXT("\n		return ESUE4LuaTestEnum.Enum2")
			TEXT("\n	end"));
		TestTrue(TEXT("SUE4Lua.GetEnumTable()"), FSUE4LuaFunction::CallGlobal<ESUE4LuaTestEnum>(VM.ToSharedRef(), TEXT("Test")) == ESUE4LuaTestEnum::Enum2);
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test(arg)")
			TEXT("\n		return ESUE4LuaTestEnum:ToString(arg)")
			TEXT("\n	end"));
		TestTrue(TEXT("SUE4Lua.Enum.ToString()"), FSUE4LuaFunction::CallGlobal<FString>(VM.ToSharedRef(), TEXT("Test"), ESUE4LuaTestEnum::Enum2) == FString("Enum2"));
	}

	return true;
}
