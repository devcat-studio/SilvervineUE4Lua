// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "Modules/ModuleInterface.h"

#include "LuaFileLoader.h"

//
// SUE4Lua 모듈 정의
//
class SILVERVINEUE4LUA_API FSilvervineUE4LuaModule : public IModuleInterface
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

public:
	static FSilvervineUE4LuaModule& Get();

	DECLARE_MULTICAST_DELEGATE_OneParam(FFileModifiedDelegate, const FString& /*Filename*/);

public:
	// 설정되어 있는 파일 로더를 반환. 
	// 기본값은 내부에서 생성한 null 로더 인스턴스. 이 로더는 아무 동작도 하지 않고 경고 메시지만 출력합니다.
	FSUE4LuaFileLoader& GetFileLoader() const;

	// 파일 로더를 설정합니다.
	// null 객체를 전달하면 기본값(null 로더 인스턴스)으로 설정된다.
	// 반환값은 이전에 설정된 파일 로더.
	TSharedPtr<FSUE4LuaFileLoader> SetFileLoader(TSharedPtr<FSUE4LuaFileLoader> NewFileLoader);

	// 파일이 수정되었을 때 발생하는 이벤트
	FFileModifiedDelegate FileModifiedDelegate;
	
private:
	// 현재 설정된 파일 로더
	TSharedPtr<FSUE4LuaFileLoader> FileLoader;
};
