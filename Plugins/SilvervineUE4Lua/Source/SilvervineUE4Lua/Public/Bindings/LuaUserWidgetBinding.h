// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"

#include "LuaBindingRegistry.h"

#include "LuaUserWidgetBinding.generated.h"


//
// UUserWidget에 대한 lua 바인딩
//
UCLASS(NotBlueprintType)
class SILVERVINEUE4LUA_API USUE4LuaUserWidgetBinding : public USUE4LuaBinding
{
	GENERATED_BODY()
	
public:
	// Begin USUE4LuaBinding Interface
	virtual UClass* GetBindingClass() const override;
	// End USUE4LuaBinding Interface

	UFUNCTION()
	static UWidget* LuaGetWidgetFromName(UUserWidget* InSelf, const FName& Name);
};