using System.IO;
using UnrealBuildTool;

namespace UnrealBuildTool.Rules
{
	public class SilvervineUE4Lua : ModuleRules
	{
		public SilvervineUE4Lua(ReadOnlyTargetRules Target) : base(Target)
		{
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
            PrivatePCHHeaderFile = "SilvervineUE4LuaPCH.h";

            PublicIncludePaths.AddRange(new string[]
            {
            });

            PrivateIncludePaths.AddRange(new string[]
            {
                "SilvervineUE4Lua/Public",
                "SilvervineUE4Lua/ThirdParty/Lua/5.3.4/src",
            });

            PublicDependencyModuleNames.AddRange(new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
            });

            PrivateDependencyModuleNames.AddRange(new string[]
            {
                "InputCore",
                "SlateCore",
                "Slate",
                "UMG",
                "GameplayTags", // For StaticBinding
                "MovieScene",   // For StaticBinding
            });

            if (Target.bBuildEditor)
            {
                PrivateDependencyModuleNames.Add("DirectoryWatcher");
            }

            // 4.20 build error workaround
            PrivateDefinitions.Add("WIN32_LEAN_AND_MEAN");
            PrivateDefinitions.Add("__STDC_WANT_SECURE_LIB__");
        }
	}
}
