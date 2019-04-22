// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaVirtualMachineFactory.h"

#include "ClassViewerFilter.h"
#include "ClassViewerModule.h"
#include "Kismet2/SClassPickerDialog.h"
#include "ModuleManager.h"

#include "SilvervineUE4LuaVirtualMachine.h"


USUE4LuaVirtualMachineFactory::USUE4LuaVirtualMachineFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	SupportedClass = USUE4LuaVirtualMachine::StaticClass();
}

bool USUE4LuaVirtualMachineFactory::ConfigureProperties()
{
	SelectedClass = nullptr;

	FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;
	Options.DisplayMode = EClassViewerDisplayMode::ListView;

	{
		struct FFilter : IClassViewerFilter
		{
			virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override
			{
				return InClass->IsChildOf(USUE4LuaVirtualMachine::StaticClass());
			}

			virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef<const IUnloadedBlueprintData> InUnloadedClassData, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override
			{
				return InUnloadedClassData->IsChildOf(USUE4LuaVirtualMachine::StaticClass());
			}
		};

		Options.ClassFilter = MakeShareable(new FFilter);
	}

	const FText TitleText = FText::FromString(TEXT("Pick Parent Class"));	// #todo: localization
	UClass* ChosenClass = NULL;
	const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, ChosenClass, USUE4LuaVirtualMachine::StaticClass());
	if (bPressedOk)
	{
		SelectedClass = ChosenClass;
	}

	return bPressedOk;
}

UObject* USUE4LuaVirtualMachineFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<USUE4LuaVirtualMachine>(InParent, SelectedClass, Name, Flags);
}