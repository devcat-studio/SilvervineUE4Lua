// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "Programs/UnrealHeaderTool/Public/IScriptGeneratorPluginInterface.h"

#include "LuaCodeGeneratorBase.h"


//
// IScriptGeneratorPluginInterface를 사용해서 다음의 두 코드를 생성하는 모듈입니다.
//	- 스태틱 바인딩을 위한 c++ 코드
//	- 함수의 인자 기본값이 저장되어 있는 lua 코드
//
// 자세한 사항은 FSUE4LuaDefaultParameterCodeGen, FSUE4LuaStaticBindingCodeGen 클래스를 참고하세요
//
class FSilvervineUE4LuaCodeGenModule : public IScriptGeneratorPluginInterface
{
public:
	// Begin IModuleInterface Interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// End IModuleInterface Interface

	// Begin IScriptGeneratorPlugin Interface
	virtual FString GetGeneratedCodeModuleName() const override;
	virtual bool ShouldExportClassesForModule(const FString& ModuleName, EBuildModuleType::Type ModuleType, const FString& ModuleGeneratedIncludeDirectory) const override;
	virtual bool SupportsTarget(const FString& TargetName) const override;
	virtual void Initialize(const FString& RootLocalPath, const FString& RootBuildPath, const FString& OutputDirectory, const FString& IncludeBase) override;
	virtual void ExportClass(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged) override;
	virtual void FinishExport() override;
	virtual FString GetGeneratorName() const override;
	// End IScriptGeneratorPlugin Interface

private:
	bool LoadConfig();

private:
	// 코드젠 결과 lua파일이 저장될 루트 경로
	FString LuaCodeGenPath;
	// 스태틱 바인딩하지 않을 엔진 모듈 목록
	TArray<FString> ExcludedEngineModules;
	// 스태틱 바인딩할 엔진 플러그인 목록
	TArray<FString> IncludedEnginePlugins;
	// 스태틱 바인딩할 게임 플러그인 목록
	TArray<FString> IncludedGamePlugins;

	FString IncludeBase;
	TArray<TUniquePtr<FSUE4LuaCodeGeneratorBase>> CodeGenerators;
	mutable FSUE4LuaCodeGenContext CodeGenContext;
};