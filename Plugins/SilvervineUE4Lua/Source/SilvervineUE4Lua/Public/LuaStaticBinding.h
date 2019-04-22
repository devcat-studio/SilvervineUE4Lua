// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "LuaStack.h"


//
// lua 함수 스태틱 바인딩에서 사용하는 유틸리티 클래스. 자동 생성된 코드에서만 사용합니다.
//
class SILVERVINEUE4LUA_API FSUE4LuaFunctionStaticBindingContext
{
public:
	FSUE4LuaFunctionStaticBindingContext(lua_State* InL, int32 InArgStackBaseIndex, const TCHAR* InFunctionName);

	// lua 스택에서 인자를 가져옵니다.
	template<typename ArgType>
	void Get(const char* ArgName, int32 ArgIndex, ArgType& OutArg, bool bNotInitialized)
	{
		if (bPassByName)
		{
			if (LuaGetField(ArgName))
			{
				OutArg = FSUE4LuaStack::Pop<ArgType>(L);
				return;
			}
			else
			{
				FSUE4LuaStack::Pop<FSUE4LuaStack::FNil>(L);
			}
		}
		else
		{
			if (ArgIndex < ArgCount)
			{
				OutArg = FSUE4LuaStack::To<ArgType>(L, ArgStackBaseIndex + ArgIndex);
				return;
			}
		}

		if (bNotInitialized)
		{
			HandleMissingParameter(ArgName);
			InitializeWithDefaultValue(OutArg);
		}
	}

	// pass-by-position일 때: 가능하다면 출력값으로 lua 스택의 인자를 수정
	// pass-by-name일 때: 파라메터 테이블의 필드값을 갱신
	template<typename ArgType>
	void ModifyIfAble(const char* ArgName, int32 ArgIndex, const ArgType& Arg)
	{
		if (bPassByName)
		{
			FSUE4LuaStack::Push(L, Arg);
			LuaSetField(ArgName);
		}
		else
		{
			if (ArgIndex < ArgCount)
			{
				// 값타입의 lua 스택밸류는 건드리지 않습니다.
			}
		}
	}

	// UStruct 타입에 대한 ModifyIfAble() 구현
	template<typename UStructType>
	void ModifyUStructIfAble(const char* ArgName, int32 ArgIndex, const UStructType& Arg)
	{
		if (bPassByName)
		{
			if (LuaGetField(ArgName))
			{
				UStructType* ArgRef = FSUE4LuaStack::GetRef<UStructType>(L, -1);
				if (ArgRef != nullptr)
				{
					*ArgRef = Arg;
				}
			}
			else
			{
				FSUE4LuaStack::Push(L, Arg);
				LuaSetField(ArgName);
			}

			FSUE4LuaStack::Pop<FSUE4LuaStack::FNil>(L);
		}
		else
		{
			if (ArgIndex < ArgCount)
			{
				UStructType* ArgRef = FSUE4LuaStack::GetRef<UStructType>(L, ArgStackBaseIndex + ArgIndex);
				if (ArgRef != nullptr)
				{
					*ArgRef = Arg;
				}
			}
		}
	}

	// lua 스택에 출력값을 푸시
	template<typename ArgType>
	void Push(const char* ArgName, int32 ArgIndex, const ArgType& Arg)
	{
		if (bPassByName)
		{
			LuaGetField(ArgName);
		}
		else
		{
			FSUE4LuaStack::Push(L, Arg);
		}
	}

	// UStruct 타입에 대한 Push 구현
	template<typename UStructType>
	void PushUStruct(const char* ArgName, int32 ArgIndex, const UStructType& Arg)
	{
		if (bPassByName)
		{
			LuaGetField(ArgName);
		}
		else
		{
			if (ArgIndex < ArgCount)
			{
				UStructType* ArgRef = FSUE4LuaStack::GetRef<UStructType>(L, ArgStackBaseIndex + ArgIndex);
				if (ArgRef != nullptr)
				{
					LuaPushValue(ArgStackBaseIndex + ArgIndex);
					return;
				}
			}

			FSUE4LuaStack::Push(L, Arg);
		}
	}

	template<typename ArgType>
	static void InitializeWithDefaultValue(ArgType& Value)
	{
		Value = {};
	}

private:
	void LuaPushValue(int32 Index);
	bool LuaGetField(const char* Name);
	void LuaSetField(const char* Name);

	void HandleMissingParameter(const char* ArgName);

private:
	lua_State* L;
	int32 ArgStackBaseIndex;
	const TCHAR* FunctionName;

	int32 ArgCount;
	bool bPassByName;
};

template<>
inline void FSUE4LuaFunctionStaticBindingContext::InitializeWithDefaultValue(FVector4& Value)
{
	Value = FVector(ForceInit);
}