// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class SUE4LuaSampleTarget : TargetRules
{
	public SUE4LuaSampleTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "SUE4LuaSample" } );
	}
}
