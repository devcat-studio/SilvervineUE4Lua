// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaUnitTests.h"

#include "LuaVirtualMachine.h"


//=============================================================================================================================
// FSUE4LuaTestCaseArray
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseArray, "Silvervine.UE4Lua.Array", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseArray::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}

	auto TestUObject = NewObject<USUE4LuaTestUObject>();

	{
		int32 TestIndex = 3;
		int32 TestValue = 123;
		TestUObject->FixedIntArrayProperty[TestIndex] = 0;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, index, value)")
			TEXT("\n		uobj.FixedIntArrayProperty[index] = value")
			TEXT("\n		return uobj.FixedIntArrayProperty[index]")
			TEXT("\n	end"));
		TestTrue(TEXT("int32[].Get"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestIndex, TestValue) == TestValue);
		TestTrue(TEXT("int32[].Set"), TestUObject->FixedIntArrayProperty[TestIndex] == TestValue);
	}
	{
		int32 TestIndex = 3;
		FVector TestValue(1.0f, 2.0f, 3.0f);
		TestUObject->FixedVectorArrayProperty[TestIndex] = FVector::ZeroVector;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, index, value)")
			TEXT("\n		uobj.FixedVectorArrayProperty[index] = value")
			TEXT("\n		return uobj.FixedVectorArrayProperty[index]")
			TEXT("\n	end"));
		TestTrue(TEXT("FVector[].Get"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestIndex, TestValue) == TestValue);
		TestTrue(TEXT("FVector[].Set"), TestUObject->FixedVectorArrayProperty[TestIndex] == TestValue);
	}
	{
		static_assert(sizeof(TestUObject->FixedIntArrayProperty1) == sizeof(TestUObject->FixedIntArrayProperty2), "different array sizes");
		for (int32 i = 0; i < sizeof(TestUObject->FixedIntArrayProperty1) / sizeof(TestUObject->FixedIntArrayProperty1[0]); ++i)
		{
			TestUObject->FixedIntArrayProperty1[i] = i + 1;
			TestUObject->FixedIntArrayProperty2[i] = 0;
		}

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		uobj.FixedIntArrayProperty2 = uobj.FixedIntArrayProperty1")
			TEXT("\n	end"));
		FSUE4LuaFunction::CallGlobal<void>(VM.ToSharedRef(), TEXT("Test"), TestUObject);

		bool bEqual = true;
		for (int32 i = 0; i < sizeof(TestUObject->FixedIntArrayProperty1) / sizeof(TestUObject->FixedIntArrayProperty1[0]); ++i)
		{
			if (TestUObject->FixedIntArrayProperty1[i] != TestUObject->FixedIntArrayProperty2[i])
			{
				bEqual = false;
				break;
			}
		}
		TestTrue(TEXT("int32[] Copy"), bEqual);
	}
	{
		TestUObject->IntArrayProperty.SetNum(123);

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		return uobj.IntArrayProperty:Num()")
			TEXT("\n	end"));
		TestTrue(TEXT("TArray.Num()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject) == TestUObject->IntArrayProperty.Num());
	}
	{
		TestUObject->IntArrayProperty.SetNum(1);

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		uobj.IntArrayProperty:Empty()")
			TEXT("\n	end"));
		FSUE4LuaFunction::CallGlobal<void>(VM.ToSharedRef(), TEXT("Test"), TestUObject);
		TestTrue(TEXT("TArray.Empty()"), TestUObject->IntArrayProperty.Num() == 0);
	}
	{
		int32 TestValue = 123;
		TestUObject->IntArrayProperty.Empty();

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.IntArrayProperty:Add(value)")
			TEXT("\n	end"));
		FSUE4LuaFunction::CallGlobal<void>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestValue);
		TestTrue(TEXT("TArray.Add()"), TestUObject->IntArrayProperty.IsValidIndex(0) && TestUObject->IntArrayProperty[0] == TestValue);
	}
	{
		int32 TestValue = 123;
		TestUObject->IntArrayProperty.Empty();

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.IntArrayProperty:Insert(0, value)")
			TEXT("\n	end"));
		FSUE4LuaFunction::CallGlobal<void>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestValue);
		TestTrue(TEXT("TArray.Insert()"), TestUObject->IntArrayProperty.IsValidIndex(0) && TestUObject->IntArrayProperty[0] == TestValue);
	}
	{
		TestUObject->IntArrayProperty.SetNum(1);

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		uobj.IntArrayProperty:Remove(0)")
			TEXT("\n	end"));
		FSUE4LuaFunction::CallGlobal<void>(VM.ToSharedRef(), TEXT("Test"), TestUObject);
		TestTrue(TEXT("TArray.Remove()"), TestUObject->IntArrayProperty.Num() == 0);
	}
	{
		int32 TestIndex = 3;
		int32 TestValue = 123;
		TestUObject->IntArrayProperty.SetNum(TestIndex + 1);
		TestUObject->IntArrayProperty[TestIndex] = 0;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, index, value)")
			TEXT("\n		uobj.IntArrayProperty[index] = value")
			TEXT("\n		return uobj.IntArrayProperty[index]")
			TEXT("\n	end"));
		TestTrue(TEXT("TArray<int32>.Get"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestIndex, TestValue) == TestValue);
		TestTrue(TEXT("TArray<int32>.Set"), TestUObject->IntArrayProperty[TestIndex] == TestValue);
	}
	{
		int32 TestIndex = 3;
		FVector TestValue(1.0f, 2.0f, 3.0f);
		TestUObject->VectorArrayProperty.SetNum(TestIndex + 1);
		TestUObject->VectorArrayProperty[TestIndex] = FVector::ZeroVector;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, index, value)")
			TEXT("\n		uobj.VectorArrayProperty[index] = value")
			TEXT("\n		return uobj.VectorArrayProperty[index]")
			TEXT("\n	end"));
		TestTrue(TEXT("TArray<FVector>.Get"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestIndex, TestValue) == TestValue);
		TestTrue(TEXT("TArray<FVector>.Set"), TestUObject->VectorArrayProperty[TestIndex] == TestValue);
	}
	{
		FSUE4LuaTestStruct TestValue;
		TestValue.IntProperty = 123;
		TestUObject->StructArrayProperty.Empty();
		TestUObject->StructArrayProperty.SetNum(2);
		TestUObject->StructArrayProperty[0] = TestValue;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		local value = uobj.StructArrayProperty:GetCopy(0)")
			TEXT("\n		uobj.StructArrayProperty:Set(0, uobj.StructArrayProperty:GetCopy(1))")
			TEXT("\n		return value.IntProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("TArray<UStruct>.GetCopy()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject) == TestValue.IntProperty);
	}
	{
		FSUE4LuaTestStruct TestValue;
		TestValue.IntProperty = 123;
		TestUObject->StructArrayProperty.Empty();
		TestUObject->StructArrayProperty.SetNum(2);
		TestUObject->StructArrayProperty[0] = TestValue;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		local ref = uobj.StructArrayProperty:GetRef(1)")
			TEXT("\n		uobj.StructArrayProperty:Set(1, uobj.StructArrayProperty:GetCopy(0))")
			TEXT("\n		return ref.IntProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("TArray<UStruct>.GetRef()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject) == TestValue.IntProperty);
	}
	{
		int32 TestValue = 123;
		TestUObject->StructProperty.IntArrayProperty.Empty();

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.StructProperty.IntArrayProperty:Add(value)")
			TEXT("\n	end"));
		FSUE4LuaFunction::CallGlobal<void>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestValue);
		TestTrue(TEXT("TArray<UStruct>.Add()"), TestUObject->StructProperty.IntArrayProperty.IsValidIndex(0) && TestUObject->StructProperty.IntArrayProperty[0] == TestValue);
	}
	{
		int32 TestValue = 0;
		for (int32 i = 0; i < sizeof(TestUObject->FixedIntArrayProperty) / sizeof(TestUObject->FixedIntArrayProperty[0]); ++i)
		{
			TestUObject->FixedIntArrayProperty[i] = i + 1;
			TestValue += (i + 1) * TestUObject->FixedIntArrayProperty[i];
		}

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		local sum = 0")
			TEXT("\n		for k, v in pairs(uobj.FixedIntArrayProperty) do")
			TEXT("\n			sum = sum + (k + 1) * v")
			TEXT("\n		end")
			TEXT("\n		return sum")
			TEXT("\n	end"));
		TestTrue(TEXT("int32[] pairs()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject) == TestValue);
	}
	{
		TestUObject->IntArrayProperty.SetNum(10);
		int32 TestValue = 0;
		for (int32 i = 0; i < TestUObject->IntArrayProperty.Num(); ++i)
		{
			TestUObject->IntArrayProperty[i] = i + 1;
			TestValue += (i + 1) * TestUObject->IntArrayProperty[i];
		}

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		local sum = 0")
			TEXT("\n		for k, v in pairs(uobj.IntArrayProperty) do")
			TEXT("\n			sum = sum + (k + 1) * v")
			TEXT("\n		end")
			TEXT("\n		return sum")
			TEXT("\n	end"));
		TestTrue(TEXT("TArray pairs()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject) == TestValue);
	}

	return true;
}
