// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaUnitTests.h"

#include "Camera/CameraActor.h"
#include "Components/ActorComponent.h"
#include "Components/MeshComponent.h"
#include "GameFramework/Character.h"

#include "Bindings/LuaObjectBinding.h"
#include "LuaVirtualMachine.h"


//=============================================================================================================================
// FSUE4LuaTestCaseBinding
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseBinding, "Silvervine.UE4Lua.Binding", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseBinding::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}

	{
		FSUE4LuaBindingRegistry BindingRegistry;
		BindingRegistry.Register(NewObject<USUE4LuaObjectBinding>());

		// Root-+---A
		//      |
		//      +---B-+---C
		//		|     |
		//		|     +---D
		//		|
		//		+---E-----F
		//
		//	A: Field
		//  B: Actor
		//  C: Character
		//  D: Camera
		//  E: SceneComponent
		//  F: MeshComponent

		auto TestA = NewObject<USUE4TestBinding>();
		TestA->SetBindingClass(UField::StaticClass());

		auto TestB = NewObject<USUE4TestBinding>();
		TestB->SetBindingClass(AActor::StaticClass());

		auto TestC = NewObject<USUE4TestBinding>();
		TestC->SetBindingClass(ACharacter::StaticClass());

		auto TestD = NewObject<USUE4TestBinding>();
		TestD->SetBindingClass(ACameraActor::StaticClass());

		auto TestE = NewObject<USUE4TestBinding>();
		TestE->SetBindingClass(USceneComponent::StaticClass());

		auto TestF = NewObject<USUE4TestBinding>();
		TestF->SetBindingClass(UMeshComponent::StaticClass());

		BindingRegistry.Register(TestA);
		BindingRegistry.Register(TestC);
		BindingRegistry.Register(TestD);
		BindingRegistry.Register(TestB);
		BindingRegistry.Register(TestE);
		BindingRegistry.Register(TestF);

		{
			TArray<USUE4LuaBinding*> AllBindings;
			BindingRegistry.GetAllBindings(AllBindings);
			TestTrue(TEXT("Add Binding: TestA"), 7 == AllBindings.Num() && AllBindings[1] == TestA);
			TestTrue(TEXT("Add Binding: TestB"), 7 == AllBindings.Num() && AllBindings[2] == TestB);
			TestTrue(TEXT("Add Binding: TestC"), 7 == AllBindings.Num() && AllBindings[3] == TestC);
			TestTrue(TEXT("Add Binding: TestD"), 7 == AllBindings.Num() && AllBindings[4] == TestD);
			TestTrue(TEXT("Add Binding: TestE"), 7 == AllBindings.Num() && AllBindings[5] == TestE);
			TestTrue(TEXT("Add Binding: TestF"), 7 == AllBindings.Num() && AllBindings[6] == TestF);
		}
		{
			TArray<USUE4LuaBinding*> AllBindings;
			BindingRegistry.GetAllBindings(AllBindings);
			for (auto Binding : AllBindings)
			{
				auto TestUObject = Binding->GetBindingClass()->GetDefaultObject();

				VM->ExecuteString(
					TEXT("\n	function Test(uobj)")
					TEXT("\n		return uobj:GetClass()")
					TEXT("\n	end"));
				TestTrue(FString::Printf(TEXT("Call Binding Func: %s"), *Binding->GetBindingClass()->GetName()), FSUE4LuaFunction::CallGlobal<UClass*>(VM.ToSharedRef(), TEXT("Test"), TestUObject) == Binding->GetBindingClass());
			}
		}

		BindingRegistry.PurgeBinding(TestB);
		BindingRegistry.PurgeBinding(TestF);

		{
			TArray<USUE4LuaBinding*> AllBindings;
			BindingRegistry.GetAllBindings(AllBindings);
			TestTrue(TEXT("Remove Binding: TestA"), 5 == AllBindings.Num() && AllBindings[1] == TestA);
			TestTrue(TEXT("Remove Binding: TestE"), 5 == AllBindings.Num() && AllBindings[2] == TestE);
			TestTrue(TEXT("Remove Binding: TestC"), 5 == AllBindings.Num() && AllBindings[3] == TestC);
			TestTrue(TEXT("Remove Binding: TestD"), 5 == AllBindings.Num() && AllBindings[4] == TestD);
		}
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test()")
			TEXT("\n		local AActor = UE4.FindClass('Actor')")
			TEXT("\n		return AActor.GetDebugName(nil) == 'NULL'")
			TEXT("\n	end"));
		TestTrue(TEXT("Call Static Function(AActor.GetDebugName)"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test")));
	}

	return true;
}

//=============================================================================================================================
// FSUE4LuaTestCaseStructBinding
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseStructBinding, "Silvervine.UE4Lua.StructBinding", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseStructBinding::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}

	auto TestUObject = NewObject<USUE4LuaTestUObject>();

	{
		TestUObject->ValueProperty = nullptr;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		local uobjType = uobj:getType()")
			TEXT("\n		return 0 < uobjType:GetFieldNames():Num()")
			TEXT("\n	end"));
		TestTrue(TEXT("UStruct.GetFieldNames()"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), TestUObject));
	}
	{
		TestUObject->ValueProperty = nullptr;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		local uobjType = uobj:getType()")
			TEXT("\n		return 0 < uobjType:GetPropertyNames():Num()")
			TEXT("\n	end"));
		TestTrue(TEXT("UStruct.GetPropertyNames()"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), TestUObject));
	}
	{
		TestUObject->ValueProperty = nullptr;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		local uobjType = uobj:getType()")
			TEXT("\n		return 0 < uobjType:GetFunctionNames():Num()")
			TEXT("\n	end"));
		TestTrue(TEXT("UStruct.GetFunctionNames()"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), TestUObject));
	}

	return true;
}

//=============================================================================================================================
// FSUE4LuaTestCaseClassBinding
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseClassBinding, "Silvervine.UE4Lua.ClassBinding", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseClassBinding::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}
	
	auto TestUObject = NewObject<USUE4LuaTestUObject>();

	{
		TestUObject->ValueProperty = nullptr;

		VM->ExecuteString(
			TEXT("\n	function Test()")
			TEXT("\n		return UE4.FindClass('Actor'):GetDefaultObject() ~= nil")
			TEXT("\n	end"));
		TestTrue(TEXT("UClass.GetDefaultObject()"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test")));
	}

	return true;
}