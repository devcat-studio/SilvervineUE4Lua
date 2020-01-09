// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "LuaVirtualMachine.h"

#include "SilvervineUE4LuaVirtualMachine.generated.h"


class UGameInstance;

USTRUCT()
struct FSUE4LuaVMInstance
{
	GENERATED_BODY()
	
	TSharedPtr<FSUE4LuaVirtualMachine> VM;

	UPROPERTY()
	UGameInstance* GameInstance = nullptr;
};

//
// luaVM을 UObject로 관리하기 위해서 작성된 클래스 입니다.
//
UCLASS(Blueprintable)
class SILVERVINEUE4LUA_API USUE4LuaVirtualMachine : public UObject
{
	GENERATED_BODY()

public:
	// lua VM을 생성할 때마다 이 경로의 파일을 자동으로 실행합니다.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString StartupFilePath;

	// 게임 인스턴스를 등록하고 luaVM을 생성합니다.
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "RegisterGameInstance"))
	bool BlueprintRegisterGameInstance(UGameInstance* GameInstance);
	TSharedPtr<FSUE4LuaVirtualMachine> RegisterGameInstance(UGameInstance* GameInstance);

	// 게임 인스턴스 등록을 해제하고 luaVM을 제거합니다.
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "UnregisterGameInstance"))
	bool BlueprintUnregisterGameInstance(UGameInstance* GameInstance);
	bool UnregisterGameInstance(UGameInstance* GameInstance);

	// 등록된 게임 인스턴스에 대한 luaVM을 반환합니다.
	FSUE4LuaVirtualMachine* FindVMFromGameInstance(UGameInstance* GameInstance);
	FSUE4LuaVirtualMachine* FindVMFromWorldContext(UObject* WorldContext);

protected:
	FSUE4LuaVMInstance* FindVMInstanceFromGameInstance(UGameInstance* GameInstance);
	FSUE4LuaVMInstance* FindVMInstanceFromWorldContext(UObject* WorldContext);

protected:
	// 등록된 게임 인스턴스 VM
	UPROPERTY(Transient)
	TArray<FSUE4LuaVMInstance> GameVMInstances;

private:
	FSUE4LuaVMInstance* GetEditorVMInstance();
	void OnPreExit();

private:
	// 에디터 VM 인스턴스
	UPROPERTY(Transient)
	FSUE4LuaVMInstance EditorVMInstance;
};
