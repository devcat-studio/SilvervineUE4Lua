// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#include "LuaUnitTests.h"

#include "LuaVirtualMachine.h"


//=============================================================================================================================
// FSUE4LuaTestCaseULuaValue
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseULuaValue, "Silvervine.UE4Lua.ULuaValue", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseULuaValue::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}

	auto TestUObject = NewObject<USUE4LuaTestUObject>();

	{
		bool TestValue = true;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.ValueProperty = value")
			TEXT("\n		return uobj.ValueProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("ULuaValue bool"), FSUE4LuaFunction::CallGlobal<decltype(TestValue)>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestValue) == TestValue);
	}
	{
		int64 TestValue = 123;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.ValueProperty = value")
			TEXT("\n		return uobj.ValueProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("ULuaValue int"), FSUE4LuaFunction::CallGlobal<decltype(TestValue)>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestValue) == TestValue);
	}
	{
		double TestValue = 123.456;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.ValueProperty = value")
			TEXT("\n		return uobj.ValueProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("ULuaValue number"), FSUE4LuaFunction::CallGlobal<decltype(TestValue)>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestValue) == TestValue);
	}
	{
		FString TestValue = TEXT("test");

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.ValueProperty = value")
			TEXT("\n		return uobj.ValueProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("ULuaValue string"), FSUE4LuaFunction::CallGlobal<decltype(TestValue)>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestValue) == TestValue);
	}
	{
		bool TestValue = true;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		uobj.ValueProperty = function () end")
			TEXT("\n		return uobj.ValueProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("ULuaValue function"), FSUE4LuaFunction::CallGlobal<TSharedPtr<FSUE4LuaFunction>>(VM.ToSharedRef(), TEXT("Test"), TestUObject).IsValid());
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		uobj.ValueProperty = {}")
			TEXT("\n	end"));
		FSUE4LuaFunction::CallGlobal<void>(VM.ToSharedRef(), TEXT("Test"), TestUObject);
		TestTrue(TEXT("ULuaValue table"), TestUObject->ValueProperty != nullptr && TestUObject->ValueProperty->IsTableType());

		int32 TestValue = 123;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.ValueProperty.abc = value")
			TEXT("\n		return uobj.ValueProperty.abc")
			TEXT("\n	end"));
		TestTrue(TEXT("ULuaValue table changed"), FSUE4LuaFunction::CallGlobal<decltype(TestValue)>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestValue) == TestValue);
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		uobj.ValueProperty = UE4.NewStruct('SUE4LuaTestStruct')")
			TEXT("\n	end"));
		FSUE4LuaFunction::CallGlobal<void>(VM.ToSharedRef(), TEXT("Test"), TestUObject);
		TestTrue(TEXT("ULuaValue userdata"), TestUObject->ValueProperty != nullptr && TestUObject->ValueProperty->IsUserDataType());
	}
	{
		UObject* TestValue = TestUObject;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.ValueProperty = value")
			TEXT("\n		return uobj.ValueProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("ULuaValue uobject"), FSUE4LuaFunction::CallGlobal<decltype(TestValue)>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestValue) == TestValue);
	}
	{
		TestUObject->ValueProperty = nullptr;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		uobj.ValueProperty = function () end")
			TEXT("\n		return uobj:getObjectProperty('ValueProperty')")
			TEXT("\n	end"));
		TestTrue(TEXT("ULuaValue object property"), FSUE4LuaFunction::CallGlobal<UObject*>(VM.ToSharedRef(), TEXT("Test"), TestUObject) == TestUObject->ValueProperty);
	}
	{
		TestUObject->StructProperty.ValueProperty = nullptr;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		uobj.StructProperty.ValueProperty = function () end")
			TEXT("\n		return uobj.StructProperty:getObjectProperty('ValueProperty')")
			TEXT("\n	end"));
		TestTrue(TEXT("ULuaValue struct property"), FSUE4LuaFunction::CallGlobal<UObject*>(VM.ToSharedRef(), TEXT("Test"), TestUObject) == TestUObject->StructProperty.ValueProperty);
	}
	{
		bool TestValue = true;

		VM->ExecuteString(
			TEXT("\n	function Test(value)")
			TEXT("\n		return value")
			TEXT("\n	end"));
		USUE4LuaValue* LuaValueObj = FSUE4LuaFunction::CallGlobal<USUE4LuaValue*>(VM.ToSharedRef(), TEXT("Test"), TestValue);
		TestTrue(TEXT("ULuaValue GetBool()"), LuaValueObj != nullptr && LuaValueObj->GetBool() == TestValue);
	}
	{
		int64 TestValue = 123;

		VM->ExecuteString(
			TEXT("\n	function Test(value)")
			TEXT("\n		return value")
			TEXT("\n	end"));
		USUE4LuaValue* LuaValueObj = FSUE4LuaFunction::CallGlobal<USUE4LuaValue*>(VM.ToSharedRef(), TEXT("Test"), TestValue);
		TestTrue(TEXT("ULuaValue GetInteger()"), LuaValueObj != nullptr && LuaValueObj->GetInteger() == TestValue);
	}
	{
		double TestValue = 123.456;

		VM->ExecuteString(
			TEXT("\n	function Test(value)")
			TEXT("\n		return value")
			TEXT("\n	end"));
		USUE4LuaValue* LuaValueObj = FSUE4LuaFunction::CallGlobal<USUE4LuaValue*>(VM.ToSharedRef(), TEXT("Test"), TestValue);
		TestTrue(TEXT("ULuaValue GetNumber()"), LuaValueObj != nullptr && FMath::IsNearlyEqual(LuaValueObj->GetNumber(), TestValue));
	}
	{
		FString TestValue = TEXT("hello");

		VM->ExecuteString(
			TEXT("\n	function Test(value)")
			TEXT("\n		return value")
			TEXT("\n	end"));
		USUE4LuaValue* LuaValueObj = FSUE4LuaFunction::CallGlobal<USUE4LuaValue*>(VM.ToSharedRef(), TEXT("Test"), TestValue);
		TestTrue(TEXT("ULuaValue GetString()"), LuaValueObj != nullptr && LuaValueObj->GetString() == TestValue);
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test()")
			TEXT("\n		return function () end")
			TEXT("\n	end"));
		USUE4LuaValue* LuaValueObj = FSUE4LuaFunction::CallGlobal<USUE4LuaValue*>(VM.ToSharedRef(), TEXT("Test"));
		TestTrue(TEXT("ULuaValue GetFunction()"), LuaValueObj != nullptr && LuaValueObj->GetFunction().IsValid());
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test()")
			TEXT("\n		return {}")
			TEXT("\n	end"));
		USUE4LuaValue* LuaValueObj = FSUE4LuaFunction::CallGlobal<USUE4LuaValue*>(VM.ToSharedRef(), TEXT("Test"));
		TestTrue(TEXT("ULuaValue IsTableType()"), LuaValueObj != nullptr && LuaValueObj->IsTableType());
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test(value)")
			TEXT("\n		return value")
			TEXT("\n	end"));
		USUE4LuaValue* LuaValueObj = FSUE4LuaFunction::CallGlobal<USUE4LuaValue*>(VM.ToSharedRef(), TEXT("Test"), FSUE4LuaTestStruct());
		TestTrue(TEXT("ULuaValue IsUserDataType()"), LuaValueObj != nullptr && LuaValueObj->IsUserDataType());
	}
	{
		UObject* TestValue = TestUObject;

		VM->ExecuteString(
			TEXT("\n	function Test(value)")
			TEXT("\n		return value")
			TEXT("\n	end"));
		USUE4LuaValue* LuaValueObj = FSUE4LuaFunction::CallGlobal<USUE4LuaValue*>(VM.ToSharedRef(), TEXT("Test"), TestValue);
		TestTrue(TEXT("ULuaValue GetUObject()"), LuaValueObj != nullptr && LuaValueObj->GetUObject() == TestValue);
	}
	{
		bool TestValue = true;

		VM->ExecuteString(
			TEXT("\n	function Test(value)")
			TEXT("\n		return { Value = value }")
			TEXT("\n	end"));
		USUE4LuaValue* LuaValueObj = FSUE4LuaFunction::CallGlobal<USUE4LuaValue*>(VM.ToSharedRef(), TEXT("Test"), TestValue);
		TestTrue(TEXT("ULuaValue GetTableBool()"), LuaValueObj != nullptr && LuaValueObj->GetTableBool(TEXT("Value")) == TestValue);
	}
	{
		int64 TestValue = 123;

		VM->ExecuteString(
			TEXT("\n	function Test(value)")
			TEXT("\n		return { Value = value }")
			TEXT("\n	end"));
		USUE4LuaValue* LuaValueObj = FSUE4LuaFunction::CallGlobal<USUE4LuaValue*>(VM.ToSharedRef(), TEXT("Test"), TestValue);
		TestTrue(TEXT("ULuaValue GetTableInteger()"), LuaValueObj != nullptr && LuaValueObj->GetTableInteger(TEXT("Value")) == TestValue);
	}
	{
		double TestValue = 123.456;

		VM->ExecuteString(
			TEXT("\n	function Test(value)")
			TEXT("\n		return { Value = value }")
			TEXT("\n	end"));
		USUE4LuaValue* LuaValueObj = FSUE4LuaFunction::CallGlobal<USUE4LuaValue*>(VM.ToSharedRef(), TEXT("Test"), TestValue);
		TestTrue(TEXT("ULuaValue GetTableNumber()"), LuaValueObj != nullptr && FMath::IsNearlyEqual(LuaValueObj->GetTableNumber(TEXT("Value")), TestValue));
	}
	{
		FString TestValue = TEXT("hello");

		VM->ExecuteString(
			TEXT("\n	function Test(value)")
			TEXT("\n		return { Value = value }")
			TEXT("\n	end"));
		USUE4LuaValue* LuaValueObj = FSUE4LuaFunction::CallGlobal<USUE4LuaValue*>(VM.ToSharedRef(), TEXT("Test"), TestValue);
		TestTrue(TEXT("ULuaValue GetTableString()"), LuaValueObj != nullptr && LuaValueObj->GetTableString(TEXT("Value")) == TestValue);
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test()")
			TEXT("\n		return { Value = function () end }")
			TEXT("\n	end"));
		USUE4LuaValue* LuaValueObj = FSUE4LuaFunction::CallGlobal<USUE4LuaValue*>(VM.ToSharedRef(), TEXT("Test"));
		TestTrue(TEXT("ULuaValue GetTableFunction()"), LuaValueObj != nullptr && LuaValueObj->GetTableFunction(TEXT("Value")).IsValid());
	}
	{
		UObject* TestValue = TestUObject;

		VM->ExecuteString(
			TEXT("\n	function Test(value)")
			TEXT("\n		return { Value = value }")
			TEXT("\n	end"));
		USUE4LuaValue* LuaValueObj = FSUE4LuaFunction::CallGlobal<USUE4LuaValue*>(VM.ToSharedRef(), TEXT("Test"), TestValue);
		TestTrue(TEXT("ULuaValue GetTableUObject()"), LuaValueObj != nullptr && LuaValueObj->GetTableUObject(TEXT("Value")) == TestValue);
	}
	{
		int32 TestValue = 123;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		return uobj:TestLuaValueArg(value)")
			TEXT("\n	end"));
		TestTrue(TEXT("ULuaValue passed to parameter"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestValue) == TestValue);
	}

	return true;
}
