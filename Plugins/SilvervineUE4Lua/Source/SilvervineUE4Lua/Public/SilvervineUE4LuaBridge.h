// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "LuaStack.h"
#include "LuaThread.h"
#include "SilvervineUE4LuaVirtualMachine.h"

#include "SilvervineUE4LuaBridge.generated.h"


struct FFrame;

// NativeDispatch()를 쉽게 사용하기 위한 매크로입니다.
//
// 사용 예:
//	void UMyClass::Foo(int32 P1, int32 P2)
//	{
//		SUE4LUA_DISPATCH(LuaBridge, P1, P2);
//	}
#define SUE4LUA_DISPATCH(LuaBridge, ...)										\
	if( LuaBridge)																\
	{																			\
		LuaBridge->NativeDispatch(this, ANSI_TO_TCHAR(__func__), __VA_ARGS__);	\
	}																			\

//
// Blueprint에서 USUE4LuaBridge를 간단하게 변수로 추가하기 위한 헬퍼
//
USTRUCT(BlueprintType)
struct SILVERVINEUE4LUA_API FSUE4LuaBridgeHelper
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Instanced, EditDefaultsOnly)
	class USUE4LuaBridge* Instance;
};

//
// 블루프린트나 C++ 함수 구현을 lua로 작성할 때 사용하는 클래스 입니다.
//
UCLASS(BlueprintType, editinlinenew)
class SILVERVINEUE4LUA_API USUE4LuaBridge : public UObject
{
	GENERATED_BODY()

public:
	// 호출자 블루프린트에 대한 lua 함수를 호출합니다.
	//
	// FunctionName을 전달하지 않을 경우 호출 함수 이름을 찾아서 사용하므로 대부분의 경우 FunctionName를 전달할 필요가 없습니다.
	// 위젯의 이벤트 핸들러와 같이 자동생성되는 복잡한 이름을 가진 함수를 디스패치할 때 이름을 지정하는 것을 권장합니다.
	//
	// 함수의 파라메터는 lua 테이블에 담겨서 전달됩니다. 필드의 이름은 파라메터의 이름과 같습니다.
	// 예: Foo(int32 P1, int32 P2) -> Foo({ P1 = ..., P2 = ..., })
	UFUNCTION(BlueprintCallable, CustomThunk)
	void Dispatch(const FString& FunctionName);

	// Dispatch()와 비슷하게 c++ 함수에 대한 lua 함수를 호출합니다.
	//
	// 함수의 파라메터는 lua 테이블에 배열 형태로 전달됩니다.
	// 예: Foo(int32 P1, int32 P2) -> Foo({ P1, P2 })
	template<typename ...ArgTypes>
	void NativeDispatch(const UObject* Object, const FString& FunctionName, ArgTypes&&... Args);

protected:
	// 사용할 VM
	UPROPERTY(EditDefaultsOnly)
	USUE4LuaVirtualMachine* RefVM;

public:
	void NativeInitialize(USUE4LuaVirtualMachine* InVM);

	//유닛 테스트용 VM 설정
	void SetUnitTestVMInstance(TSharedPtr<FSUE4LuaVirtualMachine> InVM);

	// BeginUObject Interface
	virtual void PostInitProperties() override;
	// End UObject Interface

	DECLARE_FUNCTION(execDispatch);

private:
	FSUE4LuaVirtualMachine* GetVM();

	void DispatchInternal(FFrame& InStack, const FString& InFunctionName);
	bool NativeDispatchCall(lua_State* L);
	void NativeDispatchError(lua_State* L, const FString& Message);

	void LuaNewTable(lua_State* L);
	void LuaSetTable(lua_State* L);
	void LuaGetTable(lua_State* L);
	void LuaPop(lua_State* L, int32 N);
	void LuaPushValue(lua_State* L, int32 Index);

	// 스텍 최상단에 있는 테이블의 값으로 Args를 순서대로 넣습니다.
	template<typename ...ArgTypes>
	void FillTableWithArgs(lua_State* L, const ArgTypes&... Args);

	template<typename ArgType, typename ...RestArgTypes>
	void FillTableWithArgsInternal(lua_State* L, int Index, const ArgType& Arg, const RestArgTypes&... RestArgs);
	static void FillTableWithArgsInternal(lua_State* L, int Index) {}

	// 스텍 최상단에 있는 테이블의 값을 Args에 순서대로 넣습니다.
	template<typename ...ArgTypes>
	void FillArgsWithTable(lua_State* L, ArgTypes&&... Args);

	// Arg가 const가 아닐 경우
	template<typename ArgType, typename ...RestArgTypes>
	void FillArgsWithTableInternal(lua_State* L, int Index, ArgType& Arg, RestArgTypes&&... RestArgs);

	// Arg가 const일 경우
	template<typename ArgType, typename ...RestArgTypes>
	void FillArgsWithTableInternal(lua_State* L, int Index, const ArgType& Arg, RestArgTypes&&... RestArgs);
	static void FillArgsWithTableInternal(lua_State* L, int Index) {}

private:
	// 유닛테스트용 VM 캐시
	TSharedPtr<FSUE4LuaVirtualMachine> UnitTestVM;
};

template<typename ...ArgTypes>
void USUE4LuaBridge::NativeDispatch(const UObject* Object, const FString& FunctionName, ArgTypes&&... Args)
{
	if (Object == nullptr)
	{
		return;
	}

	auto VM = GetVM();
	if (VM != nullptr)
	{
		FString LuaFunctionName = FunctionName;
		if (!LuaFunctionName.StartsWith(TEXT("Native")))
		{
			LuaFunctionName.InsertAt(0, TEXT("Native"));
		}

		auto L = VM->GetCurrentLuaState();
		FSUE4LuaStackGuard StackGuard(L);

		// Stack: (empty)
		LuaNewTable(L);
		// Stack: ParamTable
		
		if (VM->PushDispatchHandler(L, Object, LuaFunctionName))
		{
			// Stack: ParamTable, LuaFunction
			FSUE4LuaStack::Push(L, Object);
			LuaPushValue(L, -3);
			FillTableWithArgs(L, Args...);
			// Stack: ParamTable, LuaFunction, Object, ParamTable

			if (NativeDispatchCall(L))
			{
				// lua에서 출력한 값들을 가져옵니다.
				FillArgsWithTable(L, Forward<ArgTypes>(Args)...);
			}
		}
		else
		{
			NativeDispatchError(L, FString::Printf(TEXT("NativeDispatchHandler is not found: %s.%s"), *Object->GetClass()->GetName(), *LuaFunctionName));
		}

		// Stack: ParamTable
		LuaPop(L, 1);
		// Stack: (empty)
	}
}

template<typename ...ArgTypes>
void USUE4LuaBridge::FillTableWithArgs(lua_State* L, const ArgTypes&... Args)
{
	FillTableWithArgsInternal(L, 1, Args...);
}

template<typename ArgType, typename ...RestArgTypes>
void USUE4LuaBridge::FillTableWithArgsInternal(lua_State* L, int Index, const ArgType& Arg, const RestArgTypes&... RestArgs)
{
	FSUE4LuaStack::Push(L, Index);
	FSUE4LuaStack::Push(L, Arg);
	LuaSetTable(L);
	FillTableWithArgsInternal(L, Index + 1, RestArgs...);
}

template<typename ...ArgTypes>
void USUE4LuaBridge::FillArgsWithTable(lua_State* L, ArgTypes&&... Args)
{
	FillArgsWithTableInternal(L, 1, Forward<ArgTypes>(Args)...);
}

template<typename ArgType, typename ...RestArgTypes>
void USUE4LuaBridge::FillArgsWithTableInternal(lua_State* L, int Index, ArgType& Arg, RestArgTypes&&... RestArgs)
{
	FSUE4LuaStack::Push(L, Index);
	LuaGetTable(L);
	Arg = FSUE4LuaStack::To<ArgType>(L, -1);
	LuaPop(L, 1);
	FillArgsWithTableInternal(L, Index + 1, Forward<RestArgTypes>(RestArgs)...);
}

template<typename ArgType, typename ...RestArgTypes>
void USUE4LuaBridge::FillArgsWithTableInternal(lua_State* L, int Index, const ArgType& Arg, RestArgTypes&&... RestArgs)
{
	FillArgsWithTableInternal(L, Index + 1, Forward<RestArgTypes>(RestArgs)...);
}