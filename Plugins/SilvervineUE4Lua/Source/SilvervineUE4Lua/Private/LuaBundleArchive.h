// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"
#include "Templates/SharedPointer.h"


//
// 압축한 zip 파일에 대한 FBufferArchive 구현. lua 번들 파일을 처리할 때 사용합니다.
//
// 참조 : https://en.wikipedia.org/wiki/Zip_(file_format)
//
class SILVERVINEUE4LUA_API FSUE4LuaBundleArchive : public FBufferArchive
{
public:
	static TSharedRef<FSUE4LuaBundleArchive> Create(const TCHAR* BundlePath);

	// 번들에서 파일을 읽습니다.
	bool ReadFile(FString& OutResult, const TCHAR* Filename);
	// 번들에 들어있는 파일 목록을 얻습니다.
	void GetFilenames(TArray<FString>& OutFilenames) { LocalFileHeaderOffsetMappings.GetKeys(OutFilenames); };

public:
	// Begin FBufferArchive Interface
	virtual FString GetArchiveName() const override { return *FString::Printf(TEXT("Lua Bundle(%s)"), *ArchiveName.ToString()); }
	// EndFBufferArchive Interface

private:
	FSUE4LuaBundleArchive(bool bIsPersistent = false, const FName BundleName = NAME_None);

	bool ReadCentralDirectories();

private:
	TMap<FString, uint32> LocalFileHeaderOffsetMappings;
};