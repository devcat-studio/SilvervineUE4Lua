// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "LuaBindingRegistry.h"

#include "LuaObjectBinding.generated.h"


//
// UObject에 대한 lua 바인딩
//
UCLASS(NotBlueprintType)
class SILVERVINEUE4LUA_API USUE4LuaObjectBinding : public USUE4LuaBinding
{
	GENERATED_BODY()
	
public:
	// Begin USUE4LuaBinding Interface
	virtual UClass* GetBindingClass() const override;
	// End USUE4LuaBinding Interface

	UFUNCTION()
	static FString LuaGetName(UObject* InSelf);

	UFUNCTION()
	static UClass* LuaGetClass(UObject* InSelf);

	UFUNCTION()
	static bool LuaIsA(UObject* InSelf, FName ClassName);

	UFUNCTION()
	static class UWorld* LuaGetWorld(UObject* InSelf);
};