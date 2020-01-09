// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "Bindings/LuaStructBinding.h"

#include "LuaLog.h"


UClass* USUE4LuaStructBinding::GetBindingClass() const
{
	return UStruct::StaticClass();
}

UStruct* USUE4LuaStructBinding::LuaGetSuperStruct(UStruct* InSelf)
{
	return InSelf->GetInheritanceSuper();
}

TArray<FString> USUE4LuaStructBinding::LuaGetFieldNames(UStruct* InSelf)
{
	TArray<FString> OutputNames;

	for (TFieldIterator<UField> FieldIt(InSelf, EFieldIteratorFlags::ExcludeSuper); FieldIt; ++FieldIt)
	{
		const UField* Field = *FieldIt;
		const UClass* FieldClass = Field->GetClass();
		
		if (FieldClass->HasAnyCastFlag(CASTCLASS_UProperty | CASTCLASS_UFunction))
		{
			OutputNames.Add(Field->GetName());
		}
	}

	return OutputNames;
}

TArray<FString> USUE4LuaStructBinding::LuaGetPropertyNames(UStruct* InSelf)
{
	TArray<FString> OutputNames;

	for (TFieldIterator<UProperty> PropIt(InSelf, EFieldIteratorFlags::ExcludeSuper); PropIt; ++PropIt)
	{
		OutputNames.Add(PropIt->GetName());
	}

	return OutputNames;
}

TArray<FString> USUE4LuaStructBinding::LuaGetFunctionNames(UStruct* InSelf)
{
	TArray<FString> OutputNames;

	for (TFieldIterator<UFunction> FuncIt(InSelf, EFieldIteratorFlags::ExcludeSuper); FuncIt; ++FuncIt)
	{
		OutputNames.Add(FuncIt->GetName());
	}

	return OutputNames;
}
