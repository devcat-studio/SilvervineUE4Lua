// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

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
			TEXT("\n		return UE4.PointerEvent.GetScreenSpacePosition(value)")
			TEXT("\n	end"));
		TestTrue(TEXT("PointerEvent.GetScreenSpacePosition()"), FSUE4LuaFunction::CallGlobal<FVector2D>(VM.ToSharedRef(), TEXT("Test"), TestValue) == TestValue.GetScreenSpacePosition());
	}
	{
		FPointerEvent TestValue;

		VM->ExecuteString(
			TEXT("\n	function Test(value)")
			TEXT("\n		return UE4.PointerEvent.GetLastScreenSpacePosition(value)")
			TEXT("\n	end"));
		TestTrue(TEXT("PointerEvent.GetLastScreenSpacePosition()"), FSUE4LuaFunction::CallGlobal<FVector2D>(VM.ToSharedRef(), TEXT("Test"), TestValue) == TestValue.GetLastScreenSpacePosition());
	}
	{
		FPointerEvent TestValue;

		VM->ExecuteString(
			TEXT("\n	function Test(value)")
			TEXT("\n		return UE4.PointerEvent.GetCursorDelta(value)")
			TEXT("\n	end"));
		TestTrue(TEXT("PointerEvent.GetCursorDelta()"), FSUE4LuaFunction::CallGlobal<FVector2D>(VM.ToSharedRef(), TEXT("Test"), TestValue) == TestValue.GetCursorDelta());
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
	{
		FPointerEvent TestValue;

		VM->ExecuteString(
			TEXT("\n	function Test(value)")
			TEXT("\n		return UE4.PointerEvent.GetEffectingButton(value)")
			TEXT("\n	end"));
		TestTrue(TEXT("PointerEvent.GetEffectingButton()"), FSUE4LuaFunction::CallGlobal<FKey>(VM.ToSharedRef(), TEXT("Test"), TestValue) == TestValue.GetEffectingButton());
	}
	{
		FPointerEvent TestValue;

		VM->ExecuteString(
			TEXT("\n	function Test(value)")
			TEXT("\n		return UE4.PointerEvent.GetWheelDelta(value)")
			TEXT("\n	end"));
		TestTrue(TEXT("PointerEvent.GetWheelDelta()"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), TestValue) == TestValue.GetWheelDelta());
	}
	{
		FPointerEvent TestValue;

		VM->ExecuteString(
			TEXT("\n	function Test(value)")
			TEXT("\n		return UE4.PointerEvent.GetUserIndex(value)")
			TEXT("\n	end"));
		TestTrue(TEXT("PointerEvent.GetUserIndex()"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), TestValue) == TestValue.GetUserIndex());
	}
	{
		FPointerEvent TestValue;

		VM->ExecuteString(
			TEXT("\n	function Test(value)")
			TEXT("\n		return UE4.PointerEvent.GetPointerIndex(value)")
			TEXT("\n	end"));
		TestTrue(TEXT("PointerEvent.GetPointerIndex()"), FSUE4LuaFunction::CallGlobal<int>(VM.ToSharedRef(), TEXT("Test"), TestValue) == TestValue.GetPointerIndex());
	}
	{
		FPointerEvent TestValue;

		VM->ExecuteString(
			TEXT("\n	function Test(value)")
			TEXT("\n		return UE4.PointerEvent.GetTouchpadIndex(value)")
			TEXT("\n	end"));
		TestTrue(TEXT("PointerEvent.GetTouchpadIndex()"), FSUE4LuaFunction::CallGlobal<int>(VM.ToSharedRef(), TEXT("Test"), TestValue) == TestValue.GetTouchpadIndex());
	}
	{
		FPointerEvent TestValue;

		VM->ExecuteString(
			TEXT("\n	function Test(value)")
			TEXT("\n		return UE4.PointerEvent.IsTouchEvent(value)")
			TEXT("\n	end"));
		TestTrue(TEXT("PointerEvent.IsTouchEvent()"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), TestValue) == TestValue.IsTouchEvent());
	}
	{
		FPointerEvent TestValue;

		VM->ExecuteString(
			TEXT("\n	function Test(value)")
			TEXT("\n		return UE4.PointerEvent.GetGestureType(value)")
			TEXT("\n	end"));
		TestTrue(TEXT("PointerEvent.GetGestureType()"), FSUE4LuaFunction::CallGlobal<EGestureEvent>(VM.ToSharedRef(), TEXT("Test"), TestValue) == TestValue.GetGestureType());
	}
	{
		FPointerEvent TestValue;

		VM->ExecuteString(
			TEXT("\n	function Test(value)")
			TEXT("\n		return UE4.PointerEvent.GetGestureDelta(value)")
			TEXT("\n	end"));
		TestTrue(TEXT("PointerEvent.GetGestureDelta()"), FSUE4LuaFunction::CallGlobal<FVector2D>(VM.ToSharedRef(), TEXT("Test"), TestValue) == TestValue.GetGestureDelta());
	}

	return true;
}
