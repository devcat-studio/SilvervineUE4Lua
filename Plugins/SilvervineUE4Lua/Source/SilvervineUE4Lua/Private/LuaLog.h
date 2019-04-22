// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.


#pragma once

#include "CoreMinimal.h"


struct lua_State;

DECLARE_LOG_CATEGORY_EXTERN(LogSUE4L, Log, All);

// Log, Warning, Error 로그 출력 매크로
#define SUE4LVM_LOG(L, Format, ...)																																\
	{																																							\
		UE_LOG(LogSUE4L, Log, TEXT("[LuaVM] [%s] ") Format, SUE4LuaLog::GetCurrentFileLocation(L, TEXT(__FUNCTION__)), ##__VA_ARGS__);							\
		SUE4LuaLog::DebugPrint(L, *FString::Printf(TEXT("[Log] [%s] ") Format, SUE4LuaLog::GetCurrentFileLocation(L, TEXT(__FUNCTION__)), ##__VA_ARGS__));		\
	}
#define SUE4LVM_WARNING(L, Format, ...)																															\
	{																																							\
		UE_LOG(LogSUE4L, Warning, TEXT("[LuaVM] [%s] ") Format, SUE4LuaLog::GetCurrentFileLocation(L, TEXT(__FUNCTION__)), ##__VA_ARGS__);						\
		SUE4LuaLog::DebugPrint(L, *FString::Printf(TEXT("[Warning] [%s] ") Format, SUE4LuaLog::GetCurrentFileLocation(L, TEXT(__FUNCTION__)), ##__VA_ARGS__));	\
	}
#define SUE4LVM_ERROR(L, Format, ...)																															\
	{																																							\
		UE_LOG(LogSUE4L, Error, TEXT("[LuaVM] [%s] ") Format, SUE4LuaLog::GetCurrentFileLocation(L, TEXT(__FUNCTION__)), ##__VA_ARGS__);						\
		SUE4LuaLog::DebugPrint(L, *FString::Printf(TEXT("[Error] [%s] ") Format, SUE4LuaLog::GetCurrentFileLocation(L, TEXT(__FUNCTION__)), ##__VA_ARGS__));	\
	}

namespace SUE4LuaLog
{
	const TCHAR* GetCurrentFileLocation(lua_State* L, const TCHAR* CFunctionName);
	void DebugPrint(lua_State* L, const TCHAR* Message);

	// 디버깅을 위해 청크이름에 대한 가상경로를 저장
	void AddFilename(const FString& VirtualFilename, const FString& ChunkName);

	// 디버깅을 위해 c++에서 호출하는 lua 함수 이름을 기록
	struct FFileLocationHintHelper
	{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		FFileLocationHintHelper(lua_State* L, int32 FuncStackIndex);
		~FFileLocationHintHelper();

		FString FunctionLocation;
		const FFileLocationHintHelper* Prev = nullptr;
#else
		FFileLocationHintHelper(lua_State* L, int32 FuncStackIndex) {}
		~FFileLocationHintHelper() {}
#endif // !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	};
}
