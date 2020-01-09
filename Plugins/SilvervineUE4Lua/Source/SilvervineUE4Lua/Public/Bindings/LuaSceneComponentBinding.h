// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "Components/SceneComponent.h"

#include "LuaBindingRegistry.h"

#include "LuaSceneComponentBinding.generated.h"


//
// USceneObject에 대한 lua 바인딩
//
UCLASS(NotBlueprintType)
class SILVERVINEUE4LUA_API USUE4LuaSceneComponentBinding : public USUE4LuaBinding
{
	GENERATED_BODY()
	
public:
	// Begin USUE4LuaBinding Interface
	virtual UClass* GetBindingClass() const override;
	// End USUE4LuaBinding Interface

	UFUNCTION()
	static void LuaSetupAttachment(USceneComponent* InSelf, USceneComponent* InParent, FName InSocketName = NAME_None);
};