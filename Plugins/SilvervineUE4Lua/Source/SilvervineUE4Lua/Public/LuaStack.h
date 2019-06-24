// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "CoreUObject.h"


class FSUE4LuaFunction;
class FSUE4LuaThread;
struct FExpressionInput;
struct FQualifiedFrameTime;
struct lua_State;

//
// lua 스택의 푸시, 팝 실수를 막기 위한 장치
//
class SILVERVINEUE4LUA_API FSUE4LuaStackGuard : public FNoncopyable
{
public:
	FSUE4LuaStackGuard(lua_State* InL);
	~FSUE4LuaStackGuard();

private:
	lua_State* L = nullptr;
	int32 SavedTopIndex = 0;
};

//
// lua 스택에 객체를 넣거나 가져올 때 사용하는 클래스. 내부 구현에만 사용됩니다.
//
class SILVERVINEUE4LUA_API FSUE4LuaStack
{
public:
	// nil을 전달할 때 사용
	struct FNil { bool operator==(const FNil& R) { return true; } };
	static const FNil Nil;

	template<typename ...ArgTypes>
	static void Push(lua_State* L, const ArgTypes&... Args);

	template<typename RetType>
	static RetType Pop(lua_State* L);

	template<typename RetType>
	static RetType To(lua_State* L, int32 Index);

	template<typename UStructType, typename = TEnableIf<TIsFunction<decltype(UStructType::StaticStruct)>::Value>::Type>
	static UStructType*  GetRef(lua_State* L, int32 Index)
	{
		return reinterpret_cast<UStructType*>(GetUStructRef(L, Index, UStructType::StaticStruct()));
	}

	static bool IsPrimitiveStructTypeOf(lua_State* L, int32 Index, const char* TypeName);

private:
	FSUE4LuaStack();
	~FSUE4LuaStack();

private:
	static void LuaPushNil(lua_State* L);
	static void LuaPop(lua_State* L, int32 N);

	template<typename ArgType, typename ...RestArgTypes>
	static void PushInternal(lua_State* L, const ArgType& Arg, const RestArgTypes&... RestArgs);
	static void PushInternal(lua_State* L) {}

	// nil 처리
	static void PushObject(lua_State* L, const FNil&)					{ LuaPushNil(L); }
	static void ToObject(lua_State*L, FNil&, int32 Index);

	// 정수 값타입 처리
	static void PushObject(lua_State* L, int8 InValue)					{ PushInt64(L, InValue); }
	static void ToObject(lua_State* L, int8& OutValue, int32 Index)		{ OutValue = ToInt64(L, Index); }
	static void PushObject(lua_State* L, uint8 InValue)					{ PushInt64(L, InValue); }
	static void ToObject(lua_State* L, uint8& OutValue, int32 Index)	{ OutValue = ToInt64(L, Index); }
	static void PushObject(lua_State* L, int16 InValue)					{ PushInt64(L, InValue); }
	static void ToObject(lua_State* L, int16& OutValue, int32 Index)	{ OutValue = ToInt64(L, Index); }
	static void PushObject(lua_State* L, uint16 InValue)				{ PushInt64(L, InValue); }
	static void ToObject(lua_State* L, uint16& OutValue, int32 Index)	{ OutValue = ToInt64(L, Index); }
	static void PushObject(lua_State* L, int32 InValue)					{ PushInt64(L, InValue); }
	static void ToObject(lua_State* L, int32& OutValue, int32 Index)	{ OutValue = ToInt64(L, Index); }
	static void PushObject(lua_State* L, uint32 InValue)				{ PushInt64(L, InValue); }
	static void ToObject(lua_State* L, uint32& OutValue, int32 Index)	{ OutValue = ToInt64(L, Index); }
	static void PushObject(lua_State* L, int64 InValue)					{ PushInt64(L, InValue); }
	static void ToObject(lua_State* L, int64& OutValue, int32 Index)	{ OutValue = ToInt64(L, Index); }
	static void PushObject(lua_State* L, uint64 InValue)				{ PushInt64(L, InValue); }
	static void ToObject(lua_State* L, uint64& OutValue, int32 Index)	{ OutValue = ToInt64(L, Index); }

	static void PushInt64(lua_State* L, int64 Value);
	static int64 ToInt64(lua_State* L, int32 Index);

	// enum 값타입 처리
	template<typename EnumType>
	static typename TEnableIf<TIsEnum<EnumType>::Value>::Type PushObject(lua_State* L, EnumType InValue)				{ PushInt64(L, static_cast<int64>(InValue)); }
	template<typename EnumType>
	static typename TEnableIf<TIsEnum<EnumType>::Value>::Type ToObject(lua_State* L, EnumType& OutValue, int32 Index)	{ OutValue = static_cast<EnumType>(ToInt64(L, Index)); }
	template<typename EnumType>
	static void PushObject(lua_State* L, TEnumAsByte<EnumType> InValue)													{ PushInt64(L, static_cast<int64>(InValue)); }
	template<typename EnumType>
	static void ToObject(lua_State* L, TEnumAsByte<EnumType>& OutValue, int32 Index)									{ OutValue = static_cast<EnumType>(ToInt64(L, Index)); }

	// 부동소수점 값타입 처리
	static void PushObject(lua_State* L, float InValue)					{ PushDouble(L, InValue); }
	static void ToObject(lua_State* L, float& OutValue, int32 Index)	{ OutValue = ToFloat(L, Index); }
	static void PushObject(lua_State* L, double InValue)				{ PushDouble(L, InValue); }
	static void ToObject(lua_State* L, double& OutValue, int32 Index)	{ OutValue = ToDouble(L, Index); }

	static void PushDouble(lua_State* L, double Value);
	static double ToFloat(lua_State* L, int32 Index)					{ return static_cast<float>(ToDouble(L, Index)); }
	static double ToDouble(lua_State* L, int32 Index);

	// 불 값타입 처리
	static void PushObject(lua_State* L, bool InValue);
	static void ToObject(lua_State* L, bool& OutValue, int32 Index);

	// 스트링 처리
	static void PushObject(lua_State* L, const char* InValue)			{ PushStringUTF8(L, InValue); }
	static void PushObject(lua_State* L, const TCHAR* InValue)			{ PushStringTCHAR(L, InValue); }
	static void PushObject(lua_State* L, const FString& InValue)		{ PushStringTCHAR(L, *InValue); }
	static void ToObject(lua_State* L, FString& OutValue, int32 Index)	{ OutValue = ToStringTCHAR(L, Index); }
	static void PushObject(lua_State* L, const FText& InValue)			{ PushStringTCHAR(L, *InValue.ToString()); }
	static void ToObject(lua_State* L, FText& OutValue, int32 Index)	{ OutValue = FText::FromString(*ToStringTCHAR(L, Index)); }
	static void PushObject(lua_State* L, const FName InValue);
	static void ToObject(lua_State* L, FName& OutValue, int32 Index);

	static void PushStringUTF8(lua_State* L, const char* InValue);
	static void PushStringTCHAR(lua_State* L, const TCHAR* InValue);
	static FString ToStringTCHAR(lua_State* L, int32 Index);

	// noexport타입의 구조체 처리
	static void PushObject(lua_State* L, const FVector& InValue);
	static void ToObject(lua_State* L, FVector& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FVector2D& InValue);
	static void ToObject(lua_State* L, FVector2D& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FVector4& InValue);
	static void ToObject(lua_State* L, FVector4& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FIntPoint& InValue);
	static void ToObject(lua_State* L, FIntPoint& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FIntVector& InValue);
	static void ToObject(lua_State* L, FIntVector& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FPlane& InValue);
	static void ToObject(lua_State* L, FPlane& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FMatrix& InValue);
	static void ToObject(lua_State* L, FMatrix& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FRotator& InValue);
	static void ToObject(lua_State* L, FRotator& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FQuat& InValue);
	static void ToObject(lua_State* L, FQuat& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FLinearColor& InValue);
	static void ToObject(lua_State* L, FLinearColor& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FColor& InValue);
	static void ToObject(lua_State* L, FColor& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FTransform& InValue);
	static void ToObject(lua_State* L, FTransform& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FBox& InValue);
	static void ToObject(lua_State* L, FBox& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FBox2D& InValue);
	static void ToObject(lua_State* L, FBox2D& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FBoxSphereBounds& InValue);
	static void ToObject(lua_State* L, FBoxSphereBounds& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FGuid& InValue);
	static void ToObject(lua_State* L, FGuid& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FDateTime& InValue);
	static void ToObject(lua_State* L, FDateTime& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FTimecode& InValue);
	static void ToObject(lua_State* L, FTimecode& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FTimespan& InValue);
	static void ToObject(lua_State* L, FTimespan& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FRandomStream& InValue);
	static void ToObject(lua_State* L, FRandomStream& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FFrameRate& InValue);
	static void ToObject(lua_State* L, FFrameRate& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FFrameNumber& InValue);
	static void ToObject(lua_State* L, FFrameNumber& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FFrameTime& InValue);
	static void ToObject(lua_State* L, FFrameTime& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FQualifiedFrameTime& InValue);
	static void ToObject(lua_State* L, FQualifiedFrameTime& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FPrimaryAssetType& InValue);
	static void ToObject(lua_State* L, FPrimaryAssetType& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FPrimaryAssetId& InValue);
	static void ToObject(lua_State* L, FPrimaryAssetId& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FSoftObjectPath& InValue);
	static void ToObject(lua_State* L, FSoftObjectPath& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FFloatRangeBound& InValue);
	static void ToObject(lua_State* L, FFloatRangeBound& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FFloatRange& InValue);
	static void ToObject(lua_State* L, FFloatRange& OutValue, int32 Index);
	static void PushObject(lua_State* L, const FInt32Interval& InValue);
	static void ToObject(lua_State* L, FInt32Interval& OutValue, int32 Index);

	// noexport타입이지만 구조가 복잡해서 UStruct로 처리하는 타입들
	static const UScriptStruct* GetCoreUObjectUStructStaticClass(const TCHAR* ClassName);
	static void PushObject(lua_State* L, const FPolyglotTextData& InValue)				{ PushUStruct(L, TBaseStructure<FPolyglotTextData>::Get(), &InValue); }
	static void ToObject(lua_State* L, FPolyglotTextData& OutValue, int32 Index)		{ ToUStruct(L, Index, TBaseStructure<FPolyglotTextData>::Get(), &OutValue); }
	static void PushObject(lua_State* L, const FInterpCurveFloat& InValue)				{ PushUStruct(L, GetCoreUObjectUStructStaticClass(TEXT("InterpCurveFloat")), &InValue); }
	static void ToObject(lua_State* L, FInterpCurveFloat& OutValue, int32 Index)		{ ToUStruct(L, Index, GetCoreUObjectUStructStaticClass(TEXT("InterpCurveFloat")), &OutValue); }
	static void PushObject(lua_State* L, const FInterpCurveVector2D& InValue)			{ PushUStruct(L, GetCoreUObjectUStructStaticClass(TEXT("InterpCurveVector2D")), &InValue); }
	static void ToObject(lua_State* L, FInterpCurveVector2D& OutValue, int32 Index)		{ ToUStruct(L, Index, GetCoreUObjectUStructStaticClass(TEXT("InterpCurveVector2D")), &OutValue); }
	static void PushObject(lua_State* L, const FInterpCurveVector& InValue)				{ PushUStruct(L, GetCoreUObjectUStructStaticClass(TEXT("InterpCurveVector")), &InValue); }
	static void ToObject(lua_State* L, FInterpCurveVector& OutValue, int32 Index)		{ ToUStruct(L, Index, GetCoreUObjectUStructStaticClass(TEXT("InterpCurveVector")), &OutValue); }
	static void PushObject(lua_State* L, const FInterpCurveTwoVectors& InValue)			{ PushUStruct(L, GetCoreUObjectUStructStaticClass(TEXT("InterpCurveTwoVectors")), &InValue); }
	static void ToObject(lua_State* L, FInterpCurveTwoVectors& OutValue, int32 Index)	{ ToUStruct(L, Index, GetCoreUObjectUStructStaticClass(TEXT("InterpCurveTwoVectors")), &OutValue); }
	static void PushObject(lua_State* L, const FInterpCurveLinearColor& InValue)		{ PushUStruct(L, GetCoreUObjectUStructStaticClass(TEXT("InterpCurveLinearColor")), &InValue); }
	static void ToObject(lua_State* L, FInterpCurveLinearColor& OutValue, int32 Index)	{ ToUStruct(L, Index, GetCoreUObjectUStructStaticClass(TEXT("InterpCurveLinearColor")), &OutValue); }
	static void PushObject(lua_State* L, const FExpressionInput& InValue)				{ PushUStruct(L, GetCoreUObjectUStructStaticClass(TEXT("ExpressionInput")), &InValue); }
	static void ToObject(lua_State* L, FExpressionInput& OutValue, int32 Index)			{ ToUStruct(L, Index, GetCoreUObjectUStructStaticClass(TEXT("ExpressionInput")), &OutValue); }

	// 함수 타입 처리
	static void PushObject(lua_State* L, TSharedPtr<FSUE4LuaFunction> InValue);
	static void ToObject(lua_State* L, TSharedPtr<FSUE4LuaFunction>& OutValue, int32 Index);

	// UObject 타입 처리
	static void PushObject(lua_State* L, const UObject* InValue);
	template<typename UObjectType, typename = TEnableIf<TIsDerivedFrom<UObjectType, UObject>::IsDerived>::Type>
	static void ToObject(lua_State* L, UObjectType*& OutValue, int32 Index)
	{
		OutValue = Cast<UObjectType>(ToUObject(L, Index, UObjectType::StaticClass()));
	}
	static UObject* ToUObject(lua_State* L, int32 Index, UClass* Class);

	// UStruct 타입 처리
	template<typename UStructType, typename = TEnableIf<TIsFunction<decltype(UStructType::StaticStruct)>::Value>::Type>
	static void PushObject(lua_State* L, const UStructType& InValue)
	{
		PushUStruct(L, UStructType::StaticStruct(), &InValue);
	}
	template<typename UStructType, typename = TEnableIf<TIsFunction<decltype(UStructType::StaticStruct)>::Value>::Type>
	static void ToObject(lua_State* L, UStructType& OutValue, int32 Index)
	{
		ToUStruct(L, Index, UStructType::StaticStruct(), &OutValue);
	}
	static void PushUStruct(lua_State* L, const UScriptStruct* Struct, const void* Src);
	static void ToUStruct(lua_State* L, int32 Index, const UScriptStruct* Struct, void* Dest);
	static void* GetUStructRef(lua_State* L, int32 Index, const UScriptStruct* Struct);
};

template<typename ...ArgTypes>
inline void FSUE4LuaStack::Push(lua_State* L, const ArgTypes&... Args)
{
	PushInternal(L, Args...);
}

template<typename ArgType, typename ...RestArgTypes>
inline void FSUE4LuaStack::PushInternal(lua_State* L, const ArgType& Arg, const RestArgTypes&... RestArgs)
{
	PushObject(L, Arg);
	PushInternal(L, RestArgs...);
}

template<typename RetType>
inline RetType FSUE4LuaStack::Pop(lua_State* L)
{
	RetType RetValue = To<RetType>(L, -1);
	
	LuaPop(L, 1);
	return RetValue;
}

template<>
inline void FSUE4LuaStack::Pop<void>(lua_State* L)
{
}

template<typename RetType>
inline RetType FSUE4LuaStack::To(lua_State* L, int32 Index)
{
	RetType RetValue;
	
	ToObject(L, RetValue, Index);

	return RetValue;
}