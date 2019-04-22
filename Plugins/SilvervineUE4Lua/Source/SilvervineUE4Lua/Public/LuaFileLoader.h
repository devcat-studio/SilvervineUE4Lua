// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "CoreMinimal.h"

//
// 실행 환경마다 lua 파일이 있는 위치가 다를 수 있기 때문에, 모든 lua 코드는 이 클래스의 자식 인스턴스로부터 로드됩니다.
//
// 사용자가 로더 종류에 상관없이 lua 파일을 로드할 수 있도록 다음과 같은 규칙으로 경로를 지정:
//
//	 * SUE4Lua 플러그인이 사용하는 파일은 '/SUE4Lua'로 시작. 예) '/SUE4Lua/Framework/Core.lua'
//	 * 게임 모듈이 사용하는 파일은 '/Game'으로 시작. 예) '/Game/Scenario/Volume01.lua'
//
// 각각의 가상 경로를 실제 파일 경로로 변환하는 역할은 파생 클래스가 처리합니다.
//
class SILVERVINEUE4LUA_API FSUE4LuaFileLoader : public TSharedFromThis<FSUE4LuaFileLoader>
{
public:
	FSUE4LuaFileLoader();
	virtual ~FSUE4LuaFileLoader();

	// 주어진 가상 경로에 대한 lua 소스 스트링을 반환
	virtual bool LoadFileToString(FString& Result, const TCHAR* Filename) = 0;
	// 주어진 가상 경로에 대한 실제 파일 경로를 반환
	virtual FString GetLocalFilePath(const TCHAR* Filename) = 0;
	// 주어진 가상 경로 및 모든 하위 경로에 존재하는 파일들의 가상 경로 목록을 반환
	virtual TArray<FString> GetFilenames(const TCHAR* Path) = 0;

public:
	// 경로 문자
	static const TCHAR PathSeperator = TCHAR('/');

	// 깨끗한 경로 형식으로 바꾼다.
	//	- 좌우 공백문자 제거
	//	- 좌우 경로문자 각각 최대 1개 제거
	static FString MakeCleanPath(const TCHAR* Path);
};

//
// lua 파일 로더를 쉽게 사용하기 위한 헬퍼 클래스
//
struct SILVERVINEUE4LUA_API FSUE4LuaFileLoaderHelper
{
	static bool LoadFileToString(FString& Result, const TCHAR* Filename);
	static FString GetLocalFilePath(const TCHAR* Filename);
	static TArray<FString> GetFilenames(const TCHAR* Path);
};
