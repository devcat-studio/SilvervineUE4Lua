// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#include "LuaUnitTests.h"

#include "LuaVirtualMachine.h"


//=============================================================================================================================
// FSUE4LuaTestCaseFunction
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseFunction, "Silvervine.UE4Lua.Function", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseFunction::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}

	VM->ExecuteString(
		TEXT("\n	function ReturnArg(arg1, ...)")
		TEXT("\n		return arg1")
		TEXT("\n	end")
		TEXT("\n	function ReturnFunc(...)")
		TEXT("\n		return ReturnArg")
		TEXT("\n	end"));

	auto LuaFunc_ReturnArg = FSUE4LuaFunction::CreateFromGlobal(VM.ToSharedRef(), TEXT("ReturnArg"));
	TestTrue(TEXT("Function.CreateFromGlobal()"), LuaFunc_ReturnArg.IsValid());

	if (!LuaFunc_ReturnArg.IsValid())
	{
		return false;
	}

	{
		const int32 Arg = 123;
		const int32 UnusedArg = 456;
		TestTrue(TEXT("Function(Arg, UnusedArg)"), LuaFunc_ReturnArg->Call<int32>(Arg, UnusedArg) == Arg);
	}
	{
		auto ReturnedLuaFunction = FSUE4LuaFunction::CallGlobal<TSharedPtr<FSUE4LuaFunction>>(VM.ToSharedRef(), TEXT("ReturnFunc"));
		TestTrue(TEXT("Function() Returns Function"), ReturnedLuaFunction.IsValid());

		if (ReturnedLuaFunction.IsValid())
		{
			const int32 Arg = 123;
			TestTrue(TEXT("ReturnedLuaFunction()"), ReturnedLuaFunction->Call<int32>(Arg) == Arg);
		}
	}

	{
		auto TestFunction = FSUE4LuaFunction::CreateFromGlobal(VM.ToSharedRef(), TEXT("ReturnArg"));
		TestTrue(TEXT("Function.IsSame()"), LuaFunc_ReturnArg->IsSame(TestFunction));
	}

	return true;
}