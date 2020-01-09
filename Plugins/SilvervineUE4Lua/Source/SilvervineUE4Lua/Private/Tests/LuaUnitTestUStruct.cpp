// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#include "LuaUnitTests.h"

#include "LuaVirtualMachine.h"


//=============================================================================================================================
// FSUE4LuaTestCaseUStruct
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseUStruct, "Silvervine.UE4Lua.UStruct", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseUStruct::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}

	auto TestUObject = NewObject<USUE4LuaTestUObject>();

	{
		int32 TestValue = 123;
		TestUObject->StructProperty.IntProperty = 0;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.StructProperty.IntProperty = value")
			TEXT("\n		return uobj.StructProperty.IntProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("UStruct.Get()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestValue) == TestValue);
		TestTrue(TEXT("UStruct.Set()"), TestUObject->StructProperty.IntProperty == TestValue);
	}
	{
		int32 TestValue = 123;
		TestUObject->NestedStructProperty.StructProperty.IntProperty = 0;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.NestedStructProperty.StructProperty.IntProperty = value")
			TEXT("\n		return uobj.NestedStructProperty.StructProperty.IntProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("UStruct.Get() : nested UStruct"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestValue) == TestValue);
		TestTrue(TEXT("UStruct.Set() : nested UStruct"), TestUObject->NestedStructProperty.StructProperty.IntProperty == TestValue);
	}
	{
		int32 TestValue = 123;
		TestUObject->MultipleNestedStructProperty.NestedStructProperty.StructProperty.IntProperty = 0;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.MultipleNestedStructProperty.NestedStructProperty.StructProperty.IntProperty = value")
			TEXT("\n		return uobj.MultipleNestedStructProperty.NestedStructProperty.StructProperty.IntProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("UStruct.Get() : multiple nested UStruct"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestValue) == TestValue);
		TestTrue(TEXT("UStruct.Set() : multiple nested UStruct"), TestUObject->MultipleNestedStructProperty.NestedStructProperty.StructProperty.IntProperty == TestValue);
	}
	{
		int32 TestIndex = 3;
		int32 TestValue = 123;
		TestUObject->FixedStructArrayProperty[TestIndex].IntProperty = 0;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, index, value)")
			TEXT("\n		uobj.FixedStructArrayProperty[index].IntProperty = value")
			TEXT("\n		return uobj.FixedStructArrayProperty[index].IntProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("UStruct.Get() : UStruct[]"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestIndex, TestValue) == TestValue);
		TestTrue(TEXT("UStruct.Set() : UStruct[]"), TestUObject->FixedStructArrayProperty[TestIndex].IntProperty == TestValue);
	}
	{
		int32 TestIndex = 3;
		int32 TestValue = 123;
		TestUObject->FixedNestedStructArrayProperty[TestIndex].StructProperty.IntProperty = 0;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, index, value)")
			TEXT("\n		uobj.FixedNestedStructArrayProperty[index].StructProperty.IntProperty = value")
			TEXT("\n		return uobj.FixedNestedStructArrayProperty[index].StructProperty.IntProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("UStruct.Get() : UStruct[] in UStruct"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestIndex, TestValue) == TestValue);
		TestTrue(TEXT("UStruct.Set() : UStruct[] in UStruct"), TestUObject->FixedNestedStructArrayProperty[TestIndex].StructProperty.IntProperty == TestValue);
	}
	{
		int32 TestIndex = 3;
		int32 TestValue = 123;
		TestUObject->StructProperty.FixedIntArrayProperty[TestIndex] = 0;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, index, value)")
			TEXT("\n		uobj.StructProperty.FixedIntArrayProperty[index] = value")
			TEXT("\n		return uobj.StructProperty.FixedIntArrayProperty[index]")
			TEXT("\n	end"));
		TestTrue(TEXT("UStruct.Get() : int32[] in UStruct"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestIndex, TestValue) == TestValue);
		TestTrue(TEXT("UStruct.Set() : int32[] in UStruct"), TestUObject->StructProperty.FixedIntArrayProperty[TestIndex] == TestValue);
	}
	{
		int32 TestIndex = 3;
		int32 TestValue = 123;
		TestUObject->StructProperty.IntArrayProperty.SetNum(TestIndex + 1);
		TestUObject->StructProperty.IntArrayProperty[TestIndex] = 0;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, index, value)")
			TEXT("\n		uobj.StructProperty.IntArrayProperty[index] = value")
			TEXT("\n		return uobj.StructProperty.IntArrayProperty[index]")
			TEXT("\n	end"));
		TestTrue(TEXT("UStruct.Get() : TArray<int32> in UStruct"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestIndex, TestValue) == TestValue);
		TestTrue(TEXT("UStruct.Set() : TArray<int32> in UStruct"), TestUObject->StructProperty.IntArrayProperty[TestIndex] == TestValue);
	}
	{
		int32 TestValue = 123;
		TestUObject->StructProperty.IntArrayProperty.SetNum(1);
		TestUObject->StructProperty.IntArrayProperty[0] = TestValue;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		for k, v in pairs(uobj.StructProperty) do")
			TEXT("\n			if k == 'IntArrayProperty' then return v[0] end")
			TEXT("\n		end")
			TEXT("\n	end"));
		TestTrue(TEXT("UStruct pairs()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject) == TestValue);
	}
	{
		int32 TestValue = 123;
		TestUObject->StructProperty.IntProperty = 0;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		local struct = UE4.NewStruct('SUE4LuaTestStruct')")
			TEXT("\n		struct.IntProperty = value")
			TEXT("\n		uobj.StructProperty = struct")
			TEXT("\n	end"));
		FSUE4LuaFunction::CallGlobal<void>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestValue);
		TestTrue(TEXT("UE4.NewStruct"), TestUObject->StructProperty.IntProperty == TestValue);
	}
	{
		FSUE4LuaTestStruct TestValue;
		TestValue.IntProperty = 123;

		VM->ExecuteString(
			TEXT("\n	function Test(value)")
			TEXT("\n		return value.IntProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("UStruct push"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestValue) == TestValue.IntProperty);
	}
	{
		int32 TestValue = 123;

		VM->ExecuteString(
			TEXT("\n	function Test(value)")
			TEXT("\n		local struct = UE4.NewStruct('SUE4LuaTestStruct')")
			TEXT("\n		struct.IntProperty = value")
			TEXT("\n		return struct")
			TEXT("\n	end"));
		TestTrue(TEXT("UStruct pop"), FSUE4LuaFunction::CallGlobal<FSUE4LuaTestStruct>(VM.ToSharedRef(), TEXT("Test"), TestValue).IntProperty == TestValue);
	}

	return true;
}