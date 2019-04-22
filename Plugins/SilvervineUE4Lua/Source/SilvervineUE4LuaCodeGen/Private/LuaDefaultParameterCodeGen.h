// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "LuaCodeGeneratorBase.h"


//
// UFUNCTION()의 파라메터 기본값을 lua 파일로 출력합니다.
//
// 생성된 파일 경로: Private/Scripts/Framework/CodeGen/DefaultParameters.g.lua
//
class FSUE4LuaDefaultParameterCodeGen : public FSUE4LuaCodeGeneratorBase
{
public:
	FSUE4LuaDefaultParameterCodeGen(const FString& InOutputDirectory);

public:
	// Begin FSUE4LuaCodeGeneratorBase Interface
	virtual void ExportClass(const FSUE4LuaCodeGenContext& CodeGenContext) override;
	virtual void FinishExport(const FSUE4LuaCodeGenContext& CodeGenContext) override;
	// End FSUE4LuaCodeGeneratorBase Interface

private:
	void ExportDefaultParameters(const UClass* Class);

	FString GetLuaDefaultParameter(const UFunction* Function, const UProperty* Param);

private:
	FSUE4LuaCodeBuilder CodeBuilder;
};