// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#include "LuaUnitTests.h"

#include "LuaVirtualMachine.h"


//=============================================================================================================================
// FSUE4LuaTestCaseDelegate
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseDelegate, "Silvervine.UE4Lua.Delegate", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseDelegate::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}
	
	auto TestUObject = NewObject<USUE4LuaTestUObject>();

	{
		int32 TestValue = 123;

		VM->ExecuteString(
			TEXT("\n	testValue = nil")
			TEXT("\n	local function EventHandler(params)")
			TEXT("\n		testValue = params.InArg1")
			TEXT("\n	end")
			TEXT("\n	function Test(uobj)")
			TEXT("\n		uobj.ValueProperty = EventHandler")
			TEXT("\n		uobj.IntDelegate:Bind(uobj:getObjectProperty('ValueProperty'))")
			TEXT("\n		return uobj.IntDelegate:IsBound()")
			TEXT("\n	end"));
		TestTrue(TEXT("Delegate.Bind()"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), TestUObject));
		TestTrue(TEXT("Delegate.IsBound()"), TestUObject->IntDelegate.IsBound());

		TestUObject->IntDelegate.ExecuteIfBound(TestValue);

		VM->ExecuteString(
			TEXT("\n	function Test()")
			TEXT("\n		return testValue")
			TEXT("\n	end"));
		TestTrue(TEXT("Delegate.ExecuteIfBound()"), FSUE4LuaFunction::CallGlobal<decltype(TestValue)>(VM.ToSharedRef(), TEXT("Test")) == TestValue);

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		uobj.IntDelegate:Unbind()")
			TEXT("\n		return uobj.IntDelegate:IsBound()")
			TEXT("\n	end"));
		TestTrue(TEXT("Delegate.Unbind()"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), TestUObject) == false);
		TestTrue(TEXT("!Delegate.IsBound()"), TestUObject->IntDelegate.IsBound() == false);
	}
	{
		int32 TestValue = 123;

		VM->ExecuteString(
			TEXT("\n	testValue = nil")
			TEXT("\n	local function EventHandler(params)")
			TEXT("\n		testValue = params.InArg1")
			TEXT("\n	end")
			TEXT("\n	function Test(uobj)")
			TEXT("\n		uobj.ValueProperty = EventHandler")
			TEXT("\n		uobj.IntMulticastDelegate:AddUnique(uobj:getObjectProperty('ValueProperty'))")
			TEXT("\n		return uobj.IntMulticastDelegate:Contains(uobj:getObjectProperty('ValueProperty'))")
			TEXT("\n	end"));
		TestTrue(TEXT("MulticastDelegate.AddUnique()"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), TestUObject));
		TestTrue(TEXT("MulticastDelegate.Contains()"), TestUObject->IntMulticastDelegate.Contains(TestUObject->ValueProperty, TEXT("DummyFunc")));

		TestUObject->IntMulticastDelegate.Broadcast(TestValue);

		VM->ExecuteString(
			TEXT("\n	function Test()")
			TEXT("\n		return testValue")
			TEXT("\n	end"));
		TestTrue(TEXT("MulticastDelegate.Broadcast()"), FSUE4LuaFunction::CallGlobal<decltype(TestValue)>(VM.ToSharedRef(), TEXT("Test")) == TestValue);

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		uobj.IntMulticastDelegate:RemoveAll(uobj:getObjectProperty('ValueProperty'))")
			TEXT("\n		return uobj.IntMulticastDelegate:Contains(uobj:getObjectProperty('ValueProperty'))")
			TEXT("\n	end"));
		TestTrue(TEXT("MulticastDelegate.RemoveAll()"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), TestUObject) == false);
	}
	{
		VM->ExecuteString(
			TEXT("\n	callCount = 0")
			TEXT("\n	local function EventHandler1(params)")
			TEXT("\n		callCount = callCount + 1")
			TEXT("\n	end")
			TEXT("\n	local function EventHandler2(params)")
			TEXT("\n		callCount = callCount + 1")
			TEXT("\n	end")
			TEXT("\n	function Test(uobj)")
			TEXT("\n		uobj.ValueProperty1 = EventHandler1")
			TEXT("\n		uobj.ValueProperty2 = EventHandler2")
			TEXT("\n		uobj.IntMulticastDelegate:AddUnique(uobj:getObjectProperty('ValueProperty1'))")
			TEXT("\n		uobj.IntMulticastDelegate:AddUnique(uobj:getObjectProperty('ValueProperty2'))")
			TEXT("\n		return true")
			TEXT("\n	end"));
		FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), TestUObject);

		TestUObject->IntMulticastDelegate.Broadcast(0);
		TestUObject->IntMulticastDelegate.Clear();

		VM->ExecuteString(
			TEXT("\n	function Test()")
			TEXT("\n		return callCount")
			TEXT("\n	end"));
		TestTrue(TEXT("MulticastDelegate.AddUnique() Multiple Times"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test")) == 2);
	}
	{
		int32 TestValue = 123;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.ValueProperty = function (params)")
			TEXT("\n			params.OutArg1 = value")
			TEXT("\n		end")
			TEXT("\n		uobj.IntOutDelegate:Bind(uobj:getObjectProperty('ValueProperty'))")
			TEXT("\n	end"));
		FSUE4LuaFunction::CallGlobal<void>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestValue);

		int32 OutValue = 0;
		TestUObject->IntOutDelegate.ExecuteIfBound(OutValue);
		TestTrue(TEXT("Delegate int32 Out Parameter"), OutValue == TestValue);

		TestUObject->IntOutDelegate.Unbind();
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		uobj.ValueProperty = function (params)")
			TEXT("\n			params.OutArg1.IntProperty = params.InArg1.IntProperty * 2")
			TEXT("\n		end")
			TEXT("\n		uobj.StructDelegate:Bind(uobj:getObjectProperty('ValueProperty'))")
			TEXT("\n	end"));
		FSUE4LuaFunction::CallGlobal<void>(VM.ToSharedRef(), TEXT("Test"), TestUObject);

		FSUE4LuaTestStruct TestInValue;
		FSUE4LuaTestStruct TestOutValue;

		TestInValue.IntProperty = 123;

		TestUObject->StructDelegate.ExecuteIfBound(TestInValue, TestOutValue);
		TestTrue(TEXT("Delegate UStruct Out Parameter"), TestOutValue.IntProperty == TestInValue.IntProperty * 2);

		TestUObject->StructDelegate.Unbind();
	}
	{
		int32 TestValue = 123;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.ValueProperty = function (params)")
			TEXT("\n			params.InArg1:Set(0, value)")
			TEXT("\n		end")
			TEXT("\n		uobj.IntArrayDelegate:Bind(uobj:getObjectProperty('ValueProperty'))")
			TEXT("\n	end"));
		FSUE4LuaFunction::CallGlobal<void>(VM.ToSharedRef(), TEXT("Test"), TestUObject, TestValue);

		TArray<int32> TestContainer;
		TestContainer.Add(0);
		TestUObject->IntArrayDelegate.ExecuteIfBound(TestContainer);
		TestTrue(TEXT("Delegate TArray<int32> Out Parameter"), TestContainer[0] == TestValue);

		TestUObject->IntArrayDelegate.Unbind();
	}

	return true;
}
