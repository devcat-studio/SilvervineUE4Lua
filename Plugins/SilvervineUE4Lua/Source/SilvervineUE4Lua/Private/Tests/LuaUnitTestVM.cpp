// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#include "LuaUnitTests.h"

#include "LuaVirtualMachine.h"


//=============================================================================================================================
// FSUE4LuaTestCaseVM
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseVM, "Silvervine.UE4Lua.VM", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseVM::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();

	TestTrue(TEXT("VM.Create()"), VM.IsValid());

	return true;
}

