// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaUnitTests.h"

#include "LuaVirtualMachine.h"


//=============================================================================================================================
// FSUE4LuaTestCaseSUE4Lua
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseSUE4Lua, "Silvervine.UE4Lua.SUE4Lua", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseSUE4Lua::RunTest(const FString& Parameters)
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
			TEXT("\n		return SUE4Lua.IsFixedArray(uobj.FixedIntArrayProperty)")
			TEXT("\n	end"));
		TestTrue(TEXT("SUE4Lua.IsFixedArray()"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), TestUObject));
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		return SUE4Lua.IsArray(uobj.IntArrayProperty)")
			TEXT("\n	end"));
		TestTrue(TEXT("SUE4Lua.IntArray()"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), TestUObject));
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		return SUE4Lua.IsMap(uobj.IntMapProperty)")
			TEXT("\n	end"));
		TestTrue(TEXT("SUE4Lua.IsMap()"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), TestUObject));
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		return SUE4Lua.IsSet(uobj.IntSetProperty)")
			TEXT("\n	end"));
		TestTrue(TEXT("SUE4Lua.IsSet()"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), TestUObject));
	}

	return true;
}
