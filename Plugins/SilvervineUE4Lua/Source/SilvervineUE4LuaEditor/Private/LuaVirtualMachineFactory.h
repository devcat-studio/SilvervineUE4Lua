// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"

#include "LuaVirtualMachineFactory.generated.h"


//
// USUE4LuaVirtualMachine을 uasset으로 만들어주는 클래스 입니다. 
//
UCLASS()
class USUE4LuaVirtualMachineFactory : public UFactory
{
	GENERATED_UCLASS_BODY()
	
public:
	// Begin UFactory Interface
	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	// End UFactory Interface

private:
	UPROPERTY()
	UClass* SelectedClass = nullptr;
};