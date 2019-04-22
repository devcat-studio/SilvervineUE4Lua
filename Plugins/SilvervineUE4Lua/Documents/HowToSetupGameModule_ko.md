
게임 모듈 설정하기
================

게임 모듈의 C++ 코드에서 SUE4Lua 코드를 사용하려면 몇 가지 설정이 필요합니다.

Blueprint에서만 Lua를 사용하고 있다면 아래 과정은 생략해도 상관 없습니다.

1. 게임 모듈의 Build.cs 파일을 열고 아래 코드를 추가합니다.

```cpp
PrivateDependencyModuleNames.Add("SilvervineUE4Lua");
```

예를 들면,

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

2. 게임 모듈을 정의하는 cpp 파일을 열고 적당한 위치에 `#include "SilvervineUE4LuaCodeGen_Game.g.inl"` 인클루드 코드를 추가합니다.

3. StartupModule() 함수에 아래 코드를 추가합니다.

```cpp
SUE4LUA_REGISTER_BINDINGS();
```

예를 들면,

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

----------------------------------------------------
[프로그래밍 가이드](ProgrammingGuide_ko.md)로 돌아가기