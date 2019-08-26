// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaBundleFileLoader.h"

#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/MemoryReader.h"

#include "LuaBundleArchive.h"
#include "LuaLog.h"


FSUE4LuaBundleFileLoader::FSUE4LuaBundleFileLoader()
{
}

FSUE4LuaBundleFileLoader::~FSUE4LuaBundleFileLoader()
{
}

bool FSUE4LuaBundleFileLoader::LoadFileToString(FString& Result, const TCHAR* Filename)
{
	if (Filename == nullptr)
	{
		UE_LOG(LogSUE4L, Error, TEXT("Null path. [%s]"), __SUE4LUA_FUNCTION__);
		return false;
	}

	FString CleanFilename = MakeCleanPath(Filename);

	int32 PathSeperatorIndex = -1;
	if (!CleanFilename.FindChar(PathSeperator, PathSeperatorIndex) || PathSeperatorIndex <= 0)
	{
		UE_LOG(LogSUE4L, Error, TEXT("Invalid virtual path: %s. [%s]"), Filename, __SUE4LUA_FUNCTION__);
		return false;
	}

	FString VirtualPath = CleanFilename.Left(PathSeperatorIndex);
	VirtualPath.ToLowerInline();

	auto LuaBundle = PathMappings.Find(VirtualPath);
	if (LuaBundle == nullptr)
	{
		UE_LOG(LogSUE4L, Error, TEXT("Virtual path '%s' is not found. [%s]"), *VirtualPath, __SUE4LUA_FUNCTION__);
		return false;
	}

	FString LuaFilename = CleanFilename.Mid(PathSeperatorIndex + 1);

	return (*LuaBundle)->ReadFile(Result, *LuaFilename);
}

FString FSUE4LuaBundleFileLoader::GetLocalFilePath(const TCHAR* Filename)
{
	return Filename;
}

TArray<FString> FSUE4LuaBundleFileLoader::GetFilenames(const TCHAR* Path)
{
	if (Path == nullptr)
	{
		UE_LOG(LogSUE4L, Error, TEXT("Null path. [%s]"), __SUE4LUA_FUNCTION__);
		return TArray<FString>();
	}

	// 가상경로에 해당하는 Bundle을 찾습니다.
	FString CleanFilename = MakeCleanPath(Path);

	int32 PathSeperatorIndex = -1;
	CleanFilename.FindChar(PathSeperator, PathSeperatorIndex);
	FString VirtualPath = (PathSeperatorIndex <= 0) ? CleanFilename : CleanFilename.Left(PathSeperatorIndex); (PathSeperatorIndex <= 0);

	auto LuaBundle = PathMappings.Find(VirtualPath.ToLower());

	if (LuaBundle == nullptr)
	{
		UE_LOG(LogSUE4L, Error, TEXT("Virtual path '%s' is not found. [%s]"), Path, __SUE4LUA_FUNCTION__);
		return TArray<FString>();
	}

	// Bundle 안에 있는 하위 경로를 전부 읽어옵니다.
	TArray<FString> AllFilenamesInBundle;
	(*LuaBundle)->GetFilenames(AllFilenamesInBundle);

	// 그 중 경로가 일치하는 것을 찾아 분류
	TArray<FString> Filenames;

	FString ScriptPath = (PathSeperatorIndex <= 0) ? "" : MakeCleanPath(*CleanFilename.Mid(PathSeperatorIndex));
	for (int32 Idx = 0; Idx < AllFilenamesInBundle.Num(); Idx++)
	{
		if (ScriptPath.IsEmpty() || AllFilenamesInBundle[Idx].Contains(ScriptPath))
		{
			Filenames.Add(FPaths::Combine(VirtualPath, AllFilenamesInBundle[Idx]));
		}
	}

	return Filenames;
}

bool FSUE4LuaBundleFileLoader::Map(const TCHAR* VirtualPath, const TCHAR* BundlePath)
{
	FString VirtualPathStr = MakeCleanPath(VirtualPath);
	FString BundlePathStr = MakeCleanPath(BundlePath);

	if (VirtualPathStr.IsEmpty() || BundlePathStr.IsEmpty())
	{
		UE_LOG(LogSUE4L, Error, TEXT("Invalid path. [%s] "), __SUE4LUA_FUNCTION__);
		return false;
	}

	// 가상 경로는 모두 소문자로 변환
	VirtualPathStr.ToLowerInline();

	if (PathMappings.Find(VirtualPathStr) != nullptr)
	{
		UE_LOG(LogSUE4L, Error, TEXT("VirtualPath '%s' already exists. [%s] "), *VirtualPathStr, __SUE4LUA_FUNCTION__);
		return false;
	}
	
	auto LuaBundle = FSUE4LuaBundleArchive::Create(BundlePath);
	if (LuaBundle->Num() == 0)
	{
		UE_LOG(LogSUE4L, Error, TEXT("Bundle '%s' does not exist [%s]"), *BundlePathStr, __SUE4LUA_FUNCTION__);
		return false;
	}

	PathMappings.Add(VirtualPathStr, LuaBundle);

	UE_LOG(LogSUE4L, Verbose, TEXT("Mapping '%s' to '%s [%s]"), *VirtualPathStr, *BundlePathStr, __SUE4LUA_FUNCTION__);
	
	return true;
}