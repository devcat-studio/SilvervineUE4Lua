// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "LuaBindingRegistry.h"
#include "SilvervineUE4LuaValue.h"

#include "LuaUnitTests.generated.h"


#define SUE4LUA_ATF_UNITTEST			(EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

UENUM()
enum class ESUE4LuaTestEnum : uint8
{
	Default	= 0,
	Enum1	= 1,
	Enum2	= 2,
};

USTRUCT()
struct FSUE4LuaTestStruct
{
	GENERATED_BODY()
	
	int8 Dummy[128];
	
	UPROPERTY()
	int32 IntProperty = 0;

	UPROPERTY()
	int32 FixedIntArrayProperty[10];

	UPROPERTY()
	TArray<int32> IntArrayProperty;

	UPROPERTY()
	USUE4LuaValue* ValueProperty = nullptr;
};

USTRUCT()
struct FSUE4LuaTestNestedStruct
{
	GENERATED_BODY()
	
	int8 Dummy[128];

	UPROPERTY()
	FSUE4LuaTestStruct StructProperty;
};

USTRUCT()
struct FSUE4LuaTestMultipleNestedStruct
{
	GENERATED_BODY()

	int8 Dummy[128];

	UPROPERTY()
	FSUE4LuaTestNestedStruct NestedStructProperty;
};

USTRUCT()
struct FSUE4LuaTestDerivedStruct : public FSUE4LuaTestStruct
{
	GENERATED_BODY()

	UPROPERTY()
	int32 DerivedIntProperty = 0;
};

//
// This class is used for unit test.
// In order to testing static binding, SILVERVINEUE4LUA_API is used although it is internal class.
//
UCLASS()
class SILVERVINEUE4LUA_API USUE4LuaTestUObject : public UObject
{
	GENERATED_BODY()
	
public:
	int8 Dummy[128];

	UPROPERTY()
	bool BoolProperty = false;

	UPROPERTY()
	int8 ByteProperty = 0;

	UPROPERTY()
	int32 IntProperty = 0;

	UPROPERTY()
	int32 IntProperty1 = 0;

	UPROPERTY()
	int32 IntProperty2 = 0;

	UPROPERTY()
	int64 Int64Property = 0;

	UPROPERTY()
	float FloatProperty = 0.0f;

	UPROPERTY()
	ESUE4LuaTestEnum EnumProperty = ESUE4LuaTestEnum::Default;

	UPROPERTY()
	FName NameProperty;

	UPROPERTY()
	FString StringProperty;

	UPROPERTY()
	FText TextProperty;

	UPROPERTY()
	USUE4LuaTestUObject* UObjectProperty = nullptr;

	UPROPERTY()
	FSUE4LuaTestStruct StructProperty;

	UPROPERTY()
	FSUE4LuaTestStruct StructProperty1;
	UPROPERTY()
	FSUE4LuaTestStruct StructProperty2;
	UPROPERTY()
	FSUE4LuaTestStruct StructProperty3;

	UPROPERTY()
	FSUE4LuaTestNestedStruct NestedStructProperty;

	UPROPERTY()
	FSUE4LuaTestMultipleNestedStruct MultipleNestedStructProperty;

	UPROPERTY()
	int32 FixedIntArrayProperty[10];

	UPROPERTY()
	int32 FixedIntArrayProperty1[20];

	UPROPERTY()
	int32 FixedIntArrayProperty2[20];

	UPROPERTY()
	FVector FixedVectorArrayProperty[10];

	UPROPERTY()
	FSUE4LuaTestStruct FixedStructArrayProperty[10];

	UPROPERTY()
	FSUE4LuaTestNestedStruct FixedNestedStructArrayProperty[10];

	UPROPERTY()
	TArray<int32> IntArrayProperty;

	UPROPERTY()
	TArray<FVector> VectorArrayProperty;

	UPROPERTY()
	TArray<FSUE4LuaTestStruct> StructArrayProperty;

	UPROPERTY()
	TMap<int32, int32> IntMapProperty;

	UPROPERTY()
	TMap<int32, FSUE4LuaTestStruct> StructMapProperty;

	UPROPERTY()
	TMap<FString, int32> StringKeyMapProperty;

	UPROPERTY()
	TSet<int32> IntSetProperty;

	UPROPERTY()
	USUE4LuaValue* ValueProperty;

	UPROPERTY()
	USUE4LuaValue* ValueProperty1;

	UPROPERTY()
	USUE4LuaValue* ValueProperty2;

	UPROPERTY()
	class USUE4LuaBridge* LuaBridge;

	DECLARE_DYNAMIC_DELEGATE_OneParam(FSUE4LuaTestIntDelegate, int32, InArg1);
	UPROPERTY()
	FSUE4LuaTestIntDelegate IntDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSUE4LuaTestIntMulticastDelegate, int32, InArg1);
	UPROPERTY()
	FSUE4LuaTestIntMulticastDelegate IntMulticastDelegate;

	DECLARE_DYNAMIC_DELEGATE_OneParam(FSUE4LuaTestIntOutDelegate, int32&, OutArg1);
	UPROPERTY()
	FSUE4LuaTestIntOutDelegate IntOutDelegate;

	DECLARE_DYNAMIC_DELEGATE_TwoParams(FSUE4LuaTestStructDelegate, const FSUE4LuaTestStruct&, InArg1, FSUE4LuaTestStruct&, OutArg1);
	UPROPERTY()
	FSUE4LuaTestStructDelegate StructDelegate;

	DECLARE_DYNAMIC_DELEGATE_OneParam(FSUE4LuaTestIntArrayDelegate, TArray<int32>&, InArg1);
	UPROPERTY()
	FSUE4LuaTestIntArrayDelegate IntArrayDelegate;

	UPROPERTY()
	TWeakObjectPtr<UObject> WeakObjectProperty;

	UPROPERTY()
	TSoftObjectPtr<UObject> SoftObjectProperty;

	UFUNCTION()
	int32 TestArgs(int32 InIntArg1, int32 InIntArg2, const int32& InIntArg3, int32& OutIntArg1, int32& OutIntArg2);

	UFUNCTION(BlueprintCallable) // Default parameter is supported only for BlueprintCallable
	int32 TestDefaultArgs(int32 InIntArg1 = 123, int32 InIntArg2 = 456);

	UFUNCTION()
	FVector TestVector(const FVector& InArg);

	UFUNCTION()
	ESUE4LuaTestEnum TestEnum(ESUE4LuaTestEnum Arg);

	UFUNCTION()
	FTransform TestTransform(const FTransform& InArg);

	UFUNCTION()
	void TestStruct(FSUE4LuaTestStruct InArg1, const FSUE4LuaTestStruct& InArg2, FSUE4LuaTestStruct& OutArg1);

	UFUNCTION()
	int32 TestLuaValueArg(USUE4LuaValue* InArg);

	UFUNCTION()
	void TestArrayArg(TArray<UObject*>& OutArg, UObject* InArg);

	void TestNativeDispatch(FSUE4LuaTestNestedStruct InParam, const FSUE4LuaTestNestedStruct& InParam2, FSUE4LuaTestNestedStruct& OutParam3);

	UFUNCTION() 
	int32 TestCoroutine(int32 InArg);

	UFUNCTION()
	void TestTimer();

	UFUNCTION()
	class USUE4LuaTestUnknownUObjectType* TestUnknownUObjectType();
};

//
// This class is used for unit test.
//
UCLASS()
class USUE4LuaTestUObjectDerived : public USUE4LuaTestUObject
{
	GENERATED_BODY()
};

//
// This class is used for unit test.
//
UCLASS()
class USUE4TestBinding : public USUE4LuaBinding
{
	GENERATED_BODY()
	
public:
	// Begin USUE4LuaBinding Interface
	virtual UClass* GetBindingClass() const override;
	// End USUE4LuaBinding Interface

	void SetBindingClass(UClass* InClass);

protected:
	UPROPERTY()
	UClass* BindingClass = nullptr;
};