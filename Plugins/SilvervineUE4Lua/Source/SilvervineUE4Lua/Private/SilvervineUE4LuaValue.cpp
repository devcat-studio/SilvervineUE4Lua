// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#include "SilvervineUE4LuaValue.h"

#include "LuaAPI.h"
#include "LuaCall.h"
#include "LuaLog.h"
#include "LuaUProperty.h"
#include "LuaUObject.h"


namespace SUE4LuaValueImpl
{
	template<typename T>
	static T GetTableField(lua_State* L, FSUE4LuaValueReferencer* ValueReferencer, const TCHAR* FieldName)
	{
		ValueReferencer->Push(L);
		lua_getfield(L, -1, TCHAR_TO_UTF8(FieldName));
		lua_remove(L, -2);

		return FSUE4LuaStack::Pop<T>(L);
	}
}

USUE4LuaValue* USUE4LuaValue::Create(TSharedPtr<FSUE4LuaFunction> LuaFunction)
{
	if (!LuaFunction.IsValid())
	{
		return nullptr;
	}

	auto LuaValueObj = NewObject<USUE4LuaValue>();
	LuaValueObj->ValueType = EValueType::Function;
	LuaValueObj->FunctionValue = LuaFunction;

	return LuaValueObj;
}

USUE4LuaValue* USUE4LuaValue::CreateFromStack(lua_State* L, int32 StackIndex)
{
	USUE4LuaValue* ValueObj = nullptr;

	if (auto VM = FSUE4LuaVirtualMachine::PinVM(L))
	{
		if (lua_isboolean(L, StackIndex))
		{
			ValueObj = NewObject<USUE4LuaValue>();
			ValueObj->ValueType = EValueType::Bool;
			ValueObj->bBoolValue = FSUE4LuaStack::To<bool>(L, StackIndex);
		}
		else if (lua_isinteger(L, StackIndex))
		{
			ValueObj = NewObject<USUE4LuaValue>();
			ValueObj->ValueType = EValueType::Integer;
			ValueObj->IntValue = FSUE4LuaStack::To<int64>(L, StackIndex);
		}
		else if (lua_isnumber(L, StackIndex))
		{
			ValueObj = NewObject<USUE4LuaValue>();
			ValueObj->ValueType = EValueType::Number;
			ValueObj->NumberValue = FSUE4LuaStack::To<double>(L, StackIndex);
		}
		else if (lua_isstring(L, StackIndex))
		{
			ValueObj = NewObject<USUE4LuaValue>();
			ValueObj->ValueType = EValueType::String;
			ValueObj->StringValue = FSUE4LuaStack::To<FString>(L, StackIndex);
		}
		else if (lua_isfunction(L, StackIndex))
		{
			ValueObj = Create(FSUE4LuaFunction::CreateFromStack(L, StackIndex));
		}
		else if (lua_istable(L, StackIndex))
		{
			ValueObj = NewObject<USUE4LuaValue>();
			ValueObj->ValueType = EValueType::Table;
			ValueObj->ValueReferencer.Reset(new FSUE4LuaValueReferencer(L, StackIndex));
		}
		else if (lua_isuserdata(L, StackIndex))
		{
			UObject* UObjectValue = SUE4LuaUObject::TryToRef(L, StackIndex);
			if (UObjectValue != nullptr)
			{
				ValueObj = NewObject<USUE4LuaValue>();
				ValueObj->ValueType = EValueType::UObject;
				ValueObj->UObjectValue = UObjectValue;
			}
			else
			{
				ValueObj = NewObject<USUE4LuaValue>();
				ValueObj->ValueType = EValueType::UserData;
				ValueObj->ValueReferencer.Reset(new FSUE4LuaValueReferencer(L, StackIndex));
			}
		}
		else if (!lua_isnil(L, StackIndex))
		{
			SUE4LVM_ERROR(L, TEXT("type '%s' is not supported."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, StackIndex))));
		}
	}

	return ValueObj;
}

void USUE4LuaValue::Push(lua_State* L) const
{
	if (IsBoolType())
	{
		FSUE4LuaStack::Push(L, bBoolValue);
	}
	else if (IsIntegerType())
	{
		FSUE4LuaStack::Push(L, IntValue);
	}
	else if (IsNumberType())
	{
		FSUE4LuaStack::Push(L, NumberValue);
	}
	else if (IsStringType())
	{
		FSUE4LuaStack::Push(L, StringValue);
	}
	else if (IsFunctionType())
	{
		FunctionValue->Push(L);
	}
	else if (IsTableType() || IsUserDataType())
	{
		ValueReferencer->Push(L);
	}
	else if (IsUObjectType())
	{
		FSUE4LuaStack::Push(L, UObjectValue);
	}
	else
	{
		SUE4LVM_ERROR(L, TEXT("Invalid value type(%d)"), static_cast<int32>(ValueType));
		lua_pushnil(L);
	}
}

bool USUE4LuaValue::Bind(lua_State* L, FScriptDelegate& InDelegate, UFunction* InSignatureFunction)
{
	if (InSignatureFunction == nullptr)
	{
		SUE4LVM_ERROR(L, TEXT("Invalid function signature."));
		return false;
	}

	if (!IsFunctionType())
	{
		SUE4LVM_ERROR(L, TEXT("Binding target must be a function value."));
		return false;
	}

	if (SignatureFunction != nullptr && SignatureFunction != InSignatureFunction)
	{
		SUE4LVM_WARNING(L, TEXT("Function signatures are different. New signature will be used."));
	}

	// #todo: 함수 호환성 검사 필요. 시그니처가 일치하지 않으면 호출할 때 크래시된다.

	SignatureFunction = InSignatureFunction;
	InDelegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(USUE4LuaValue, DummyFunc));

	return true;
}

bool USUE4LuaValue::AddUnique(lua_State* L, FMulticastScriptDelegate& InMulticastDelegate, UFunction* InSignatureFunction)
{
	if (InSignatureFunction == nullptr)
	{
		SUE4LVM_ERROR(L, TEXT("Invalid function signature."));
		return false;
	}

	if (!IsFunctionType())
	{
		SUE4LVM_ERROR(L, TEXT("Binding target must be a function value."));
		return false;
	}

	if (SignatureFunction != nullptr && SignatureFunction != InSignatureFunction)
	{
		SUE4LVM_WARNING(L, TEXT("Function signatures are different. New signature will be used."));
	}

	// #todo: 함수 호환성 검사 필요. 시그니처가 일치하지 않으면 호출할 때 크래시된다.

	SignatureFunction = InSignatureFunction;

	FScriptDelegate Delegate;
	Delegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(USUE4LuaValue, DummyFunc));

	InMulticastDelegate.AddUnique(Delegate);

	return true;
}

bool USUE4LuaValue::GetTableBool(const TCHAR* FieldName) const
{
	bool bRet = false;

	if (IsValidTable() && FieldName != nullptr)
	{
		if (auto VM = ValueReferencer->PinVM())
		{
			bRet = SUE4LuaValueImpl::GetTableField<bool>(VM->GetCurrentLuaState(), ValueReferencer.Get(), FieldName);
		}
	}

	return bRet;
}

int64 USUE4LuaValue::GetTableInteger(const TCHAR* FieldName) const
{
	int64 Ret = 0;

	if (IsValidTable() && FieldName != nullptr)
	{
		if (auto VM = ValueReferencer->PinVM())
		{
			Ret = SUE4LuaValueImpl::GetTableField<decltype(Ret)>(VM->GetCurrentLuaState(), ValueReferencer.Get(), FieldName);
		}
	}

	return Ret;
}

double USUE4LuaValue::GetTableNumber(const TCHAR* FieldName) const
{
	double Ret = 0.0;

	if (IsValidTable() && FieldName != nullptr)
	{
		if (auto VM = ValueReferencer->PinVM())
		{
			Ret = SUE4LuaValueImpl::GetTableField<decltype(Ret)>(VM->GetCurrentLuaState(), ValueReferencer.Get(), FieldName);
		}
	}

	return Ret;
}

FString USUE4LuaValue::GetTableString(const TCHAR* FieldName) const
{
	FString Ret;

	if (IsValidTable() && FieldName != nullptr)
	{
		if (auto VM = ValueReferencer->PinVM())
		{
			Ret = SUE4LuaValueImpl::GetTableField<decltype(Ret)>(VM->GetCurrentLuaState(), ValueReferencer.Get(), FieldName);
		}
	}

	return Ret;
}

TSharedPtr<FSUE4LuaFunction> USUE4LuaValue::GetTableFunction(const TCHAR* FieldName) const
{
	TSharedPtr<FSUE4LuaFunction> Ret;

	if (IsValidTable() && FieldName != nullptr)
	{
		if (auto VM = ValueReferencer->PinVM())
		{
			Ret = SUE4LuaValueImpl::GetTableField<decltype(Ret)>(VM->GetCurrentLuaState(), ValueReferencer.Get(), FieldName);
		}
	}

	return Ret;
}

UObject* USUE4LuaValue::GetTableUObject(const TCHAR* FieldName) const
{
	UObject* Ret = nullptr;

	if (IsValidTable() && FieldName != nullptr)
	{
		if (auto VM = ValueReferencer->PinVM())
		{
			Ret = SUE4LuaValueImpl::GetTableField<decltype(Ret)>(VM->GetCurrentLuaState(), ValueReferencer.Get(), FieldName);
		}
	}

	return Ret;
}

void USUE4LuaValue::BeginDestroy()
{
	FunctionValue.Reset();
	ValueReferencer.Reset();
	UObjectValue = nullptr;

	Super::BeginDestroy();
}

// 딜리게이트에 바인딩된 lua 함수를 호출합니다.
void USUE4LuaValue::ProcessEvent(UFunction* FunctionNotUsed, void* Parms)
{
	if (!FunctionValue.IsValid())
	{
		return;
	}

	if (SignatureFunction == nullptr || SignatureFunction->IsPendingKill())
	{
		return;
	}

	auto VM = FunctionValue->PinVM();
	if (!VM.IsValid())
	{
		return;
	}

	auto L = VM->GetCurrentLuaState();
	FSUE4LuaStackGuard StackGuard(L);

	// lua에 전달할 인자와 반환값을 저장할 파라메터 블럭을 준비합니다.
	TRefCountPtr<FSUE4LuaUStructValue> FuncParams(new FSUE4LuaUStructValue(SignatureFunction));

	// 파라메터 테이블을 생성. 함수 호출 후에도 사용해야 하므로 스택에 먼저 푸시합니다.
#if WITH_EDITOR
	{
		// 에디터 빌드에서 대소문자 오류를 잡아내기 위해 일반 테이블을 사용

		lua_newtable(L);
		// Stack: ParamTable
	}
#else
	{
		lua_getglobal(L, "CaseInsensitiveTable");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_call(L, 0, 1);
		// Stack: ParamTable 
	}
#endif // WITH_EDITOR

	// 호출할 lua 함수를 푸시
	{
		FunctionValue->Push(L);
		// Stack: ParamTable, LuaFunction
	}

	// 파라메터 테이블을 채웁니다.
	{
		lua_pushvalue(L, -2);
		// Stack: ParamTable, LuaFunction, ParamTable

		for (TFieldIterator<UProperty> PropIt(SignatureFunction); PropIt; ++PropIt)
		{
			const UProperty* Property = *PropIt;

			if (Property->HasAnyPropertyFlags(CPF_ReturnParm))
			{
				continue;
			}

			FSUE4LuaStack::Push(L, Property->GetFName());

			Property->CopyCompleteValue_InContainer(FuncParams->GetValueAddress(), Parms);
			SUE4LuaUProperty::Push(L, Property, Property->ContainerPtrToValuePtr<void>(FuncParams->GetValueAddress()), FuncParams);

			lua_settable(L, -3);
		}
	}

	// lua 함수 호출
	if (!SUE4LuaCall::PCall(L, 1, 0))
	{
		// Stack: ParamTable
		lua_pop(L, 1);
		// Stack: (empty)

		return;
	}

	// CPF_OutParm 파라메터들을 Parms로 복사
	for (TFieldIterator<UProperty> PropIt(SignatureFunction); PropIt; ++PropIt)
	{
		const UProperty* Property = *PropIt;

		if (!Property->HasAnyPropertyFlags(CPF_OutParm) || Property->HasAnyPropertyFlags(CPF_ConstParm))
		{
			continue;
		}

		FSUE4LuaStack::Push(L, Property->GetFName());
		lua_gettable(L, -2);
		SUE4LuaUProperty::CopyToNative(L, -1, Property, Property->ContainerPtrToValuePtr<void>(Parms));
		lua_pop(L, 1);
	}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	// 파라메터 테이블에 사용되지 않는 값이 있나 조사합니다.
	{
		lua_pushnil(L);
		// Stack: ParamTable, nil

		while (lua_next(L, -2))
		{
			// Stack: ParamTable, key, value
			lua_pushvalue(L, -2);
			// Stack: ParamTable, key, value, key

			if (lua_isstring(L, -1))
			{
				FString KeyStr = FSUE4LuaStack::To<FString>(L, -1);
				FName KeyName(*KeyStr);

				bool bValidParameter = false;

				if (const UProperty* Property = SignatureFunction->FindPropertyByName(KeyName))
				{
					if (Property->HasAnyPropertyFlags(CPF_Parm))
					{
						bValidParameter = true;
					}
				}

				if (!bValidParameter)
				{
					SUE4LVM_WARNING(L, TEXT("Found unused parameter '%s' while calling function '%s'."), *KeyStr, *SignatureFunction->GetName());
				}
			}

			// Stack: ParamTable, key, value, key
			lua_pop(L, 2);
			// Stack: ParamTable, key
		}
	}
#endif // !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

	// Stack: ParamTable
	lua_pop(L, 1);
	// Stack: (empty)
}