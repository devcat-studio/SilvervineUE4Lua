// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaUnitTests.h"

#include "LuaVirtualMachine.h"


//=============================================================================================================================
// FSUE4LuaTestCaseUFunction
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseUFunction, "Silvervine.UE4Lua.UFunction", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseUFunction::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}
	
	auto TestUObject = NewObject<USUE4LuaTestUObject>();
	
	{
		const int32 InArg1 = 123;
		const int32 InArg2 = 456;
		const int32 InArg3 = 789;
		int32 OutArg1 = 0;
		int32 OutArg2 = 0;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, ...)")
			TEXT("\n		local args = { ... }")
			TEXT("\n		local func1Params = { InIntArg1 = args[1] , InIntArg2 = args[2], InIntArg3 = args[3], OutIntArg1 = args[4], OutIntArg2 = args[5] }")
			TEXT("\n		local retValue = uobj:TestArgs(func1Params)")
			TEXT("\n		uobj.IntProperty1 = func1Params.OutIntArg1")
			TEXT("\n		uobj.IntProperty2 = func1Params.OutIntArg2")
			TEXT("\n		return retValue")
			TEXT("\n	end"));
		TestTrue(TEXT("UFunction Call"),
			FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject, InArg1, InArg2, InArg3, OutArg1, OutArg2) == TestUObject->TestArgs(InArg1, InArg2, InArg3, OutArg1, OutArg2) &&
			TestUObject->IntProperty1 == OutArg1 && TestUObject->IntProperty2 == OutArg2);
	}
	{
		const int32 InArg1 = 123;
		const int32 InArg2 = 456;
		const int32 InArg3 = 789;
		int32 OutArg1 = 0;
		int32 OutArg2 = 0;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, ...)")
			TEXT("\n		local args = { ... }")
			TEXT("\n		local funcParams = { InIntArg1 = args[1] , InIntArg2 = args[2], InIntArg3 = args[3], OutIntArg1 = args[4], OutIntArg2 = args[5] }")
			TEXT("\n		local ret1 = { uobj:TestArgs(funcParams) }")
			TEXT("\n		local ret2 = { uobj:TestArgs(...) }")
			TEXT("\n		return #ret1 == 3 and #ret2 == 3 and ret1[1] == ret2[1] and ret1[2] == ret2[2] and ret1[3] == ret2[3]")
			TEXT("\n	end"));
		TestTrue(TEXT("UFunction pass-by-positions, pass-by-names equivalence"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), TestUObject, InArg1, InArg2, InArg3, OutArg1, OutArg2));
	}
	{
		const int32 InArg1 = 123;
		const int32 InArg2 = 456;
		const int32 InArg3 = 789;
		int32 OutArg1 = 0;
		int32 OutArg2 = 0;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, ...)")
			TEXT("\n		local args = { ... }")
			TEXT("\n		local funcParams = { InIntArg1 = args[1] , InIntArg2 = args[2], InIntArg3 = args[3], OutIntArg1 = args[4], OutIntArg2 = args[5] }")
			TEXT("\n		local _, OutIntArg1, OutIntArg2 = uobj:TestArgs(funcParams)")
			TEXT("\n		return OutIntArg1 == funcParams.OutIntArg1 and OutIntArg2 == OutIntArg2")
			TEXT("\n	end"));
		TestTrue(TEXT("UFunction return values"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), TestUObject, InArg1, InArg2, InArg3, OutArg1, OutArg2));
	}
	{
		const int32 InArg1 = 123;
		const int32 InArg2 = 456;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, ...)")
			TEXT("\n		return uobj:TestDefaultArgs() == uobj:TestDefaultArgs(...)")
			TEXT("\n	end"));
		TestTrue(TEXT("UFunction default parameters"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), TestUObject, InArg1, InArg2));
	}
	{
		auto TestUObjectDerived = NewObject<USUE4LuaTestUObjectDerived>();

		VM->ExecuteString(
			TEXT("\n	function Test(uobj1, uobj2)")
			TEXT("\n		return uobj1:TestDefaultArgs() == uobj2:TestDefaultArgs()")
			TEXT("\n	end"));
		TestTrue(TEXT("UFunction default parameters inheritance"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestUObjectDerived));
	}
	{
		FVector Arg(1.0f, 2.0f, 3.0f);
		VM->ExecuteString(
			TEXT("\n	function Test(uobj, arg)")
			TEXT("\n		return uobj:TestVector(arg)")
			TEXT("\n	end"));
		TestTrue(TEXT("UFunction pass-by-positions with FVector"), FSUE4LuaFunction::CallGlobal<decltype(Arg)>(VM.ToSharedRef(), TEXT("Test"), TestUObject, Arg) == Arg);
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		local ret = uobj:TestArrayArg(nil, uobj)")
			TEXT("\n		return ret[0]")
			TEXT("\n	end"));
		TestTrue(TEXT("UFunction TArray out parameters"), FSUE4LuaFunction::CallGlobal<UObject*>(VM.ToSharedRef(), TEXT("Test"), TestUObject) == TestUObject);
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		local params = { InArg1 = uobj.StructProperty, InArg2 = uobj.StructProperty }")
			TEXT("\n		uobj:TestStruct(params)")
			TEXT("\n		return params.OutArg1 ~= nil")
			TEXT("\n	end"));
		TestTrue(TEXT("UFunction pass-by-names output"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), TestUObject));
	}
	{
		ESUE4LuaTestEnum Arg = ESUE4LuaTestEnum::Enum2;
		VM->ExecuteString(
			TEXT("\n	function Test(uobj, arg)")
			TEXT("\n		return uobj:TestEnum(arg)")
			TEXT("\n	end"));
		TestTrue(TEXT("UFunction enum arguments"), FSUE4LuaFunction::CallGlobal<decltype(Arg)>(VM.ToSharedRef(), TEXT("Test"), TestUObject, Arg) == Arg);
	}
	{
		FTransform Arg(FQuat(1.0f, 2.0f, 3.0f, 4.0f), FVector(5.0f, 6.0f, 7.0f), FVector(8.0f, 9.0f, 10.0f));
		VM->ExecuteString(
			TEXT("\n	function Test(uobj, arg)")
			TEXT("\n		return uobj:TestTransform(arg)")
			TEXT("\n	end"));
		TestTrue(TEXT("UFunction FTransform arguments"), FSUE4LuaFunction::CallGlobal<decltype(Arg)>(VM.ToSharedRef(), TEXT("Test"), TestUObject, Arg).Equals(Arg));
	}
	{
		TestUObject->StructProperty1.IntProperty = 123;
		TestUObject->StructProperty2.IntProperty = 456;
		TestUObject->StructProperty3.IntProperty = 0;
		TestUObject->TestStruct(TestUObject->StructProperty1, TestUObject->StructProperty2, TestUObject->StructProperty3);

		int32 TestValue = TestUObject->StructProperty3.IntProperty;
		TestUObject->StructProperty3.IntProperty = 0;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		uobj:TestStruct(uobj.StructProperty1, uobj.StructProperty2, uobj.StructProperty3)")
			TEXT("\n	end"));
		FSUE4LuaFunction::CallGlobal<void>(VM.ToSharedRef(), TEXT("Test"), TestUObject);
		TestTrue(TEXT("UFunction UStruct& arguments(pass-by-positions)"), TestUObject->StructProperty3.IntProperty == TestValue);
	}
	{
		TestUObject->StructProperty1.IntProperty = 123;
		TestUObject->StructProperty2.IntProperty = 456;
		TestUObject->StructProperty3.IntProperty = 0;
		TestUObject->TestStruct(TestUObject->StructProperty1, TestUObject->StructProperty2, TestUObject->StructProperty3);

		int32 TestValue = TestUObject->StructProperty3.IntProperty;
		TestUObject->StructProperty3.IntProperty = 0;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		uobj:TestStruct{ InArg1 = uobj.StructProperty1, InArg2 = uobj.StructProperty2, OutArg1 = uobj.StructProperty3 }")
			TEXT("\n	end"));
		FSUE4LuaFunction::CallGlobal<void>(VM.ToSharedRef(), TEXT("Test"), TestUObject);
		TestTrue(TEXT("UFunction UStruct& arguments(pass-by-names)"), TestUObject->StructProperty3.IntProperty == TestValue);
	}

	return true;
}
