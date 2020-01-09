// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#include "LuaDefaultParameterCodeGen.h"

#include "Launch/Resources/Version.h"
#include "Misc/FileHelper.h"
#include "Misc/DefaultValueHelper.h"
#include "UObject/EnumProperty.h"
#include "UObject/TextProperty.h"


FSUE4LuaDefaultParameterCodeGen::FSUE4LuaDefaultParameterCodeGen(const FString& InOutputDirectory)
	: FSUE4LuaCodeGeneratorBase(InOutputDirectory)
{
	CodeBuilder.AppendCodeGenBanner(TEXT("--"));
	CodeBuilder.AppendLine(TEXT("local function INVTEXT(...) return ... end"));
	CodeBuilder.AppendLine();
}

FString FSUE4LuaDefaultParameterCodeGen::GetLuaDefaultParameter(const UFunction* Function, const UProperty* Param)
{
	FString Result;

	FString ParamValueString = GetDefaultParameter(Function, Param);

	if (Param && !ParamValueString.IsEmpty())
	{
		if (Param->IsA(UIntProperty::StaticClass()) || Param->IsA(UBoolProperty::StaticClass()) || Param->IsA(UFloatProperty::StaticClass()))
		{
			Result = ParamValueString;
		}
		else if (Param->IsA(UByteProperty::StaticClass()))
		{
			const UByteProperty* ByteProp = CastChecked<UByteProperty>(Param);
			const UEnum* Enum = ByteProp->Enum;
			if (Enum != NULL)
			{
				int32 Index = Enum->GetIndexByNameString(*ParamValueString);
				if (Index != INDEX_NONE)
				{
					Result = FString::FromInt(Index) + FString::Printf(TEXT(" --%s::%s"), *Enum->CppType, *ParamValueString);
				}
			}
			else
			{
				Result = ParamValueString;
			}
		}
		else if (Param->IsA(UEnumProperty::StaticClass()))
		{
			const UEnumProperty* EnumProp = CastChecked<UEnumProperty>(Param);
			const UEnum* Enum = EnumProp->GetEnum();
			if (Enum != NULL)
			{
				int32 Index = Enum->GetIndexByNameString(ParamValueString);
				if (Index != INDEX_NONE)
				{
					Result = FString::FromInt(Index) + FString::Printf(TEXT(" --%s::%s"), *Enum->CppType, *ParamValueString);
				}
			}
		}
		else if (Param->IsA(UStrProperty::StaticClass()) || Param->IsA(UNameProperty::StaticClass()))
		{
			Result = FString::Printf(TEXT("\"%s\""), *ParamValueString);
		}
		else if (Param->IsA(UTextProperty::StaticClass()))
		{
			FText TextValue;

#if 22 <= ENGINE_MINOR_VERSION
			FTextStringHelper::ReadFromBuffer(*ParamValueString, TextValue);
			FTextStringHelper::WriteToBuffer(Result, TextValue, true);
#else
			FTextStringHelper::ReadFromString(*ParamValueString, TextValue);
			FTextStringHelper::WriteToString(Result, TextValue, true);
#endif
		}
		else if (Param->IsA(UStructProperty::StaticClass()))
		{
			static FName Name_Vector2D("Vector2D");
			static FName Name_Vector("Vector");
			static FName Name_Rotator("Rotator");
			static FName Name_Color("Color");
			static FName Name_LinearColor("LinearColor");

			const UStructProperty* StructProp = CastChecked<UStructProperty>(Param);
			if (StructProp->Struct->GetFName() == Name_Vector2D)
			{
				FVector2D Vector2D;
				Vector2D.InitFromString(ParamValueString);
				Result = FString::Printf(TEXT("UE4.Vector2D.new(%f, %f)"),
					Vector2D.X, Vector2D.Y);
			}
			else if (StructProp->Struct->GetFName() == Name_Vector)
			{
				FVector Vector;
				FDefaultValueHelper::ParseVector(ParamValueString, Vector);
				Result = FString::Printf(TEXT("UE4.Vector.new(%f, %f, %f)"),
					Vector.X, Vector.Y, Vector.Z);
			}
			else if (StructProp->Struct->GetFName() == Name_Rotator)
			{
				FRotator Rotator;
				FDefaultValueHelper::ParseRotator(ParamValueString, Rotator);
				Result = FString::Printf(TEXT("UE4.Rotator.new(%f, %f, %f)"),
					Rotator.Pitch, Rotator.Yaw, Rotator.Roll);
			}
			else if (StructProp->Struct->GetFName() == Name_LinearColor)
			{
				FLinearColor LinearColor;
				if (LinearColor.InitFromString(ParamValueString))
				{
					Result = FString::Printf(TEXT("UE4.LinearColor.new(%f, %f, %f, %f)"),
						LinearColor.R, LinearColor.G, LinearColor.B, LinearColor.A);
				}
			}
			else if (StructProp->Struct->GetFName() == Name_Color)
			{
				FColor Color;
				if (FDefaultValueHelper::ParseColor(ParamValueString, Color))
				{
					Result = FString::Printf(TEXT("UE4.Color.new(%d, %d, %d, %d)"),
						Color.R, Color.G, Color.B, Color.A);
				}
			}
		}
	}

	return Result;
}

void FSUE4LuaDefaultParameterCodeGen::ExportClass(const FSUE4LuaCodeGenContext& CodeGenContext)
{
	ExportDefaultParameters(CodeGenContext.Class);
}

void FSUE4LuaDefaultParameterCodeGen::ExportDefaultParameters(const UClass* Class)
{
	const FString ClassName = Class->GetName();	
	const FString ClassNameLower = ClassName.ToLower();

	TArray<FString> BodyText;

	// 상속받은 함수는 Gen 대상에서 제외
	for (TFieldIterator<UFunction> FuncIt(Class, EFieldIteratorFlags::ExcludeSuper); FuncIt; ++FuncIt)
	{
		const UFunction* Function = *FuncIt;
		const FString FunctionName = Function->GetName();
		const FString FunctionNameLower = FunctionName.ToLower();

		bool bStartWrite = false;

		for (TFieldIterator<UProperty> ParamIt(Function); ParamIt; ++ParamIt)
		{
			const UProperty* Param = *ParamIt;
			const FString ParamName = Param->GetName();
			const FString ParamNameLower = ParamName.ToLower();

			if (IsReturnParameter(Param) || IsOutParameter(Param))
			{
				continue;
			}

			const FString ValueString = GetLuaDefaultParameter(Function, Param);

			if (!ValueString.IsEmpty())
			{
				if (!bStartWrite)
				{
					BodyText.Add(FString::Printf(TEXT("--%s"), *FunctionName));
					BodyText.Add(FString::Printf(TEXT("DefaultParameters.%s.%s = Class(DefaultParameterClass)"),
						*ClassNameLower, *FunctionNameLower));
					bStartWrite = true;
				}
				BodyText.Add(FString::Printf(TEXT("DefaultParameters.%s.%s.%s = %s"),
					*ClassNameLower, *FunctionNameLower, *ParamNameLower, *ValueString));
			}
		}
	}
	if (BodyText.Num() != 0)
	{
		CodeBuilder.AppendLine(FString::Printf(TEXT("--%s"), *ClassName));
		CodeBuilder.AppendLine(FString(TEXT("do")));
		{
			SUE4LUACODEGEN_INDENT(CodeBuilder);

			CodeBuilder.AppendLine(FString::Printf(TEXT("DefaultParameters.%s = DefaultParameters.%s or {}"), *ClassNameLower, *ClassNameLower));
			CodeBuilder.AppendLine();
			CodeBuilder.AppendLines(BodyText);
		}
		CodeBuilder.AppendLine(FString(TEXT("end")));
		CodeBuilder.AppendLine();
	}
}

void FSUE4LuaDefaultParameterCodeGen::FinishExport(const FSUE4LuaCodeGenContext& CodeGenContext)
{
	SaveIfChanged(OutputDirectory / TEXT("DefaultParameters.g.lua"), CodeBuilder.ToString());
}
