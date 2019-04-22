// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaCall.h"

#include "LuaAPI.h"
#include "LuaLog.h"
#include "LuaStack.h"
#include "LuaVirtualMachine.h"


namespace SUE4LuaCall
{
	bool PCall(lua_State* L, int32 ArgCount, int32 RetCount)
	{
		SUE4LuaLog::FFileLocationHintHelper FileLocationHintHelper(L, lua_gettop(L) - ArgCount);

		if (auto VM = FSUE4LuaVirtualMachine::PinVM(L))
		{
			// Stack: Func, Arg1, ..., ArgN
			VM->PushErrorHandler(L);
			lua_insert(L, -2 - ArgCount);
			// Stack: ErrorHandler, Func, Arg1, ..., ArgN

			bool bSucceeded = true;
			int32 TopIndex = lua_gettop(L);

			if (lua_pcall(L, ArgCount, RetCount, -2 - ArgCount) == 0)
			{
				if (RetCount == LUA_MULTRET)
				{
					RetCount = lua_gettop(L) - (TopIndex - 1 - ArgCount);
				}

				// Stack: ErrorHandler, Ret1, ..., RetN
				lua_rotate(L, -1 - RetCount, -1);
				// Stack: Ret1, ..., RetN, ErrorHandler
			}
			else
			{
				bSucceeded = false;

				// 에러메시지는 이미 에러핸들러가 처리함
				// Stack: ErrorHandler, ErrorCode
				lua_pop(L, 1);
				// Stack: ErrorHandler
			}

			VM->PopErrorHandler(L);
			
			// Stack: Ret1, ..., RetN
			return bSucceeded;
		}

		return false;
	}
}
