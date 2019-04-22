// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "LuaFileLoader.h"


class FSUE4LuaBundleArchive;

//
// zip 압축 파일에서 lua 파일을 읽는 로더 클래스
//
class SILVERVINEUE4LUA_API FSUE4LuaBundleFileLoader : public FSUE4LuaFileLoader
{
public:
	FSUE4LuaBundleFileLoader();
	~FSUE4LuaBundleFileLoader();

	virtual bool LoadFileToString(FString& Result, const TCHAR* Filename) override;
	virtual FString GetLocalFilePath(const TCHAR* Filename) override;
	virtual TArray<FString> GetFilenames(const TCHAR* Path) override;

public:
	// 로그 출력을 비활성화
	void SuppressLogs();

	// 가상 경로를 실제 경로의 번들에 연결
	bool Map(const TCHAR* VirtualPath, const TCHAR* Path);

private:
	TMap<FString, TSharedRef<FSUE4LuaBundleArchive>> PathMappings;
};

