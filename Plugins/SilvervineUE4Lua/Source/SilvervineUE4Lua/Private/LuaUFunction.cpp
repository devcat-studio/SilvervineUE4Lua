// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaUFunction.h"

#include "LuaAPI.h"
#include "LuaBindingRegistry.h"
#include "LuaLog.h"
#include "LuaNativeValue.h"
#include "LuaUObject.h"
#include "LuaStack.h"
#include "LuaUProperty.h"
#include "LuaUserData.h"
#include "LuaVirtualMachine.h"

namespace SUE4LuaUFunction
{
	//현재 활성화된 lua_State를 추적하기 위한 도구
	struct FLuaStateTracker : public FNoncopyable
	{
		lua_State* OldL = nullptr;
		TSharedPtr<FSUE4LuaVirtualMachine> VM;

		FLuaStateTracker(lua_State* L)
		{
			if (VM = FSUE4LuaVirtualMachine::PinVM(L))
			{
				OldL = VM->GetCurrentLuaState();
				VM->SetCurrentLuaState(L);
			}
		}

		~FLuaStateTracker()
		{
			if (VM)
			{
				VM->SetCurrentLuaState(OldL);
			}
		}		
	};

	static bool IsParameter(const UProperty* Param)
	{
		return Param->HasAnyPropertyFlags(CPF_Parm);
	}

	static bool IsReturnParameter(const UProperty* Param)
	{
		return Param->HasAnyPropertyFlags(CPF_ReturnParm);
	}

	static bool IsOutParameter(const UProperty* Param)
	{
		const bool bReturn = IsReturnParameter(Param);
		const bool bOut = Param->HasAnyPropertyFlags(CPF_OutParm);
		const bool bConst = Param->HasAnyPropertyFlags(CPF_ConstParm);

		return bOut && !(bReturn || bConst);
	}

	static bool IsBindingClassFunction(const UFunction* Function)
	{
		return Function->GetOwnerClass()->IsChildOf(USUE4LuaBinding::StaticClass());
	}

	static FString GetClassName(const UFunction* Function)
	{
		UClass* Class = Function->GetOwnerClass();

		if (IsBindingClassFunction(Function))
		{
			auto ClassDefaultObj = Class->GetDefaultObject<USUE4LuaBinding>();

			return ClassDefaultObj->GetBindingClass()->GetName();
		}
		else
		{
			return Class->GetName();
		}
	}

	static FString GetFunctionName(const UFunction* Function)
	{
		FString FunctionName = Function->GetName();

		if (IsBindingClassFunction(Function))
		{
			if (FunctionName.StartsWith(TEXT("Lua")))
			{
				FunctionName = FunctionName.StartsWith(TEXT("LuaStatic")) ? FunctionName.Mid(9) : FunctionName.Mid(3);
			}
		}

		return FunctionName;
	}

	static FString GetFunctionFullName(const UFunction* Function)
	{
		FString ClassName = GetClassName(Function);
		FString FunctionName = GetFunctionName(Function);

		return FString::Printf(TEXT("U%s::%s"), *ClassName, *FunctionName);
	}

	static void CopyDefaultValue(lua_State* L, const UFunction* Function, const UProperty* Property, void* ContainerPtr)
	{
		check(Function != nullptr);
		check(Property != nullptr);
		check(ContainerPtr != nullptr);

		if (IsOutParameter(Property))
		{
			return;
		}

		// UObject* 타입의 기본값은 항상 nullptr
		if (Property->IsA<UObjectProperty>())
		{
			Property->ClearValue_InContainer(ContainerPtr);
			return;
		}

		lua_getglobal(L, "DefaultParameters");
		lua_getfield(L, -1, TCHAR_TO_UTF8(*GetClassName(Function)));
		// Stack: DefaultParameters, Class

		bool bFoundDefaultValue = false;

		if (!lua_isnil(L, -1))
		{
			lua_getfield(L, -1, TCHAR_TO_UTF8(*GetFunctionName(Function)));
			// Stack: DefaultParameters, Class, Function

			if (!lua_isnil(L, -1))
			{
				lua_getfield(L, -1, TCHAR_TO_UTF8(*Property->GetName()));
				// Stack: DefaultParameters, Class, Function, DefaultValue

				if (!lua_isnil(L, -1))
				{
					bFoundDefaultValue = SUE4LuaUProperty::CopyToNative(L, -1, Property, Property->ContainerPtrToValuePtr<void>(ContainerPtr));
				}

				lua_pop(L, 1);
				// Stack: DefaultParameters, Class, Function
			}

			lua_pop(L, 1);
			// Stack: DefaultParameters, Class
		}

		lua_pop(L, 2);
		// Stack: (empty)

		// FString, FVector, FVector2D, FRotator와 같은 일부 타입은 이미 기본값으로 초기화되어 있습니다.
		if (!bFoundDefaultValue && (Property->HasAnyPropertyFlags(CPF_ZeroConstructor | CPF_IsPlainOldData)))
		{
			Property->ClearValue_InContainer(ContainerPtr);
			bFoundDefaultValue = true;
		}

		if (!bFoundDefaultValue)
		{
			SUE4LVM_WARNING(L, TEXT("Found missing parameter '%s' while calling function '%s'."), *Property->GetName(), *GetFunctionFullName(Function));
		}
	}

	static int32 LuaCallFunction(lua_State* L)
	{
		UFunction* Function = FSUE4LuaStack::To<UFunction*>(L, lua_upvalueindex(1));
		check(Function != nullptr);

		return CallFunction(L, Function);
	}

	bool IsParameterTable(lua_State* L, int32 StackIndex)
	{
		bool bParameterTable = false;

		if (lua_gettop(L) == StackIndex && lua_istable(L, StackIndex))
		{
			bParameterTable = true;

			// 테이블이 UE4.Vector 등의 기본타입 객체면 pass-by-names 테이블이 아닙니다.
			{
				lua_getfield(L, StackIndex, "isPrimitiveType");
				// Stack: isPrimitiveType
				if (lua_isboolean(L, -1) && lua_toboolean(L, -1))
				{
					bParameterTable = false;
				}
				lua_pop(L, 1);
				// Stack: (empty)
			}
		}

		return bParameterTable;
	}

	void PushClosure(lua_State* L, const UFunction* Function)
	{
		check(Function != nullptr);

		// #todo: 매번 클로저를 생성하면 너무 비효율적이다. 메타테이블에 캐싱하자.

		FSUE4LuaStack::Push(L, Function);
		lua_pushcclosure(L, &LuaCallFunction, 1);
	}

	int32 CallFunction(lua_State* L, const UFunction* Function)
	{
		check(Function != nullptr);

		UClass* Class = Function->GetOwnerClass();
		check(Class != nullptr);

		// 바인딩 멤버 함수는 첫번째 인자가 반드시 UObject이어야 합니다.
		if (IsBindingClassFunction(Function) && !Function->GetName().StartsWith(TEXT("LuaStatic")))
		{
			if (SUE4LuaUObject::ToRef(L, 1) == nullptr)
			{
				SUE4LVM_ERROR(L, TEXT("Invalid 'InSelf' while calling function '%s'."), *GetFunctionFullName(Function));
				return 0;
			}
		}

		const bool bStaticFunction = Function->HasAnyFunctionFlags(EFunctionFlags::FUNC_Static);
		const int32 StackTopIndex = lua_gettop(L);

		// 스태틱 함수가 아니면 첫번째 인자가 'self'
		UObject* TargetUObject = nullptr;
		int32 ArgStackBaseIndex = 1;
		if (bStaticFunction)
		{
			TargetUObject = Class;
		}
		else
		{
			TargetUObject = SUE4LuaUObject::ToRef(L, 1);	// self
			if (TargetUObject == nullptr)
			{
				return 0;
			}

			++ArgStackBaseIndex;
		}

		// 파라메터 전달 방식을 확인합니다. false면 pass-by-position.
		const bool bPassByNames = IsParameterTable(L, ArgStackBaseIndex);

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		// 파라메터 테이블에 사용되지 않는 값이 있나 조사합니다.
		if (bPassByNames)
		{
			lua_pushnil(L);
			while (lua_next(L, ArgStackBaseIndex))
			{
				// Stack: key, value
				lua_pushvalue(L, -2);
				// Stack: key, value, key

				if (lua_isstring(L, -1))
				{
					FString KeyStr = FSUE4LuaStack::To<FString>(L, -1);
					FName KeyName(*KeyStr);

					bool bValidParameter = false;

					if (const UProperty* Property = Function->FindPropertyByName(KeyName))
					{
						check(!IsReturnParameter(Property));

						if (IsParameter(Property))
						{
							bValidParameter = true;
						}
					}

					if (!bValidParameter)
					{
						SUE4LVM_WARNING(L, TEXT("Found unused parameter name '%s' while calling function '%s'."), *KeyStr, *GetFunctionFullName(Function));
					}
				}

				// Stack: key, value, key
				lua_pop(L, 2);
				// Stack: key
			}
		}
#endif // !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

		// lua 상태 추적
		FLuaStateTracker LuaStateTracker(L);

		// 스태틱 바인딩 함수가 있으면 빠르게 함수 호출을 처리할 수 있습니다.
		if (auto StaticBindingHandler = FSUE4LuaBindingRegistry::Get().FindFunctionStaticBindingHandler(Function))
		{
			return StaticBindingHandler(L, TargetUObject);
		}

		// UFunction에 전달할 인자와 반환값을 저장할 파라메터 블럭을 준비합니다.
		TRefCountPtr<FSUE4LuaUStructValue> FuncParams(new FSUE4LuaUStructValue(Function));

		// lua 스택에서 파라메터를 가져옵니다.
		if (bPassByNames)
		{
			for (TFieldIterator<UProperty> PropIt(Function); PropIt; ++PropIt)
			{
				const UProperty* Property = *PropIt;

				if (!IsParameter(Property) || IsReturnParameter(Property))
				{
					continue;
				}

				FSUE4LuaStack::Push(L, Property->GetName());
				lua_gettable(L, ArgStackBaseIndex);
				// Stack: value

				if (!lua_isnil(L, -1))
				{
					SUE4LuaUProperty::CopyToNative(L, -1, Property, Property->ContainerPtrToValuePtr<void>(FuncParams->GetValueAddress()));
				}
				else
				{
					CopyDefaultValue(L, Function, Property, FuncParams->GetValueAddress());
				}

				lua_pop(L, 1);
				// Stack: (empty)
			}	
		}
		else
		{
			int32 ParamStackIndex = ArgStackBaseIndex;
			for (TFieldIterator<UProperty> PropIt(Function); PropIt; ++PropIt)
			{
				const UProperty* Property = *PropIt;

				if (!IsParameter(Property) || IsReturnParameter(Property))
				{
					continue;
				}

				if (ParamStackIndex <= StackTopIndex && !lua_isnil(L, ParamStackIndex))
				{
					SUE4LuaUProperty::CopyToNative(L, ParamStackIndex, Property, Property->ContainerPtrToValuePtr<void>(FuncParams->GetValueAddress()));
				}
				else
				{
					CopyDefaultValue(L, Function, Property, FuncParams->GetValueAddress());
				}

				++ParamStackIndex;
			}
		}

		// 함수 호출
		TargetUObject->ProcessEvent(const_cast<UFunction*>(Function), FuncParams->GetValueAddress());

		// 출력 파라메터들을 lua로 복사합니다.
		if (bPassByNames)
		{
			for (TFieldIterator<UProperty> PropIt(Function); PropIt; ++PropIt)
			{
				const UProperty* Property = *PropIt;

				if (!IsParameter(Property) || IsReturnParameter(Property))
				{
					continue;
				}

				if (IsOutParameter(Property))
				{
					const void* SrcValueAddress = Property->ContainerPtrToValuePtr<void>(FuncParams->GetValueAddress());

					// Stack: NamedArgTable
					FSUE4LuaStack::Push(L, Property->GetName());
					lua_gettable(L, -2);
					// Stack: NamedArgTable, InValue

					if (!SUE4LuaUProperty::TryCopyToLua(L, -1, Property, SrcValueAddress))
					{
						FSUE4LuaStack::Push(L, Property->GetName());
						SUE4LuaUProperty::Push(L, Property, SrcValueAddress, FuncParams);
						// Stack: NamedArgTable, InValue, KeyName, OutValue
						lua_settable(L, -4);
						// Stack: NamedArgTable, InValue
					}

					lua_pop(L, 1);
					// Stack: NamedArgTable
				}
			}
		}
		else
		{
			int32 ParamStackIndex = ArgStackBaseIndex;
			for (TFieldIterator<UProperty> PropIt(Function); PropIt && ParamStackIndex <= StackTopIndex; ++PropIt)
			{
				const UProperty* Property = *PropIt;

				if (!IsParameter(Property) || IsReturnParameter(Property))
				{
					continue;
				}

				if (IsOutParameter(Property))
				{
					const void* SrcValueAddress = Property->ContainerPtrToValuePtr<void>(FuncParams->GetValueAddress());

					SUE4LuaUProperty::TryCopyToLua(L, ParamStackIndex, Property, SrcValueAddress);
				}

				++ParamStackIndex;
			}
		}

		int32 ReturnValueCount = 0;
		{
			// 결과값 반환
			if (const UProperty* ReturnProperty = Function->GetReturnProperty())
			{
				SUE4LuaUProperty::Push(L, ReturnProperty, ReturnProperty->ContainerPtrToValuePtr<void>(FuncParams->GetValueAddress()), FuncParams);
				++ReturnValueCount;
			}

			// 출력값을 차례대로 반환합니다.
			// 입력 파라메터를 그대로 푸시하는 것이 좋겠으나 프록시는 달라도 참조하는 객체는 같기 때문에 이렇게 해도 문제되지 않습니다.
			for (TFieldIterator<UProperty> PropIt(Function); PropIt; ++PropIt)
			{
				const UProperty* Property = *PropIt;

				if (!IsParameter(Property) || IsReturnParameter(Property))
				{
					continue;
				}

				if (IsOutParameter(Property))
				{
					const void* SrcValueAddress = Property->ContainerPtrToValuePtr<void>(FuncParams->GetValueAddress());

					SUE4LuaUProperty::Push(L, Property, SrcValueAddress, FuncParams);
					++ReturnValueCount;
				}
			}
		}

		return ReturnValueCount;
	}
}