// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "Bindings/LuaUserWidgetBinding.h"

#include "LuaLog.h"


UClass* USUE4LuaUserWidgetBinding::GetBindingClass() const
{
	return UUserWidget::StaticClass();
}

UWidget* USUE4LuaUserWidgetBinding::LuaGetWidgetFromName(UUserWidget* InSelf, const FName& Name)
{
	return InSelf->GetWidgetFromName(Name);
}