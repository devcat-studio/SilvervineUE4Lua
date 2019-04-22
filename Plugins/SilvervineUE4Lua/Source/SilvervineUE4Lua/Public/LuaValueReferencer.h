// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "LuaThread.h"


//
// lua 값을 c++에 저장하고 싶을 때 사용합니다. 내부 구현에만 사용됩니다.
//
class FSUE4LuaValueReferencer : public FNoncopyable
{
public:
	FSUE4LuaValueReferencer(lua_State* L, int32 StackIndex);
	~FSUE4LuaValueReferencer();

	TSharedPtr<FSUE4LuaVirtualMachine> PinVM() const;
	void Push(lua_State* L);

private:
	TSharedPtr<FSUE4LuaThread> LuaMainThread;
};