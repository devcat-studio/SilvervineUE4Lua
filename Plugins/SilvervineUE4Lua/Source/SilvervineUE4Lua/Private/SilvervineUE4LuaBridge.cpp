// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "SilvervineUE4LuaBridge.h"

#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/World.h"
#include "Misc/CoreDelegates.h"

#include "LuaAPI.h"
#include "LuaCall.h"
#include "LuaFunction.h"
#include "LuaLog.h"
#include "LuaNativeValue.h"
#include "LuaUProperty.h"


namespace USUE4LuaBridgeImpl
{
	static bool IsInputValueParam(const UProperty* Property)
	{
		return Property->HasAnyPropertyFlags(CPF_Parm) && !Property->HasAnyPropertyFlags(CPF_OutParm);
	}

	static bool IsInputRefParam(const UProperty* Property)
	{
		return Property->HasAllPropertyFlags(CPF_OutParm | CPF_ReferenceParm);
	}

	static void* FindOutParamValueAddress(const FFrame& Stack, const UProperty* Property)
	{
		void* ValueAddress = nullptr;
		{
			FOutParmRec* OutParms = Stack.OutParms;
			while (OutParms->Property != Property)
			{
				OutParms = OutParms->NextOutParm;
			}
			ValueAddress = OutParms->PropAddr;
		}

		return ValueAddress;
	}
}

void USUE4LuaBridge::NativeInitialize(USUE4LuaVirtualMachine* InVM)
{
	if (InVM == nullptr)
	{
		UE_LOG(LogSUE4L, Error, TEXT("USUE4LuaVirtualMachine is null. [%s]"), TEXT(__FUNCTION__));
		return;
	}

	RefVM = InVM;
}

void USUE4LuaBridge::SetUnitTestVMInstance(TSharedPtr<FSUE4LuaVirtualMachine> InVM)
{
	UnitTestVM = InVM;
}

void USUE4LuaBridge::PostInitProperties()
{
	Super::PostInitProperties();
}

DEFINE_FUNCTION(USUE4LuaBridge::execDispatch)
{
	P_GET_PROPERTY(UStrProperty, FunctionName);
	P_FINISH;
	P_NATIVE_BEGIN;
	USUE4LuaBridge* LuaBridge = Cast<USUE4LuaBridge>(Context);
	check(LuaBridge != nullptr);

	LuaBridge->DispatchInternal(Stack, FunctionName);
	P_NATIVE_END;
}

FSUE4LuaVirtualMachine* USUE4LuaBridge::GetVM()
{
	if (UnitTestVM.IsValid())
	{
		return UnitTestVM.Get();
	}

	if (RefVM == nullptr)
	{
		UE_LOG(LogSUE4L, Error, TEXT("RefVM is null. [%s]"), TEXT(__FUNCTION__));
		return nullptr;
	}

	if (RefVM->IsPendingKill())
	{
		return nullptr;
	}

	return RefVM->FindVMFromWorldContext(this);
}

void USUE4LuaBridge::DispatchInternal(FFrame& InStack, const FString& InFunctionName)
{
	using namespace USUE4LuaBridgeImpl;

	auto VM = GetVM();
	if (VM == nullptr)
	{
		// 종료시 이렇게 될 수 있어서 경고 없이 조용히 넘어가도록 함
		// UE_LOG(LogSUE4L, Error, TEXT("VM is invalid. [%s]"), TEXT(__FUNCTION__));
		return;
	}

	FFrame* Stack = &InStack;
	UFunction* SignatureFunction = Stack->Node;
	if (SignatureFunction->GetName().StartsWith(TEXT("ExecuteUbergraph")))
	{
		// 이벤트 함수 같은 경우가 이 경우에 해당
		Stack = InStack.PreviousFrame;
		SignatureFunction = Stack->Node;
	}

	auto L = VM->GetCurrentLuaState();
	FSUE4LuaStackGuard StackGuard(L);

	// lua에 전달할 인자와 반환값을 저장할 파라메터 블럭을 준비합니다.
	TRefCountPtr<FSUE4LuaUStructValue> FuncParams(new FSUE4LuaUStructValue(SignatureFunction));

	// 파라메터 테이블을 생성. 함수 호출 후에도 사용해야 하므로 스택에 먼저 푸시합니다.
	{
		lua_newtable(L);
		// Stack: ParamTable
	}

	// 호출할 함수를 찾아서 푸시합니다.
	if (!VM->PushDispatchHandler(L, Stack->Object, InFunctionName.IsEmpty() ? SignatureFunction->GetName() : InFunctionName))
	{
		if (InFunctionName.IsEmpty())
		{
			SUE4LVM_ERROR(L, TEXT("DispatchHandler is not found: %s.%s"), *Stack->Object->GetClass()->GetName(), *SignatureFunction->GetName());
		}
		else
		{
			SUE4LVM_ERROR(L, TEXT("DispatchHandler is not found: %s.%s(%s)"), *Stack->Object->GetClass()->GetName(), *InFunctionName, *SignatureFunction->GetName());
		}

		// Stack: ParamTable
		lua_pop(L, 1);
		// Stack: (empty)

		return;
	}

	// 입력 파라메터 테이블을 채웁니다.
	{
		FSUE4LuaStack::Push(L, Stack->Object);
		lua_pushvalue(L, -3);
		// Stack: ParamTable, LuaFunction, Object, ParamTable

		for (TFieldIterator<UProperty> PropIt(SignatureFunction); PropIt; ++PropIt)
		{
			const UProperty* Property = *PropIt;
			void* SrcValueAddress = Property->ContainerPtrToValuePtr<void>(FuncParams->GetValueAddress());

			if (IsInputValueParam(Property))
			{
				FSUE4LuaStack::Push(L, Property->GetFName());
				Property->CopyCompleteValue(SrcValueAddress, Property->ContainerPtrToValuePtr<void>(Stack->Locals));
				SUE4LuaUProperty::Push(L, Property, SrcValueAddress, FuncParams);
				lua_settable(L, -3);
			}
			else if (IsInputRefParam(Property))
			{
				FSUE4LuaStack::Push(L, Property->GetFName());
				Property->CopyCompleteValue(SrcValueAddress, FindOutParamValueAddress(*Stack, Property));
				SUE4LuaUProperty::Push(L, Property, SrcValueAddress, FuncParams);
				lua_settable(L, -3);
			}
		}
	}

	// lua 함수 호출
	if (!SUE4LuaCall::PCall(L, 2, 0))
	{
		// Stack: ParamTable
		lua_pop(L, 1);
		// Stack: (empty)

		return;
	}

	// 출력  파라메터들을 Parms로 복사
	for (TFieldIterator<UProperty> PropIt(SignatureFunction); PropIt; ++PropIt)
	{
		const UProperty* Property = *PropIt;

		if (Property->HasAnyPropertyFlags(CPF_OutParm) && !IsInputRefParam(Property))
		{
			FSUE4LuaStack::Push(L, Property->GetFName());
			lua_gettable(L, -2);
			SUE4LuaUProperty::CopyToNative(L, -1, Property, FindOutParamValueAddress(*Stack, Property));
			lua_pop(L, 1);
		}
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

bool USUE4LuaBridge::NativeDispatchCall(lua_State* L)
{
	// Stack: ParamTable, LuaFunction, Object, ParamTable
	return SUE4LuaCall::PCall(L, 2, 0);
	// Stack: ParamTable
}

void USUE4LuaBridge::NativeDispatchError(lua_State* L, const FString& Message)
{
	SUE4LVM_ERROR(L, TEXT("%s"), *Message);
}

void USUE4LuaBridge::LuaNewTable(lua_State* L)
{
	lua_newtable(L);
}

void USUE4LuaBridge::LuaSetTable(lua_State* L)
{
	lua_settable(L, -3);
}

void USUE4LuaBridge::LuaGetTable(lua_State* L)
{
	lua_gettable(L, -2);
}

void USUE4LuaBridge::LuaPop(lua_State* L, int32 N)
{
	lua_pop(L, N);
}

void USUE4LuaBridge::LuaPushValue(lua_State* L, int32 Index)
{
	lua_pushvalue(L, Index);
}