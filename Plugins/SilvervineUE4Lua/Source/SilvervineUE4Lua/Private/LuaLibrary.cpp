// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaLibrary.h"

#include "Blueprint/UserWidget.h"
#include "CoreUObject.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Engine/UserDefinedEnum.h"
#include "Events.h"
#include "GameFramework/PlayerController.h"
#include "HAL/PlatformTime.h"
#include "InputCoreTypes.h"
#include "Internationalization/TextFormatter.h"
#include "Kismet/KismetMathLibrary.h"

#include "LuaAPI.h"
#include "LuaLog.h"
#include "LuaFileLoader.h"
#include "LuaFunction.h"
#include "LuaStack.h"
#include "LuaThread.h"
#include "LuaUObject.h"
#include "LuaUProperty.h"
#include "LuaUStruct.h"


//=============================================================================================================================
// SUE4LuaSUE4LuaLibrary
//=============================================================================================================================
namespace SUE4LuaSUE4LuaLibrary
{
	// bool ExecuteString(LuaString)
	static int32 LuaExecuteString(lua_State* L)
	{
		const char* StrPtr = lua_tostring(L, 1);
		if (StrPtr == nullptr)
		{
			SUE4LVM_ERROR(L, TEXT("SUE4Lua.LuaExecuteString(): Invalid arg1"));

			FSUE4LuaStack::Push(L, false);
			return 1;
		}

		if (auto VM = FSUE4LuaVirtualMachine::PinVM(L))
		{
			bool bRet = VM->ExecuteString(L, UTF8_TO_TCHAR(StrPtr));
			FSUE4LuaStack::Push(L, bRet);
		}
		else
		{
			FSUE4LuaStack::Push(L, false);
		}

		return 1;
	}

	// bool ExecuteFile(LuaFilename)
	static int32 LuaExecuteFile(lua_State* L)
	{
		const char* StrPtr = lua_tostring(L, 1);
		if (StrPtr == nullptr)
		{
			SUE4LVM_ERROR(L, TEXT("SUE4Lua.ExecuteFile(): Invalid arg1"));

			FSUE4LuaStack::Push(L, false);
			return 1;
		}

		if (auto VM = FSUE4LuaVirtualMachine::PinVM(L))
		{
			bool bRet = VM->ExecuteFile(L, UTF8_TO_TCHAR(StrPtr));
			FSUE4LuaStack::Push(L, bRet);
		}
		else
		{
			FSUE4LuaStack::Push(L, false);
		}

		return 1;
	}

	// function LoadString(LuaString)
	static int32 LuaLoadString(lua_State* L)
	{
		const char* StrPtr = lua_tostring(L, 1);
		if (StrPtr == nullptr)
		{
			SUE4LVM_ERROR(L, TEXT("SUE4Lua.LoadString(): Invalid arg1"));

			FSUE4LuaStack::Push(L, false);
			return 1;
		}

		if (auto VM = FSUE4LuaVirtualMachine::PinVM(L))
		{
			auto RetFunction = VM->LoadString(L, UTF8_TO_TCHAR(StrPtr));
			FSUE4LuaStack::Push(L, RetFunction);
		}
		else
		{
			lua_pushnil(L);
		}

		return 1;
	}

	// function LoadFile(LuaFilename)
	static int32 LuaLoadFile(lua_State* L)
	{
		const char* StrPtr = lua_tostring(L, 1);
		if (StrPtr == nullptr)
		{
			SUE4LVM_ERROR(L, TEXT("SUE4Lua.LoadFile(): Invalid arg1"));

			FSUE4LuaStack::Push(L, false);
			return 1;
		}

		if (auto VM = FSUE4LuaVirtualMachine::PinVM(L))
		{
			auto RetFunction = VM->LoadFile(L, UTF8_TO_TCHAR(StrPtr));
			FSUE4LuaStack::Push(L, RetFunction);
		}
		else
		{
			lua_pushnil(L);
		}

		return 1;
	}

	// table GetEnumTable(EnumName)
	static int32 LuaGetEnumTable(lua_State* L)
	{
		const char* StrPtr = lua_tostring(L, 1);
		if (StrPtr == nullptr)
		{
			SUE4LVM_ERROR(L, TEXT("SUE4Lua.GetEnumTable(): Invalid arg1"));

			lua_pushnil(L);
			return 1;
		}

		FString ClassName(UTF8_TO_TCHAR(StrPtr));
		UEnum* Class = FindObject<UEnum>(ANY_PACKAGE, *ClassName);

		if (Class != nullptr)
		{
			// Stack: (empty)
			lua_getglobal(L, "UE4");
			lua_getfield(L, -1, "Enum");
			lua_getfield(L, -1, "new");
			lua_remove(L, -2);
			lua_remove(L, -2);
			lua_pcall(L, 0, 1, 0);
			// Stack: Table

			check(0 < Class->NumEnums());

			const bool bUserDefinedEnum = Class->IsA<UUserDefinedEnum>();

			for (int32 Index = 0; Index < Class->NumEnums() - 1; ++Index)	// 자동생성되는 _Max enum값은 제외하려고 1을 뺌
			{
				if (bUserDefinedEnum)
				{
					// BP enum은 디스플레이 이름이 c++ enum의 이름에 해당
					FSUE4LuaStack::Push(L, Class->GetDisplayNameTextByIndex(Index));
				}
				else
				{
					// 타입이름을 제거하기 위해 GetNameByIndex()을 사용하지 않음
					FSUE4LuaStack::Push(L, Class->GetNameStringByIndex(Index));
				}

				FSUE4LuaStack::Push(L, Class->GetValueByIndex(Index));
				lua_settable(L, -3);
			}

			// Stack: Table
			return 1;
		}
		else
		{
			SUE4LVM_WARNING(L, TEXT("SUE4Lua.GetEnumTable(): class '%s' not found"), *ClassName);
			return 0;
		}
	}

	// bool IsDebuggable()
	static int32 LuaIsDebuggable(lua_State* L)
	{
		if (auto VM = FSUE4LuaVirtualMachine::PinVM(L))
		{
			FSUE4LuaStack::Push(L, VM->IsDebuggable());
		}
		else
		{
			FSUE4LuaStack::Push(L, false);
		}

		return 1;
	}

	// void SetDispatchHandlerFactory(function)
	static int32 LuaSetDispatchHandlerFactory(lua_State* L)
	{
		auto LuaFunction = FSUE4LuaFunction::CreateFromStack(L, 1);

		if (auto VM = FSUE4LuaVirtualMachine::PinVM(L))
		{
			VM->SetDispatchHandlerFactory(LuaFunction);
		}

		return 0;
	}

	// bool IsFixedArray(userdata)
	static int32 LuaIsFixedArray(lua_State* L)
	{
		FSUE4LuaStack::Push(L, SUE4LuaUProperty::IsFixedArray(L, 1));
		return 1;
	}

	// bool IsArray(userdata)
	static int32 LuaIsArray(lua_State* L)
	{
		FSUE4LuaStack::Push(L, SUE4LuaUProperty::IsArray(L, 1));
		return 1;
	}

	// bool IsMap(userdata)
	static int32 LuaIsMap(lua_State* L)
	{
		FSUE4LuaStack::Push(L, SUE4LuaUProperty::IsMap(L, 1));
		return 1;
	}

	// bool IsSet(userdata)
	static int32 LuaIsSet(lua_State* L)
	{
		FSUE4LuaStack::Push(L, SUE4LuaUProperty::IsSet(L, 1));
		return 1;
	}

	static void Register(lua_State* L)
	{
		lua_getglobal(L, "SUE4Lua");
		// Stack: SUE4Lua

		lua_pushcfunction(L, &LuaExecuteString);
		lua_setfield(L, -2, "ExecuteString");

		lua_pushcfunction(L, &LuaExecuteFile);
		lua_setfield(L, -2, "ExecuteFile");

		lua_pushcfunction(L, &LuaLoadString);
		lua_setfield(L, -2, "LoadString");

		lua_pushcfunction(L, &LuaLoadFile);
		lua_setfield(L, -2, "LoadFile");

		lua_pushcfunction(L, &LuaGetEnumTable);
		lua_setfield(L, -2, "GetEnumTable");

		lua_pushcfunction(L, &LuaIsDebuggable);
		lua_setfield(L, -2, "IsDebuggable");

		lua_pushcfunction(L, &LuaSetDispatchHandlerFactory);
		lua_setfield(L, -2, "SetDispatchHandlerFactory");

		lua_pushcfunction(L, &LuaIsFixedArray);
		lua_setfield(L, -2, "IsFixedArray");

		lua_pushcfunction(L, &LuaIsArray);
		lua_setfield(L, -2, "IsArray");

		lua_pushcfunction(L, &LuaIsMap);
		lua_setfield(L, -2, "IsMap");

		lua_pushcfunction(L, &LuaIsSet);
		lua_setfield(L, -2, "IsSet");

		// Stack: SUE4Lua
		lua_pop(L, 1);
		// Stack: (empty)
	}
}

//=============================================================================================================================
// SUE4LuaUE4Library
//=============================================================================================================================
namespace SUE4LuaUE4Library
{
	// UClass* FindClass(ClassName)
	static int32 LuaFindClass(lua_State* L)
	{
		const char* StrPtr = lua_tostring(L, 1);
		if (StrPtr == nullptr)
		{
			SUE4LVM_ERROR(L, TEXT("UE4.FindClass(): Invalid arg1"));

			lua_pushnil(L);
			return 1;
		}

		FString ClassName(UTF8_TO_TCHAR(StrPtr));
		UClass* Class = FindObject<UClass>(ANY_PACKAGE, *ClassName);

		if (Class != nullptr)
		{
			SUE4LuaUObject::PushRef(L, Class);
			return 1;
		}
		else
		{
			SUE4LVM_WARNING(L, TEXT("UE4.FindClass(): class '%s' not found"), *ClassName);

			lua_pushnil(L);
			return 1;
		}
	}

	// UObject* LoadObject(Class, Outer, Name)
	static int32 LuaLoadObject(lua_State* L)
	{
		if (lua_gettop(L) < 3)
		{
			SUE4LVM_ERROR(L, TEXT("UE4.LoadObject(): Invalid argument count"));

			lua_pushnil(L);
			return 1;
		}

		UObject* ClassObject = SUE4LuaUObject::ToRef(L, 1);
		if (!(ClassObject != nullptr && ClassObject->IsA<UClass>()))
		{
			SUE4LVM_ERROR(L, TEXT("UE4.LoadObject(): Invalid arg1"));

			lua_pushnil(L);
			return 1;
		}

		UObject* Outer = SUE4LuaUObject::ToRef(L, 2);

		FString Name;
		{
			const char* StrPtr = lua_tostring(L, 3);
			if (StrPtr == nullptr)
			{
				SUE4LVM_ERROR(L, TEXT("UE4.LoadObject(): Invalid arg3"));

				lua_pushnil(L);
				return 1;
			}

			Name = UTF8_TO_TCHAR(StrPtr);
		}

		UClass* Class = Cast<UClass>(ClassObject);
		UObject* ReturnObject = StaticLoadObject(Class, Outer, *Name);

		SUE4LuaUObject::PushRef(L, ReturnObject);
		return 1;
	}

	// UObject* NewObject(Class, Outer)
	static int32 LuaNewObject(lua_State* L)
	{
		int32 ArgCount = lua_gettop(L);

		if (ArgCount < 1)
		{
			SUE4LVM_ERROR(L, TEXT("UE4.NewObject(): Invalid argument count"));

			lua_pushnil(L);
			return 1;
		}

		UObject* ClassObject = SUE4LuaUObject::ToRef(L, 1);
		if (!(ClassObject != nullptr && ClassObject->IsA<UClass>()))
		{
			SUE4LVM_ERROR(L, TEXT("UE4.NewObject(): Invalid arg1"));

			lua_pushnil(L);
			return 1;
		}

		UObject* Outer = 1 < ArgCount ? SUE4LuaUObject::ToRef(L, 2) : GetTransientPackage();

		UClass* Class = Cast<UClass>(ClassObject);
		UObject* ReturnObject = StaticConstructObject_Internal(Class, Outer, NAME_None, RF_NoFlags, EInternalObjectFlags::None, nullptr, false, nullptr);

		SUE4LuaUObject::PushRef(L, ReturnObject);
		return 1;
	}

	// void* NewStruct(StructName)
	static int32 LuaNewStruct(lua_State* L)
	{
		if (lua_gettop(L) < 1)
		{
			SUE4LVM_ERROR(L, TEXT("UE4.NewStruct(): Invalid argument count"));

			lua_pushnil(L);
			return 1;
		}

		FString StructName = FSUE4LuaStack::To<FString>(L, 1);
		if (StructName.IsEmpty())
		{
			SUE4LVM_ERROR(L, TEXT("UE4.NewStruct(): Invalid arg1"));

			lua_pushnil(L);
			return 1;
		}

		UObject* StructObject = StaticFindObject(UScriptStruct::StaticClass(), ANY_PACKAGE, *StructName);
		if (!(StructObject != nullptr && StructObject->IsA<UScriptStruct>()))
		{
			SUE4LVM_ERROR(L, TEXT("UE4.NewStruct(): class '%s' not found"), *StructName);

			lua_pushnil(L);
			return 1;
		}

		UScriptStruct* Struct = Cast<UScriptStruct>(StructObject);
		SUE4LuaUStruct::PushValue(L, Struct);

		return 1;
	}

	// UWidget* CreateWidget(OwningObject, Class)
	static int32 LuaCreateWidget(lua_State* L)
	{
		if (lua_gettop(L) < 2)
		{
			SUE4LVM_ERROR(L, TEXT("UE4.CreateWidget(): Invalid argument count"));

			lua_pushnil(L);
			return 1;
		}

		UObject* OwningObject = SUE4LuaUObject::ToRef(L, 1);
		if (OwningObject == nullptr)
		{
			SUE4LVM_ERROR(L, TEXT("UE4.CreateWidget(): Invalid 1"));

			lua_pushnil(L);
			return 1;
		}

		UObject* ClassObject = SUE4LuaUObject::ToRef(L, 2);
		if (!(ClassObject != nullptr && ClassObject->IsA<UClass>()))
		{
			SUE4LVM_ERROR(L, TEXT("UE4.CreateWidget(): Invalid 2"));

			lua_pushnil(L);
			return 1;
		}

		UClass* Class = Cast<UClass>(ClassObject);

		if (!Class->IsChildOf<UUserWidget>())
		{
			SUE4LVM_ERROR(L, TEXT("UE4.CreateWidget(): CreateWidget can only be used to create UserWidget instances."));

			lua_pushnil(L);
			return 1;
		}

		UWidget* Widget = nullptr;
		if (auto World = Cast<UWorld>(OwningObject))
		{
			Widget = CreateWidget<UUserWidget>(World, Class);
		}
		else if (auto GameInstance = Cast<UGameInstance>(OwningObject))
		{
			Widget = CreateWidget<UUserWidget>(GameInstance, Class);
		}
		else if (auto PlayerController = Cast<APlayerController>(OwningObject))
		{
			Widget = CreateWidget<UUserWidget>(PlayerController, Class);
		}
		else
		{
			SUE4LVM_ERROR(L, TEXT("UE4.CreateWidget(): The type of owning object must be World, GameInstance or PlayerController: %s(%s)"), *OwningObject->GetName(), *OwningObject->GetClass()->GetName());

			lua_pushnil(L);
			return 1;
		}

		SUE4LuaUObject::PushRef(L, Widget);
		return 1;
	}

	// bool IsValid(UObject)
	static int32 LuaIsValid(lua_State* L)
	{
		UObject* Object = SUE4LuaUObject::ToRef(L, 1);

		FSUE4LuaStack::Push(L, ::IsValid(Object));

		return 1;
	}

	static void Register(lua_State* L)
	{
		lua_getglobal(L, "UE4");
		// Stack: UE4

		lua_pushcfunction(L, &LuaFindClass);
		lua_setfield(L, -2, "FindClass");

		lua_pushcfunction(L, &LuaLoadObject);
		lua_setfield(L, -2, "LoadObject");

		lua_pushcfunction(L, &LuaNewObject);
		lua_setfield(L, -2, "NewObject");

		lua_pushcfunction(L, &LuaNewStruct);
		lua_setfield(L, -2, "NewStruct");

		lua_pushcfunction(L, &LuaCreateWidget);
		lua_setfield(L, -2, "CreateWidget");

		lua_pushcfunction(L, &LuaIsValid);
		lua_setfield(L, -2, "IsValid");

		// Stack: UE4
		lua_pop(L, 1);
		// Stack: (empty)
	}
}

//=============================================================================================================================
// SUE4LuaPlatformTimeLibrary
//=============================================================================================================================
namespace SUE4LuaPlatformTimeLibrary
{
	// float PlatformTime()
	// #todo: 유닛테스트 추가
	static int32 LuaPlatformTimeSeconds(lua_State* L)
	{
		FSUE4LuaStack::Push(L, FPlatformTime::Seconds());
		return 1;
	}

	static void Register(lua_State* L)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "PlatformTime");
		// Stack: UE4, PlatformTime

		lua_pushstring(L, "Seconds");
		lua_pushcfunction(L, &LuaPlatformTimeSeconds);
		lua_rawset(L, -3);

		// Stack: UE4, PlatformTime
		lua_pop(L, 2);
		// Stack: (empty)
	}
}

//=============================================================================================================================
// SUE4LuaMathLibrary
//=============================================================================================================================
namespace SUE4LuaMathLibrary
{
	// float Hypotenuse(Width, Height)
	static int32 LuaHypotenuse(lua_State* L)
	{
		float Width = FSUE4LuaStack::To<float>(L, 1);
		float Height = FSUE4LuaStack::To<float>(L, 2);

		Width = FMath::Abs(Width);
		Height = FMath::Abs(Height);
		float Min = FGenericPlatformMath::Min(Width, Height);
		float Max = FGenericPlatformMath::Max(Width, Height);
		float Ratio = Min / Max;

		float Result = Max * FMath::Sqrt(1.f + Ratio * Ratio);

		FSUE4LuaStack::Push(L, Result);

		return 1;
	}

	// float GridSnap(Location, GridSize)
	static int32 LuaGridSnap(lua_State* L)
	{
		float Location = FSUE4LuaStack::To<float>(L, 1);
		float GridSize = FSUE4LuaStack::To<float>(L, 2);

		FSUE4LuaStack::Push(L, FMath::GridSnap(Location, GridSize));

		return 1;
	}

	// float ClampAngle(AngleDegrees, MinAngleDegrees, MaxAngleDegrees)
	static int32 LuaClampAngle(lua_State* L)
	{
		float AngleDegrees = FSUE4LuaStack::To<float>(L, 1);
		float MinAngleDegrees = FSUE4LuaStack::To<float>(L, 2);
		float MaxAngleDegrees = FSUE4LuaStack::To<float>(L, 3);

		FSUE4LuaStack::Push(L, FMath::ClampAngle(AngleDegrees, MinAngleDegrees, MaxAngleDegrees));

		return 1;
	}

	// float Clamp(Value, Min, Max)
	static int32 LuaClamp(lua_State* L)
	{
		float Value = FSUE4LuaStack::To<float>(L, 1);
		float Min = FSUE4LuaStack::To<float>(L, 2);
		float Max = FSUE4LuaStack::To<float>(L, 3);

		FSUE4LuaStack::Push(L, FMath::Clamp(Value, Min, Max));

		return 1;
	}

	// float Lerp(F1, F2, Alpha)
	static int32 LuaLerp(lua_State* L)
	{
		float F1 = FSUE4LuaStack::To<float>(L, 1);
		float F2 = FSUE4LuaStack::To<float>(L, 2);
		float Alpha = FSUE4LuaStack::To<float>(L, 3);

		FSUE4LuaStack::Push(L, F1 + Alpha * (F2 - F1));

		return 1;
	}

	// int Round(F)
	static int32 LuaRound(lua_State* L)
	{
		float F = FSUE4LuaStack::To<float>(L, 1);

		FSUE4LuaStack::Push(L, FMath::RoundToInt(F));

		return 1;
	}

	// int Truncate(F)
	static int32 LuaTruncate(lua_State* L)
	{
		float F = FSUE4LuaStack::To<float>(L, 1);

		FSUE4LuaStack::Push(L, FMath::TruncToInt(F));

		return 1;
	}

	// IntVector TruncateVector(V)
	static int32 LuaTruncateVector(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector>(L, 1);

		FSUE4LuaStack::Push(L, FIntVector(FMath::TruncToInt(V.X),
			FMath::TruncToInt(V.Y),
			FMath::TruncToInt(V.Z)));

		return 1;
	}

	// float SignOfFloat(F)
	static int32 LuaSignOfFloat(lua_State* L)
	{
		float F = FSUE4LuaStack::To<float>(L, 1);

		FSUE4LuaStack::Push(L, FMath::Sign(F));

		return 1;
	}

	// float NormalizeToRange(Value, RangeMin, RangeMax)
	static int32 LuaNormalizeToRange(lua_State* L)
	{
		float Value = FSUE4LuaStack::To<float>(L, 1);
		float RangeMin = FSUE4LuaStack::To<float>(L, 2);
		float RangeMax = FSUE4LuaStack::To<float>(L, 3);

		float Result;

		if (RangeMin == RangeMax)
		{
			if (Value < RangeMin)
			{
				Result = 0.f;
			}
			else
			{
				Result = 1.f;
			}
		}
		else
		{
			if (RangeMin > RangeMax)
			{
				Swap(RangeMin, RangeMax);
			}
			Result = (Value - RangeMin) / (RangeMax - RangeMin);
		}

		FSUE4LuaStack::Push(L, Result);

		return 1;
	}

	// float MapRangeUnclamped(Value, InRangeA, InRangeB, OutRangeA, OutRangeB)
	static int32 LuaMapRangeUnclamped(lua_State* L)
	{
		float Value = FSUE4LuaStack::To<float>(L, 1);
		float InRangeA = FSUE4LuaStack::To<float>(L, 2);
		float InRangeB = FSUE4LuaStack::To<float>(L, 3);
		float OutRangeA = FSUE4LuaStack::To<float>(L, 4);
		float OutRangeB = FSUE4LuaStack::To<float>(L, 5);

		FSUE4LuaStack::Push(L, FMath::GetMappedRangeValueUnclamped(FVector2D(InRangeA, InRangeB), FVector2D(OutRangeA, OutRangeB), Value));

		return 1;
	}

	// float MapRangeClamped(Value, InRangeA, InRangeB, OutRangeA, OutRangeB)
	static int32 LuaMapRangeClamped(lua_State* L)
	{
		float Value = FSUE4LuaStack::To<float>(L, 1);
		float InRangeA = FSUE4LuaStack::To<float>(L, 2);
		float InRangeB = FSUE4LuaStack::To<float>(L, 3);
		float OutRangeA = FSUE4LuaStack::To<float>(L, 4);
		float OutRangeB = FSUE4LuaStack::To<float>(L, 5);

		FSUE4LuaStack::Push(L, FMath::GetMappedRangeValueClamped(FVector2D(InRangeA, InRangeB), FVector2D(OutRangeA, OutRangeB), Value));

		return 1;
	}

	// float InterpEaseInOut(F1, F2, Alpha, Exponent)
	static int32 LuaInterpEaseInOut(lua_State* L)
	{
		float F1 = FSUE4LuaStack::To<float>(L, 1);
		float F2 = FSUE4LuaStack::To<float>(L, 2);
		float Alpha = FSUE4LuaStack::To<float>(L, 3);
		float Exponent = FSUE4LuaStack::To<float>(L, 4);

		FSUE4LuaStack::Push(L, FMath::InterpEaseInOut(F1, F2, Alpha, Exponent));

		return 1;
	}

	// float MakePulsatingValue(InCurrentTime, InPulsesPerSecond, InPhase)
	static int32 LuaMakePulsatingValue(lua_State* L)
	{
		float InCurrentTime = FSUE4LuaStack::To<float>(L, 1);
		float InPulsesPerSecond = FSUE4LuaStack::To<float>(L, 2);

		if (lua_isnumber(L, 3))
		{
			float InPhase = FSUE4LuaStack::To<float>(L, 3);
			FSUE4LuaStack::Push(L, FMath::MakePulsatingValue((double)InCurrentTime, InPulsesPerSecond, InPhase));
		}
		else
		{
			FSUE4LuaStack::Push(L, FMath::MakePulsatingValue((double)InCurrentTime, InPulsesPerSecond));
		}

		return 1;
	}

	// float FixedTurn(InCurrent, InDesired, InDeltaRate)
	static int32 LuaFixedTurn(lua_State* L)
	{
		float InCurrent = FSUE4LuaStack::To<float>(L, 1);
		float InDesired = FSUE4LuaStack::To<float>(L, 2);
		float InDeltaRate = FSUE4LuaStack::To<float>(L, 3);

		FSUE4LuaStack::Push(L, FMath::FixedTurn(InCurrent, InDesired, InDeltaRate));

		return 1;
	}

	// float RandomFloat(Min, Max)
	// #todo: 유닛테스트 추가	
	static int32 LuaRandomFloat(lua_State* L)
	{
		if (lua_gettop(L) >= 2)
		{
			float Lhs = FSUE4LuaStack::To<float>(L, 1);
			float Rhs = FSUE4LuaStack::To<float>(L, 2);
			FSUE4LuaStack::Push(L, FMath::RandRange(Lhs, Rhs));
		}
		else if (lua_gettop(L) == 1)
		{
			float Rhs = FSUE4LuaStack::To<float>(L, 1);
			FSUE4LuaStack::Push(L, FMath::RandRange(0.0f, Rhs));
		}
		else
		{
			FSUE4LuaStack::Push(L, UKismetMathLibrary::RandomFloat());
		}

		return 1;
	}

	// int RandomInteger(Min, Max)
	// #todo: 유닛테스트 추가	
	static int32 LuaRandomInteger(lua_State* L)
	{
		if (lua_gettop(L) >= 2)
		{
			int32 Lhs = FSUE4LuaStack::To<int32>(L, 1);
			int32 Rhs = FSUE4LuaStack::To<int32>(L, 2);
			FSUE4LuaStack::Push(L, FMath::RandRange(Lhs, Rhs));
		}
		else if (lua_gettop(L) == 1)
		{
			int32 Rhs = FSUE4LuaStack::To<int32>(L, 1);
			FSUE4LuaStack::Push(L, FMath::RandRange(0, Rhs));
		}
		else
		{
			FSUE4LuaStack::Push(L, UKismetMathLibrary::RandomInteger(INT32_MAX));
		}

		return 1;
	}

	// float Sin(Rad)
	// #todo: 유닛테스트 추가	
	static int32 LuaSin(lua_State* L)
	{
		if (lua_gettop(L) >= 1)
		{
			float Rad = FSUE4LuaStack::To<float>(L, 1);
			FSUE4LuaStack::Push(L, UKismetMathLibrary::Sin(Rad));
		}
		else
		{
			SUE4LVM_ERROR(L, TEXT("UE4.Math.Sin(): input radian angle is nil."));

			lua_pushnil(L);
		}

		return 1;
	}

	// float Asin(Value)
	// #todo: 유닛테스트 추가	
	static int32 LuaAsin(lua_State* L)
	{
		if (lua_gettop(L) >= 1)
		{
			float A = FSUE4LuaStack::To<float>(L, 1);
			FSUE4LuaStack::Push(L, UKismetMathLibrary::Asin(A));
		}
		else
		{
			SUE4LVM_ERROR(L, TEXT("UE4.Math.Asin(): input value is nil."));

			lua_pushnil(L);
		}

		return 1;
	}

	// float Cos(Rad)
	// #todo: 유닛테스트 추가	
	static int32 LuaCos(lua_State* L)
	{
		if (lua_gettop(L) >= 1)
		{
			float Rad = FSUE4LuaStack::To<float>(L, 1);
			FSUE4LuaStack::Push(L, UKismetMathLibrary::Cos(Rad));
		}
		else
		{
			SUE4LVM_ERROR(L, TEXT("UE4.Math.Cos(): input radian angle is nil."));

			lua_pushnil(L);
		}

		return 1;
	}

	// float Acos(Value)
	// #todo: 유닛테스트 추가	
	static int32 LuaAcos(lua_State* L)
	{
		if (lua_gettop(L) >= 1)
		{
			float A = FSUE4LuaStack::To<float>(L, 1);
			FSUE4LuaStack::Push(L, UKismetMathLibrary::Acos(A));
		}
		else
		{
			SUE4LVM_ERROR(L, TEXT("UE4.Math.Acos(): input value is nil."));

			lua_pushnil(L);
		}

		return 1;
	}

	// float Tan(Rad)
	// #todo: 유닛테스트 추가	
	static int32 LuaTan(lua_State* L)
	{
		if (lua_gettop(L) >= 1)
		{
			float Rad = FSUE4LuaStack::To<float>(L, 1);
			FSUE4LuaStack::Push(L, UKismetMathLibrary::Tan(Rad));
		}
		else
		{
			SUE4LVM_ERROR(L, TEXT("UE4.Math.Tan(): input radian angle is nil."));

			lua_pushnil(L);
		}

		return 1;
	}

	// float Atan(Value)
	// #todo: 유닛테스트 추가	
	static int32 LuaAtan(lua_State* L)
	{
		if (lua_gettop(L) >= 1)
		{
			float A = FSUE4LuaStack::To<float>(L, 1);
			FSUE4LuaStack::Push(L, UKismetMathLibrary::Atan(A));
		}
		else
		{
			SUE4LVM_ERROR(L, TEXT("UE4.Math.Atan(): input value is nil."));

			lua_pushnil(L);
		}

		return 1;
	}

	// float Atan2(A, B)
	// #todo: 유닛테스트 추가	
	static int32 LuaAtan2(lua_State* L)
	{
		if (lua_gettop(L) >= 2)
		{
			float A = FSUE4LuaStack::To<float>(L, 1);
			float B = FSUE4LuaStack::To<float>(L, 2);
			FSUE4LuaStack::Push(L, UKismetMathLibrary::Atan2(A, B));
		}
		else
		{
			SUE4LVM_ERROR(L, TEXT("UE4.Math.Atan2(): invalid argument count."));

			lua_pushnil(L);
		}

		return 1;
	}

	// float DegreesToRadians(Degree)
	// #todo: 유닛테스트 추가	
	static int32 LuaDegreesToRadians(lua_State* L)
	{
		if (lua_gettop(L) >= 1)
		{
			float A = FSUE4LuaStack::To<float>(L, 1);
			FSUE4LuaStack::Push(L, UKismetMathLibrary::DegreesToRadians(A));
		}
		else
		{
			SUE4LVM_ERROR(L, TEXT("UE4.Math.DegreesToRadians(): invalid argument count."));

			lua_pushnil(L);
		}

		return 1;
	}

	// float RadiansToDegrees(Rad)
	// #todo: 유닛테스트 추가	
	static int32 LuaRadiansToDegrees(lua_State* L)
	{
		if (lua_gettop(L) >= 1)
		{
			float A = FSUE4LuaStack::To<float>(L, 1);
			FSUE4LuaStack::Push(L, UKismetMathLibrary::RadiansToDegrees(A));
		}
		else
		{
			SUE4LVM_ERROR(L, TEXT("UE4.Math.RadiansToDegrees(): invalid argument count."));

			lua_pushnil(L);
		}

		return 1;
	}

	// bool IsNearlyZero(Value)
	// #todo: 유닛테스트 추가	
	static int32 LuaIsNearlyZero(lua_State* L)
	{
		if (lua_gettop(L) >= 1)
		{
			float A = FSUE4LuaStack::To<float>(L, 1);
			FSUE4LuaStack::Push(L, FMath::IsNearlyZero(A));
		}
		else
		{
			SUE4LVM_ERROR(L, TEXT("UE4.Math.IsNearlyZero(): invalid argument count."));

			lua_pushnil(L);
		}

		return 1;
	}

	static void Register(lua_State* L)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "Math");
		// Stack: UE4, Math

		lua_pushstring(L, "Hypotenuse");
		lua_pushcfunction(L, &LuaHypotenuse);
		lua_rawset(L, -3);

		lua_pushstring(L, "GridSnap");
		lua_pushcfunction(L, &LuaGridSnap);
		lua_rawset(L, -3);

		lua_pushstring(L, "ClampAngle");
		lua_pushcfunction(L, &LuaClampAngle);
		lua_rawset(L, -3);

		lua_pushstring(L, "Clamp");
		lua_pushcfunction(L, &LuaClamp);
		lua_rawset(L, -3);

		lua_pushstring(L, "Lerp");
		lua_pushcfunction(L, &LuaLerp);
		lua_rawset(L, -3);

		lua_pushstring(L, "Round");
		lua_pushcfunction(L, &LuaRound);
		lua_rawset(L, -3);

		lua_pushstring(L, "Truncate");
		lua_pushcfunction(L, &LuaTruncate);
		lua_rawset(L, -3);

		lua_pushstring(L, "TruncateVector");
		lua_pushcfunction(L, &LuaTruncateVector);
		lua_rawset(L, -3);

		lua_pushstring(L, "SignOfFloat");
		lua_pushcfunction(L, &LuaSignOfFloat);
		lua_rawset(L, -3);

		lua_pushstring(L, "NormalizeToRange");
		lua_pushcfunction(L, &LuaNormalizeToRange);
		lua_rawset(L, -3);

		lua_pushstring(L, "MapRangeUnclamped");
		lua_pushcfunction(L, &LuaMapRangeUnclamped);
		lua_rawset(L, -3);

		lua_pushstring(L, "MapRangeClamped");
		lua_pushcfunction(L, &LuaMapRangeClamped);
		lua_rawset(L, -3);
		
		lua_pushstring(L, "InterpEaseInOut");
		lua_pushcfunction(L, &LuaInterpEaseInOut);
		lua_rawset(L, -3);

		lua_pushstring(L, "MakePulsatingValue");
		lua_pushcfunction(L, &LuaMakePulsatingValue);
		lua_rawset(L, -3);

		lua_pushstring(L, "FixedTurn");
		lua_pushcfunction(L, &LuaFixedTurn);
		lua_rawset(L, -3);

		lua_pushstring(L, "RandomFloat");
		lua_pushcfunction(L, &LuaRandomFloat);
		lua_rawset(L, -3);

		lua_pushstring(L, "RandomInteger");
		lua_pushcfunction(L, &LuaRandomInteger);
		lua_rawset(L, -3);

		lua_pushstring(L, "Sin");
		lua_pushcfunction(L, &LuaSin);
		lua_rawset(L, -3);

		lua_pushstring(L, "Asin");
		lua_pushcfunction(L, &LuaAsin);
		lua_rawset(L, -3);

		lua_pushstring(L, "Cos");
		lua_pushcfunction(L, &LuaCos);
		lua_rawset(L, -3);

		lua_pushstring(L, "Acos");
		lua_pushcfunction(L, &LuaAcos);
		lua_rawset(L, -3);

		lua_pushstring(L, "Tan");
		lua_pushcfunction(L, &LuaTan);
		lua_rawset(L, -3);

		lua_pushstring(L, "Atan");
		lua_pushcfunction(L, &LuaAtan);
		lua_rawset(L, -3);

		lua_pushstring(L, "Atan2");
		lua_pushcfunction(L, &LuaAtan2);
		lua_rawset(L, -3);

		lua_pushstring(L, "DegreesToRadians");
		lua_pushcfunction(L, &LuaDegreesToRadians);
		lua_rawset(L, -3);

		lua_pushstring(L, "RadiansToDegrees");
		lua_pushcfunction(L, &LuaRadiansToDegrees);
		lua_rawset(L, -3);

		lua_pushstring(L, "IsNearlyZero");
		lua_pushcfunction(L, &LuaIsNearlyZero);
		lua_rawset(L, -3);

		lua_pop(L, 2);
		// Stack: (empty)
	}
}

//=============================================================================================================================
// namespace SUE4LuaVectorLibrary
//=============================================================================================================================
namespace SUE4LuaVectorLibrary
{
	// operator-(V)
	static int32 LuaOperatorUnaryMinus(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector>(L, 1);

		FSUE4LuaStack::Push(L, -V);
		return 1;
	}

	// operator+(V1, V2)
	static int32 LuaOperatorAdd(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector>(L, 1);

		if (lua_isnumber(L, 2))
		{
			auto V2 = FSUE4LuaStack::To<float>(L, 2);
			FSUE4LuaStack::Push(L, V1 + V2);
		}
		else
		{
			auto V2 = FSUE4LuaStack::To<FVector>(L, 2);
			FSUE4LuaStack::Push(L, V1 + V2);
		}
		
		return 1;
	}

	// operator-(V1, V2)
	static int32 LuaOperatorSubtract(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector>(L, 1);

		if (lua_isnumber(L, 2))
		{
			auto V2 = FSUE4LuaStack::To<float>(L, 2);
			FSUE4LuaStack::Push(L, V1 - V2);
		}
		else
		{
			auto V2 = FSUE4LuaStack::To<FVector>(L, 2);
			FSUE4LuaStack::Push(L, V1 - V2);
		}
		
		return 1;
	}

	// operator*(V1, Scale or V2)
	static int32 LuaOperatorMultiply(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector>(L, 1);

		if (lua_isnumber(L, 2))
		{
			auto Scale = FSUE4LuaStack::To<float>(L, 2);
			FSUE4LuaStack::Push(L, V1 * Scale);
		}
		else
		{
			auto V2 = FSUE4LuaStack::To<FVector>(L, 2);
			FSUE4LuaStack::Push(L, V1 * V2);
		}
		
		return 1;
	}

	// operator/(V1, Scale or V2)
	static int32 LuaOperatorDivide(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector>(L, 1);

		if (lua_isnumber(L, 2))
		{
			auto Scale = FSUE4LuaStack::To<float>(L, 2);
			FSUE4LuaStack::Push(L, V1 / Scale);
		}
		else
		{
			auto V2 = FSUE4LuaStack::To<FVector>(L, 2);
			FSUE4LuaStack::Push(L, V1 / V2);
		}

		return 1;
	}

	// operator==(V1, V2)
	static int32 LuaOperatorEqual(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector>(L, 1);
		auto V2 = FSUE4LuaStack::To<FVector>(L, 2);

		FSUE4LuaStack::Push(L, V1 == V2);
		return 1;
	}

	// bool Equals(V1, V2, Tolerance?)
	static int32 LuaEquals(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector>(L, 1);
		auto V2 = FSUE4LuaStack::To<FVector>(L, 2);

		bool bRet = false;

		if (lua_isnumber(L, 3))
		{
			auto Tolerance = FSUE4LuaStack::To<float>(L, 3);

			bRet = V1.Equals(V2, Tolerance);
		}
		else
		{
			bRet = V1.Equals(V2);
		}
		
		FSUE4LuaStack::Push(L, bRet);
		return 1;
	}

	// bool AllComponentsEqual(V, Tolerance?)
	static int32 LuaAllComponentsEqual(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector>(L, 1);

		bool bRet = false;

		if (lua_isnumber(L, 2))
		{
			auto Tolerance = FSUE4LuaStack::To<float>(L, 2);

			bRet = V.AllComponentsEqual(Tolerance);
		}
		else
		{
			bRet = V.AllComponentsEqual();
		}

		FSUE4LuaStack::Push(L, bRet);
		return 1;
	}

	// float DotProduct(V1, V2)
	static int32 LuaDotProduct(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector>(L, 1);
		auto V2 = FSUE4LuaStack::To<FVector>(L, 2);

		FSUE4LuaStack::Push(L, FVector::DotProduct(V1, V2));
		return 1;
	}

	// Vector CrossProduct(V1, V2)
	static int32 LuaCrossProduct(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector>(L, 1);
		auto V2 = FSUE4LuaStack::To<FVector>(L, 2);

		FSUE4LuaStack::Push(L, FVector::CrossProduct(V1, V2));
		return 1;
	}

	// float Size(V)
	static int32 LuaSize(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector>(L, 1);

		FSUE4LuaStack::Push(L, V.Size());
		return 1;
	}

	// float SizeSquared(V)
	static int32 LuaSizeSquared(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector>(L, 1);

		FSUE4LuaStack::Push(L, V.SizeSquared());
		return 1;
	}

	// float Size2D(V)
	static int32 LuaSize2D(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector>(L, 1);

		FSUE4LuaStack::Push(L, V.Size2D());
		return 1;
	}

	// float SizeSquared2D(V)
	static int32 LuaSizeSquared2D(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector>(L, 1);

		FSUE4LuaStack::Push(L, V.SizeSquared2D());
		return 1;
	}

	// FVector ClampToSize(V, Min, Max)
	static int32 LuaClampToSize(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector>(L, 1);
		auto Min = FSUE4LuaStack::To<float>(L, 2);
		auto Max = FSUE4LuaStack::To<float>(L, 3);

		FSUE4LuaStack::Push(L, V.GetClampedToSize(Min, Max));
		return 1;
	}

	// float Distance(V1, V2)
	static int32 LuaDistance(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector>(L, 1);
		auto V2 = FSUE4LuaStack::To<FVector>(L, 2);

		FSUE4LuaStack::Push(L, FVector::Dist(V1, V2));
		return 1;
	}

	// float DistanceSquared(V1, V2)
	static int32 LuaDistanceSquared(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector>(L, 1);
		auto V2 = FSUE4LuaStack::To<FVector>(L, 2);

		FSUE4LuaStack::Push(L, FVector::DistSquared(V1, V2));
		return 1;
	}

	// float Distance2D(V1, V2)
	static int32 LuaDistance2D(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector>(L, 1);
		auto V2 = FSUE4LuaStack::To<FVector>(L, 2);

		FSUE4LuaStack::Push(L, FVector::Dist2D(V1, V2));
		return 1;
	}

	// float DistanceSquared2D(V1, V2)
	static int32 LuaDistanceSquared2D(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector>(L, 1);
		auto V2 = FSUE4LuaStack::To<FVector>(L, 2);

		FSUE4LuaStack::Push(L, FVector::DistSquared2D(V1, V2));
		return 1;
	}

	// float DistancePointToSegment(Point, SegmentStart, SegmentEnd)
	static int32 LuaDistancePointToSegment(lua_State* L)
	{
		auto Point = FSUE4LuaStack::To<FVector>(L, 1);
		auto SegmentStart = FSUE4LuaStack::To<FVector>(L, 2);
		auto SegmentEnd = FSUE4LuaStack::To<FVector>(L, 3);

		FSUE4LuaStack::Push(L, FMath::PointDistToSegment(Point, SegmentStart, SegmentEnd));
		return 1;
	}

	// float DistancePointToLine(Point, LineOrigin, LineDirection)
	static int32 LuaDistancePointToLine(lua_State* L)
	{
		auto Point = FSUE4LuaStack::To<FVector>(L, 1);
		auto LineOrigin = FSUE4LuaStack::To<FVector>(L, 2);
		auto LineDirection = FSUE4LuaStack::To<FVector>(L, 3);

		FSUE4LuaStack::Push(L, FMath::PointDistToLine(Point, LineDirection, LineOrigin));
		return 1;
	}

	// bool IsNearlyZero(V, Tolerance?)
	static int32 LuaIsNearlyZero(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector>(L, 1);

		if (lua_isnumber(L, 2))
		{
			auto Tolerance = FSUE4LuaStack::To<float>(L, 2);

			FSUE4LuaStack::Push(L, V.IsNearlyZero(Tolerance));
		}
		else
		{
			FSUE4LuaStack::Push(L, V.IsNearlyZero());
		}

		return 1;
	}

	// bool IsZero(V)
	static int32 LuaIsZero(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector>(L, 1);

		FSUE4LuaStack::Push(L, V.IsZero());
		return 1;
	}

	// bool IsNormalized(V)
	static int32 LuaIsNormalized(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector>(L, 1);

		FSUE4LuaStack::Push(L, V.IsNormalized());
		return 1;
	}
	
	// Vector UnrotateByRotator(V, Rotator)
	static int LuaUnrotateByRotator(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector>(L, 1);
		auto Rotator = FSUE4LuaStack::To<FRotator>(L, 2);
		
		FSUE4LuaStack::Push(L, Rotator.UnrotateVector(V));
		return 1;
	}

	// Vector RotateByRotator(V, Rotator)
	static int LuaRotateByRotator(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector>(L, 1);
		auto Rotator = FSUE4LuaStack::To<FRotator>(L, 2);

		FSUE4LuaStack::Push(L, Rotator.RotateVector(V));
		return 1;
	}

	// Vector RotateByAngleAxis(V, AngleDeg, Axis)
	static int LuaRotateByAngleAxis(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector>(L, 1);
		auto AngleDeg = FSUE4LuaStack::To<float>(L, 2);
		auto Axis = FSUE4LuaStack::To<FVector>(L, 3);

		FSUE4LuaStack::Push(L, V.RotateAngleAxis(AngleDeg, Axis));
		return 1;
	}

	// Vector Lerp(V1, V2, Alpha)
	static int LuaLerp(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector>(L, 1);
		auto V2 = FSUE4LuaStack::To<FVector>(L, 2);
		auto Alpha = FSUE4LuaStack::To<float>(L, 3);
		
		FSUE4LuaStack::Push(L, V1 + Alpha * (V2 - V1));
		return 1;
	}

	// Vector Reflection(V, SurfaceNormal)
	static int LuaReflection(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector>(L, 1);
		auto Surface = FSUE4LuaStack::To<FVector>(L, 2);
		
		FSUE4LuaStack::Push(L, FMath::GetReflectionVector(V, Surface));
		return 1;
	}

	// Vector FindClosestPointOnSegment(Point, SegmentStart, SegmentEnd)
	static int LuaFindClosestPointOnSegment(lua_State* L)
	{
		auto Point = FSUE4LuaStack::To<FVector>(L, 1);
		auto SegmentStart = FSUE4LuaStack::To<FVector>(L, 2);
		auto SegmentEnd = FSUE4LuaStack::To<FVector>(L, 3);

		FSUE4LuaStack::Push(L, FMath::ClosestPointOnSegment(Point, SegmentStart, SegmentEnd));
		return 1;
	}

	// Vector FindClosestPointOnLine(Point, LineOrigin, LineDirection)
	static int LuaFindClosestPointOnLine(lua_State* L)
	{
		auto Point = FSUE4LuaStack::To<FVector>(L, 1);
		auto LineOrigin = FSUE4LuaStack::To<FVector>(L, 2);
		auto LineDirection = FSUE4LuaStack::To<FVector>(L, 3);

		const FVector SafeDir = LineDirection.GetSafeNormal();
		const FVector ClosestPoint = LineOrigin + (SafeDir * ((Point - LineOrigin) | SafeDir));

		FSUE4LuaStack::Push(L, ClosestPoint);
		return 1;
	}

	// Vector ProjectVectorOnToVector(V1, V2) 
	static int LuaProjectVectorOnToVector(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector>(L, 1);
		auto V2 = FSUE4LuaStack::To<FVector>(L, 2);

		FSUE4LuaStack::Push(L, V1.ProjectOnTo(V2));
		return 1;
	}

	// Vector ProjectPointOnToPlane(Point, PlaneBase, PlaneNormal)
	static int32 LuaProjectPointOnToPlane(lua_State* L)
	{
		auto Point = FSUE4LuaStack::To<FVector>(L, 1);
		auto PlaneBase = FSUE4LuaStack::To<FVector>(L, 2);
		auto PlaneNormal = FSUE4LuaStack::To<FVector>(L, 3);

		FSUE4LuaStack::Push(L, FVector::PointPlaneProject(Point, PlaneBase, PlaneNormal));
		return 1;
	}

	// Vector ProjectVectorOnToPlane(V, PlaneNormal)
	static int32 LuaProjectVectorOnToPlane(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector>(L, 1);
		auto PlaneNormal = FSUE4LuaStack::To<FVector>(L, 2);

		FSUE4LuaStack::Push(L, FVector::VectorPlaneProject(V, PlaneNormal));
		return 1;
	}

	// float GetMin(V)
	static int32 LuaGetMin(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector>(L, 1);

		FSUE4LuaStack::Push(L, V.GetMin());
		return 1;
	}

	// float GetMax(V)
	static int32 LuaGetMax(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector>(L, 1);

		FSUE4LuaStack::Push(L, V.GetMax());
		return 1;
	}

	// Vector GetNormal(V, Tolerance?)
	static int32 LuaGetNormal(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector>(L, 1);

		if (lua_isnumber(L, 2))
		{
			auto Tolerance = FSUE4LuaStack::To<float>(L, 2);
			FSUE4LuaStack::Push(L, V.GetSafeNormal(Tolerance));
		}
		else
		{
			FSUE4LuaStack::Push(L, V.GetSafeNormal());
		}
		return 1;
	}

	// Vector GetUnitDirection(V1, V2)
	static int32 LuaGetUnitDirection(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector>(L, 1);
		auto V2 = FSUE4LuaStack::To<FVector>(L, 2);

		FSUE4LuaStack::Push(L, (V2 - V1).GetSafeNormal());
		return 1;
	}

	// Transform ToTransform(V)
	static int32 LuaToTransform(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector>(L, 1);

		FSUE4LuaStack::Push(L, FTransform(V));
		return 1;	
	}

	static void Register(lua_State* L)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "Vector");

		// Operators
		{
			lua_getfield(L, -1, "metaTable");
			// Stack: UE4, Vector, metaTable

			lua_pushstring(L, "__unm");
			lua_pushcfunction(L, &LuaOperatorUnaryMinus);
			lua_rawset(L, -3);

			lua_pushstring(L, "__add");
			lua_pushcfunction(L, &LuaOperatorAdd);
			lua_rawset(L, -3);

			lua_pushstring(L, "__sub");
			lua_pushcfunction(L, &LuaOperatorSubtract);
			lua_rawset(L, -3);

			lua_pushstring(L, "__mul");
			lua_pushcfunction(L, &LuaOperatorMultiply);
			lua_rawset(L, -3);

			lua_pushstring(L, "__div");
			lua_pushcfunction(L, &LuaOperatorDivide);
			lua_rawset(L, -3);

			lua_pushstring(L, "__eq");
			lua_pushcfunction(L, &LuaOperatorEqual);
			lua_rawset(L, -3);

			lua_pop(L, 1);
			// Stack: UE4, Vector
		}

		lua_pushstring(L, "Equals");
		lua_pushcfunction(L, &LuaEquals);
		lua_rawset(L, -3);

		lua_pushstring(L, "AllComponentsEqual");
		lua_pushcfunction(L, &LuaAllComponentsEqual);
		lua_rawset(L, -3);

		lua_pushstring(L, "DotProduct");
		lua_pushcfunction(L, &LuaDotProduct);
		lua_rawset(L, -3);

		lua_pushstring(L, "CrossProduct");
		lua_pushcfunction(L, &LuaCrossProduct);
		lua_rawset(L, -3);

		lua_pushstring(L, "Size");
		lua_pushcfunction(L, &LuaSize);
		lua_rawset(L, -3);

		lua_pushstring(L, "SizeSquared");
		lua_pushcfunction(L, &LuaSizeSquared);
		lua_rawset(L, -3);

		lua_pushstring(L, "Size2D");
		lua_pushcfunction(L, &LuaSize2D);
		lua_rawset(L, -3);

		lua_pushstring(L, "SizeSquared2D");
		lua_pushcfunction(L, &LuaSizeSquared2D);
		lua_rawset(L, -3);

		lua_pushstring(L, "ClampToSize");
		lua_pushcfunction(L, &LuaClampToSize);
		lua_rawset(L, -3);

		lua_pushstring(L, "Distance");
		lua_pushcfunction(L, &LuaDistance);
		lua_rawset(L, -3);

		lua_pushstring(L, "DistanceSquared");
		lua_pushcfunction(L, &LuaDistanceSquared);
		lua_rawset(L, -3);

		lua_pushstring(L, "Distance2D");
		lua_pushcfunction(L, &LuaDistance2D);
		lua_rawset(L, -3);
		
		lua_pushstring(L, "DistanceSquared2D");
		lua_pushcfunction(L, &LuaDistanceSquared2D);
		lua_rawset(L, -3);

		lua_pushstring(L, "DistancePointToSegment");
		lua_pushcfunction(L, &LuaDistancePointToSegment);
		lua_rawset(L, -3);

		lua_pushstring(L, "DistancePointToLine");
		lua_pushcfunction(L, &LuaDistancePointToLine);
		lua_rawset(L, -3);
		
		lua_pushstring(L, "IsNearlyZero");
		lua_pushcfunction(L, &LuaIsNearlyZero);
		lua_rawset(L, -3);

		lua_pushstring(L, "IsZero");
		lua_pushcfunction(L, &LuaIsZero);
		lua_rawset(L, -3);

		lua_pushstring(L, "IsNormalized");
		lua_pushcfunction(L, &LuaIsNormalized);
		lua_rawset(L, -3);

		lua_pushstring(L, "UnrotateByRotator");
		lua_pushcfunction(L, &LuaUnrotateByRotator);
		lua_rawset(L, -3);

		lua_pushstring(L, "RotateByRotator");
		lua_pushcfunction(L, &LuaRotateByRotator);
		lua_rawset(L, -3);

		lua_pushstring(L, "RotateByAngleAxis");
		lua_pushcfunction(L, &LuaRotateByAngleAxis);
		lua_rawset(L, -3);

		lua_pushstring(L, "Lerp");
		lua_pushcfunction(L, &LuaLerp);
		lua_rawset(L, -3);

		lua_pushstring(L, "Reflection");
		lua_pushcfunction(L, &LuaReflection);
		lua_rawset(L, -3);

		lua_pushstring(L, "FindClosestPointOnSegment");
		lua_pushcfunction(L, &LuaFindClosestPointOnSegment);
		lua_rawset(L, -3);

		lua_pushstring(L, "FindClosestPointOnLine");
		lua_pushcfunction(L, &LuaFindClosestPointOnLine);
		lua_rawset(L, -3);

		lua_pushstring(L, "ProjectVectorOnToVector");
		lua_pushcfunction(L, &LuaProjectVectorOnToVector);
		lua_rawset(L, -3);

		lua_pushstring(L, "ProjectPointOnToPlane");
		lua_pushcfunction(L, &LuaProjectPointOnToPlane);
		lua_rawset(L, -3);

		lua_pushstring(L, "ProjectVectorOnToPlane");
		lua_pushcfunction(L, &LuaProjectVectorOnToPlane);
		lua_rawset(L, -3);

		lua_pushstring(L, "GetMin");
		lua_pushcfunction(L, &LuaGetMin);
		lua_rawset(L, -3);

		lua_pushstring(L, "GetMax");
		lua_pushcfunction(L, &LuaGetMax);
		lua_rawset(L, -3);

		lua_pushstring(L, "GetNormal");
		lua_pushcfunction(L, &LuaGetNormal);
		lua_rawset(L, -3);

		lua_pushstring(L, "GetUnitDirection");
		lua_pushcfunction(L, &LuaGetUnitDirection);
		lua_rawset(L, -3);

		lua_pushstring(L, "ToTransform");
		lua_pushcfunction(L, &LuaToTransform);
		lua_rawset(L, -3);

		lua_pop(L, 2);
		// Stack: (empty)
	}
}

//=============================================================================================================================
// namespace SUE4LuaVector2DLibrary
//=============================================================================================================================
namespace SUE4LuaVector2DLibrary
{
	// operator-(V)
	static int32 LuaOperatorUnaryMinus(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector2D>(L, 1);

		FSUE4LuaStack::Push(L, -V);
		return 1;
	}

	// operator+(V1, V2)
	static int32 LuaOperatorAdd(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector2D>(L, 1);

		if (lua_isnumber(L, 2))
		{
			auto V2 = FSUE4LuaStack::To<float>(L, 2);
			FSUE4LuaStack::Push(L, V1 + V2);
		}
		else
		{
			auto V2 = FSUE4LuaStack::To<FVector2D>(L, 2);
			FSUE4LuaStack::Push(L, V1 + V2);
		}

		return 1;
	}

	// operator-(V1, V2)
	static int32 LuaOperatorSubtract(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector2D>(L, 1);

		if (lua_isnumber(L, 2))
		{
			auto V2 = FSUE4LuaStack::To<float>(L, 2);
			FSUE4LuaStack::Push(L, V1 - V2);
		}
		else
		{
			auto V2 = FSUE4LuaStack::To<FVector2D>(L, 2);
			FSUE4LuaStack::Push(L, V1 - V2);
		}

		return 1;
	}

	// operator*(V1, Scale or V2)
	static int32 LuaOperatorMultiply(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector2D>(L, 1);

		if (lua_isnumber(L, 2))
		{
			auto Scale = FSUE4LuaStack::To<float>(L, 2);
			FSUE4LuaStack::Push(L, V1 * Scale);
		}
		else
		{
			auto V2 = FSUE4LuaStack::To<FVector2D>(L, 2);
			FSUE4LuaStack::Push(L, V1 * V2);
		}

		return 1;
	}

	// operator/(V1, Scale or V2)
	static int32 LuaOperatorDivide(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector2D>(L, 1);

		if (lua_isnumber(L, 2))
		{
			auto Scale = FSUE4LuaStack::To<float>(L, 2);
			FSUE4LuaStack::Push(L, V1 / Scale);
		}
		else
		{
			auto V2 = FSUE4LuaStack::To<FVector2D>(L, 2);
			FSUE4LuaStack::Push(L, V1 / V2);
		}

		return 1;
	}

	// operator==(V1, V2)
	static int32 LuaOperatorEqual(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector2D>(L, 1);
		auto V2 = FSUE4LuaStack::To<FVector2D>(L, 2);

		FSUE4LuaStack::Push(L, V1 == V2);
		return 1;
	}

	// bool Equals(V1, V2, Tolerance?)
	static int32 LuaEquals(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector2D>(L, 1);
		auto V2 = FSUE4LuaStack::To<FVector2D>(L, 2);

		bool bRet = false;

		if (lua_isnumber(L, 3))
		{
			auto Tolerance = FSUE4LuaStack::To<float>(L, 3);

			bRet = V1.Equals(V2, Tolerance);
		}
		else
		{
			bRet = V1.Equals(V2);
		}

		FSUE4LuaStack::Push(L, bRet);
		return 1;
	}
	
	// float DotProduct(V1, V2)
	static int32 LuaDotProduct(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector2D>(L, 1);
		auto V2 = FSUE4LuaStack::To<FVector2D>(L, 2);

		FSUE4LuaStack::Push(L, FVector2D::DotProduct(V1, V2));
		return 1;
	}

	// float CrossProduct(V1, V2)
	static int32 LuaCrossProduct(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector2D>(L, 1);
		auto V2 = FSUE4LuaStack::To<FVector2D>(L, 2);

		FSUE4LuaStack::Push(L, FVector2D::CrossProduct(V1, V2));
		return 1;
	}

	// float Size(V)
	static int32 LuaSize(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector2D>(L, 1);

		FSUE4LuaStack::Push(L, V.Size());
		return 1;
	}

	// float SizeSquared(V)
	static int32 LuaSizeSquared(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector2D>(L, 1);

		FSUE4LuaStack::Push(L, V.SizeSquared());
		return 1;
	}
	
	// float Distance(V1, V2)
	static int32 LuaDistance(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector2D>(L, 1);
		auto V2 = FSUE4LuaStack::To<FVector2D>(L, 2);

		FSUE4LuaStack::Push(L, FVector2D::Distance(V1, V2));
		return 1;
	}

	// float DistanceSquared(V1, V2)
	static int32 LuaDistanceSquared(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector2D>(L, 1);
		auto V2 = FSUE4LuaStack::To<FVector2D>(L, 2);

		FSUE4LuaStack::Push(L, FVector2D::DistSquared(V1, V2));
		return 1;
	}

	// bool IsNearlyZero(V, Tolerance?)
	static int32 LuaIsNearlyZero(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector2D>(L, 1);

		if (lua_isnumber(L, 2))
		{
			auto Tolerance = FSUE4LuaStack::To<float>(L, 2);

			FSUE4LuaStack::Push(L, V.IsNearlyZero(Tolerance));
		}
		else
		{
			FSUE4LuaStack::Push(L, V.IsNearlyZero());
		}

		return 1;
	}

	// bool IsZero(V)
	static int32 LuaIsZero(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector2D>(L, 1);
		
		FSUE4LuaStack::Push(L, V.IsZero());
		return 1;
	}

	// Vector2D Rotate(V, AngleDeg)
	static int LuaRotate(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector2D>(L, 1);
		auto AngleDeg = FSUE4LuaStack::To<float>(L, 2);

		FSUE4LuaStack::Push(L, V.GetRotated(AngleDeg));
		return 1;
	}

	// Vector2D Lerp(V1, V2, Alpha)
	static int LuaLerp(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector2D>(L, 1);
		auto V2 = FSUE4LuaStack::To<FVector2D>(L, 2);
		auto Alpha = FSUE4LuaStack::To<float>(L, 3);

		FSUE4LuaStack::Push(L, V1 + Alpha * (V2 - V1));
		return 1;
	}

	// float GetMin(V)
	static int32 LuaGetMin(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector2D>(L, 1);

		FSUE4LuaStack::Push(L, V.GetMin());
		return 1;
	}

	// float GetMax(V)
	static int32 LuaGetMax(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector2D>(L, 1);

		FSUE4LuaStack::Push(L, V.GetMax());
		return 1;
	}

	// Vector2D GetNormal(V, Tolerance?)
	static int32 LuaGetNormal(lua_State* L)
	{
		auto V = FSUE4LuaStack::To<FVector2D>(L, 1);
		
		if (lua_isnumber(L, 2))
		{
			auto Tolerance = FSUE4LuaStack::To<float>(L, 2);
			FSUE4LuaStack::Push(L, V.GetSafeNormal(Tolerance));
		}
		else
		{
			FSUE4LuaStack::Push(L, V.GetSafeNormal());
		}
		return 1;
	}

	// Vector2D GetUnitDirection(V1, V2)
	static int32 LuaGetUnitDirection(lua_State* L)
	{
		auto V1 = FSUE4LuaStack::To<FVector2D>(L, 1);
		auto V2 = FSUE4LuaStack::To<FVector2D>(L, 2);

		FSUE4LuaStack::Push(L, (V2 - V1).GetSafeNormal());
		return 1;
	}

	static void Register(lua_State* L)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "Vector2D");

		// Operators
		{
			lua_getfield(L, -1, "metaTable");
			// Stack: UE4, Vector, metaTable

			lua_pushstring(L, "__unm");
			lua_pushcfunction(L, &LuaOperatorUnaryMinus);
			lua_rawset(L, -3);

			lua_pushstring(L, "__add");
			lua_pushcfunction(L, &LuaOperatorAdd);
			lua_rawset(L, -3);

			lua_pushstring(L, "__sub");
			lua_pushcfunction(L, &LuaOperatorSubtract);
			lua_rawset(L, -3);

			lua_pushstring(L, "__mul");
			lua_pushcfunction(L, &LuaOperatorMultiply);
			lua_rawset(L, -3);

			lua_pushstring(L, "__div");
			lua_pushcfunction(L, &LuaOperatorDivide);
			lua_rawset(L, -3);

			lua_pushstring(L, "__eq");
			lua_pushcfunction(L, &LuaOperatorEqual);
			lua_rawset(L, -3);

			lua_pop(L, 1);
			// Stack: UE4, Vector
		}

		lua_pushstring(L, "Equals");
		lua_pushcfunction(L, &LuaEquals);
		lua_rawset(L, -3);
		
		lua_pushstring(L, "DotProduct");
		lua_pushcfunction(L, &LuaDotProduct);
		lua_rawset(L, -3);

		lua_pushstring(L, "CrossProduct");
		lua_pushcfunction(L, &LuaCrossProduct);
		lua_rawset(L, -3);

		lua_pushstring(L, "Size");
		lua_pushcfunction(L, &LuaSize);
		lua_rawset(L, -3);

		lua_pushstring(L, "SizeSquared");
		lua_pushcfunction(L, &LuaSizeSquared);
		lua_rawset(L, -3);

		lua_pushstring(L, "Distance");
		lua_pushcfunction(L, &LuaDistance);
		lua_rawset(L, -3);

		lua_pushstring(L, "DistanceSquared");
		lua_pushcfunction(L, &LuaDistanceSquared);
		lua_rawset(L, -3);

		lua_pushstring(L, "IsNearlyZero");
		lua_pushcfunction(L, &LuaIsNearlyZero);
		lua_rawset(L, -3);

		lua_pushstring(L, "IsZero");
		lua_pushcfunction(L, &LuaIsZero);
		lua_rawset(L, -3);

		lua_pushstring(L, "Rotate");
		lua_pushcfunction(L, &LuaRotate);
		lua_rawset(L, -3);

		lua_pushstring(L, "Lerp");
		lua_pushcfunction(L, &LuaLerp);
		lua_rawset(L, -3);
		
		lua_pushstring(L, "GetMin");
		lua_pushcfunction(L, &LuaGetMin);
		lua_rawset(L, -3);

		lua_pushstring(L, "GetMax");
		lua_pushcfunction(L, &LuaGetMax);
		lua_rawset(L, -3);

		lua_pushstring(L, "GetNormal");
		lua_pushcfunction(L, &LuaGetNormal);
		lua_rawset(L, -3);

		lua_pushstring(L, "GetUnitDirection");
		lua_pushcfunction(L, &LuaGetUnitDirection);
		lua_rawset(L, -3);

		lua_pop(L, 2);
		// Stack: (empty)
	}
}

//=============================================================================================================================
// namespace SUE4LuaTransformLibrary
//=============================================================================================================================
namespace SUE4LuaTransformLibrary
{
	// operator+(T1, T2)
	static int32 LuaOperatorAdd(lua_State* L)
	{
		auto T1 = FSUE4LuaStack::To<FTransform>(L, 1);
		auto T2 = FSUE4LuaStack::To<FTransform>(L, 2);

		FSUE4LuaStack::Push(L, T1 + T2);	
		return 1;
	}

	// operator*(T1, T2)
	static int32 LuaOperatorMultiply(lua_State* L)
	{
		auto T1 = FSUE4LuaStack::To<FTransform>(L, 1);
		auto T2 = FSUE4LuaStack::To<FTransform>(L, 2);

		FSUE4LuaStack::Push(L, T1 * T2);
		return 1;
	}

	// operator==(T1, T2)
	static int32 LuaOperatorEqual(lua_State* L)
	{
		auto T1 = FSUE4LuaStack::To<FTransform>(L, 1);
		auto T2 = FSUE4LuaStack::To<FTransform>(L, 2);

		FSUE4LuaStack::Push(L, T1.Equals(T2));
		return 1;
	}

	// bool Equals(T1, T2, Tolerance?)
	static int32 LuaEquals(lua_State* L)
	{
		auto T1 = FSUE4LuaStack::To<FTransform>(L, 1);
		auto T2 = FSUE4LuaStack::To<FTransform>(L, 2);

		bool bRet = false;

		if (lua_isnumber(L, 3))
		{
			auto Tolerance = FSUE4LuaStack::To<float>(L, 3);

			bRet = T1.Equals(T2, Tolerance);
		}
		else
		{
			bRet = T1.Equals(T2);
		}

		FSUE4LuaStack::Push(L, bRet);
		return 1;
	}

	// Vector TransformLocation(T, Location)
	static int32 LuaTransformLocation(lua_State* L)
	{
		auto T = FSUE4LuaStack::To<FTransform>(L, 1);
		auto Location = FSUE4LuaStack::To<FVector>(L, 2);

		FSUE4LuaStack::Push(L, T.TransformPosition(Location));
		return 1;
	}

	// Vector TransformDirection(T, Direction)
	static int32 LuaTransformDirection(lua_State* L)
	{
		auto T = FSUE4LuaStack::To<FTransform>(L, 1);
		auto Direction = FSUE4LuaStack::To<FVector>(L, 2);

		FSUE4LuaStack::Push(L, T.TransformPositionNoScale(Direction));
		return 1;
	}

	// Rotator TransformRotation(T, Rotation)
	static int32 LuaTransformRotation(lua_State* L)
	{
		auto T = FSUE4LuaStack::To<FTransform>(L, 1);
		auto Rotation = FSUE4LuaStack::To<FRotator>(L, 2);

		FSUE4LuaStack::Push(L, T.TransformRotation(Rotation.Quaternion()).Rotator());
		return 1;
	}

	// Vector InverseTransformLocation(T, Location)
	static int32 LuaInverseTransformLocation(lua_State* L)
	{
		auto T = FSUE4LuaStack::To<FTransform>(L, 1);
		auto Location = FSUE4LuaStack::To<FVector>(L, 2);

		FSUE4LuaStack::Push(L, T.InverseTransformPosition(Location));
		return 1;
	}

	// Vector InverseTransformDirection(T, Direction)
	static int32 LuaInverseTransformDirection(lua_State* L)
	{
		auto T = FSUE4LuaStack::To<FTransform>(L, 1);
		auto Direction = FSUE4LuaStack::To<FVector>(L, 2);

		FSUE4LuaStack::Push(L, T.InverseTransformPositionNoScale(Direction));
		return 1;
	}

	// Rotator InverseTransformRotation(T, Rotation)
	static int32 LuaInverseTransformRotation(lua_State* L)
	{
		auto T = FSUE4LuaStack::To<FTransform>(L, 1);
		auto Rotation = FSUE4LuaStack::To<FRotator>(L, 2);

		FSUE4LuaStack::Push(L, T.InverseTransformRotation(Rotation.Quaternion()).Rotator());
		return 1;
	}

	// Transform Inverse(T)
	static int32 LuaInverse(lua_State* L)
	{
		auto T = FSUE4LuaStack::To<FTransform>(L, 1);

		FSUE4LuaStack::Push(L, T.Inverse());
		return 1;
	}

	// Transform Lerp(T1, T2, Alpha)
	// Quaternion interpolation만 구현
	static int32 LuaLerp(lua_State* L)
	{
		auto T1 = FSUE4LuaStack::To<FTransform>(L, 1);
		auto T2 = FSUE4LuaStack::To<FTransform>(L, 2);
		auto Alpha = FSUE4LuaStack::To<float>(L, 3);

		T1.NormalizeRotation(); 
		T2.NormalizeRotation();

		FTransform Result;
		Result.Blend(T1, T2, Alpha);

		FSUE4LuaStack::Push(L, Result);
		return 1;
	}

	static void Register(lua_State* L)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "Transform");

		// Operators
		{
			lua_getfield(L, -1, "metaTable");
			// Stack: UE4, Transform, metaTable

			lua_pushstring(L, "__add");
			lua_pushcfunction(L, &LuaOperatorAdd);
			lua_rawset(L, -3);

			lua_pushstring(L, "__mul");
			lua_pushcfunction(L, &LuaOperatorMultiply);
			lua_rawset(L, -3);

			lua_pushstring(L, "__eq");
			lua_pushcfunction(L, &LuaOperatorEqual);
			lua_rawset(L, -3);

			lua_pop(L, 1);
			// Stack: UE4, Transform
		}

		lua_pushstring(L, "Equals");
		lua_pushcfunction(L, &LuaEquals);
		lua_rawset(L, -3);

		lua_pushstring(L, "TransformLocation");
		lua_pushcfunction(L, &LuaTransformLocation);
		lua_rawset(L, -3);

		lua_pushstring(L, "TransformDirection");
		lua_pushcfunction(L, &LuaTransformDirection);
		lua_rawset(L, -3);

		lua_pushstring(L, "TransformRotation");
		lua_pushcfunction(L, &LuaTransformRotation);
		lua_rawset(L, -3);

		lua_pushstring(L, "InverseTransformLocation");
		lua_pushcfunction(L, &LuaInverseTransformLocation);
		lua_rawset(L, -3);

		lua_pushstring(L, "InverseTransformDirection");
		lua_pushcfunction(L, &LuaInverseTransformDirection);
		lua_rawset(L, -3);

		lua_pushstring(L, "InverseTransformRotation");
		lua_pushcfunction(L, &LuaInverseTransformRotation);
		lua_rawset(L, -3);

		lua_pushstring(L, "Inverse");
		lua_pushcfunction(L, &LuaInverse);
		lua_rawset(L, -3);

		lua_pushstring(L, "Lerp");
		lua_pushcfunction(L, &LuaLerp);
		lua_rawset(L, -3);

		lua_pop(L, 2);
		// Stack: (empty)
	}
}

//=============================================================================================================================
// namespace SUE4LuaRotatorLibrary
//=============================================================================================================================
namespace SUE4LuaRotatorLibrary
{
	// operator+(R1, R2)
	static int32 LuaOperatorAdd(lua_State* L)
	{
		auto R1 = FSUE4LuaStack::To<FRotator>(L, 1);
		auto R2 = FSUE4LuaStack::To<FRotator>(L, 2);
		
		FSUE4LuaStack::Push(L, R1 + R2);
		return 1;
	}

	// operator-(R1, R2)
	static int32 LuaOperatorSubtract(lua_State* L)
	{
		auto R1 = FSUE4LuaStack::To<FRotator>(L, 1);
		auto R2 = FSUE4LuaStack::To<FRotator>(L, 2);

		FSUE4LuaStack::Push(L, R1 - R2);
		return 1;
	}

	// operator*(R, Scale)
	static int32 LuaOperatorMultiply(lua_State* L)
	{
		auto R = FSUE4LuaStack::To<FRotator>(L, 1);
		auto Scale = FSUE4LuaStack::To<float>(L, 2);

		FSUE4LuaStack::Push(L, R * Scale);
		return 1;
	}

	// operator==(R1, R2)
	static int32 LuaOperatorEqual(lua_State* L)
	{
		auto R1 = FSUE4LuaStack::To<FRotator>(L, 1);
		auto R2 = FSUE4LuaStack::To<FRotator>(L, 2);

		FSUE4LuaStack::Push(L, R1 == R2);
		return 1;
	}

	// bool Equals(R1, R2, Tolerance?)
	static int32 LuaEquals(lua_State* L)
	{
		auto R1 = FSUE4LuaStack::To<FRotator>(L, 1);
		auto R2 = FSUE4LuaStack::To<FRotator>(L, 2);

		bool bRet = false;

		if (lua_isnumber(L, 3))
		{
			auto Tolerance = FSUE4LuaStack::To<float>(L, 3);

			bRet = R1.Equals(R2, Tolerance);
		}
		else
		{
			bRet = R1.Equals(R2);
		}

		FSUE4LuaStack::Push(L, bRet);
		return 1;
	}
	
	// bool IsNearlyZero(R, Tolerance?)
	static int32 LuaIsNearlyZero(lua_State* L)
	{
		auto R = FSUE4LuaStack::To<FRotator>(L, 1);

		if (lua_isnumber(L, 2))
		{
			auto Tolerance = FSUE4LuaStack::To<float>(L, 2);

			FSUE4LuaStack::Push(L, R.IsNearlyZero(Tolerance));
		}
		else
		{
			FSUE4LuaStack::Push(L, R.IsNearlyZero());
		}

		return 1;
	}

	// bool IsZero(R)
	static int32 LuaIsZero(lua_State* L)
	{
		auto R = FSUE4LuaStack::To<FRotator>(L, 1);

		FSUE4LuaStack::Push(L, R.IsZero());
		return 1;
	}

	// Rotator Inverse(R)
	static int32 LuaInverse(lua_State* L)
	{
		auto R = FSUE4LuaStack::To<FRotator>(L, 1);

		FSUE4LuaStack::Push(L, R.GetInverse());
		return 1;
	}

	// Rotator Lerp(R1, R2, Alpha, bShortestPath) 
	static int32 LuaLerp(lua_State* L)
	{
		auto R1 = FSUE4LuaStack::To<FRotator>(L, 1);
		auto R2 = FSUE4LuaStack::To<FRotator>(L, 2);
		auto Alpha = FSUE4LuaStack::To<float>(L, 3);
		auto bShortestPath = FSUE4LuaStack::To<bool>(L, 4);

		// if shortest path, we use Quaternion to interpolate instead of using FRotator
		if (bShortestPath)
		{
			FQuat AQuat(R1);
			FQuat BQuat(R2);

			FQuat Result = FQuat::Slerp(AQuat, BQuat, Alpha);
			Result.Normalize();
			auto a = Result.Rotator();

			FSUE4LuaStack::Push(L, Result.Rotator());
		}
		else
		{
			FRotator DeltaAngle = R2 - R1;
			auto a = R1 + Alpha * DeltaAngle;
			FSUE4LuaStack::Push(L, R1 + Alpha * DeltaAngle);
		}

		return 1;
	}

	// Rotator Delta(R1, R2) 
	static int32 LuaDelta(lua_State* L)
	{
		auto R1 = FSUE4LuaStack::To<FRotator>(L, 1);
		auto R2 = FSUE4LuaStack::To<FRotator>(L, 2);

		FRotator Delta = R1 - R2;
		Delta.Normalize();

		FSUE4LuaStack::Push(L, Delta);
		return 1;
	}

	// Vector GetAxisX(R)
	static int32 LuaGetAxisX(lua_State* L)
	{
		auto R = FSUE4LuaStack::To<FRotator>(L, 1);
		FRotationMatrix RMatrix(R); 
		FVector XAxis = RMatrix.GetScaledAxis(EAxis::X);

		FSUE4LuaStack::Push(L, XAxis);
		return 1;
	}

	// Vector GetAxisY(R)
	static int32 LuaGetAxisY(lua_State* L)
	{
		auto R = FSUE4LuaStack::To<FRotator>(L, 1);
		FRotationMatrix RMatrix(R);
		FVector YAxis = RMatrix.GetScaledAxis(EAxis::Y);

		FSUE4LuaStack::Push(L, YAxis);
		return 1;
	}

	// Vector GetAxisZ(R)
	static int32 LuaGetAxisZ(lua_State* L)
	{
		auto R = FSUE4LuaStack::To<FRotator>(L, 1);
		FRotationMatrix RMatrix(R);
		FVector ZAxis = RMatrix.GetScaledAxis(EAxis::Z);

		FSUE4LuaStack::Push(L, ZAxis);
		return 1;		
	}

	// Rotator GetNormal(R)
	static int32 LuaGetNormal(lua_State* L)
	{
		auto R = FSUE4LuaStack::To<FRotator>(L, 1);

		FSUE4LuaStack::Push(L, R.GetNormalized());
		return 1;
	}

	// Quat ToQuat(R)
	static int32 LuaToQuat(lua_State* L)
	{
		auto R = FSUE4LuaStack::To<FRotator>(L, 1);

		FSUE4LuaStack::Push(L, R.Quaternion());
		return 1;
	}

	static void Register(lua_State* L)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "Rotator");

		// Operators
		{
			lua_getfield(L, -1, "metaTable");
			// Stack: UE4, Rotator, metaTable

			lua_pushstring(L, "__add");
			lua_pushcfunction(L, &LuaOperatorAdd);
			lua_rawset(L, -3);

			lua_pushstring(L, "__sub");
			lua_pushcfunction(L, &LuaOperatorSubtract);
			lua_rawset(L, -3);

			lua_pushstring(L, "__mul");
			lua_pushcfunction(L, &LuaOperatorMultiply);
			lua_rawset(L, -3);

			lua_pushstring(L, "__eq");
			lua_pushcfunction(L, &LuaOperatorEqual);
			lua_rawset(L, -3);

			lua_pop(L, 1);
			// Stack: UE4, Rotator
		}

		lua_pushstring(L, "Equals");
		lua_pushcfunction(L, &LuaEquals);
		lua_rawset(L, -3);

		lua_pushstring(L, "IsNearlyZero");
		lua_pushcfunction(L, &LuaIsNearlyZero);
		lua_rawset(L, -3);

		lua_pushstring(L, "IsZero");
		lua_pushcfunction(L, &LuaIsZero);
		lua_rawset(L, -3);

		lua_pushstring(L, "Inverse");
		lua_pushcfunction(L, &LuaInverse);
		lua_rawset(L, -3);

		lua_pushstring(L, "Lerp");
		lua_pushcfunction(L, &LuaLerp);
		lua_rawset(L, -3);

		lua_pushstring(L, "Delta");
		lua_pushcfunction(L, &LuaDelta);
		lua_rawset(L, -3);

		lua_pushstring(L, "GetAxisX");
		lua_pushcfunction(L, &LuaGetAxisX);
		lua_rawset(L, -3);

		lua_pushstring(L, "GetAxisY");
		lua_pushcfunction(L, &LuaGetAxisY);
		lua_rawset(L, -3);

		lua_pushstring(L, "GetAxisZ");
		lua_pushcfunction(L, &LuaGetAxisZ);
		lua_rawset(L, -3);

		lua_pushstring(L, "GetNormal");
		lua_pushcfunction(L, &LuaGetNormal);
		lua_rawset(L, -3);

		lua_pushstring(L, "ToQuat");
		lua_pushcfunction(L, &LuaToQuat);
		lua_rawset(L, -3);

		lua_pop(L, 2);
		// Stack: (empty)
	}
}

//=============================================================================================================================
// namespace SUE4LuaQuatLibrary
//=============================================================================================================================
namespace SUE4LuaQuatLibrary 
{
	// operator+(Q1, Q2)
	static int32 LuaOperatorAdd(lua_State* L)
	{
		auto Q1 = FSUE4LuaStack::To<FQuat>(L, 1);
		auto Q2 = FSUE4LuaStack::To<FQuat>(L, 2);

		FSUE4LuaStack::Push(L, Q1 + Q2);
		return 1;
	}
	
	// operator-(Q1, Q2)
	static int32 LuaOperatorSubtract(lua_State* L)
	{
		auto Q1 = FSUE4LuaStack::To<FQuat>(L, 1);
		auto Q2 = FSUE4LuaStack::To<FQuat>(L, 2);
		
		FSUE4LuaStack::Push(L, Q1 - Q2);
		return 1;
	}

	// operator*(Q1, Scale or Q2)
	static int32 LuaOperatorMultiply(lua_State* L)
	{
		auto Q1 = FSUE4LuaStack::To<FQuat>(L, 1);

		if (lua_isnumber(L, 2))
		{
			auto Scale = FSUE4LuaStack::To<float>(L, 2);
			FSUE4LuaStack::Push(L, Q1 * Scale);
		}
		else
		{
			auto Q2 = FSUE4LuaStack::To<FQuat>(L, 2);
			FSUE4LuaStack::Push(L, Q1 * Q2);
		}

		return 1;
	}

	// operator/(Q1, Scale)
	static int32 LuaOperatorDivide(lua_State* L)
	{
		auto Q1 = FSUE4LuaStack::To<FQuat>(L, 1);
		auto Scale = FSUE4LuaStack::To<float>(L, 2);

		FSUE4LuaStack::Push(L, Q1 / Scale);
		return 1;
	}

	// operator==(Q1, Q2)
	static int32 LuaOperatorEqual(lua_State* L)
	{
		auto Q1 = FSUE4LuaStack::To<FQuat>(L, 1);
		auto Q2 = FSUE4LuaStack::To<FQuat>(L, 2);

		FSUE4LuaStack::Push(L, Q1.Equals(Q2));
		return 1;
	}

	// bool Equals(Q1, Q2, Tolerance?)
	static int32 LuaEquals(lua_State* L)
	{
		auto Q1 = FSUE4LuaStack::To<FQuat>(L, 1);
		auto Q2 = FSUE4LuaStack::To<FQuat>(L, 2);

		bool bRet = false;

		if (lua_isnumber(L, 3))
		{
			auto Tolerance = FSUE4LuaStack::To<float>(L, 3);

			bRet = Q1.Equals(Q2, Tolerance);
		}
		else
		{
			bRet = Q1.Equals(Q2);
		}

		FSUE4LuaStack::Push(L, bRet);
		return 1;		
	}
	
	// float Size(Q)
	static int32 LuaSize(lua_State* L)
	{
		auto Q = FSUE4LuaStack::To<FQuat>(L, 1);

		FSUE4LuaStack::Push(L, Q.Size());
		return 1;
	}

	// float SizeSquared(Q)
	static int32 LuaSizeSquared(lua_State* L)
	{
		auto Q = FSUE4LuaStack::To<FQuat>(L, 1);
		
		FSUE4LuaStack::Push(L, Q.SizeSquared());
		return 1;
	}

	// bool IsNormalized(Q)
	static int32 LuaIsNormalized(lua_State* L)
	{
		auto Q = FSUE4LuaStack::To<FQuat>(L, 1);

		FSUE4LuaStack::Push(L, Q.IsNormalized());
		return 1;
	}

	// Quat Inverse(Q)
	static int32 LuaInverse(lua_State* L)
	{
		auto Q = FSUE4LuaStack::To<FQuat>(L, 1);

		FSUE4LuaStack::Push(L, Q.Inverse());
		return 1;
	}

	// Quat Lerp(Q1, Q2, Alpha)
	static int32 LuaLerp(lua_State* L)
	{
		auto Q1 = FSUE4LuaStack::To<FQuat>(L, 1);
		auto Q2 = FSUE4LuaStack::To<FQuat>(L, 2);
		auto Alpha = FSUE4LuaStack::To<float>(L, 3);
		
		FSUE4LuaStack::Push(L, FMath::Lerp(Q1, Q2, Alpha));
		return 1;
	}

	// float AngularDistance(Q1, Q2)
	static int32 LuaAngularDistance(lua_State* L)
	{
		auto Q1 = FSUE4LuaStack::To<FQuat>(L, 1);
		auto Q2 = FSUE4LuaStack::To<FQuat>(L, 2);

		FSUE4LuaStack::Push(L, Q1.AngularDistance(Q2));
		return 1;
	}

	// float GetAngle(Q)
	static int32 LuaGetAngle(lua_State* L)
	{
		auto Q = FSUE4LuaStack::To<FQuat>(L, 1);

		FSUE4LuaStack::Push(L, Q.GetAngle());
		return 1;
	}

	// Vector GetRotationAxis(Q)
	static int32 LuaGetRotationAxis(lua_State* L)
	{
		auto Q = FSUE4LuaStack::To<FQuat>(L, 1);

		FSUE4LuaStack::Push(L, Q.GetRotationAxis());
		return 1;
	}

	// Vector GetAxisX(Q)
	static int32 LuaGetAxisX(lua_State* L)
	{
		auto Q = FSUE4LuaStack::To<FQuat>(L, 1);

		FSUE4LuaStack::Push(L, Q.GetAxisX());
		return 1;
	}

	// Vector GetAxisY(Q)
	static int32 LuaGetAxisY(lua_State* L)
	{
		auto Q = FSUE4LuaStack::To<FQuat>(L, 1);

		FSUE4LuaStack::Push(L, Q.GetAxisY());
		return 1;
	}

	// Vector GetAxisZ(Q)
	static int32 LuaGetAxisZ(lua_State* L)
	{
		auto Q = FSUE4LuaStack::To<FQuat>(L, 1);

		FSUE4LuaStack::Push(L, Q.GetAxisZ());
		return 1;
	}

	// Quat GetNormal(Q)
	static int32 LuaGetNormal(lua_State* L)
	{
		auto Q = FSUE4LuaStack::To<FQuat>(L, 1);

		FSUE4LuaStack::Push(L, Q.GetNormalized());
		return 1;
	}

	// Rotator ToRotator(Q)
	static int32 LuaToRotator(lua_State* L)
	{
		auto Q = FSUE4LuaStack::To<FQuat>(L, 1);

		FSUE4LuaStack::Push(L, Q.Rotator());
		return 1;
	}

	static void Register(lua_State* L)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "Quat");

		// Operators
		{
			lua_getfield(L, -1, "metaTable");
			// Stack: UE4, Quat, metaTable

			lua_pushstring(L, "__add");
			lua_pushcfunction(L, &LuaOperatorAdd);
			lua_rawset(L, -3);

			lua_pushstring(L, "__sub");
			lua_pushcfunction(L, &LuaOperatorSubtract);
			lua_rawset(L, -3);

			lua_pushstring(L, "__mul");
			lua_pushcfunction(L, &LuaOperatorMultiply);
			lua_rawset(L, -3);

			lua_pushstring(L, "__div");
			lua_pushcfunction(L, &LuaOperatorDivide);
			lua_rawset(L, -3);

			lua_pushstring(L, "__eq");
			lua_pushcfunction(L, &LuaOperatorEqual);
			lua_rawset(L, -3);

			lua_pop(L, 1);
			// Stack: UE4, Quat
		}

		lua_pushstring(L, "Equals");
		lua_pushcfunction(L, &LuaEquals);
		lua_rawset(L, -3);

		lua_pushstring(L, "Size");
		lua_pushcfunction(L, &LuaSize);
		lua_rawset(L, -3);

		lua_pushstring(L, "SizeSquared");
		lua_pushcfunction(L, &LuaSizeSquared);
		lua_rawset(L, -3);

		lua_pushstring(L, "IsNormalized");
		lua_pushcfunction(L, &LuaIsNormalized);
		lua_rawset(L, -3);
		
		lua_pushstring(L, "Inverse");
		lua_pushcfunction(L, &LuaInverse);
		lua_rawset(L, -3);

		lua_pushstring(L, "Lerp");
		lua_pushcfunction(L, &LuaLerp);
		lua_rawset(L, -3);

		lua_pushstring(L, "AngularDistance");
		lua_pushcfunction(L, &LuaAngularDistance);
		lua_rawset(L, -3);

		lua_pushstring(L, "GetAngle");
		lua_pushcfunction(L, &LuaGetAngle);
		lua_rawset(L, -3);

		lua_pushstring(L, "GetRotationAxis");
		lua_pushcfunction(L, &LuaGetRotationAxis);
		lua_rawset(L, -3);

		lua_pushstring(L, "GetAxisX");
		lua_pushcfunction(L, &LuaGetAxisX);
		lua_rawset(L, -3);

		lua_pushstring(L, "GetAxisY");
		lua_pushcfunction(L, &LuaGetAxisY);
		lua_rawset(L, -3);

		lua_pushstring(L, "GetAxisZ");
		lua_pushcfunction(L, &LuaGetAxisZ);
		lua_rawset(L, -3);

		lua_pushstring(L, "GetNormal");
		lua_pushcfunction(L, &LuaGetNormal);
		lua_rawset(L, -3);

		lua_pushstring(L, "ToRotator");
		lua_pushcfunction(L, &LuaToRotator);
		lua_rawset(L, -3);

		lua_pop(L, 2);
		// Stack: (empty)
	}
}

//=============================================================================================================================
// namespace SUE4LuaColorLibrary
//=============================================================================================================================
namespace SUE4LuaColorLibrary
{
	// operator+(C1, C2)
	static int32 LuaOperatorAdd(lua_State* L)
	{
		auto C1 = FSUE4LuaStack::To<FColor>(L, 1);
		auto C2 = FSUE4LuaStack::To<FColor>(L, 2);
		C1 += C2;

		FSUE4LuaStack::Push(L, C1);
		return 1;
	}
	
	// operator==(C1, C2)
	static int32 LuaOperatorEqual(lua_State* L)
	{
		auto C1 = FSUE4LuaStack::To<FColor>(L, 1);
		auto C2 = FSUE4LuaStack::To<FColor>(L, 2);

		FSUE4LuaStack::Push(L, C1 == C2);
		return 1;
	}

	// LinearColor ToLinearColor(C)
	static int32 LuaToLinearColor(lua_State* L)
	{
		auto C = FSUE4LuaStack::To<FColor>(L, 1);
		
		FSUE4LuaStack::Push(L, C.ReinterpretAsLinear());
		return 1;
	}

	static void Register(lua_State* L)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "Color");

		// Operators
		{
			lua_getfield(L, -1, "metaTable");
			// Stack: UE4, Color, metaTable

			lua_pushstring(L, "__add");
			lua_pushcfunction(L, &LuaOperatorAdd);
			lua_rawset(L, -3);

			lua_pushstring(L, "__eq");
			lua_pushcfunction(L, &LuaOperatorEqual);
			lua_rawset(L, -3);

			lua_pop(L, 1);
			// Stack: UE4, Color
		}

		lua_pushstring(L, "ToLinearColor");
		lua_pushcfunction(L, &LuaToLinearColor);
		lua_rawset(L, -3);

		lua_pop(L, 2);
		// Stack: (empty)
	}
}

//=============================================================================================================================
// namespace SUE4LuaLinearColorLibrary
//=============================================================================================================================
namespace SUE4LuaLinearColorLibrary
{	
	// operator+(LC1, LC2)
	static int32 LuaOperatorAdd(lua_State* L)
	{
		auto LC1 = FSUE4LuaStack::To<FLinearColor>(L, 1);
		auto LC2 = FSUE4LuaStack::To<FLinearColor>(L, 2);

		FSUE4LuaStack::Push(L, LC1 + LC2);
		return 1;
	}

	// operator-(LC1, LC2)
	static int32 LuaOperatorSubtract(lua_State* L)
	{
		auto LC1 = FSUE4LuaStack::To<FLinearColor>(L, 1);
		auto LC2 = FSUE4LuaStack::To<FLinearColor>(L, 2);

		FSUE4LuaStack::Push(L, LC1 - LC2);
		return 1;
	}

	// operator*(LC1, Scalar or LC2)
	static int32 LuaOperatorMultiply(lua_State* L)
	{
		auto LC1 = FSUE4LuaStack::To<FLinearColor>(L, 1);

		if (lua_isnumber(L, 2))
		{
			auto Scalar = FSUE4LuaStack::To<float>(L, 2);
			FSUE4LuaStack::Push(L, LC1 * Scalar);
		}
		else
		{
			auto LC2 = FSUE4LuaStack::To<FLinearColor>(L, 2);
			FSUE4LuaStack::Push(L, LC1 * LC2);
		}

		return 1;
	}

	// operator/(LC1, Scalar or LC2)
	static int32 LuaOperatorDivide(lua_State* L)
	{
		auto LC1 = FSUE4LuaStack::To<FLinearColor>(L, 1);

		if (lua_isnumber(L, 2))
		{
			auto Scalar = FSUE4LuaStack::To<float>(L, 2);
			FSUE4LuaStack::Push(L, LC1 / Scalar);
		}
		else
		{
			auto LC2 = FSUE4LuaStack::To<FLinearColor>(L, 2);
			FSUE4LuaStack::Push(L, LC1 / LC2);
		}

		return 1;
	}

	// operator==(LC1, LC2)
	static int32 LuaOperatorEqual(lua_State* L)
	{
		auto LC1 = FSUE4LuaStack::To<FLinearColor>(L, 1);
		auto LC2 = FSUE4LuaStack::To<FLinearColor>(L, 2);
		
		FSUE4LuaStack::Push(L, LC1 == LC2);
		return 1;
	}

	// bool Equals(LC1, LC2, Tolerance?)
	static int32 LuaEquals(lua_State* L)
	{
		auto LC1 = FSUE4LuaStack::To<FLinearColor>(L, 1);
		auto LC2 = FSUE4LuaStack::To<FLinearColor>(L, 2);

		bool bRet = false;

		if (lua_isnumber(L, 3))
		{
			auto Tolerance = FSUE4LuaStack::To<float>(L, 3);
			bRet = LC1.Equals(LC2, Tolerance);
		}
		else
		{
			bRet = LC1.Equals(LC2);
		}

		FSUE4LuaStack::Push(L, bRet);
		return 1;
	}

	// bool IsAlmostBlack(LC)
	static int32 LuaIsAlmostBlack(lua_State* L)
	{
		auto LC = FSUE4LuaStack::To<FLinearColor>(L, 1);

		FSUE4LuaStack::Push(L, LC.IsAlmostBlack());
		return 1;
	}

	// LinearColor Clamp(LC, Min, Max)
	static int32 LuaClamp(lua_State* L)
	{
		auto LC = FSUE4LuaStack::To<FLinearColor>(L, 1);
		auto Min = FSUE4LuaStack::To<float>(L, 2);
		auto Max = FSUE4LuaStack::To<float>(L, 3);

		FSUE4LuaStack::Push(L, LC.GetClamped(Min, Max));
		return 1;
	}

	// LinearColor Lerp(LC1, LC2, Alpha)
	static int32 LuaLerp(lua_State* L)
	{
		auto LC1 = FSUE4LuaStack::To<FLinearColor>(L, 1);
		auto LC2 = FSUE4LuaStack::To<FLinearColor>(L, 2);
		auto Alpha = FSUE4LuaStack::To<float>(L, 3);

		FSUE4LuaStack::Push(L, LC1 + Alpha * (LC2 - LC1));
		return 1;
	}

	// LinearColor LerpUsingHSV(LC1, LC2, Alpha)
	static int32 LuaLerpUsingHSV(lua_State* L)
	{
		auto LC1 = FSUE4LuaStack::To<FLinearColor>(L, 1);
		auto LC2 = FSUE4LuaStack::To<FLinearColor>(L, 2);
		auto Alpha = FSUE4LuaStack::To<float>(L, 3);

		FSUE4LuaStack::Push(L, FLinearColor::LerpUsingHSV(LC1, LC2, Alpha));
		return 1;
	}
	
	// LinearColor Desaturate(LC, Desaturation)
	static int32 LuaDesaturate(lua_State* L)
	{
		auto LC = FSUE4LuaStack::To<FLinearColor>(L, 1);
		auto Desaturation = FSUE4LuaStack::To<float>(L, 2);

		FSUE4LuaStack::Push(L, LC.Desaturate(Desaturation));
		return 1;
	}

	// float Distance(LC1, LC2)
	static int32 LuaDistance(lua_State* L)
	{
		auto LC1 = FSUE4LuaStack::To<FLinearColor>(L, 1);
		auto LC2 = FSUE4LuaStack::To<FLinearColor>(L, 2);

		FSUE4LuaStack::Push(L, FLinearColor::Dist(LC1, LC2));
		return 1;
	}

	// bool GetMax(LC)
	static int32 LuaGetMax(lua_State* L)
	{
		auto LC = FSUE4LuaStack::To<FLinearColor>(L, 1);

		FSUE4LuaStack::Push(L, LC.GetMax());
		return 1;
	}

	// bool GetMin(LC)
	static int32 LuaGetMin(lua_State* L)
	{
		auto LC = FSUE4LuaStack::To<FLinearColor>(L, 1);

		FSUE4LuaStack::Push(L, LC.GetMin());
		return 1;
	}

	// bool GetLuminance(LC)
	static int32 LuaGetLuminance(lua_State* L)
	{
		auto LC = FSUE4LuaStack::To<FLinearColor>(L, 1);

		FSUE4LuaStack::Push(L, LC.GetLuminance());
		return 1;
	}

	// Color ToColor(LC, bSRGB)
	static int32 LuaToColor(lua_State* L)
	{
		auto LC = FSUE4LuaStack::To<FLinearColor>(L, 1);
		auto bSRGB = FSUE4LuaStack::To<bool>(L, 2);

		FSUE4LuaStack::Push(L, LC.ToFColor(bSRGB));
		return 1;
	}

	static void Register(lua_State* L)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "LinearColor");

		// Operators
		{
			lua_getfield(L, -1, "metaTable");
			// Stack: UE4, LinearColor, metaTable

			lua_pushstring(L, "__add");
			lua_pushcfunction(L, &LuaOperatorAdd);
			lua_rawset(L, -3);

			lua_pushstring(L, "__sub");
			lua_pushcfunction(L, &LuaOperatorSubtract);
			lua_rawset(L, -3);

			lua_pushstring(L, "__mul");
			lua_pushcfunction(L, &LuaOperatorMultiply);
			lua_rawset(L, -3);

			lua_pushstring(L, "__div");
			lua_pushcfunction(L, &LuaOperatorDivide);
			lua_rawset(L, -3);

			lua_pushstring(L, "__eq");
			lua_pushcfunction(L, &LuaOperatorEqual);
			lua_rawset(L, -3);

			lua_pop(L, 1);
			// Stack: UE4, LinearColor
		}

		lua_pushstring(L, "Equals");
		lua_pushcfunction(L, &LuaEquals);
		lua_rawset(L, -3);

		lua_pushstring(L, "IsAlmostBlack");
		lua_pushcfunction(L, &LuaIsAlmostBlack);
		lua_rawset(L, -3);

		lua_pushstring(L, "Clamp");
		lua_pushcfunction(L, &LuaClamp);
		lua_rawset(L, -3);

		lua_pushstring(L, "Lerp");
		lua_pushcfunction(L, &LuaLerp);
		lua_rawset(L, -3);

		lua_pushstring(L, "LerpUsingHSV");
		lua_pushcfunction(L, &LuaLerpUsingHSV);
		lua_rawset(L, -3);

		lua_pushstring(L, "Desaturate");
		lua_pushcfunction(L, &LuaDesaturate);
		lua_rawset(L, -3);

		lua_pushstring(L, "Distance");
		lua_pushcfunction(L, &LuaDistance);
		lua_rawset(L, -3);

		lua_pushstring(L, "GetMax");
		lua_pushcfunction(L, &LuaGetMax);
		lua_rawset(L, -3);

		lua_pushstring(L, "GetMin");
		lua_pushcfunction(L, &LuaGetMin);
		lua_rawset(L, -3);

		lua_pushstring(L, "GetLuminance");
		lua_pushcfunction(L, &LuaGetLuminance);
		lua_rawset(L, -3);

		lua_pushstring(L, "ToColor");
		lua_pushcfunction(L, &LuaToColor);
		lua_rawset(L, -3);

		lua_pop(L, 2);
		// Stack: (empty)
	}
}

//=============================================================================================================================
// namespace SUE4LuaTextFormatterLibrary
//=============================================================================================================================
namespace SUE4LuaTextFormatterLibrary
{
	// string Format(Text, ...)
	static int32 LuaFormat(lua_State* L)
	{
		int32 TopIndex = lua_gettop(L);

		if (TopIndex == 0)
		{
			FSUE4LuaStack::Push(L, FString());
			return 1;
		}
		else
		{
			FText FormatText = FSUE4LuaStack::To<FText>(L, 1);

			if (TopIndex == 2 && lua_istable(L, 2))
			{
				// FFormatNamedArguments로 처리

				FFormatNamedArguments FormatArgs;
				{
					lua_pushvalue(L, -1);
					lua_pushnil(L);
					// Stack: NamedArgTable, nil

					while (lua_next(L, -2))
					{
						// Stack: NamedArgTable, key, value
						lua_pushvalue(L, -2);
						// Stack: NamedArgTable, key, value, key

						if (lua_isstring(L, -1))
						{
							FString KeyStr = FSUE4LuaStack::Pop<FString>(L);
							// Stack: NamedArgTable, key, value

							// lua_isnumber()는 스트링을 숫자로 파싱해보기 때문에 스트링인지 제일 먼저 확인
							if (lua_isstring(L, -1))
							{
								FormatArgs.Add(KeyStr, FSUE4LuaStack::Pop<FText>(L));
							}
							else if (lua_isinteger(L, -1))
							{
								FormatArgs.Add(KeyStr, FSUE4LuaStack::Pop<int32>(L));
							}
							else if (lua_isnumber(L, -1))
							{
								FormatArgs.Add(KeyStr, FSUE4LuaStack::Pop<float>(L));
							}
							else
							{
								SUE4LVM_ERROR(L, TEXT("UE4.TextFormatter.Format(): not supported arg type: {%s}"), *KeyStr);

								FSUE4LuaStack::Push(L, FString());
								return 1;
							}
						}
						else
						{
							lua_pop(L, 1);
						}

						// Stack: NamedArgTable, key
					}

					// Stack: NamedArgTable
					lua_pop(L, 1);
					// Stack: (empty)
				}

				FSUE4LuaStack::Push(L, FTextFormatter::Format(FormatText, MoveTemp(FormatArgs), false, false));
				return 1;
			}
			else
			{
				// TArray<FFormatArgumentData>로 처리

				TArray<FFormatArgumentValue> FormatArgs;
				for (int32 Index = 2; Index <= TopIndex; ++Index)
				{
					FFormatArgumentValue FormatArg;

					// lua_isnumber()는 스트링을 숫자로 파싱해보기 때문에 스트링인지 제일 먼저 확인
					if (lua_isstring(L, Index))
					{
						FormatArg = FSUE4LuaStack::To<FText>(L, Index);
					}
					else if (lua_isinteger(L, Index))
					{
						FormatArg = FSUE4LuaStack::To<int32>(L, Index);
					}
					else if (lua_isnumber(L, Index))
					{
						FormatArg = FSUE4LuaStack::To<float>(L, Index);
					}
					else
					{
						SUE4LVM_ERROR(L, TEXT("UE4.TextFormatter.Format(): not supported arg type: {%d}"), Index - 2);

						FSUE4LuaStack::Push(L, FString());
						return 1;
					}

					FormatArgs.Add(MoveTemp(FormatArg));
				}

				FSUE4LuaStack::Push(L, FTextFormatter::Format(FormatText, MoveTemp(FormatArgs), false, false));
				return 1;
			}
		}
	}

	static void Register(lua_State* L)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "TextFormatter");
		// Stack: UE4, TextFormatter

		lua_pushcfunction(L, &LuaFormat);
		lua_setfield(L, -2, "Format");

		// Stack: UE4, TextFormatter
		lua_pop(L, 2);
		// Stack: (empty)
	}
}

//=============================================================================================================================
// namespace SUE4LuaPointerEventLibrary
//=============================================================================================================================
namespace SUE4LuaPointerEventLibrary
{
	// bool IsMouseLeftButtonDown(PointEvent)
	static int32 LuaIsMouseLeftButtonDown(lua_State* L)
	{
		if (!SUE4LuaUStruct::IsUStruct(L, 1))
		{
			SUE4LVM_ERROR(L, TEXT("UE4.IsMouseLeftButtonDown(): Invalid arg1"));

			FSUE4LuaStack::Push(L, false);
			return 1;
		}

		auto PointerEvent = FSUE4LuaStack::To<FPointerEvent>(L, 1);
		FSUE4LuaStack::Push(L, PointerEvent.IsMouseButtonDown(EKeys::LeftMouseButton));
		return 1;
	}

	// bool IsMouseRightButtonDown(PointEvent)
	static int32 LuaIsMouseRightButtonDown(lua_State* L)
	{
		if (!SUE4LuaUStruct::IsUStruct(L, 1))
		{
			SUE4LVM_ERROR(L, TEXT("UE4.IsMouseRightButtonDown(): Invalid arg1"));

			FSUE4LuaStack::Push(L, false);
			return 1;
		}

		auto PointerEvent = FSUE4LuaStack::To<FPointerEvent>(L, 1);
		FSUE4LuaStack::Push(L, PointerEvent.IsMouseButtonDown(EKeys::RightMouseButton));
		return 1;
	}

	static void Register(lua_State* L)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "PointerEvent");
		// Stack: UE4, PointerEvent

		lua_pushcfunction(L, &LuaIsMouseLeftButtonDown);
		lua_setfield(L, -2, "IsLeftMouseButtonDown");

		lua_pushcfunction(L, &LuaIsMouseRightButtonDown);
		lua_setfield(L, -2, "IsRightMouseButtonDown");

		// Stack: UE4, PointerEvent
		lua_pop(L, 2);
		// Stack: (empty)
	}
}

//=============================================================================================================================
// namespace SUE4LuaLibrary
//=============================================================================================================================
namespace SUE4LuaLibrary
{
	void RegisterSUE4LuaLibrary(lua_State* L)
	{
		SUE4LuaSUE4LuaLibrary::Register(L);
	}

	void RegisterPlatformTimeLibrary(lua_State* L)
	{
		SUE4LuaPlatformTimeLibrary::Register(L);
	}

	void RegisterUE4Library(lua_State* L)
	{
		SUE4LuaUE4Library::Register(L);
	}

	void RegisterMathLibrary(lua_State* L)
	{
		SUE4LuaMathLibrary::Register(L);
	}

	void RegisterVectorLibrary(lua_State* L)
	{
		SUE4LuaVectorLibrary::Register(L);
	}

	void RegisterVector2DLibrary(lua_State* L)
	{
		SUE4LuaVector2DLibrary::Register(L);
	}

	void RegisterTransformLibrary(lua_State* L)
	{
		SUE4LuaTransformLibrary::Register(L);
	}

	void RegisterRotatorLibrary(lua_State* L)
	{
		SUE4LuaRotatorLibrary::Register(L);
	}

	void RegisterQuatLibrary(lua_State* L)
	{
		SUE4LuaQuatLibrary::Register(L);
	}

	void RegisterColorLibrary(lua_State* L)
	{
		SUE4LuaColorLibrary::Register(L);
	}

	void RegisterLinearColorLibrary(lua_State* L)
	{
		SUE4LuaLinearColorLibrary::Register(L);
	}

	void RegisterTextFormatterLibrary(lua_State* L)
	{
		SUE4LuaTextFormatterLibrary::Register(L);
	}

	void RegisterPointerEventLibrary(lua_State* L)
	{
		SUE4LuaPointerEventLibrary::Register(L);
	}
}