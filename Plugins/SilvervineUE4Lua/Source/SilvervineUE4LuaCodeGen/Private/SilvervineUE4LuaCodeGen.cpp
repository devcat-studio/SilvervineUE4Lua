// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#include "SilvervineUE4LuaCodeGen.h"

#include "Features/IModularFeatures.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "ProjectDescriptor.h"
#include "UObject/UnrealType.h" 

#include "LuaDefaultParameterCodeGen.h"
#include "LuaStaticBindingCodeGen.h"


IMPLEMENT_MODULE(FSilvervineUE4LuaCodeGenModule, SilvervineUE4LuaCodeGen)

//
// Project Config 파일을 UHT에서 읽기 위한 Helper
// UProperty의 ImportText를 사용할 수 없어서 작성된 코드입니다.
//
namespace ConfigHelper
{
	static void SkipWhitespace(const TCHAR*& Str)
	{
		while (FChar::IsWhitespace(*Str))
		{
			Str++;
		}
	}

	const TCHAR* ImportText(const TCHAR* Buffer, FString& OutValue)
	{
		Buffer = UPropertyHelpers::ReadToken(Buffer, OutValue);
		return Buffer;
	}

	template <typename Type>
	const TCHAR* ImportText(const TCHAR* Buffer, Type& OutValue)
	{
		return nullptr;
	};

	// 구현 참조 : UMapProperty::ImportText_Internal
	template <typename KeyType, typename ValueType>
	const TCHAR* ImportText(const TCHAR* Buffer, TMap<KeyType, ValueType>& OutValue)
	{
		if (*Buffer++ != TCHAR('('))
		{
			return nullptr;
		}
		SkipWhitespace(Buffer);

		if (*Buffer == TCHAR(')'))
		{
			return nullptr;
		}

		for (;;)
		{
			bool bFind = false;

			if (*Buffer++ != TCHAR('('))
			{
				return nullptr;
			}

			SkipWhitespace(Buffer);
			// Parse the key
			KeyType Key;
			Buffer = ImportText(Buffer, Key);
			if (!Buffer)
			{
				return nullptr;
			}

			SkipWhitespace(Buffer);
			if (*Buffer++ != TCHAR(','))
			{
				return nullptr;
			}
			SkipWhitespace(Buffer);
			// Parse the value
			ValueType Value;
			Buffer = ImportText(Buffer, Value);
			if (!Buffer)
			{
				return nullptr;
			}

			OutValue.Add(Key, Value);

			SkipWhitespace(Buffer);
			if (*Buffer++ != TCHAR(')'))
			{
				return nullptr;
			}

			switch (*Buffer++)
			{
			case TCHAR(')'):
				return Buffer;
			case TCHAR(','):
				SkipWhitespace(Buffer);
				break;
			default:
				return nullptr;
			}
		}
	}
}

bool FSilvervineUE4LuaCodeGenModule::LoadConfig()
{
	// FPaths::ProjectDir() 등은 UHT 프로젝트를 기준으로 하므로 사용할 수 없습니다.
	// 대신 FPaths::GetProjectFilePath()을 사용하여 직접 경로 정보를 만들고 있습니다.
	FString ProjectPath = FPaths::GetPath(FPaths::GetProjectFilePath());
	FString PlatformName = FPlatformProperties::PlatformName();

	FString SourceConfigDir = FString::Printf(TEXT("%s/Plugins/SilvervineUE4Lua/Config/"), *ProjectPath);
	FString GeneratedConfigDir = FString::Printf(TEXT("%s/Saved/Config/"), *ProjectPath);
	
	// PluginManager가 Load해주는 ini 정보 또한 UHT 프로젝트 기준이라 사용할 수 없습니다.
	// 캐시로 저장하지 않는 임시 ConfigFile을 만들어 사용합니다.
	FConfigFile ConfigFile;
	if (!FConfigCacheIni::LoadExternalIniFile(ConfigFile, TEXT("SilvervineUE4Lua"), *FPaths::EngineConfigDir(), *SourceConfigDir, true, *PlatformName, false, true, true, *GeneratedConfigDir))
	{
		UE_LOG(LogTemp, Error, TEXT("[SUE4LuaCodeGen] Ini file is not found."), );
		return false;
	}

	// ConfigFile에서 LocalFileMap의 "SUE4Lua" 경로를 가져온다
	const FString SectionName = TEXT("/Script/SilvervineUE4Lua.SUE4LuaSettings");
	FConfigSection* ConfigSection = ConfigFile.Find(SectionName);
	if (!ConfigSection)
	{
		UE_LOG(LogTemp, Error, TEXT("[SUE4LuaCodeGen] Section %s is not found in Ini file."), *SectionName);
		return false;
	}

	// LocalFileLoadMap
	{
		const FName KeyName = TEXT("LocalFileLoadMap");
		FConfigValue* ConfigValue = ConfigSection->Find(KeyName);
		if (!ConfigValue)
		{
			UE_LOG(LogTemp, Error, TEXT("[SUE4LuaCodeGen] Value %s is not found in Ini file."), *KeyName.ToString());
			return false;
		}

		FString Value = ConfigValue->GetValue();

		TMap<FString, FString> LocalFileMap;
		ConfigHelper::ImportText(*Value, LocalFileMap);
		const FString* FilePath = LocalFileMap.Find("SUE4Lua");
		if (FilePath == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("[SUE4LuaCodeGen] CodeGen path is not found in ini file."));
			return false;
		}

		LuaCodeGenPath = FPaths::Combine(ProjectPath, *FilePath, TEXT("/Framework/CodeGen"));
		FPaths::MakeStandardFilename(LuaCodeGenPath);
	}

	// ExcludedEngineModules
	{
		const FName KeyName = TEXT("ExcludedEngineModules");
		ConfigSection->MultiFind(KeyName, ExcludedEngineModules);
	}

	// IncludedEnginePlugins
	{
		const FName KeyName = TEXT("IncludedEnginePlugins");
		ConfigSection->MultiFind(KeyName, IncludedEnginePlugins);
	}

	// IncludedGamePlugins
	{
		const FName KeyName = TEXT("IncludedGamePlugins");
		ConfigSection->MultiFind(KeyName, IncludedGamePlugins);
	}

	// bEnableStaticBinding
	{
		const FName KeyName = TEXT("bEnableStaticBinding");
		if (auto* ConfigValue = ConfigSection->Find(KeyName))
		{
			CodeGenContext.bStaticBindingEnabled = FCString::ToBool(*ConfigValue->GetValue());
		}
	}

	return true;
}

void FSilvervineUE4LuaCodeGenModule::StartupModule()
{
	IModularFeatures::Get().RegisterModularFeature(TEXT("ScriptGenerator"), this);
}

void FSilvervineUE4LuaCodeGenModule::ShutdownModule()
{
	IModularFeatures::Get().UnregisterModularFeature(TEXT("ScriptGenerator"), this);
}

FString FSilvervineUE4LuaCodeGenModule::GetGeneratedCodeModuleName() const
{
	return TEXT("SilvervineUE4Lua");
}

FString FSilvervineUE4LuaCodeGenModule::GetGeneratorName() const
{
	return TEXT("SilvervineUE4Lua Code Generator");
}

bool FSilvervineUE4LuaCodeGenModule::ShouldExportClassesForModule(const FString& ModuleName, EBuildModuleType::Type ModuleType, const FString& ModuleGeneratedIncludeDirectory) const
{
	const bool bEngineRuntime = ModuleType == EBuildModuleType::EngineRuntime;
	const bool bGameRuntime = ModuleType == EBuildModuleType::GameRuntime;

	if (bEngineRuntime || bGameRuntime)
	{
		// 엔진 코드 수정 없이 모듈 정보를 얻을 시점이 지금 뿐이라서 이렇게 해킹 스러운 방법을 사용함

		CodeGenContext.ModuleName = ModuleName;
		CodeGenContext.bPlugin = ModuleGeneratedIncludeDirectory.Contains(TEXT("/Plugins/"));
		CodeGenContext.bIncludedPlugin = false;

		if (bEngineRuntime)
		{
			CodeGenContext.bEngineModule = true;
			CodeGenContext.bExcludedModule = ExcludedEngineModules.Contains(ModuleName);
			CodeGenContext.IncludeBase = FPaths::EngineSourceDir();

			if (CodeGenContext.bPlugin)
			{
				CodeGenContext.bIncludedPlugin = IncludedEnginePlugins.Contains(ModuleName);
			}
		}
		else
		{
			CodeGenContext.bEngineModule = false;
			CodeGenContext.bExcludedModule = false;
			CodeGenContext.IncludeBase = IncludeBase;

			if (CodeGenContext.bPlugin)
			{
				CodeGenContext.bIncludedPlugin = IncludedGamePlugins.Contains(ModuleName);
			}
		}

		// SUE4Lua 모듈의 바인딩 코드가 Engine 코드젠에 포함되도록 엔진 모듈로 취급합니다.
		if (ModuleName == GetGeneratedCodeModuleName())
		{
			CodeGenContext.bEngineModule = true;
			CodeGenContext.bIncludedPlugin = true;
		}

		return true;
	}

	return false;
}

bool FSilvervineUE4LuaCodeGenModule::SupportsTarget(const FString& TargetName) const
{
	bool bSupportsTarget = false;
	if (FPaths::IsProjectFilePathSet())
	{
		FProjectDescriptor ProjectDescriptor;
		FText OutError;
		if (ProjectDescriptor.Load(FPaths::GetProjectFilePath(), OutError))
		{
			for (auto& PluginDescriptor : ProjectDescriptor.Plugins)
			{
				// .uproject에 SilvervineUE4Lua Plugin이 포함되어 있을 때만 CodeGen 합니다.
				FString ModuleName = GetGeneratedCodeModuleName();
				if (PluginDescriptor.bEnabled && PluginDescriptor.Name == ModuleName)
				{
					bSupportsTarget = true;
					break;
				}
			}
		}
	}
	return bSupportsTarget;
}

void FSilvervineUE4LuaCodeGenModule::Initialize(const FString& InRootLocalPath, const FString& InRootBuildPath, const FString& InOutputDirectory, const FString& InIncludeBase)
{
	IncludeBase = InIncludeBase;

	if (LoadConfig())
	{
		CodeGenerators.Add(MakeUnique<FSUE4LuaDefaultParameterCodeGen>(LuaCodeGenPath));
		CodeGenerators.Add(MakeUnique<FSUE4LuaStaticBindingCodeGen>(InOutputDirectory, true));
		CodeGenerators.Add(MakeUnique<FSUE4LuaStaticBindingCodeGen>(InOutputDirectory, false));
	}
}

void FSilvervineUE4LuaCodeGenModule::ExportClass(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged)
{
	if (CodeGenContext.ExportedClasses.Contains(Class->GetFName()))
	{
		return;
	}

	// 소스 헤더파일이 없는 경우는 UField, UClass 등의 코어 클래스가 대부분입니다.
	// 이 타입들은 런타임에서 거의 처리하지 않기때문에 익스포트하지 않습니다.
	if (SourceHeaderFilename.IsEmpty())
	{
		return;
	}

	FString NormalizedSourceHeaderFilename = SourceHeaderFilename;
	FPaths::NormalizeFilename(NormalizedSourceHeaderFilename); 

	// 4.22 부터 생긴 빌드 오류 문제 해결
	if (NormalizedSourceHeaderFilename.Contains(TEXT("Experimental")))
	{
		return;
	}

	CodeGenContext.Class = Class;
	CodeGenContext.SourceHeaderFilename = NormalizedSourceHeaderFilename;

	// 절대경로 형식의 SourceHeaderFilename를 상대경로로 바꿔줍니다.
	FPaths::MakePathRelativeTo(CodeGenContext.SourceHeaderFilename, *CodeGenContext.IncludeBase);

	if (FSUE4LuaCodeGeneratorBase::IsSupportedClass(CodeGenContext))
	{
		CodeGenContext.ExportedClasses.Add(Class->GetFName());

		for (auto& CodeGenerator : CodeGenerators)
		{
			CodeGenerator->ExportClass(CodeGenContext);
		}
	}
}

void FSilvervineUE4LuaCodeGenModule::FinishExport()
{
	for (auto& CodeGenerator : CodeGenerators)
	{
		CodeGenerator->FinishExport(CodeGenContext);
	}
}
