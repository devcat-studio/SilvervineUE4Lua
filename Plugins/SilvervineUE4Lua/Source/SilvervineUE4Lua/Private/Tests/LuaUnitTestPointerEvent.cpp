// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaUnitTests.h"

#include "Input/Events.h"

#include "LuaVirtualMachine.h"


//=============================================================================================================================
// FSUE4LuaTestCasePointerEvent
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCasePointerEvent, "Silvervine.UE4Lua.PointerEvent", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCasePointerEvent::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}

	{
		FPointerEvent TestValue;

		VM->ExecuteString(
			TEXT("\n	function Test(value)")
			TEXT("\n		return UE4.PointerEvent.IsLeftMouseButtonDown(value)")
			TEXT("\n	end"));
		TestTrue(TEXT("PointerEvent.IsLeftMouseButtonDown()"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), TestValue) == TestValue.IsMouseButtonDown(EKeys::LeftMouseButton));
	}
	{
		FPointerEvent TestValue;

		VM->ExecuteString(
			TEXT("\n	function Test(value)")
			TEXT("\n		return UE4.PointerEvent.IsRightMouseButtonDown(value)")
			TEXT("\n	end"));
		TestTrue(TEXT("PointerEvent.IsRightMouseButtonDown()"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), TestValue) == TestValue.IsMouseButtonDown(EKeys::RightMouseButton));
	}

	return true;
}
