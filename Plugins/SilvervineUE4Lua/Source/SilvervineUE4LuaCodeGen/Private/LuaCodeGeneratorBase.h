// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "CoreUObject.h"

#define SUE4LUACODEGEN_INDENT_CONCATENATE(a, b)	a##b
#define SUE4LUACODEGEN_INDENT_INTERNAL(Line)	FSUE4LuaCodeBuilder::FIndentHelper SUE4LUACODEGEN_INDENT_CONCATENATE(__IndentHelper, Line)
#define SUE4LUACODEGEN_INDENT(CodeBuilder)		SUE4LUACODEGEN_INDENT_INTERNAL(__LINE__)(CodeBuilder)


//
// 코드 스트링 생성을 쉽게 하기 위한 헬퍼 클래스
//
class FSUE4LuaCodeBuilder
{
public:
	FSUE4LuaCodeBuilder()
		: IndentCount(0)
	{
	}

	void Indent()
	{
		++IndentCount;
	}

	void Unindent()
	{
		--IndentCount;
	}

	struct FIndentHelper
	{
		FIndentHelper(FSUE4LuaCodeBuilder& InCodeBuilder)
			: CodeBuilder(InCodeBuilder)
		{
			CodeBuilder.Indent();
		}
		~FIndentHelper()
		{
			CodeBuilder.Unindent();
		}

		FSUE4LuaCodeBuilder& CodeBuilder;
	};

	void AppendLine(const TCHAR* StringPtr = nullptr)
	{
		for (int32 Index = 0; Index < IndentCount; Index++)
		{
			Code += TEXT("    ");
		}
		if (StringPtr != nullptr)
		{
			Code += StringPtr;
		}
		Code += LINE_TERMINATOR;
	}

	void AppendLine(const FString& String)
	{
		AppendLine(*String);
	}

	void AppendLine(const FName& Name)
	{
		AppendLine(Name.ToString());
	}

	void AppendLine(const FText& Text)
	{
		AppendLine(Text.ToString());
	}

	void AppendLines(const TArray<FString>& Lines)
	{
		for (const FString& Line : Lines)
		{
			AppendLine(Line);
		}	
	}

	void AppendCodeGenBanner(const FString& InLineComment);

	void Clear()
	{
		Code.Empty();
	}

	bool IsEmpty() const
	{
		return Code.IsEmpty();
	}

	FString ToString() const
	{
		return Code;
	}

private:
	FString Code;
	int32 IndentCount;
};


//
//
// 익스포트 현재 상태를 저장하고 있는 객체
//
struct FSUE4LuaCodeGenContext
{
	// 이번에 익스포트할 클래스
	const UClass* Class;
	// 익스포트할 클래스가 포함된 모듈
	FString ModuleName;
	// 모듈의 인클루드 기본 경로 
	FString IncludeBase;
	// 익스포트할 클래스가 선언된 헤더파일 경로
	FString SourceHeaderFilename;
	// 엔진 모듈이면 true.
	bool bEngineModule = false;
	// 플러그인이면 true.
	bool bPlugin = false;
	// 설정에서 제외시킨 모듈에 해당하면 true
	bool bExcludedModule = false;
	// 설정에서 포함시킨 플러그인에 해당하면 true
	bool bIncludedPlugin = false;
	// 스태틱 바인딩이 활성화되어있으면 true
	bool bStaticBindingEnabled = true;
	// 지금까지 익스포트한 클래스 목록
	TSet<FName> ExportedClasses;

	// 엔진 런타임이면 true
	bool IsEngineRuntime() const
	{
		return bEngineModule;
	}

	// 게임 런타임이면 true
	bool IsGameRuntime() const
	{
		return !IsEngineRuntime();
	}

	// 제외된 모듈이면 true
	bool IsExcluded() const
	{
		if (bExcludedModule)
		{
			return true;
		}

		if (bPlugin)
		{
			return !bIncludedPlugin;
		}

		return false;
	}
};

//
// 코드젠 기본 클래스
//
class FSUE4LuaCodeGeneratorBase
{
public:
	FSUE4LuaCodeGeneratorBase(const FString& InOutputDirectory);
	virtual ~FSUE4LuaCodeGeneratorBase() {}

	static bool IsSupportedClass(const FSUE4LuaCodeGenContext& CodeGenContext);

	virtual void ExportClass(const FSUE4LuaCodeGenContext& CodeGenContext) = 0;
	virtual void FinishExport(const FSUE4LuaCodeGenContext& CodeGenContext) = 0;

protected:

	static bool SaveIfChanged(const FString& Path, const FString& NewContents);

	// TSubclass<>, TSoftObject<> 등이면 true
	static bool IsUObjectWrapper(const UProperty* Property);

	static bool IsParameter(const UProperty* Param);
	static bool IsReturnParameter(const UProperty* Param);
	static bool IsOutParameter(const UProperty* Param);
	static FString GetDefaultParameter(const UFunction* Function, const UProperty* Param);

protected:
	FString OutputDirectory;
};