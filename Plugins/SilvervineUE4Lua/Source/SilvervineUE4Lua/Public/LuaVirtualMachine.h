// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "Tickable.h"


class FSUE4LuaFunction;
class FSUE4LuaThread;
class FSUE4LuaUserData;
struct lua_State;

//
// lua 코드를 실행하는 가상 머신 클래스
//
class SILVERVINEUE4LUA_API FSUE4LuaVirtualMachine : public TSharedFromThis<FSUE4LuaVirtualMachine>, public FNoncopyable, public FTickableGameObject
{
public:
	virtual ~FSUE4LuaVirtualMachine();

	// VM을 생성합니다.
	//
	// VSCode 등으로 디버깅하고 싶다면 bDebuggable에 true를 전달하세요.
	static TSharedPtr<FSUE4LuaVirtualMachine> Create(bool bDebuggable = false);

	// 디스포즈된(내부 상태가 모두 정리된) 상태면 true
	bool IsDisposed() const;
	// 디버깅 가능한 VM이면 true
	bool IsDebuggable() const;

	// 메인 스레드를 반환
	TSharedPtr<FSUE4LuaThread> GetLuaMainThread() const;

	// 할당한 모든 lua 자원을 해제하고 VM을 닫습니다.
	// 파괴자에서 자동으로 호출하므로 명시적으로 호출할 필요는 없습니다.
	void Dispose();

	// 게임 World을 알고 있는 객체를 설정합니다. 일반적으로 GameInstance를 전달합니다.
	// 약참조를 보관하므로 언제든지 null이 될 수 있습니다.
	UObject* GetWorldContextObject() const;
	void SetWorldContextObject(UObject* WorldContextObject);

	// lua 코드를 실행합니다. 실행이 성공하면 true를 반환합니다.
	bool ExecuteString(const TCHAR* Source);
	bool ExecuteFile(const TCHAR* Filename);

	// lua 코드를 로드한 후 호출자가 직접 실행할 수 있도록 청크를 반환합니다.
	TSharedPtr<FSUE4LuaFunction> LoadString(const TCHAR* Source);
	TSharedPtr<FSUE4LuaFunction> LoadFile(const TCHAR* Filename);

	// 디스패치 핸들러 팩토리 설정
	TSharedPtr<FSUE4LuaFunction> GetDispatchHandlerFactory() const;
	void SetDispatchHandlerFactory(TSharedPtr<FSUE4LuaFunction> NewFactory);

	// 특정 타입에 대한 디스패치 핸들러를 직접 설정. 이미 설정된 핸들러가 있으면 덮어씁니다.
	void SetDispatchHandler(UClass* DispatchClass, TSharedPtr<FSUE4LuaFunction> HandlerChunk);

public:
	//
	// 아래는 내부 구현에만 사용됩니다.
	//

	// Start FTickableObjectBase Interface
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(FSUE4LuaVirtualMachine, STATGROUP_Tickables); }
	// End FTickableObjectBase Interface

	// lua_State로부터 VM을 얻고 VM이 파괴되지 않도록 참조를 생성합니다.
	static TSharedPtr<FSUE4LuaVirtualMachine> PinVM(lua_State* L);

	// Execute*() 실제 구현
	bool ExecuteString(lua_State* L, const TCHAR* Source);
	bool ExecuteFile(lua_State* L, const TCHAR* Filename);

	// LoadString*() 실제 구현
	TSharedPtr<FSUE4LuaFunction> LoadString(lua_State* L, const TCHAR* Source);
	TSharedPtr<FSUE4LuaFunction> LoadFile(lua_State* L, const TCHAR* Filename);

	// 현재 활성화된 lua_State를 관리합니다. c++에서 lua 함수를 호출할 때 사용.
	lua_State* GetCurrentLuaState() const;
	void SetCurrentLuaState(lua_State* L);

	// lua_pcall() 등에서 사용
	void PushErrorHandler(lua_State* L);
	void PopErrorHandler(lua_State* L);

	// 디버깅을 위해 생성된 유저 데이터를 추적
	void AddUserData(FSUE4LuaUserData* UserData);
	void RemoveUserData(FSUE4LuaUserData* UserData);

	// 디스패치 핸들러를 lua 스택에 푸시
	bool PushDispatchHandler(lua_State* L, const UObject* DispatchObject, const FString& FunctionName);

private:
	FSUE4LuaVirtualMachine();

	class FSUE4LuaVMContext* Context = nullptr;
};