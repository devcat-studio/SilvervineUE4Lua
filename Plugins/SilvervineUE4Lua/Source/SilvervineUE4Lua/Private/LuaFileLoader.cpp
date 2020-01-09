// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#include "LuaFileLoader.h"
#include "SilvervineUE4Lua.h"


FSUE4LuaFileLoader::FSUE4LuaFileLoader()
{
}

FSUE4LuaFileLoader::~FSUE4LuaFileLoader()
{
}

FString FSUE4LuaFileLoader::MakeCleanPath(const TCHAR* Path)
{
	if (Path != nullptr)
	{
		// trim
		while (*Path != 0)
		{
			if (FChar::IsWhitespace(*Path))
			{
				++Path;
			}
			else
			{
				break;
			}
		}

		// 첫 글자 '/'는 무시
		Path = *Path == PathSeperator ? Path + 1 : Path;

		int32 Len = FCString::Strlen(Path);

		// trim trailing
		{
			int32 Pos = Len - 1;
			while (0 <= Pos)
			{
				if (FChar::IsWhitespace(Path[Pos]))
				{
					--Pos;
				}
				else
				{
					break;
				}
			}
			Len = Pos + 1;
		}

		// 마지막 '/'는 무시
		Len = (0 < Len && Path[Len - 1] == PathSeperator) ? Len - 1 : Len;

		return FString(Len, Path);
	}
	else
	{
		return FString();
	}
}

bool FSUE4LuaFileLoaderHelper::LoadFileToString(FString& Result, const TCHAR* Filename)
{
	return FSilvervineUE4LuaModule::Get().GetFileLoader().LoadFileToString(Result, Filename);
}

FString FSUE4LuaFileLoaderHelper::GetLocalFilePath(const TCHAR* Filename)
{
	return FSilvervineUE4LuaModule::Get().GetFileLoader().GetLocalFilePath(Filename);
}

TArray<FString> FSUE4LuaFileLoaderHelper::GetFilenames(const TCHAR* Path)
{
	return FSilvervineUE4LuaModule::Get().GetFileLoader().GetFilenames(Path);
}
