// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "LuaCodeGeneratorBase.h"


//
// 스태틱 바인딩을 지원하기 위한 코드젠 클래스
//
// * 스태틱 바인딩: c++ 바인딩 코드에 의해 lua에서 c++에 접근한다. 성능이 좋지만 컴파일 필요.
// * 다이나믹 바인딩: ue4 리플렉션을 사용해서 lua에서 c++에 접근한다. 성능이 나쁘지만 컴파일 필요없음.
//
class FSUE4LuaStaticBindingCodeGen : public FSUE4LuaCodeGeneratorBase
{
public:
	FSUE4LuaStaticBindingCodeGen(const FString& InOutputDirectory, bool bInEngineRuntime);

public:
	// Begin FSUE4LuaCodeGeneratorBase Interface
	virtual void ExportClass(const FSUE4LuaCodeGenContext& CodeGenContext) override;
	virtual void FinishExport(const FSUE4LuaCodeGenContext& CodeGenContext) override;
	// End FSUE4LuaCodeGeneratorBase Interface

private:
	static bool IsSupportedProperty(const FSUE4LuaCodeGenContext& CodeGenContext, const UProperty* Property);
	static bool IsSupportedParameter(const FSUE4LuaCodeGenContext& CodeGenContext, const UProperty* Param);
	static bool IsSupportedFunction(const FSUE4LuaCodeGenContext& CodeGenContext, const UFunction* Function);

	static FString GetPropertyCppType(const UProperty* Property);
	
	struct FFunctionArgInfo
	{
		const UProperty* Param = nullptr;
		FString Name;			// 예: Abc
		FString GenName;		// 예: PARAM_Abc
		FString CppType;		// 예: FVector
		FString DefaultValue;	// 예: 1.0f, 2.0f, 3.0f
		bool bOut;				// 출력값이면 true
	};
	static FFunctionArgInfo GetFunctionArgInfo(const FSUE4LuaCodeGenContext& CodeGenContext, const UFunction* Function, const UProperty* Param);

	void ExportFunction(const FSUE4LuaCodeGenContext& CodeGenContext, const FString& ClassCPPName, const UFunction* Function);
	void ExportProperty(const FSUE4LuaCodeGenContext& CodeGenContext, const FString& ClassCPPName, const UProperty* Property);
	void ExportFunctionArg(const FSUE4LuaCodeGenContext& CodeGenContext, const FFunctionArgInfo& ArgInfo);

private:
	// true면 엔진 런타임을 익스포트, false면 게임 런타임을 익스포트
	bool bEngineRuntime;

	FSUE4LuaCodeBuilder HeaderCodeBuilder;
	FSUE4LuaCodeBuilder BodyCodeBuilder;
	FSUE4LuaCodeBuilder FooterCodeBuilder;
	FString OutputFilename;

	int32 ExportedClassCount = 0;
	int32 ExportedPropertyCount = 0;
	int32 ExportedFunctionCount = 0;
};