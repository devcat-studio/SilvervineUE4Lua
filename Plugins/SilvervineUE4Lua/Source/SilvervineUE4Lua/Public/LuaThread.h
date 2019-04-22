// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "CoreMinimal.h"


class FSUE4LuaVirtualMachine;
class FSUE4LuaFunction;
struct lua_State;

//
// lua state에 해당하는 클래스
//
// VM에 대한 참조를 보관하고 싶다면 대신 이 클래스의 참조를 보관한 후에 PinVM()을 호출하세요.
//
class SILVERVINEUE4LUA_API FSUE4LuaThread : public TSharedFromThis<FSUE4LuaThread>, public FNoncopyable
{
public:
	~FSUE4LuaThread();

	static TSharedPtr<FSUE4LuaThread> Create(TSharedRef<FSUE4LuaVirtualMachine> VM, lua_State* LuaState);

	// 디스포즈된 상태면 true
	bool IsDisposed() const { return LuaState == nullptr; }

	// VM에 대한 강참조를 가져옵니다.
	// VM이 파괴되었거나 디스포즈 상태면 null이 반환됩니다.
	TSharedPtr<FSUE4LuaVirtualMachine> PinVM() const;

public:
	//
	// 아래는 내부 구현에만 사용됩니다.
	//

	// VM이 디스포즈될 때 호출됩니다.
	void SetDisposed();

private:
	FSUE4LuaThread(TSharedRef<FSUE4LuaVirtualMachine> VM, lua_State* LuaState);

private:
	TWeakPtr<FSUE4LuaVirtualMachine> WeakVM;
	lua_State* LuaState = nullptr;
};