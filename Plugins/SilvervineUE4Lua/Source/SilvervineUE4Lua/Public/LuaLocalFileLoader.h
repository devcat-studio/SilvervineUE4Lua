// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "LuaFileLoader.h"


//
// 로컬 경로에서 lua 파일을 읽는 로더 클래스
//
class SILVERVINEUE4LUA_API FSUE4LuaLocalFileLoader : public FSUE4LuaFileLoader
{
public:
	FSUE4LuaLocalFileLoader(bool bInUsedForUnitTest = false);
	virtual ~FSUE4LuaLocalFileLoader();

	virtual bool LoadFileToString(FString& Result, const TCHAR* Filename) override;
	virtual FString GetLocalFilePath(const TCHAR* Filename) override;
	virtual TArray<FString> GetFilenames(const TCHAR* Path) override;

	bool MakeVirtualPath(FString& LocalPath) const;

public:
	// 가상 경로를 실제 경로에 연결
	bool Map(const TCHAR* VirtualPath, const TCHAR* LocalPath);

private:
	void OnFileChanged(const TArray<struct FFileChangeData>& FileChanges);

private:
	TMap<FString, FString> PathMappings;
	bool bUsedForUnitTest = false;
};