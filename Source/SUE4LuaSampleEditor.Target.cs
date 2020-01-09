// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class SUE4LuaSampleEditorTarget : TargetRules
{
	public SUE4LuaSampleEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

#if UE_4_24_OR_LATER
        DefaultBuildSettings = BuildSettingsVersion.Latest;
#endif

        ExtraModuleNames.AddRange( new string[] { "SUE4LuaSample" } );
	}
}
