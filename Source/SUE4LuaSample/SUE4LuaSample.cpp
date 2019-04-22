// Fill out your copyright notice in the Description page of Project Settings.

#include "SUE4LuaSample.h"
#include "Modules/ModuleManager.h"

#include "SilvervineUE4LuaCodeGen_Game.g.inl"

class FSUE4LuaSampleGameModuleImpl : public FDefaultGameModuleImpl
{
public:
	// Begin IModuleInterface
	virtual void StartupModule() override
	{
		SUE4LUA_REGISTER_BINDINGS();
	}

	virtual void ShutdownModule() override
	{
	}
	// End IModuleInterface
};

IMPLEMENT_PRIMARY_GAME_MODULE(FSUE4LuaSampleGameModuleImpl, SUE4LuaSample, "SUE4LuaSample");

