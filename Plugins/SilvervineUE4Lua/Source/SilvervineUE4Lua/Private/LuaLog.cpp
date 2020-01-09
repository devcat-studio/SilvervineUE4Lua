// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#include "LuaLog.h"

#include "LuaAPI.h"


namespace SUE4LuaLog
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	// (ChunkName, VirtualFilename) 맵
	static TMap<FString, FString>	VirtualFilenames;

	// 최근에 c++에서 호출한 lua 함수 이름
	static const FFileLocationHintHelper* LatestLuaFileLocationCalledByNative = nullptr;

	// 스택을 뒤져서 가장 먼저 발견되는 lua 함수의 파일 위치를 반환. 없으면 null 반환
	static const TCHAR* FindFirstLuaFunctionLocation(lua_State* L, int32 Depth)
	{
		static FString Buffer;
		bool bFound = false;

		for (;;)
		{
			// debug.getinfo(Depth, 'nS')
			{
				lua_getglobal(L, "debug");
				lua_getfield(L, -1, "getinfo");
				lua_remove(L, -2);
				lua_pushinteger(L, Depth);
				lua_pushstring(L, "nlS");
				lua_call(L, 2, 1);
				// Stack: info
			}

			if (!lua_isnil(L, -1))
			{
				// if (info.what == "C") continue; 
				{
					lua_getfield(L, -1, "what");
					const char* What = lua_tostring(L, -1);
					if (FCStringAnsi::Strcmp(What, "C") == 0)
					{
						// Stack: info, what
						lua_pop(L, 2);
						// Stack: (empty)

						++Depth;
						continue;
					}
					lua_pop(L, 1);
				}

				// info.source
				{
					lua_getfield(L, -1, "source");
					if (lua_isstring(L, -1))
					{
						FString ChunkName = UTF8_TO_TCHAR(lua_tostring(L, -1));
						if (auto VirtualFilename = VirtualFilenames.Find(ChunkName))
						{
							Buffer = *VirtualFilename;
							bFound = true;
						}
					}
					lua_pop(L, 1);
				}

				// info.currentline
				if (bFound)
				{
					lua_getfield(L, -1, "currentline");
					Buffer += FString::Printf(TEXT(":%d"), lua_tointeger(L, -1));
					lua_pop(L, 1);
				}
			}

			break;
		}

		// Stack: info
		lua_pop(L, 1);
		// Stack: (empty)

		if (bFound)
		{
			return *Buffer;
		}

		return nullptr;
	}

	// 스택의 lua 함수 위치를 얻습니다.
	static FString GetLuaFunctionLocation(lua_State* L, int32 StackIndex)
	{
		FString Buffer;
		bool bFound = false;

		if (!lua_isfunction(L, StackIndex))
		{
			return FString();
		}

		// debug.getinfo(Depth, 'nS')
		{
			lua_getglobal(L, "debug");
			lua_getfield(L, -1, "getinfo");
			lua_remove(L, -2);
			lua_pushvalue(L, StackIndex);
			lua_pushstring(L, "nS");
			lua_call(L, 2, 1);
			// Stack: info
		}

		if (!lua_isnil(L, -1))
		{
			{
				lua_getfield(L, -1, "what");
				const char* What = lua_tostring(L, -1);
				if (FCStringAnsi::Strcmp(What, "C") == 0)
				{
					// Stack: info, what
					lua_pop(L, 2);
					// Stack: (empty)

					return FString();
				}
				lua_pop(L, 1);
			}

			// info.source
			{
				lua_getfield(L, -1, "source");
				if (lua_isstring(L, -1))
				{
					FString ChunkName = UTF8_TO_TCHAR(lua_tostring(L, -1));
					if (auto VirtualFilename = VirtualFilenames.Find(ChunkName))
					{
						Buffer = *VirtualFilename;
						bFound = true;
					}
				}
				lua_pop(L, 1);
			}

			// info.linedefined
			if (bFound)
			{
				lua_getfield(L, -1, "linedefined");
				Buffer += FString::Printf(TEXT(":%d"), lua_tointeger(L, -1));
				lua_pop(L, 1);
			}
		}

		// Stack: info
		lua_pop(L, 1);
		// Stack: (empty)

		if (bFound)
		{
			return MoveTemp(Buffer);
		}

		return FString();
	}
#endif // !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

	const TCHAR* GetCurrentFileLocation(lua_State* L, const TCHAR* CFunctionName)
	{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		const TCHAR* FunctionName = FindFirstLuaFunctionLocation(L, 1);
		if (FunctionName == nullptr)
		{
			if (LatestLuaFileLocationCalledByNative != nullptr)
			{
				FunctionName = *LatestLuaFileLocationCalledByNative->FunctionLocation;
			}
			else
			{
				FunctionName = CFunctionName;
			}
		}

		return FunctionName;
#else
		return TEXT("");
#endif	// #if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	}

	void DebugPrint(lua_State* L, const TCHAR* Message)
	{
		lua_getglobal(L, "SUE4Lua");
		if (lua_isnil(L, -1))
		{
			// VM 초기화 도중에 이렇게 될 수 있음
			lua_pop(L, 1);
			return;
		}

		lua_getfield(L, -1, "OnDebugPrint");
		lua_remove(L, -2);
		// Stack: OnDebugPrint

		if (lua_isfunction(L, -1))
		{
			lua_pushstring(L, TCHAR_TO_UTF8(Message));
			// Stack: OnDebugPrint, Message
			lua_pcall(L, 1, 0, 0);
			// Stack: (empty)
		}
		else
		{
			lua_pop(L, 1);
			// Stack: (empty)
		}
	}

	void AddFilename(const FString& VirtualFilename, const FString& ChunkName)
	{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		VirtualFilenames.Add(ChunkName, VirtualFilename);
#endif // !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	FFileLocationHintHelper::FFileLocationHintHelper(lua_State* L, int32 FuncStackIndex)
	{
		FunctionLocation = GetLuaFunctionLocation(L, FuncStackIndex);
		
		Prev = LatestLuaFileLocationCalledByNative;
		LatestLuaFileLocationCalledByNative = this;
	}

	FFileLocationHintHelper::~FFileLocationHintHelper()
	{
		LatestLuaFileLocationCalledByNative = Prev;
	}	
#endif // !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
}