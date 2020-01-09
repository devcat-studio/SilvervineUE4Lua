// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "LuaFunction.h"

#include "SilvervineUE4LuaValue.generated.h"


//
// lua 값을 c++에 저장하고 싶을 때 사용합니다.
// 저장된 lua 값이 함수면 직접 호출하거나 delegate에 바인딩할 수 있습니다.
//
UCLASS(BlueprintType)
class SILVERVINEUE4LUA_API USUE4LuaValue : public UObject
{
	GENERATED_BODY()
	
public:
	enum class EValueType
	{
		NotAssigned,

		Bool,
		Integer,
		Number,
		String,
		Function,
		Table,
		UserData,

		// 스레드는 아직 지원하지 않습니다.
		// Thread,

		UObject,
	};

	static USUE4LuaValue* Create(TSharedPtr<FSUE4LuaFunction> LuaFunction);

	FORCEINLINE EValueType	GetValueType() const	{ return ValueType; }
	FORCEINLINE bool		IsBoolType() const		{ return ValueType == EValueType::Bool; }
	FORCEINLINE bool		IsIntegerType() const	{ return ValueType == EValueType::Integer; }
	FORCEINLINE bool		IsNumberType() const	{ return ValueType == EValueType::Number; }
	FORCEINLINE bool		IsStringType() const	{ return ValueType == EValueType::String; }
	FORCEINLINE bool		IsFunctionType() const	{ return ValueType == EValueType::Function; }
	FORCEINLINE bool		IsTableType() const		{ return ValueType == EValueType::Table; }
	FORCEINLINE bool		IsUserDataType() const	{ return ValueType == EValueType::UserData; }
	FORCEINLINE bool		IsUObjectType() const	{ return ValueType == EValueType::UObject; }

	bool							GetBool() const		{ if (IsBoolType()) return bBoolValue; else return false; }
	int64							GetInteger() const	{ if (IsIntegerType()) return IntValue; else return 0; }
	double							GetNumber() const	{ if (IsNumberType()) return NumberValue; else return 0.0; }
	FString							GetString() const	{ return StringValue; }
	TSharedPtr<FSUE4LuaFunction>	GetFunction() const { return FunctionValue; }
	UObject*						GetUObject() const	{ return UObjectValue; }

	bool IsValidFunction() const	{ return FunctionValue.IsValid(); }
	bool IsValidTable() const		{	return ValueReferencer.IsValid(); }

	// 저장된 lua 함수를 호출
	// 반드시 IsValidFunction()일 경우에만 사용해야 합니다.
	template<typename RetType, typename ...ArgTypes>
	RetType Call(ArgTypes&&... Args)
	{
		check(IsValidFunction());

		return FunctionValue->Call<RetType>(Forward<ArgTypes>(Args)...);
	}

	// 테이블의 필드 값 반환
	// 테이블이 유효하지 않으면(IsValidTable()하지 않으면) 각 타입의 기본값을 반환합니다.
	bool							GetTableBool(const TCHAR* FieldName) const;
	int64							GetTableInteger(const TCHAR* FieldName) const;
	double							GetTableNumber(const TCHAR* FieldName) const;
	FString							GetTableString(const TCHAR* FieldName) const;
	TSharedPtr<FSUE4LuaFunction>	GetTableFunction(const TCHAR* FieldName) const;
	UObject*						GetTableUObject(const TCHAR* FieldName) const;

public:
	//
	// 아래는 내부 구현에만 사용됩니다.
	//

	// Begin UObject Interface
	virtual void BeginDestroy() override;
	virtual void ProcessEvent(UFunction* Function, void* Parms) override;
	// End UObject Interface

	static USUE4LuaValue* CreateFromStack(lua_State* L, int32 StackIndex);
	void Push(lua_State* L) const;

	bool Bind(lua_State* L, FScriptDelegate& InDelegate, UFunction* InSignatureFunction);
	bool AddUnique(lua_State* L, FMulticastScriptDelegate& InMulticastDelegate, UFunction* InSignatureFunction);

	// 딜리게이트 바인딩을 위한 더미 함수. 실제로 이 함수는 호출되지 않습니다.
	UFUNCTION()
	void DummyFunc() {}

private:
	EValueType ValueType = EValueType::NotAssigned;

	union
	{
		bool bBoolValue;
		int64 IntValue;
		double NumberValue;
	};
	FString StringValue;
	TSharedPtr<FSUE4LuaFunction> FunctionValue;
	TUniquePtr<FSUE4LuaValueReferencer> ValueReferencer;
	UPROPERTY()
	UObject* UObjectValue = nullptr;

	// 바인딩된 함수의 시그니쳐
	UPROPERTY()
	const UFunction* SignatureFunction = nullptr;
};