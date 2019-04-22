// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

#include "ModuleManager.h"


#define LOCTEXT_NAMESPACE "SilvervineUE4LuaEditor"

class FSilvervineUE4LuaEditorModule : public IModuleInterface
{
	// Begin IModuleInterface
	virtual void StartupModule() override
	{
	}

	virtual void ShutdownModule() override
	{
	}
	// End IModuleInterface
};

IMPLEMENT_MODULE(FSilvervineUE4LuaEditorModule, SilvervineUE4LuaEditor)

#undef LOCTEXT_NAMESPACE