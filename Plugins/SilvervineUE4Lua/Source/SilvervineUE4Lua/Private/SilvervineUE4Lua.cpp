// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "SilvervineUE4Lua.h"
#include "SilvervineUE4LuaSettings.h"

#include "Modules/ModuleManager.h"

#include "LuaBindingRegistry.h"
#include "LuaLog.h"
#include "LuaNativeValue.h"

#include "SilvervineUE4LuaCodeGen_Engine.g.inl"


DEFINE_LOG_CATEGORY(LogSUE4L);

// 경고만 출력하는 null lua 파일 로더
class FSUE4LuaNullFileLoader : public FSUE4LuaFileLoader
{
public:
	virtual bool LoadFileToString(FString& Result, const TCHAR* Filename) override
	{
		UE_LOG(LogSUE4L, Warning, TEXT("Accessing to Null File Loader. Check that SetFileLoader() calls is valid. [%s]"), __SUE4LUA_FUNCTION__);

		return false;
	}
	virtual FString GetLocalFilePath(const TCHAR* Filename) override
	{
		UE_LOG(LogSUE4L, Warning, TEXT("Accessing to Null File Loader. Check that SetFileLoader() calls is valid. [%s]"), __SUE4LUA_FUNCTION__);
		return FString();
	}
	virtual TArray<FString> GetFilenames(const TCHAR* Filename) override
	{
		UE_LOG(LogSUE4L, Warning, TEXT("Accessing to Null File Loader. Check that SetFileLoader() calls is valid. [%s]"), __SUE4LUA_FUNCTION__);
		return TArray<FString>();	
	}
};

IMPLEMENT_MODULE(FSilvervineUE4LuaModule, SilvervineUE4Lua)

FSilvervineUE4LuaModule& FSilvervineUE4LuaModule::Get()
{
	return FModuleManager::GetModuleChecked<FSilvervineUE4LuaModule>("SilvervineUE4Lua");
}

void FSilvervineUE4LuaModule::StartupModule()
{
	USUE4LuaSettings::Get()->SetFileLoader();
	FSUE4LuaBindingRegistry::Get().SetStaticBindingEnabled(USUE4LuaSettings::Get()->bEnableStaticBinding);

	SUE4LUA_REGISTER_BINDINGS();
}

void FSilvervineUE4LuaModule::ShutdownModule()
{
	if (0 < FSUE4LuaNativeValue::GetInstanceCount())
	{
		UE_LOG(LogSUE4L, Warning, TEXT("%d remaining native values. [%s]"), FSUE4LuaNativeValue::GetInstanceCount(), __SUE4LUA_FUNCTION__);
	}
}

FSUE4LuaFileLoader& FSilvervineUE4LuaModule::GetFileLoader() const
{
	return *FileLoader;
}

TSharedPtr<FSUE4LuaFileLoader> FSilvervineUE4LuaModule::SetFileLoader(TSharedPtr<FSUE4LuaFileLoader> NewFileLoader)
{
	auto OldFileLoader = FileLoader;
	FileLoader = NewFileLoader;

	if (!FileLoader.IsValid())
	{
		FileLoader = TSharedPtr<FSUE4LuaNullFileLoader>(new FSUE4LuaNullFileLoader);
	}

	return OldFileLoader; 
}	