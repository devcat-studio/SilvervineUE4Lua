// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaUnitTests.h"

#include "LuaBundleFileLoader.h"
#include "LuaLocalFileLoader.h"
#include "LuaVirtualMachine.h"
#include "SilvervineUE4LuaSettings.h"


//=============================================================================================================================
// FSUE4LuaTestCaseFileLoader
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseFileLoader, "Silvervine.UE4Lua.FileLoader", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseFileLoader::RunTest(const FString& Parameters)
{
	{
		TestTrue(TEXT("FileLoader.MakeCleanPath: '/' -> ''"), FSUE4LuaFileLoader::MakeCleanPath(TEXT("/")) == FString());
		TestTrue(TEXT("FileLoader.MakeCleanPath: '//' -> ''"), FSUE4LuaFileLoader::MakeCleanPath(TEXT("//")) == FString());
		TestTrue(TEXT("FileLoader.MakeCleanPath: '///' -> ''"), FSUE4LuaFileLoader::MakeCleanPath(TEXT("///")) == FString(TEXT("/")));
		TestTrue(TEXT("FileLoader.MakeCleanPath: ' A ' -> 'A'"), FSUE4LuaFileLoader::MakeCleanPath(TEXT(" A ")) == FString(TEXT("A")));
		TestTrue(TEXT("FileLoader.MakeCleanPath: ' /A/ ' -> 'A'"), FSUE4LuaFileLoader::MakeCleanPath(TEXT(" /A/ ")) == FString(TEXT("A")));
	}

	if (!USUE4LuaSettings::Get()->bUseBundleFileLoader)
	{
		{
			auto LocalFileLoader = TSharedPtr<FSUE4LuaLocalFileLoader>(new FSUE4LuaLocalFileLoader(true));

			TestTrue(TEXT("LocalFileLoader.Map('A', '') -> Error"), LocalFileLoader->Map(TEXT("A"), TEXT("")) == false);
			TestTrue(TEXT("LocalFileLoader.Map('', 'B') -> Error"), LocalFileLoader->Map(TEXT(""), TEXT("B")) == false);

			TestTrue(TEXT("LocalFileLoader.Map(A, B) -> OK"), LocalFileLoader->Map(TEXT("A"), TEXT("B")) == true);
			TestTrue(TEXT("LocalFileLoader.Map(A, B) Again -> ERROR"), LocalFileLoader->Map(TEXT("A"), TEXT("B")) == false);
		}
		{
			auto LocalFileLoader = TSharedPtr<FSUE4LuaLocalFileLoader>(new FSUE4LuaLocalFileLoader);
			LocalFileLoader->Map(TEXT("SUE4Lua"), *FPaths::Combine(FPaths::ProjectDir(), TEXT("Plugins/SilvervineUE4Lua/Source/SilvervineUE4Lua/Private/Scripts")));

			{
				FString LuaSource;
				TestTrue(TEXT("LocalFileLoader.LoadFileToString()"), LocalFileLoader->LoadFileToString(LuaSource, TEXT("SUE4Lua/Tests/AllTests.lua")));
			}
		}
		{
			auto LocalFileLoader = TSharedPtr<FSUE4LuaLocalFileLoader>(new FSUE4LuaLocalFileLoader);
			LocalFileLoader->Map(TEXT("SUE4Lua"), *FPaths::Combine(FPaths::ProjectDir(), TEXT("Plugins/SilvervineUE4Lua/Source/SilvervineUE4Lua/Private/Scripts")));

			{
				TArray<FString> LuaFilenames = LocalFileLoader->GetFilenames(TEXT("SUE4Lua/Tests"));
				TestTrue(TEXT("LocalFileLoader.GetFilenames()"), LuaFilenames.Contains(TEXT("SUE4Lua/Tests/AllTests.lua")));
			}
		}
	}

	return true;
}