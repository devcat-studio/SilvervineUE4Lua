// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "LuaStack.h"
#include "LuaThread.h"
#include "LuaValueReferencer.h"


//
// lua 함수를 c++에서 호출하거나 인자로 전달할 때 사용하는 클래스
//
class SILVERVINEUE4LUA_API FSUE4LuaFunction : public TSharedFromThis<FSUE4LuaFunction>, public FNoncopyable
{
public:
	virtual ~FSUE4LuaFunction();

	// 같은 lua 함수에 대한 객체면 true
	bool IsSame(TSharedPtr<FSUE4LuaFunction> Function) const;

	// 함수를 호출
	template<typename RetType, typename ...ArgTypes>
	RetType Call(const ArgTypes&... Args);

	// 전역 lua 함수를 호출
	// 자주 호출하는 lua 함수는 CreateFromGlobal()로 얻어두는 것이 효율적입니다.
	template<typename RetType, typename ...ArgTypes>
	static RetType CallGlobal(TSharedRef<FSUE4LuaVirtualMachine> LuaVM, const TCHAR* FunctionName, const ArgTypes&... Args);

	// lua 전역 함수로부터 생성
	static TSharedPtr<FSUE4LuaFunction> CreateFromGlobal(TSharedRef<FSUE4LuaVirtualMachine> LuaVM, const TCHAR* FuctionName);	

public:
	//
	// 아래는 내부 구현에만 사용됩니다.
	//

	// lua 스택에서 생성
	static TSharedPtr<FSUE4LuaFunction> CreateFromStack(lua_State* L, int32 Index);

	TSharedPtr<FSUE4LuaVirtualMachine> PinVM() const;
	void Push(lua_State* L);

private:
	FSUE4LuaFunction(TSharedRef<FSUE4LuaThread> InLuaMainThread);

	bool CallInternal(lua_State* L, int32 ArgCount, int32 RetCount);
	template<typename RetType>
	RetType PostCall(lua_State* L, bool bCallSucceeded);
	template<typename RetType>
	static RetType GetDefaultValue();

private:
	TSharedRef<FSUE4LuaThread> LuaMainThread;
	TUniquePtr<class FSUE4LuaValueReferencer> FunctionReferencer;
};

template<typename RetType>
inline RetType FSUE4LuaFunction::PostCall(lua_State* L, bool bCallSucceeded)
{
	if (bCallSucceeded)
	{
		return FSUE4LuaStack::Pop<RetType>(L);
	}
	else
	{
		return GetDefaultValue<RetType>();
	}
}

template<>
inline void FSUE4LuaFunction::PostCall(lua_State* L, bool bCallSucceeded)
{
}

template<typename RetType>
inline RetType FSUE4LuaFunction::GetDefaultValue()
{
	return {};
}

template<>
inline void FSUE4LuaFunction::GetDefaultValue()
{
	return;
}

// FVector4는 기본생성자가 없다...
template<>
inline FVector4 FSUE4LuaFunction::GetDefaultValue()
{
	return FVector4(ForceInit);
}

template<typename RetType, typename ...ArgTypes>
RetType FSUE4LuaFunction::Call(const ArgTypes&... Args)
{
	if (auto VM = LuaMainThread->PinVM())
	{
		auto L = VM->GetCurrentLuaState();
		FSUE4LuaStackGuard StackGuard(L);

		int32 ArgCount = sizeof...(ArgTypes);
		int32 RetCount = TIsVoidType<RetType>::Value ? 0 : 1;

		Push(L);
		FSUE4LuaStack::Push(L, Args...);
		return PostCall<RetType>(L, CallInternal(L, ArgCount, RetCount));
	}

	return GetDefaultValue<RetType>();
}

template<typename RetType, typename ...ArgTypes>
RetType FSUE4LuaFunction::CallGlobal(TSharedRef<FSUE4LuaVirtualMachine> LuaVM, const TCHAR* FunctionName, const ArgTypes&... Args)
{
	auto LuaFunction = CreateFromGlobal(LuaVM, FunctionName);
	if (!LuaFunction.IsValid())
	{
		return GetDefaultValue<RetType>();
	}

	return LuaFunction->Call<RetType>(Args...);
}
