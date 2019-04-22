// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "LuaBindingRegistry.h"

#include "LuaStructBinding.generated.h"


//
// UStruct에 대한 lua 바인딩
//
UCLASS(NotBlueprintType)
class SILVERVINEUE4LUA_API USUE4LuaStructBinding : public USUE4LuaBinding
{
	GENERATED_BODY()
	
public:
	// Begin USUE4LuaBinding Interface
	virtual UClass* GetBindingClass() const override;
	// End USUE4LuaBinding Interface

	UFUNCTION()
	static UStruct* LuaGetSuperStruct(UStruct* InSelf);

	UFUNCTION()
	static TArray<FString> LuaGetFieldNames(UStruct* InSelf);

	UFUNCTION()
	static TArray<FString> LuaGetPropertyNames(UStruct* InSelf);

	UFUNCTION()
	static TArray<FString> LuaGetFunctionNames(UStruct* InSelf);
};