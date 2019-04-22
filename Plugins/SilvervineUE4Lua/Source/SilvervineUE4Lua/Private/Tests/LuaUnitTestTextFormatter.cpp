// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaUnitTests.h"

#include "LuaVirtualMachine.h"


//=============================================================================================================================
// FSUE4LuaTestCaseTextFormatter
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseTextFormatter, "Silvervine.UE4Lua.TextFormatter", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseTextFormatter::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}

	{
		VM->ExecuteString(
			TEXT("\n	function Test()")
			TEXT("\n		return UE4.TextFormatter.Format() == ''")
			TEXT("\n	end"));
		TestTrue(TEXT("TextFormatter.Format() with no argument"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test")));
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test()")
			TEXT("\n		return UE4.TextFormatter.Format('123') == '123'")
			TEXT("\n	end"));
		TestTrue(TEXT("TextFormatter.Format() with one argument"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test")));
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test()")
			TEXT("\n		return UE4.TextFormatter.Format('{0} {1}', 123, 'str') == '123 str'")
			TEXT("\n	end"));
		TestTrue(TEXT("TextFormatter.Format() with many arguments"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test")));
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test()")
			TEXT("\n		return UE4.TextFormatter.Format('{Arg1} {Arg2}', {Arg1 = 123, Arg2 = 'str'}) == '123 str'")
			TEXT("\n	end"));
		TestTrue(TEXT("TextFormatter.Format() with NamedArg"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test")));
	}

	return true;
}