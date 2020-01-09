// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaStaticBindingCodeGen.h"

#include "CoreUObject.h"
#include "Misc/FileHelper.h"
#include "Misc/DefaultValueHelper.h"


FSUE4LuaStaticBindingCodeGen::FSUE4LuaStaticBindingCodeGen(const FString& InOutputDirectory, bool bInEngineRuntime)
	: FSUE4LuaCodeGeneratorBase(InOutputDirectory)
	, bEngineRuntime(bInEngineRuntime)
{
	HeaderCodeBuilder.AppendCodeGenBanner(TEXT("//"));
	HeaderCodeBuilder.AppendLine(TEXT("#include \"LuaBindingRegistry.h\""));
	HeaderCodeBuilder.AppendLine(TEXT("#include \"LuaStaticBinding.h\""));
	HeaderCodeBuilder.AppendLine();

	if (bEngineRuntime)
	{
		OutputFilename = TEXT("SilvervineUE4LuaCodeGen_Engine.g.inl");
	}
	else
	{
		OutputFilename = TEXT("SilvervineUE4LuaCodeGen_Game.g.inl");
	}

	// RegisterStaticBindings() 함수 생성 시작
	{
		FooterCodeBuilder.AppendLine(TEXT("namespace SUE4LuaStaticBinding"));
		FooterCodeBuilder.AppendLine(TEXT("{"));
		{
			FooterCodeBuilder.Indent();

			FooterCodeBuilder.AppendLine(TEXT("static TFunction<void(void)> RegisterStaticBindings = [](void) -> void"));
			FooterCodeBuilder.AppendLine(TEXT("{"));
			{
				FooterCodeBuilder.Indent();
			}
		}
	}
}

bool FSUE4LuaStaticBindingCodeGen::IsSupportedProperty(const FSUE4LuaCodeGenContext& CodeGenContext, const UProperty* Property)
{
	const bool bDeprecated =
		Property->HasMetaData(TEXT("DeprecatedProperty")) ||
		Property->HasMetaData(TEXT("DeprecationMessage")) ||
		Property->GetCPPType().Find(TEXT("DEPRECATED")) != INDEX_NONE ||
		Property->HasAnyPropertyFlags(CPF_Deprecated);

	if (bDeprecated)
	{
		return false;
	}

	const bool bPublic = Property->HasAnyPropertyFlags(CPF_NativeAccessSpecifierPublic);
	const bool bEditorOnly = Property->HasAnyPropertyFlags(CPF_EditorOnly);

	if (!bPublic || bEditorOnly)
	{
		return false;
	}

	const bool bFixedArray = 1 < Property->ArrayDim;

	if (bFixedArray)
	{
		return false;
	}

	// 값타입과 UObject*만 익스포트할 수 있습니다.
	const bool bNumeric = Property->GetClass()->HasAnyCastFlag(CASTCLASS_UNumericProperty);
	const bool bBool = Property->GetClass()->HasAnyCastFlag(CASTCLASS_UBoolProperty);
	const bool bEnum = Property->GetClass()->HasAnyCastFlag(CASTCLASS_UEnum);
	const bool bString = Property->GetClass()->HasAnyCastFlag(CASTCLASS_UNameProperty | CASTCLASS_UStrProperty | CASTCLASS_UTextProperty);

	if (bNumeric || bBool || bEnum || bString)
	{
		return true;
	}
	else if (auto ObjectProperty = Cast<UObjectPropertyBase>(Property))
	{
		const bool bDllExported = ObjectProperty->PropertyClass->HasAnyClassFlags(CLASS_RequiredAPI | CLASS_MinimalAPI);

		if (bDllExported)
		{
			return true;
		}
	}
	else if (auto StructProperty = Cast<UStructProperty>(Property))
	{
		// FVector 같은 구조체가 아니면 참조 및 소유자 관리가 어려워서 다이나믹 바인딩을 사용합니다.
		const bool bStructParamNoExport = StructProperty->Struct->StructFlags & STRUCT_NoExport;
		if (bStructParamNoExport)
		{
			return true;
		}
	}

	return false;
}

bool FSUE4LuaStaticBindingCodeGen::IsSupportedParameter(const FSUE4LuaCodeGenContext& CodeGenContext, const UProperty* Param)
{
	const bool bInterface = Param->GetClass()->HasAnyCastFlag(CASTCLASS_UInterfaceProperty);
	const bool bDelegate = Param->GetClass()->HasAnyCastFlag(CASTCLASS_UDelegateProperty | CASTCLASS_UMulticastDelegateProperty);
	const bool bContainer = Param->GetClass()->HasAnyCastFlag(CASTCLASS_UArrayProperty | CASTCLASS_UMapProperty | CASTCLASS_USetProperty);

	if (bInterface || bDelegate || bContainer)
	{
		return false;
	}
	
	const bool bFixedArray = 1 < Param->ArrayDim;

	if (bFixedArray)
	{
		return false;
	}

	return true;
}

bool FSUE4LuaStaticBindingCodeGen::IsSupportedFunction(const FSUE4LuaCodeGenContext& CodeGenContext, const UFunction* Function)
{
	const bool bPublic = Function->HasAnyFunctionFlags(FUNC_Public);
	const bool bDelegate = Function->HasAnyFunctionFlags(FUNC_Delegate);
	const bool bRequiredAPI = Function->HasAllFunctionFlags(FUNC_RequiredAPI);
	const bool bEditorOnly = Function->HasAllFunctionFlags(FUNC_EditorOnly);

	if (!bPublic || bDelegate || bEditorOnly)
	{
		return false;
	}

	// MinimalAPI인 클래스는 dll로 익스포트하는 함수만 처리 가능합니다(링크 에러남).
	if (CodeGenContext.Class->HasAnyClassFlags(CLASS_MinimalAPI))
	{
		if (!bRequiredAPI)
		{
			return false;
		}
	}

	const bool bDeprecated = Function->HasMetaData(TEXT("DeprecatedFunction"));
	const bool bCustomThunk = Function->HasMetaData(TEXT("CustomThunk"));
	const bool bNativeBreakFunc = Function->HasMetaData(TEXT("NativeBreakFunc"));
	const bool bNativeMakeFunc = Function->HasMetaData(TEXT("NativeMakeFunc"));
	if (bDeprecated || bCustomThunk || bNativeBreakFunc || bNativeMakeFunc)
	{
		return false;
	}

	for (TFieldIterator<UProperty> ParamIt(Function); ParamIt; ++ParamIt)
	{
		const UProperty* Param = *ParamIt;

		if (IsParameter(Param))
		{
			if (!IsSupportedParameter(CodeGenContext, Param))
			{
				return false;
			}

			// UStruct면 생성자, 파괴자가 dll로 익스포트 되는지 확인해야 합니다.
			if (auto StructParam = Cast<UStructProperty>(Param))
			{
				const bool bStructParamNoExport = StructParam->Struct->StructFlags & STRUCT_NoExport;
				const bool bStructParamRequiredAPI = StructParam->Struct->StructFlags & STRUCT_RequiredAPI;

				if (!bStructParamNoExport && !bStructParamRequiredAPI)
				{
					return false;
				}
			}
		}
	}

	return true;
}

FString FSUE4LuaStaticBindingCodeGen::GetPropertyCppType(const UProperty* Property)
{
	FString CppType = Property->GetCPPType(nullptr, CPPF_ArgumentOrReturnValue).TrimStartAndEnd();

	if (Property->IsA<UObjectPropertyBase>())
	{
		// TSubclassOf<T>를 UClass*로 교체
		if (CppType.StartsWith(TEXT("TSubclassOf")))
		{
			CppType = TEXT("UClass*");
		}

		// TSoftClassPtr<T>를 UClass*로 교체
		if (CppType.StartsWith(TEXT("TSoftClassPtr")))
		{
			CppType = TEXT("UClass*");
		}

		// WeakObjectPtr, LazyObjectPtr, SoftObjectPtr<T>를 T*로 교체
		if (CppType.Find(TEXT("WeakObjectPtr")) == 1 || CppType.Find(TEXT("LazyObjectPtr")) == 1 || CppType.Find(TEXT("SoftObjectPtr")) == 1)
		{
			static_assert(sizeof("WeakObjectPtr") == 14, "");
			static_assert(sizeof("LazyObjectPtr") == 14, "");
			static_assert(sizeof("SoftObjectPtr") == 14, "");

			CppType = CppType.Mid(15, CppType.Len() - 16) + TEXT("*");
		}
	}

	return CppType;
}

FSUE4LuaStaticBindingCodeGen::FFunctionArgInfo FSUE4LuaStaticBindingCodeGen::GetFunctionArgInfo(const FSUE4LuaCodeGenContext& CodeGenContext, const UFunction* Function, const UProperty* Param)
{
	FFunctionArgInfo OutInfo;

	OutInfo.Param = Param;
	OutInfo.Name = Param->GetName();
	OutInfo.GenName = TEXT("PARAM_") + Param->GetName();
	OutInfo.CppType = GetPropertyCppType(Param);
	OutInfo.bOut = IsOutParameter(Param);

	FString DefaultParameter = GetDefaultParameter(Function, Param);

	if (!DefaultParameter.IsEmpty())
	{
		OutInfo.DefaultValue = DefaultParameter;

		if (auto NumericParam = Cast<UNumericProperty>(Param))
		{
			if (const UEnum* Enum = NumericParam->GetIntPropertyEnum())
			{
				if (Enum->GetIndexByNameString(DefaultParameter) != INDEX_NONE)
				{
					OutInfo.DefaultValue = FString::Printf(TEXT("%s::%s"), *Enum->CppType, *DefaultParameter);
				}
			}
			else if (NumericParam->IsFloatingPoint())
			{
				OutInfo.DefaultValue += TEXT("f");
			}
		}
		else if (auto EnumParam = Cast<UEnumProperty>(Param))
		{
			if (const UEnum* Enum = EnumParam->GetEnum())
			{
				if (Enum->GetIndexByNameString(DefaultParameter) != INDEX_NONE)
				{
					OutInfo.DefaultValue = FString::Printf(TEXT("%s::%s"), *Enum->CppType, *DefaultParameter);
				}
			}
		}
		else if (Param->IsA<UStrProperty>())
		{
			OutInfo.DefaultValue = FString::Printf(TEXT("TEXT(\"%s\")"), *DefaultParameter);
		}
		else if (Param->IsA<UNameProperty>())
		{
			if (FName(*DefaultParameter).IsNone())
			{
				OutInfo.DefaultValue.Empty();
			}
			else
			{
				OutInfo.DefaultValue = FString::Printf(TEXT("TEXT(\"%s\")"), *DefaultParameter);
			}
		}
		else if (auto StructParam = Cast<UStructProperty>(Param))
		{
			static FName Name_Vector("Vector");
			static FName Name_Vector2D("Vector2D");
			static FName Name_Rotator("Rotator");
			static FName Name_LinearColor("LinearColor");
			static FName Name_Color("Color");

			if (StructParam->Struct->GetFName() == Name_Vector2D)
			{
				FVector2D Value;
				if (Value.InitFromString(DefaultParameter))
				{
					OutInfo.DefaultValue = FString::Printf(TEXT("%ff, %ff"), Value.X, Value.Y);
				}
			}
			else if (StructParam->Struct->GetFName() == Name_Vector)
			{
				FVector Value;
				if (FDefaultValueHelper::ParseVector(DefaultParameter, Value))
				{
					OutInfo.DefaultValue = FString::Printf(TEXT("%ff, %ff, %ff"), Value.X, Value.Y, Value.Z);
				}
			}
			else if (StructParam->Struct->GetFName() == Name_Rotator)
			{
				FRotator Value;
				if (FDefaultValueHelper::ParseRotator(DefaultParameter, Value))
				{
					OutInfo.DefaultValue = FString::Printf(TEXT("%ff, %ff, %ff"), Value.Pitch, Value.Yaw, Value.Roll);
				}
			}
			else if (StructParam->Struct->GetFName() == Name_LinearColor)
			{
				FLinearColor Value;
				if (Value.InitFromString(DefaultParameter))
				{
					OutInfo.DefaultValue = FString::Printf(TEXT("%ff, %ff, %ff, %ff"), Value.R, Value.G, Value.B, Value.A);
				}
			}
			else if (StructParam->Struct->GetFName() == Name_Color)
			{
				FColor Value;
				if (Value.InitFromString(DefaultParameter))
				{
					OutInfo.DefaultValue = FString::Printf(TEXT("%d, %d, %d, %d"), Value.R, Value.G, Value.B, Value.A);
				}
			}
		}
	}

	return OutInfo;
}

void FSUE4LuaStaticBindingCodeGen::ExportClass(const FSUE4LuaCodeGenContext& CodeGenContext)
{
	if (!CodeGenContext.bStaticBindingEnabled)
	{
		return;
	}

	if (CodeGenContext.IsEngineRuntime() != bEngineRuntime)
	{
		return;
	}

	if (CodeGenContext.IsExcluded())
	{
		return;
	}

	// 게임 런타임일 때 UObject*가 전달되는데 익스포트할 필요 없으므로 무시
	if (CodeGenContext.IsGameRuntime() && CodeGenContext.Class == UObject::StaticClass())
	{
		return;
	}

	// dll로 익스포트되는 클래스가 아닐 경우 링크 오류가 날 수 있습니다.
	const bool bDllExported = (CodeGenContext.Class->HasAnyClassFlags(CLASS_RequiredAPI | CLASS_MinimalAPI));
	if (!bDllExported)
	{
		return;
	}

	const UClass* Class = CodeGenContext.Class;
	const FString ClassCPPName = FString::Printf(TEXT("%s%s"), Class->GetPrefixCPP(), *Class->GetName());
	
	// Header
	{
		HeaderCodeBuilder.AppendLine(FString::Printf(TEXT("#include \"%s\""), *CodeGenContext.SourceHeaderFilename));
	}

	// 익스포트할 필드가 없으면 무시
	{
		bool bHasSupportedProperty = false;
		for (TFieldIterator<UProperty> PropIt(Class, EFieldIteratorFlags::ExcludeSuper); PropIt; ++PropIt)
		{
			const UProperty* Property = *PropIt;

			if (IsSupportedProperty(CodeGenContext, Property))
			{
				bHasSupportedProperty = true;
				break;
			}
		}

		bool bHasSupportedFunction = false;
		for (TFieldIterator<UFunction> FuncIt(Class, EFieldIteratorFlags::ExcludeSuper); FuncIt; ++FuncIt)
		{
			const UFunction* Function = *FuncIt;

			if (IsSupportedFunction(CodeGenContext, Function))
			{
				bHasSupportedFunction = true;
				break;
			}
		}

		if (!bHasSupportedProperty && !bHasSupportedFunction)
		{
			return;
		}
	}

	++ExportedClassCount;

	// Body
	{
		TArray<const UProperty*> Properties;
		TArray<const UFunction*> Functions;

		BodyCodeBuilder.AppendLine(FString::Printf(TEXT("namespace SUE4LuaStaticBinding_%s"), *ClassCPPName));
		BodyCodeBuilder.AppendLine(TEXT("{"));
		{
			SUE4LUACODEGEN_INDENT(BodyCodeBuilder);

			for (TFieldIterator<UProperty> PropIt(Class, EFieldIteratorFlags::ExcludeSuper); PropIt; ++PropIt)
			{
				const UProperty* Property = *PropIt;

				if (!IsSupportedProperty(CodeGenContext, Property))
				{
					continue;
				}

				ExportProperty(CodeGenContext, ClassCPPName, Property);
				
				Properties.Add(Property);
				++ExportedPropertyCount;
			}

			// 함수 생성
			for (TFieldIterator<UFunction> FuncIt(Class, EFieldIteratorFlags::ExcludeSuper); FuncIt; ++FuncIt)
			{
				const UFunction* Function = *FuncIt;

				if (!IsSupportedFunction(CodeGenContext, Function))
				{
					continue;
				}

				ExportFunction(CodeGenContext, ClassCPPName, Function);

				Functions.Add(Function);
				++ExportedFunctionCount;
			}

			// Register() 함수 생성
			BodyCodeBuilder.AppendLine(TEXT("static void Register()"));
			BodyCodeBuilder.AppendLine(TEXT("{"));
			{
				SUE4LUACODEGEN_INDENT(BodyCodeBuilder);

				BodyCodeBuilder.AppendLine(TEXT("auto& BindingRegistry = FSUE4LuaBindingRegistry::Get();"));

				for (const auto* Property : Properties)
				{
					const FString PropertyName = Property->GetName();

					// 예: BindingRegistry.RegisterPropertyStaticBinding(AActor::StaticClass(), TEXT("MoveAxis"), &LuaGet_MoveAxis, &LuaSet_MoveAxis);
					BodyCodeBuilder.AppendLine(FString::Printf(TEXT("BindingRegistry.RegisterPropertyStaticBinding(%s::StaticClass(), TEXT(\"%s\"), &LuaGet_%s, &LuaSet_%s);"), *ClassCPPName, *PropertyName, *PropertyName, *PropertyName));
				}
			
				for (const auto* Function : Functions)
				{
					const FString FunctionName = Function->GetName();

					// 예: BindingRegistry.RegisterFunctionStaticBinding(AActor::StaticClass(), TEXT("K2_TeleportTo"), &LuaCall_K2_TeleportTo);
					BodyCodeBuilder.AppendLine(FString::Printf(TEXT("BindingRegistry.RegisterFunctionStaticBinding(%s::StaticClass(), TEXT(\"%s\"), &LuaCall_%s);"), *ClassCPPName, *FunctionName, *FunctionName));
				}
			}
			BodyCodeBuilder.AppendLine(TEXT("}"));
		}
		BodyCodeBuilder.AppendLine(TEXT("}"));
	}

	// Footer
	{
		// 예: SUE4LuaStaticBinding_UMovementComponent::Register();
		FooterCodeBuilder.AppendLine(FString::Printf(TEXT("SUE4LuaStaticBinding_%s::Register();"), *ClassCPPName));
	}
}

void FSUE4LuaStaticBindingCodeGen::ExportProperty(const FSUE4LuaCodeGenContext& CodeGenContext, const FString& ClassCPPName, const UProperty* Property)
{
	const FString PropertyName = Property->GetName();
	const FString PropertyCPPName = Property->GetNameCPP();

	// Getter 생성
	{
		// 예: static int32 LuaGet_bHidden(lua_State* L, void* Self)
		BodyCodeBuilder.AppendLine(FString::Printf(TEXT("static int32 LuaGet_%s(lua_State* L, void* Self)"), *PropertyName));
		BodyCodeBuilder.AppendLine(TEXT("{"));
		{
			SUE4LUACODEGEN_INDENT(BodyCodeBuilder);

			// 예: AActor* CastedSelf = (AActor*)Self;
			BodyCodeBuilder.AppendLine(FString::Printf(TEXT("%s* CastedSelf = (%s*)Self;"), *ClassCPPName, *ClassCPPName));

			FString PropertyCode = FString::Printf(TEXT("CastedSelf->%s"), *PropertyCPPName);
			if (IsUObjectWrapper(Property))
			{
				PropertyCode += TEXT(".Get()");
			}

			if (Property->IsA<UObjectPropertyBase>())
			{
				// 예: FSUE4LuaStack::Push(L, (const UObject*)(CastedSelf->ParentActor));
				BodyCodeBuilder.AppendLine(FString::Printf(TEXT("FSUE4LuaStack::Push(L, (const UObject*)(%s));"), *PropertyCode));
			}
			else
			{
				// 예: FSUE4LuaStack::Push(L, CastedSelf->MoveAxis);
				BodyCodeBuilder.AppendLine(FString::Printf(TEXT("FSUE4LuaStack::Push(L, %s);"), *PropertyCode));
			}

			BodyCodeBuilder.AppendLine(FString::Printf(TEXT("return 1;")));
		}
		BodyCodeBuilder.AppendLine(TEXT("}"));
	}

	// Setter 생성
	{
		// 예: static int32 LuaSet_bHidden(lua_State* L, void* Self)
		BodyCodeBuilder.AppendLine(FString::Printf(TEXT("static int32 LuaSet_%s(lua_State* L, void* Self)"), *PropertyName));
		BodyCodeBuilder.AppendLine(TEXT("{"));
		{
			SUE4LUACODEGEN_INDENT(BodyCodeBuilder);

			// 예: AActor* CastedSelf = (AActor*)Self;
			BodyCodeBuilder.AppendLine(FString::Printf(TEXT("%s* CastedSelf = (%s*)Self;"), *ClassCPPName, *ClassCPPName));


			// 예: CastedSelf->MoveAxis = FSUE4LuaStack::To<FVector>(L, 3);
			BodyCodeBuilder.AppendLine(FString::Printf(TEXT("CastedSelf->%s = FSUE4LuaStack::To<%s>(L, 3);"), *PropertyCPPName, *GetPropertyCppType(Property)));

			BodyCodeBuilder.AppendLine(FString::Printf(TEXT("return 0;")));
		}
		BodyCodeBuilder.AppendLine(TEXT("}"));
	}
}

void FSUE4LuaStaticBindingCodeGen::ExportFunction(const FSUE4LuaCodeGenContext& CodeGenContext, const FString& ClassCPPName, const UFunction* Function)
{
	const FString FunctionName = Function->GetName();
	const bool bStaticFunction = Function->HasAnyFunctionFlags(EFunctionFlags::FUNC_Static);
	
	// 예: static int32 LuaCall_K2_MoveUpdatedComponent(lua_State* L, void* Self)
	BodyCodeBuilder.AppendLine(FString::Printf(TEXT("static int32 LuaCall_%s(lua_State* L, void* %s)"), *FunctionName, bStaticFunction ? TEXT("") : TEXT("Self")));
	BodyCodeBuilder.AppendLine(TEXT("{"));
	{
		SUE4LUACODEGEN_INDENT(BodyCodeBuilder);

		// 스태틱 함수가 아니면 UObject* 타입을 해당 클래스 타입으로 변환
		if (!bStaticFunction)
		{
			// 예: UMovementComponent* CastedSelf = (UMovementComponent*)Self;
			BodyCodeBuilder.AppendLine(FString::Printf(TEXT("%s* CastedSelf = (%s*)Self;"), *ClassCPPName, *ClassCPPName));
		}

		// 컨텍스트를 생성합니다. 스태틱 함수가 아니면 첫 번째 인자 self를 건너뛰어야 합니다.
		// 예: FSUE4LuaFunctionStaticBindingContext Context(L, 2, TEXT("K2_MoveUpdatedComponent"));
		BodyCodeBuilder.AppendLine(FString::Printf(TEXT("FSUE4LuaFunctionStaticBindingContext Context(L, %d, TEXT(\"%s\"));"), bStaticFunction ? 1 : 2, *FunctionName));

		// 함수 아규먼트 목록을 작성
		TArray<FFunctionArgInfo> ArgInfos;
		for (TFieldIterator<UProperty> ParamIt(Function); ParamIt; ++ParamIt)
		{
			const UProperty* Param = *ParamIt;

			if (!IsParameter(Param) || IsReturnParameter(Param))
			{
				continue;
			}

			ArgInfos.Add(GetFunctionArgInfo(CodeGenContext, Function, Param));
		}

		// 아규먼트 정의를 익스포트
		for (int32 ArgIndex = 0; ArgIndex < ArgInfos.Num(); ++ArgIndex)
		{
			ExportFunctionArg(CodeGenContext, ArgInfos[ArgIndex]);
		}

		// lua 스택에서 인자값을 가져옵니다.
		FString ParamDeclaration;
		for (int32 ArgIndex = 0; ArgIndex < ArgInfos.Num(); ++ArgIndex)
		{
			const auto& ArgInfo = ArgInfos[ArgIndex];

			// 예: Context.Get("Delta", 0, PARAM_Delta);
			BodyCodeBuilder.AppendLine(FString::Printf(TEXT("Context.Get(\"%s\", %d, %s, %s);"),
				*ArgInfo.Name,
				ArgIndex,
				*ArgInfo.GenName,
				ArgInfo.DefaultValue.IsEmpty() && !ArgInfo.bOut ? TEXT("true") : TEXT("false")));

			if (ParamDeclaration.IsEmpty())
			{
				ParamDeclaration = ArgInfo.GenName;
			}
			else
			{
				ParamDeclaration += TEXT(", ");
				ParamDeclaration += ArgInfo.GenName;
			}
		}

		// 함수 호출 
		{
			FString FunctionCallCode;
			if (bStaticFunction)
			{
				// 예: UMovementComponent::K2_MoveUpdatedComponent(PARAM_Delta, PARAM_NewRotation, PARAM_OutHit, PARAM_bSweep, PARAM_bTeleport)
				FunctionCallCode = FString::Printf(TEXT("%s::%s(%s)"), *ClassCPPName, *FunctionName, *ParamDeclaration);
			}
			else
			{
				// 예: CastedSelf->K2_MoveUpdatedComponent(PARAM_Delta, PARAM_NewRotation, PARAM_OutHit, PARAM_bSweep, PARAM_bTeleport)
				FunctionCallCode = FString::Printf(TEXT("CastedSelf->%s(%s)"), *FunctionName, *ParamDeclaration);
			}

			if (const UProperty* ReturnParam = Function->GetReturnProperty())
			{
				if (IsUObjectWrapper(ReturnParam))
				{
					FunctionCallCode += TEXT(".Get()");
				}

				BodyCodeBuilder.AppendLine(FString::Printf(TEXT("%s ReturnValue = %s;"), *GetPropertyCppType(ReturnParam), *FunctionCallCode));
			}
			else
			{
				BodyCodeBuilder.AppendLine(FString::Printf(TEXT("%s;"), *FunctionCallCode));
			}
		}

		// 가능하다면 출력값으로 lua 스택의 인자를 수정
		for (int32 ArgIndex = 0; ArgIndex < ArgInfos.Num(); ++ArgIndex)
		{
			const auto& ArgInfo = ArgInfos[ArgIndex];

			if (ArgInfo.bOut)
			{
				const TCHAR* ModifyFuncName = TEXT("ModifyIfAble");

				// 구조체 출력값은 바인딩 함수가 다름
				if (auto StructParam = Cast<UStructProperty>(ArgInfo.Param))
				{
					const bool bStructParamNoExport = StructParam->Struct->StructFlags & STRUCT_NoExport;
					if (bStructParamNoExport)
					{
						// FVector 같은 타입이 이 경우에 해당. 일반 값타입과 같이 처리.
					}
					else
					{
						ModifyFuncName = TEXT("ModifyUStructIfAble");
					}
				}

				// 예: Context.ModifyIfAble("Object", 0, PARAM_Object);
				BodyCodeBuilder.AppendLine(FString::Printf(TEXT("Context.%s(\"%s\", %d, %s);"), ModifyFuncName, *ArgInfo.Name, ArgIndex, *ArgInfo.GenName));
			}
		}

		// 반환값과 출력값을 lua 스택에 푸시
		{
			int32 ReturnValueCount = 0;

			if (auto ReturnProperty = Function->GetReturnProperty())
			{
				if (ReturnProperty->IsA<UObjectPropertyBase>())
				{
					// 예: FSUE4LuaStack::Push(L, (const UObject*)(ReturnValue));
					BodyCodeBuilder.AppendLine(TEXT("FSUE4LuaStack::Push(L, (const UObject*)(ReturnValue));"));
				}
				else
				{
					// 예: FSUE4LuaStack::Push(L, ReturnValue);
					BodyCodeBuilder.AppendLine(TEXT("FSUE4LuaStack::Push(L, ReturnValue);"));
				}

				++ReturnValueCount;
			}

			int32 ParamIndex = 0;
			for (int32 ArgIndex = 0; ArgIndex < ArgInfos.Num(); ++ArgIndex)
			{
				const auto& ArgInfo = ArgInfos[ArgIndex];

				if (ArgInfo.bOut)
				{
					const TCHAR* PushFuncName = TEXT("Push");

					if (auto ObjectProperty = Cast<UObjectPropertyBase>(ArgInfo.Param))
					{
						PushFuncName = TEXT("PushUObject");
					}
					else if (auto StructParam = Cast<UStructProperty>(ArgInfo.Param))
					{
						const bool bStructParamNoExport = StructParam->Struct->StructFlags & STRUCT_NoExport;
						if (bStructParamNoExport)
						{
							// FVector 같은 타입이 이 경우에 해당. 일반 값타입과 같이 처리.
						}
						else
						{
							PushFuncName = TEXT("PushUStruct");
						}
					}

					// 예: Context.Push("Object", 0, PARAM_Object);
					BodyCodeBuilder.AppendLine(FString::Printf(TEXT("Context.%s(\"%s\", %d, %s);"), PushFuncName, *ArgInfo.Name, ArgIndex, *ArgInfo.GenName));
					++ReturnValueCount;
				}
			}

			// 예: return 3;
			BodyCodeBuilder.AppendLine(FString::Printf(TEXT("return %d;"), ReturnValueCount));
		}
	}
	BodyCodeBuilder.AppendLine(TEXT("}"));
}

void FSUE4LuaStaticBindingCodeGen::ExportFunctionArg(const FSUE4LuaCodeGenContext& CodeGenContext, const FFunctionArgInfo& ArgInfo)
{
	// UObject*는 포인터라서 기본값이 없을 때도 특수하게 처리해야 합니다.
	if (ArgInfo.Param->IsA<UObjectPropertyBase>())
	{
		FString DefaultValue = ArgInfo.DefaultValue;

		// "", "None"은 "nullptr"로 교체
		if (ArgInfo.DefaultValue.IsEmpty() || ArgInfo.DefaultValue == TEXT("None"))
		{
			DefaultValue = TEXT("nullptr");
		}

		// 예: AActor* PARAM_Arg = nullptr;
		BodyCodeBuilder.AppendLine(FString::Printf(TEXT("%s PARAM_%s = %s;"), *ArgInfo.CppType, *ArgInfo.Name, *DefaultValue));

		return;
	}

	if (ArgInfo.DefaultValue.IsEmpty())
	{
		// 예: FVector PARAM_Arg;
		BodyCodeBuilder.AppendLine(FString::Printf(TEXT("%s PARAM_%s;"), *ArgInfo.CppType, *ArgInfo.Name));
	}
	else
	{
		// 예: EEnumType PARAM_Arg(EEnumType::Value);
		BodyCodeBuilder.AppendLine(FString::Printf(TEXT("%s PARAM_%s(%s);"), *ArgInfo.CppType, *ArgInfo.Name, *ArgInfo.DefaultValue));
	}
}

void FSUE4LuaStaticBindingCodeGen::FinishExport(const FSUE4LuaCodeGenContext& CodeGenContext)
{
	HeaderCodeBuilder.AppendLine();

	// RegisterStaticBindings 함수 생성 종료
	{
		{
			{
				FooterCodeBuilder.Unindent();
			}
			FooterCodeBuilder.AppendLine(TEXT("};"));

			FooterCodeBuilder.Unindent();
		}
		FooterCodeBuilder.AppendLine(TEXT("}"));
	}

	// DEPRECATED() 매크로 정보는 UHT가 주지 않아서 어쩔 수 없이 경고를 눌러둡니다.
	{
		HeaderCodeBuilder.AppendLine(TEXT("PRAGMA_DISABLE_DEPRECATION_WARNINGS"));
		FooterCodeBuilder.AppendLine(TEXT("PRAGMA_ENABLE_DEPRECATION_WARNINGS"));
	}

	FooterCodeBuilder.AppendLine();
	FooterCodeBuilder.AppendLine(TEXT("/*"));
	FooterCodeBuilder.AppendLine(FString::Printf(TEXT("ExportedClassCount: %d"), ExportedClassCount));
	FooterCodeBuilder.AppendLine(FString::Printf(TEXT("ExportedPropertyCount: %d"), ExportedPropertyCount));
	FooterCodeBuilder.AppendLine(FString::Printf(TEXT("ExportedFunctionCount: %d"), ExportedFunctionCount));
	FooterCodeBuilder.AppendLine(TEXT("*/"));

	FString CodeText = HeaderCodeBuilder.ToString();
	CodeText += BodyCodeBuilder.ToString();
	CodeText += FooterCodeBuilder.ToString();

	SaveIfChanged(OutputDirectory / OutputFilename, CodeText);
}