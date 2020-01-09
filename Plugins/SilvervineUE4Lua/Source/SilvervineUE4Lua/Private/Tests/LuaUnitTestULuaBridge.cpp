// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#include "LuaUnitTests.h"

#include "SilvervineUE4LuaBridge.h"
#include "LuaVirtualMachine.h"


//=============================================================================================================================
// FSUE4LuaTestCaseNativeDispatch
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseNativeDispatch, "Silvervine.UE4Lua.NativeDispatch", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseNativeDispatch::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}
		
	auto TestUObject = NewObject<USUE4LuaTestUObject>();
	TestUObject->LuaBridge = NewObject<USUE4LuaBridge>(TestUObject);
	TestUObject->LuaBridge->SetUnitTestVMInstance(VM);
	USUE4LuaBridge* LuaBridge = TestUObject->LuaBridge;

	{
		TSharedPtr<FSUE4LuaFunction> LoadedHandler = VM->LoadString(
			TEXT("\n	local TestUObject = {}")
			TEXT("\n	function TestUObject:NativeTestNativeDispatch(Params)")
			TEXT("\n		Params[1].StructProperty.IntProperty = 1")
			TEXT("\n		Params[2].StructProperty.IntProperty = 2")
			TEXT("\n		Params[3].StructProperty.IntProperty = 3")
			TEXT("\n	end")
			TEXT("\n	return TestUObject;"));
		VM->SetDispatchHandler(TestUObject->GetClass(), LoadedHandler);

		FSUE4LuaTestNestedStruct InParam1;
		InParam1.StructProperty.IntProperty = 11;
		FSUE4LuaTestNestedStruct InParam2;
		InParam2.StructProperty.IntProperty = 22;
		FSUE4LuaTestNestedStruct OutParam3;
		OutParam3.StructProperty.IntProperty = 33;

		FSUE4LuaTestNestedStruct InParam1_ = InParam1;
		FSUE4LuaTestNestedStruct InParam2_ = InParam2;
		FSUE4LuaTestNestedStruct OutParam3_ = OutParam3;

		TestUObject->TestNativeDispatch(InParam1, InParam2, OutParam3);
		bool Result = InParam1_.StructProperty.IntProperty == InParam1.StructProperty.IntProperty &&
			InParam2_.StructProperty.IntProperty == InParam2.StructProperty.IntProperty &&
			OutParam3_.StructProperty.IntProperty != OutParam3.StructProperty.IntProperty;
		TestTrue(TEXT("TestNativeDispatch()"), Result);
	}
	{
		TSharedPtr<FSUE4LuaFunction> LoadedHandler = VM->LoadString(
			TEXT("\n	local TestUObject = {}")
			TEXT("\n	function TestUObject:NativeTest(Params)")
			TEXT("\n		for k, v in pairs(Params) do")
			TEXT("\n			SUE4Lua.Log('Paramss[' ..k .. '] = ')")
			TEXT("\n			SUE4Lua.Log(v)")
			TEXT("\n		end")
			TEXT("\n	end")
			TEXT("\n	return TestUObject;"));
		VM->SetDispatchHandler(TestUObject->GetClass(), LoadedHandler);

		LuaBridge->NativeDispatch(TestUObject, TEXT("Test"), 1, 2.0f, TEXT("Text"));
		TestTrue(TEXT("NativeDispatch: pass literals"), true);
	}
	{
		TSharedPtr<FSUE4LuaFunction> LoadedHandler = VM->LoadString(
			TEXT("\n	local TestUObject = {}")
			TEXT("\n	function TestUObject:NativeTest(Params)")
			TEXT("\n		Params[1] = 2")
			TEXT("\n		Params[2] = 22.22")
			TEXT("\n		Params[3] = 'FText'")
			TEXT("\n	end")
			TEXT("\n	return TestUObject;"));
		VM->SetDispatchHandler(TestUObject->GetClass(), LoadedHandler);

		const int InParam1 = 1;
		const float InParam2 = 2.2f;
		const FName InParam3 = TEXT("FName");
		const int InParam1_ = InParam1;
		const float InParam2_ = InParam2;
		const FName InParam3_ = InParam3;

		LuaBridge->NativeDispatch(TestUObject, TEXT("Test"), InParam1, InParam2, InParam3);
		bool Result = InParam1_ == InParam1 && InParam2_ == InParam2 && InParam3_ == InParam3;
		TestTrue(TEXT("NativeDispatch: pass const values"), Result);
	}
	{
		TSharedPtr<FSUE4LuaFunction> LoadedHandler = VM->LoadString(
			TEXT("\n	local TestUObject = {}")
			TEXT("\n	function TestUObject:NativeTest(Params)")
			TEXT("\n		Params[1] = 2")
			TEXT("\n		Params[2] = 'FText'")
			TEXT("\n	end")
			TEXT("\n	return TestUObject;"));
		VM->SetDispatchHandler(TestUObject->GetClass(), LoadedHandler);

		int OutParam1 = 1;
		FName OutParam2 = TEXT("FName");
		const int InParam1_ = 2;
		const FName InParam2_ = TEXT("FText");

		LuaBridge->NativeDispatch(TestUObject, TEXT("Test"), OutParam1, OutParam2);
		bool Result = InParam1_ == OutParam1 && InParam2_ == OutParam2;
		TestTrue(TEXT("NativeDispatch: pass non-const values"), Result);
	}
	{
		TSharedPtr<FSUE4LuaFunction> LoadedHandler = VM->LoadString(
			TEXT("\n	local TestUObject = {}")
			TEXT("\n	function TestUObject:NativeTest(Params)")
			TEXT("\n		Params[1] = 2")
			TEXT("\n		Params[2] = 'FText'")
			TEXT("\n	end")
			TEXT("\n	return TestUObject;"));
		VM->SetDispatchHandler(TestUObject->GetClass(), LoadedHandler);

		int Param1 = 1;
		FName Param2 = TEXT("FName");
		const int& InParam1 = Param1;
		const FName& InParam2 = Param2;
		const int InParam1_ = 1;
		const FName InParam2_ = TEXT("FName");


		LuaBridge->NativeDispatch(TestUObject, TEXT("Test"), InParam1, InParam2);
		bool Result = InParam1_ == InParam1 && InParam2_ == InParam2;
		TestTrue(TEXT("NativeDispatch: pass const references"), Result);
	}
	{
		TSharedPtr<FSUE4LuaFunction> LoadedHandler = VM->LoadString(
			TEXT("\n	local TestUObject = {}")
			TEXT("\n	function TestUObject:NativeTest(Params)")
			TEXT("\n		Params[1] = 2")
			TEXT("\n		Params[2] = 'FText'")
			TEXT("\n	end")
			TEXT("\n	return TestUObject;"));
		VM->SetDispatchHandler(TestUObject->GetClass(), LoadedHandler);

		int Param1 = 1;
		FName Param2 = TEXT("FName");
		int& OutParam1 = Param1;
		FName& OutParam2 = Param2;
		const int InParam1_ = 2;
		const FName InParam2_ = TEXT("FText");

		LuaBridge->NativeDispatch(TestUObject, TEXT("Test"), OutParam1, OutParam2);
		bool Result = InParam1_ == OutParam1 && InParam2_ == OutParam2;
		TestTrue(TEXT("NativeDispatch: pass non-const references"), Result);
	}
	{
		TSharedPtr<FSUE4LuaFunction> LoadedHandler = VM->LoadString(
			TEXT("\n	local TestUObject = {}")
			TEXT("\n	function TestUObject:NativeTest(Params)")
			TEXT("\n		Params[1].StructProperty.IntProperty = 2")
			TEXT("\n	end")
			TEXT("\n	return TestUObject;"));
		VM->SetDispatchHandler(TestUObject->GetClass(), LoadedHandler);

		FSUE4LuaTestNestedStruct Param1;
		Param1.StructProperty.IntProperty = 1;
		const FSUE4LuaTestNestedStruct InParam1 = Param1;
		FSUE4LuaTestNestedStruct InParam1_;
		InParam1_.StructProperty.IntProperty = 1;

		LuaBridge->NativeDispatch(TestUObject, TEXT("Test"), InParam1);
		bool Result = InParam1_.StructProperty.IntProperty == InParam1.StructProperty.IntProperty;
		TestTrue(TEXT("NativeDispatch: pass const UStruct"), Result);
	}
	{
		TSharedPtr<FSUE4LuaFunction> LoadedHandler = VM->LoadString(
			TEXT("\n	local TestUObject = {}")
			TEXT("\n	function TestUObject:NativeTest(Params)")
			TEXT("\n		Params[1].StructProperty.IntProperty = 2")
			TEXT("\n	end")
			TEXT("\n	return TestUObject;"));
		VM->SetDispatchHandler(TestUObject->GetClass(), LoadedHandler);

		FSUE4LuaTestNestedStruct OutParam1;
		OutParam1.StructProperty.IntProperty = 1;
		FSUE4LuaTestNestedStruct InParam1_;
		InParam1_.StructProperty.IntProperty = 2;

		LuaBridge->NativeDispatch(TestUObject, TEXT("Test"), OutParam1);
		bool Result = InParam1_.StructProperty.IntProperty == OutParam1.StructProperty.IntProperty;
		TestTrue(TEXT("NativeDispatch: pass non-const UStruct"), Result);
	}	
	{
		TSharedPtr<FSUE4LuaFunction> LoadedHandler = VM->LoadString(
			TEXT("\n	local TestUObject = {}")
			TEXT("\n	function TestUObject:NativeTest(Params)")
			TEXT("\n		Params[1].StructProperty.IntProperty = 2")
			TEXT("\n	end")
			TEXT("\n	return TestUObject;"));
		VM->SetDispatchHandler(TestUObject->GetClass(), LoadedHandler);

		FSUE4LuaTestNestedStruct Param1;
		Param1.StructProperty.IntProperty = 1;
		const FSUE4LuaTestNestedStruct& InParam1 = Param1;
		FSUE4LuaTestNestedStruct InParam1_;
		InParam1_.StructProperty.IntProperty = 1;

		LuaBridge->NativeDispatch(TestUObject, TEXT("Test"), InParam1);
		bool Result = InParam1_.StructProperty.IntProperty == InParam1.StructProperty.IntProperty;
		TestTrue(TEXT("NativeDispatch: pass const UStruct&"), Result);
	}
	{
		TSharedPtr<FSUE4LuaFunction> LoadedHandler = VM->LoadString(
			TEXT("\n	local TestUObject = {}")
			TEXT("\n	function TestUObject:NativeTest(Params)")
			TEXT("\n		Params[1].StructProperty.IntProperty = 2")
			TEXT("\n	end")
			TEXT("\n	return TestUObject;"));
		VM->SetDispatchHandler(TestUObject->GetClass(), LoadedHandler);

		FSUE4LuaTestNestedStruct Param1;
		Param1.StructProperty.IntProperty = 1;
		FSUE4LuaTestNestedStruct& OutParam1 = Param1;
		FSUE4LuaTestNestedStruct InParam1_;
		InParam1_.StructProperty.IntProperty = 2;

		LuaBridge->NativeDispatch(TestUObject, TEXT("Test"), OutParam1);
		bool Result = InParam1_.StructProperty.IntProperty == OutParam1.StructProperty.IntProperty;
		TestTrue(TEXT("NativeDispatch: pass non-const UStruct&"), Result);
	}
	{
		TSharedPtr<FSUE4LuaFunction> LoadedHandler = VM->LoadString(
			TEXT("\n	local TestUObject = {}")
			TEXT("\n	function TestUObject:NativeTest(Params)")
			TEXT("\n		Params[1].X = 4.0")
			TEXT("\n		Params[1].Y = 5.0")
			TEXT("\n		Params[1].Z = 6.0")
			TEXT("\n	end")
			TEXT("\n	return TestUObject;"));
		VM->SetDispatchHandler(TestUObject->GetClass(), LoadedHandler);

		FVector Param1 = FVector(1.0f, 2.0f, 3.0f);
		FVector& OutParam1 = Param1;
		FVector InParam1_ = FVector(4.0f, 5.0f, 6.0f);

		LuaBridge->NativeDispatch(TestUObject, TEXT("Test"), OutParam1);
		bool Result = InParam1_ == OutParam1;
		TestTrue(TEXT("NativeDispatch: pass FVector"), Result);
	}
	{
		TSharedPtr<FSUE4LuaFunction> LoadedHandler = VM->LoadString(
			TEXT("\n	local TestUObject = {}")
			TEXT("\n	function TestUObject:NativeTest(Params)")
			TEXT("\n		Params[1].R = 64")
			TEXT("\n		Params[1].G = 32")
			TEXT("\n		Params[1].B = 16")
			TEXT("\n		Params[1].A = 8")
			TEXT("\n	end")
			TEXT("\n	return TestUObject;"));
		VM->SetDispatchHandler(TestUObject->GetClass(), LoadedHandler);

		FColor Param1 = FColor(8, 16, 32, 64);
		FColor& OutParam1 = Param1;
		FColor InParam1_ = FColor(64, 32, 16, 8);

		LuaBridge->NativeDispatch(TestUObject, TEXT("Test"), OutParam1);
		bool Result = InParam1_ == OutParam1;
		TestTrue(TEXT("NativeDispatch: pass FColor"), Result);
	}

	return true;
}
