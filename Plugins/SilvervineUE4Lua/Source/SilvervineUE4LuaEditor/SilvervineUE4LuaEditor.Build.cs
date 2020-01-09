using System.IO;
using UnrealBuildTool;

namespace UnrealBuildTool.Rules
{
	public class SilvervineUE4LuaEditor : ModuleRules
	{
		public SilvervineUE4LuaEditor(ReadOnlyTargetRules Target) : base(Target)
		{
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
            PrivatePCHHeaderFile = "SilvervineUE4LuaEditorPCH.h";

            PublicIncludePaths.AddRange(new string[]
            {
            });

            PrivateIncludePaths.AddRange(new string[]
            {
            });

            PublicDependencyModuleNames.AddRange(new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "SlateCore",
                "UnrealEd",

                "SilvervineUE4Lua",
            });
        }
	}
}
