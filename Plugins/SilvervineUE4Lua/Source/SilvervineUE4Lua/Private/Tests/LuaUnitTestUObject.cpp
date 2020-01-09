// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#include "LuaUnitTests.h"

#include "LuaUnitTestUnknownUObjectType.h"
#include "LuaVirtualMachine.h"


//=============================================================================================================================
// FSUE4LuaTestCaseUObject
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseUObject, "Silvervine.UE4Lua.UObject", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseUObject::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}
		
	auto TestUObject = NewObject<USUE4LuaTestUObject>();

	{
		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		return uobj")
			TEXT("\n	end"));
		TestTrue(TEXT("Parameter Passing : UObject*"), FSUE4LuaFunction::CallGlobal<decltype(TestUObject)>(VM.ToSharedRef(), TEXT("Test"), TestUObject) == TestUObject);
		TestTrue(TEXT("Parameter Passing : const UObject*"), FSUE4LuaFunction::CallGlobal<const USUE4LuaTestUObject*>(VM.ToSharedRef(), TEXT("Test"), static_cast<const USUE4LuaTestUObject*>(TestUObject)) == TestUObject);
	}
	{
		bool Arg = true;
		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.BoolProperty = value")
			TEXT("\n		return uobj.BoolProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("UObject Property : bool"), FSUE4LuaFunction::CallGlobal<decltype(Arg)>(VM.ToSharedRef(), TEXT("Test"), TestUObject, Arg) == Arg);
	}
	{
		int8 Arg = 123;
		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.ByteProperty = value")
			TEXT("\n		return uobj.ByteProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("UObject Property : byte"), FSUE4LuaFunction::CallGlobal<decltype(Arg)>(VM.ToSharedRef(), TEXT("Test"), TestUObject, Arg) == Arg);
	}
	{
		int32 Arg = 123;
		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.IntProperty = value")
			TEXT("\n		return uobj.IntProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("UObject Property : int32"), FSUE4LuaFunction::CallGlobal<decltype(Arg)>(VM.ToSharedRef(), TEXT("Test"), TestUObject, Arg) == Arg);
	}
	{
		int64 Arg = 1000ll * 1000ll * 1000ll * 1000ll;
		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.Int64Property = value")
			TEXT("\n		return uobj.Int64Property")
			TEXT("\n	end"));
		TestTrue(TEXT("UObject Property : int64"), FSUE4LuaFunction::CallGlobal<decltype(Arg)>(VM.ToSharedRef(), TEXT("Test"), TestUObject, Arg) == Arg);
	}
	{
		float Arg = 123.4f;
		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.FloatProperty = value")
			TEXT("\n		return uobj.FloatProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("UObject Property : float"), FSUE4LuaFunction::CallGlobal<decltype(Arg)>(VM.ToSharedRef(), TEXT("Test"), TestUObject, Arg) == Arg);
	}
	{
		ESUE4LuaTestEnum Arg = ESUE4LuaTestEnum::Enum2;
		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.EnumProperty = value")
			TEXT("\n		return uobj.EnumProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("UObject Property : enum"), FSUE4LuaFunction::CallGlobal<decltype(Arg)>(VM.ToSharedRef(), TEXT("Test"), TestUObject, Arg) == Arg);
	}
	{
		FName Arg(TEXT("str"));
		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.NameProperty = value")
			TEXT("\n		return uobj.NameProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("UObject Property : name"), FSUE4LuaFunction::CallGlobal<decltype(Arg)>(VM.ToSharedRef(), TEXT("Test"), TestUObject, Arg) == Arg);
	}
	{
		FString Arg(TEXT("str"));
		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.StringProperty = value")
			TEXT("\n		return uobj.StringProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("UObject Property : string"), FSUE4LuaFunction::CallGlobal<decltype(Arg)>(VM.ToSharedRef(), TEXT("Test"), TestUObject, Arg) == Arg);
	}
	{
		FText Arg = FText::FromString(TEXT("str"));
		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.TextProperty = value")
			TEXT("\n		return uobj.TextProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("UObject Property : text"), FSUE4LuaFunction::CallGlobal<decltype(Arg)>(VM.ToSharedRef(), TEXT("Test"), TestUObject, Arg).ToString() == Arg.ToString());
	}
	{
		auto Arg = NewObject<USUE4LuaTestUObject>();
		VM->ExecuteString(
			TEXT("\n	function Test(uobj, value)")
			TEXT("\n		uobj.UObjectProperty = value")
			TEXT("\n		return uobj.UObjectProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("UObject Property : UObject*"), FSUE4LuaFunction::CallGlobal<decltype(Arg)>(VM.ToSharedRef(), TEXT("Test"), TestUObject, Arg) == Arg);
	}
	{
		UObject* TestValue = TestUObject;
		TestUObject->WeakObjectProperty = TestValue;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		return uobj.WeakObjectProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("UObject Property : TWeakObjectPtr"), FSUE4LuaFunction::CallGlobal<decltype(TestValue)>(VM.ToSharedRef(), TEXT("Test"), TestUObject) == TestValue);
	}
	{
		UObject* TestValue = TestUObject;
		TestUObject->SoftObjectProperty = TestValue;

		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		return uobj.SoftObjectProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("UObject Property : TSoftObjectPtr"), FSUE4LuaFunction::CallGlobal<decltype(TestValue)>(VM.ToSharedRef(), TEXT("Test"), TestUObject) == TestValue);
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		return uobj.IntArrayProperty == uobj.IntArrayProperty")
			TEXT("\n	end"));
		TestTrue(TEXT("UObject property proxy caching"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), TestUObject));
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test(uobj)")
			TEXT("\n		return uobj:TestUnknownUObjectType()")
			TEXT("\n	end"));
		TestTrue(TEXT("UObject unknown uobject type passing"), FSUE4LuaFunction::CallGlobal<USUE4LuaTestUnknownUObjectType*>(VM.ToSharedRef(), TEXT("Test"), TestUObject) == TestUObject->TestUnknownUObjectType());
	}

	return true;
}
