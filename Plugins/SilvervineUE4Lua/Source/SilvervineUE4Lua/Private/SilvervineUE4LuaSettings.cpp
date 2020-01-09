// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#include "SilvervineUE4LuaSettings.h"

#include "Misc/Paths.h"

#include "LuaBindingRegistry.h"
#include "LuaBundleFileLoader.h"
#include "LuaLocalFileLoader.h"
#include "SilvervineUE4Lua.h"


const USUE4LuaSettings* USUE4LuaSettings::Get()
{
	return GetDefault<USUE4LuaSettings>();
}

FName USUE4LuaSettings::GetCategoryName() const
{
	return TEXT("Plugins");
}

void USUE4LuaSettings::SetFileLoader() const
{
	FString ProjectDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	if (bUseBundleFileLoader)
	{
		auto BundleFileLoader = TSharedPtr<FSUE4LuaBundleFileLoader>(new FSUE4LuaBundleFileLoader);
		for (const auto& Set : BundleFileLoadMap)
		{
			BundleFileLoader->Map(*Set.Key, *FPaths::Combine(ProjectDir, *Set.Value));
		}
		FSilvervineUE4LuaModule::Get().SetFileLoader(BundleFileLoader);
	}
	else
	{
		auto LocalFileLoader = TSharedPtr<FSUE4LuaLocalFileLoader>(new FSUE4LuaLocalFileLoader);
		for (const auto& Set : LocalFileLoadMap)
		{
			LocalFileLoader->Map(*Set.Key, *FPaths::Combine(ProjectDir, *Set.Value));
		}
		FSilvervineUE4LuaModule::Get().SetFileLoader(LocalFileLoader);
	}
}

#if WITH_EDITOR
FText USUE4LuaSettings::GetSectionText() const
{
	return NSLOCTEXT("SilvervineUE4Lua", "SilvervineUE4LuaSettingsName", "SilvervineUE4Lua");
}
FText USUE4LuaSettings::GetSectionDescription() const
{
	return NSLOCTEXT("SilvervineUE4Lua", "SilvervineUE4LuaSettingsDescription", "Configuration for SilvervineUE4Lua");
}

void USUE4LuaSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property != nullptr)
	{
		SetFileLoader();
		FSUE4LuaBindingRegistry::Get().SetStaticBindingEnabled(bEnableStaticBinding);
	}
}
#endif	// WITH_EDITOR