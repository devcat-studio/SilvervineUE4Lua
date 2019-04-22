// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaLocalFileLoader.h"

#if WITH_EDITOR
#include "DirectoryWatcherModule.h"
#include "IDirectoryWatcher.h"
#include "Modules/ModuleManager.h"
#endif // WITH_EDITOR

#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#include "LuaLog.h"
#include "SilvervineUE4Lua.h"


FSUE4LuaLocalFileLoader::FSUE4LuaLocalFileLoader(bool bInUsedForUnitTest)
	: bUsedForUnitTest(bInUsedForUnitTest)
{
}

FSUE4LuaLocalFileLoader::~FSUE4LuaLocalFileLoader()
{
}

bool FSUE4LuaLocalFileLoader::LoadFileToString(FString& Result, const TCHAR* Filename)
{
	if (Filename == nullptr)
	{
		if (!bUsedForUnitTest)
		{
			UE_LOG(LogSUE4L, Error, TEXT("Null path. [%s]"), TEXT(__FUNCTION__));
		}
		return false;
	}

	FString CleanFilename = MakeCleanPath(Filename);

	FString LocalPath;
	{
		int32 PathSeperatorIndex = -1;
		if (!CleanFilename.FindChar(PathSeperator, PathSeperatorIndex) || PathSeperatorIndex <= 0)
		{
			if (!bUsedForUnitTest)
			{
				UE_LOG(LogSUE4L, Error, TEXT("Invalid virtual path: %s. [%s]"), Filename, TEXT(__FUNCTION__));
			}
			return false;
		}

		FString VirtualPath = CleanFilename.Left(PathSeperatorIndex);
		VirtualPath.ToLowerInline();

		const FString* LocalPathMapping = PathMappings.Find(VirtualPath);
		if (LocalPathMapping == nullptr)
		{
			if (!bUsedForUnitTest)
			{
				UE_LOG(LogSUE4L, Error, TEXT("Virtual path '%s' is not found. [%s]"), *VirtualPath, TEXT(__FUNCTION__));
			}
			return false;
		}

		LocalPath = FPaths::Combine(*LocalPathMapping, CleanFilename.Mid(PathSeperatorIndex));
	}

	if (!bUsedForUnitTest)
	{
		UE_LOG(LogSUE4L, Verbose, TEXT("Loading lua file '%s'... [%s] "), *LocalPath, TEXT(__FUNCTION__));
	}

	return FFileHelper::LoadFileToString(Result, *LocalPath);
}

FString FSUE4LuaLocalFileLoader::GetLocalFilePath(const TCHAR* Filename)
{
	FString CleanFilename = MakeCleanPath(Filename);

	FString LocalPath;
	{
		int32 PathSeperatorIndex = -1;
		CleanFilename.FindChar(PathSeperator, PathSeperatorIndex);

		FString VirtualPath = (PathSeperatorIndex <= 0) ? CleanFilename : CleanFilename.Left(PathSeperatorIndex);
		VirtualPath.ToLowerInline();

		const FString* LocalPathMapping = PathMappings.Find(VirtualPath);
		if (LocalPathMapping == nullptr)
		{
			return FString();
		}
		
		LocalPath = (PathSeperatorIndex <= 0) ? *LocalPathMapping  : FPaths::Combine(*LocalPathMapping, CleanFilename.Mid(PathSeperatorIndex));
	}
	return LocalPath;
}

TArray<FString> FSUE4LuaLocalFileLoader::GetFilenames(const TCHAR* Path)
{	
	if (Path == nullptr)
	{
		if (!bUsedForUnitTest)
		{
			UE_LOG(LogSUE4L, Error, TEXT("Null path. [%s]"), TEXT(__FUNCTION__));
		}
		return TArray<FString>();
	}

	// 가상경로를 실제경로로 바꿉니다.
	FString LocalPath = GetLocalFilePath(Path);
	if (LocalPath.IsEmpty())
	{
		if (!bUsedForUnitTest)
		{
			UE_LOG(LogSUE4L, Error, TEXT("Virtual path '%s' is not found. [%s]"), Path, TEXT(__FUNCTION__));
		}
		return TArray<FString>();
	}

	// 실제경로의 하위 파일을 찾은 뒤, 다시 가상경로로 변환
	TArray<FString> Filenames;
	
	IFileManager::Get().FindFilesRecursive(Filenames, *LocalPath, TEXT("*"), true, false);
	for (FString& Name : Filenames)
	{
		Name = Name.Replace(*LocalPath, Path);
	}
	
	return Filenames;
}

bool FSUE4LuaLocalFileLoader::MakeVirtualPath(FString& LocalPath) const
{
	FString NormalizedLocalPath = LocalPath;
	FPaths::NormalizeFilename(NormalizedLocalPath);

	for (const auto& PathMapping : PathMappings)
	{
		if (NormalizedLocalPath.StartsWith(PathMapping.Value))
		{
			LocalPath = FPaths::Combine(PathMapping.Key, NormalizedLocalPath.Mid(PathMapping.Value.Len()));
			return true;
		}
	}

	return false;
}

bool FSUE4LuaLocalFileLoader::Map(const TCHAR* VirtualPath, const TCHAR* LocalPath)
{
	FString VirtualPathStr = MakeCleanPath(VirtualPath);
	FString LocalPathStr = MakeCleanPath(LocalPath);
	FPaths::NormalizeFilename(LocalPathStr);

	if (VirtualPathStr.IsEmpty() || LocalPathStr.IsEmpty())
	{
		if (!bUsedForUnitTest)
		{
			UE_LOG(LogSUE4L, Error, TEXT("Invalid path. [%s] "), TEXT(__FUNCTION__));
		}
		return false;
	}

	// 가상 경로는 모두 소문자로 변환
	VirtualPathStr.ToLowerInline();

	if (PathMappings.Find(VirtualPathStr) != nullptr)
	{
		if (!bUsedForUnitTest)
		{
			UE_LOG(LogSUE4L, Error, TEXT("VirtualPath '%s' already exists. [%s] "), *VirtualPathStr, TEXT(__FUNCTION__));
		}
		return false;
	}

	PathMappings.Add(VirtualPathStr, LocalPathStr);

	if (!bUsedForUnitTest)
	{
		UE_LOG(LogSUE4L, Verbose, TEXT("Mapping '%s' to '%s [%s]"), *VirtualPathStr, *LocalPathStr, TEXT(__FUNCTION__));
	}

#if WITH_EDITOR
	if (!bUsedForUnitTest)
	{
		FDirectoryWatcherModule& DirectoryWatcherModule = FModuleManager::LoadModuleChecked<FDirectoryWatcherModule>(TEXT("DirectoryWatcher"));
		FString DirectoryToWatch = FPaths::ConvertRelativePathToFull(LocalPath);

		UE_LOG(LogSUE4L, Verbose, TEXT("Watching '%s'..."), *DirectoryToWatch);
		
		FDelegateHandle DelegateHandle;
		DirectoryWatcherModule.Get()->RegisterDirectoryChangedCallback_Handle(
			DirectoryToWatch,
			IDirectoryWatcher::FDirectoryChanged::CreateRaw(this, &FSUE4LuaLocalFileLoader::OnFileChanged),
			DelegateHandle);
	}
#endif // WITH_EDITOR

	return true;
}

void FSUE4LuaLocalFileLoader::OnFileChanged(const TArray<struct FFileChangeData>& FileChanges)
{
#if WITH_EDITOR
	auto& SUE4LuaModule = FSilvervineUE4LuaModule::Get();

	if (&SUE4LuaModule.GetFileLoader() != this)
	{
		return;
	}

	// 같은 파일에 대해서 변경점이 여러 번 들어오는 경우가 있음
	TSet<FString> AlreadyHandled;

	for (const auto& FileChange : FileChanges)
	{
		if (AlreadyHandled.Contains(FileChange.Filename))
		{
			continue;
		}

		AlreadyHandled.Add(FileChange.Filename);

		// UE_LOG(LogSUE4L, Verbose, TEXT("FileChanged(%d): %s"), FileChange.Action, *FileChange.Filename);

		FString VirtualPath = FileChange.Filename;
		if (MakeVirtualPath(VirtualPath))
		{
			SUE4LuaModule.FileModifiedDelegate.Broadcast(VirtualPath);
		}
	}
#endif // WITH_EDITOR
}
