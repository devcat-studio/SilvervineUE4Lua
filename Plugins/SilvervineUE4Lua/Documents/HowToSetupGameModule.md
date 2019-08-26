
How to Setup a Game Module
==========================

To use SUE4Lua in the C++ code of the game module, you must write some setup codes.

If you are using lua only with a blueprint, you can skip the steps below.

1. Add the following code in your game module's Build.cs.

```cpp
PrivateDependencyModuleNames.Add("SilvervineUE4Lua");
```

For example,

```cpp
// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class SUE4LuaSample : ModuleRules
{
	public SUE4LuaSample(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

		PrivateDependencyModuleNames.Add("SilvervineUE4Lua");
	}
}
```

2. Add `#include "SilvervineUE4LuaCodeGen_Game.g.inl"` to the appropriate location of your game module's cpp.

3. Add the following code in StartupModule().

```cpp
SUE4LUA_REGISTER_BINDINGS();
```

For example,

```cpp
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
```

------------------------------------------------
[Back to Programming Guide](ProgrammingGuide.md)