// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaBindingRegistry.h"

#include "LuaAPI.h"
#include "LuaNativeValue.h"
#include "LuaLog.h"
#include "LuaStack.h"
#include "LuaUFunction.h"
#include "LuaUObject.h"
#include "LuaUProperty.h"
#include "LuaUserData.h"


// #todo: hotreload, package load,unload 등의 이벤트가 발생하면 RegisterBindingsByCDO()를 호출

void USUE4LuaBinding::PostInitProperties()
{
	Super::PostInitProperties();

	if (GetBindingClass() != nullptr && HasAnyFlags(RF_ClassDefaultObject))
	{
		FSUE4LuaBindingRegistry::Get().Register(this, true);
	}
}

class FSUE4LuaBindingObjectReferencer : public FGCObject
{
public:
	// Begin FGCObject interface
	virtual void AddReferencedObjects(FReferenceCollector& InCollector) override
	{
		FSUE4LuaBindingRegistry::Get().AddReferencedObjects(InCollector);
	}
	// End FGCObject interface
};

class FSUE4LuaBindingNode
{
public:
	FSUE4LuaBindingNode(USUE4LuaBinding* InBinding)
		: Binding(InBinding)
	{
		check(Binding != nullptr && Binding->GetBindingClass() != nullptr);

		ClassName = Binding->GetBindingClass()->GetName();
	}
	~FSUE4LuaBindingNode()
	{
	}

	FString GetClassName() const { return ClassName; }
	USUE4LuaBinding* GetBinding() const { return Binding; }

	FSUE4LuaBindingNode* GetParent() const { return Parent; }
	TArray<FSUE4LuaBindingNode*> GetChildren() const { return Children; }

	void Attach(FSUE4LuaBindingNode* InNewChild)
	{
		check(InNewChild != nullptr && InNewChild->Parent == nullptr);

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		for (auto Child : Children)
		{
			check(Child != InNewChild);
		}
#endif

		// 자식 중에서 새 노드의 자식이 될 수 있는 노드를 찾아서 옮깁니다.
		for (int32 Index = 0; Index < Children.Num(); )
		{
			auto Child = Children[Index];

			if (Child->GetBinding()->GetBindingClass()->IsChildOf(InNewChild->GetBinding()->GetBindingClass()))
			{
				Children.RemoveAt(Index);
				Child->Parent = InNewChild;
				InNewChild->Children.Add(Child);
			}
			else
			{
				++Index;
			}
		}

		InNewChild->Parent = this;
		Children.Add(InNewChild);
	}

	void Detach()
	{
		check(Parent != nullptr);

		Parent->Children.Remove(this);
		for (auto Child : Children)
		{
			Parent->Children.Add(Child);
			Child->Parent = Parent;
		}

		Parent = nullptr;
		Children.Empty();
	}

	void SetBinding(USUE4LuaBinding* InBinding)
	{
		check(InBinding != nullptr);

		if (Binding == InBinding)
		{
			return;
		}

		Binding = InBinding;

		// #todo: 변경 이벤트를 보내서 레지스트리 캐시를 제거해야 함
	}

	// 바인딩 객체가 파괴되었을 때 호출됨
	void Purge()
	{
		UE_LOG(LogSUE4L, Verbose, TEXT("Purging binding('%s')..."), *ClassName);

		Binding = nullptr;

		Detach();
	}

private:
	FString ClassName;
	USUE4LuaBinding* Binding = nullptr;
	FSUE4LuaBindingNode* Parent = nullptr;
	TArray<FSUE4LuaBindingNode*> Children;
};

FSUE4LuaBindingRegistry& FSUE4LuaBindingRegistry::Get()
{
	static FSUE4LuaBindingRegistry Instance;
	return Instance;
}

FSUE4LuaBindingRegistry::FSUE4LuaBindingRegistry()
{
	BindingObjectReferencer.Reset(new FSUE4LuaBindingObjectReferencer());
}

FSUE4LuaBindingRegistry::~FSUE4LuaBindingRegistry()
{
	for (auto BindingNode : AllBindingNodes)
	{
		delete BindingNode;
	}
	AllBindingNodes.Empty();
}

void FSUE4LuaBindingRegistry::Register(USUE4LuaBinding* InBinding, bool bInCDO)
{
	if (InBinding == nullptr || InBinding->IsPendingKill())
	{
		return;
	}

	// CDO에 의한 자동 등록이면 객체가 생성 중에 있으므로 아직 처리할 수 없습니다.
	if (bInCDO)
	{
		DeferredBindings.Add(InBinding);
		return;
	}

	UClass* NewBindingClass = InBinding->GetBindingClass();
	if (NewBindingClass == nullptr)
	{
		// 유닛테스트 중에 이렇게 될 수 있으므로 경고없이 조용히 처리합니다.
		return;
	}

	UE_LOG(LogSUE4L, Verbose, TEXT("Registering bindings(%s)..."), *NewBindingClass->GetName());

	if (!VerifyBindingConventions(InBinding))
	{
		return;
	}

	FSUE4LuaBindingNode* NewNode = new FSUE4LuaBindingNode(InBinding);
	AllBindingNodes.Add(NewNode);

	// 트리가 비어있으면 루트로 바로 설정합니다.
	// 논리적으로 새 노드는 반드시 UObject의 바인딩 객체이어야 합니다.
	if (RootBindingNode == nullptr)
	{
		check(NewBindingClass == UObject::StaticClass());

		RootBindingNode = NewNode;
		return;
	}

	// 기존 트리를 탐색하며 새 노드가 들어갈 자리를 찾습니다.
	FSUE4LuaBindingNode* ParentNode = nullptr;
	{
		TFunction<FSUE4LuaBindingNode*(FSUE4LuaBindingNode*)> FindParent = [&](FSUE4LuaBindingNode* Node) -> FSUE4LuaBindingNode*
		{
			if (Node->GetClassName() == NewNode->GetClassName())
			{
				return Node;
			}

			auto CurBinding = Node->GetBinding();
			auto CurBindingClass = CurBinding->GetBindingClass();

			check(CurBindingClass != nullptr);

			if (NewBindingClass->IsChildOf(CurBindingClass))
			{
				for (auto Child : Node->GetChildren())
				{
					if (auto Ret = FindParent(Child))
					{
						return Ret;
					}
				}

				return Node;
			}

			return nullptr;
		};
		ParentNode = FindParent(RootBindingNode);
		check(ParentNode);
	}

	if (ParentNode->GetClassName() == NewNode->GetClassName())
	{
		// 타입이 같다면 기존 노드의 바인딩만 교체합니다.
		ParentNode->SetBinding(InBinding);
	}
	else
	{
		ParentNode->Attach(NewNode);
	}
}

void FSUE4LuaBindingRegistry::RegisterBindingsByCDO()
{
	if (DeferredBindings.Num() == 0)
	{
		return;
	}

	// 루트 바인딩 노드가 없으면 우선적으로 등록합니다.
	if (RootBindingNode == nullptr)
	{
		USUE4LuaBinding* ObjectBinding = nullptr;
		for (auto Binding : DeferredBindings)
		{
			if (Binding->GetBindingClass() == UObject::StaticClass())
			{
				ObjectBinding = Binding;
				break;
			}
		}
		if (ObjectBinding == nullptr)
		{
			UE_LOG(LogSUE4L, Error, TEXT("Root Binding(UObject) was not found."));
			return;
		}

		Register(ObjectBinding);

		for (int32 Index = 0; Index < DeferredBindings.Num(); ++Index)
		{
			if (DeferredBindings[Index] == ObjectBinding)
			{
				DeferredBindings.RemoveAtSwap(Index);
				break;
			}
		}
	}

	for (auto Binding : DeferredBindings)
	{
		Register(Binding);
	}

	DeferredBindings.Empty();
}

void FSUE4LuaBindingRegistry::SetStaticBindingEnabled(bool bEnabled)
{
	bStaticBindingEnabled = bEnabled;
}

void FSUE4LuaBindingRegistry::RegisterPropertyStaticBinding(const class UClass* InClass, const TCHAR* InPropertyName, SUE4LuaStaticBindingHandler InGetterHandler, SUE4LuaStaticBindingHandler InSetterHandler)
{
	check(InClass != nullptr);
	check(InPropertyName != nullptr);
	check(InGetterHandler != nullptr);
	check(InSetterHandler != nullptr);

	const UProperty* Property = InClass->FindPropertyByName(InPropertyName);
	if (Property == nullptr)
	{
		UE_LOG(LogSUE4L, Error, TEXT("Property '%s' is not found in class '%s'."), InPropertyName, *InClass->GetName());
		return;
	}

	auto& RegisteredHandlers = PropertyStaticBindingHandlers.FindOrAdd(Property);
	if (RegisteredHandlers.Get<0>() != nullptr)
	{
		UE_LOG(LogSUE4L, Error, TEXT("Already registered property static binding: %s::%s"), *InClass->GetName(), InPropertyName);
		return;
	}
	check(RegisteredHandlers.Get<1>() == nullptr);

	RegisteredHandlers.Get<0>() = InGetterHandler;
	RegisteredHandlers.Get<1>() = InSetterHandler;
}

void FSUE4LuaBindingRegistry::RegisterFunctionStaticBinding(const class UClass* InClass, const TCHAR* InFunctionName, SUE4LuaStaticBindingHandler InHandler)
{
	check(InClass != nullptr);
	check(InFunctionName != nullptr);
	check(InHandler != nullptr);

	const UFunction* Function = InClass->FindFunctionByName(InFunctionName, EIncludeSuperFlag::ExcludeSuper);
	if (Function == nullptr)
	{
		UE_LOG(LogSUE4L, Error, TEXT("Function '%s' is not found in class '%s'."), InFunctionName, *InClass->GetName());
		return;
	}

	SUE4LuaStaticBindingHandler& RegisteredHandler = FunctionStaticBindingHandlers.FindOrAdd(Function);
	if (RegisteredHandler != nullptr)
	{
		UE_LOG(LogSUE4L, Error, TEXT("Already registered function static binding: %s::%s"), *InClass->GetName(), InFunctionName);
		return;
	}

	RegisteredHandler = InHandler;
}

const UFunction* FSUE4LuaBindingRegistry::FindFunctionByName(const UClass* InClass, FName InFunctionName) const
{
	// 주어진 클래스의 바인딩 노드를 찾습니다.
	if (RootBindingNode != nullptr)
	{
		TFunction<FSUE4LuaBindingNode*(FSUE4LuaBindingNode*)> FindBindingNode = [&](FSUE4LuaBindingNode* Node) -> FSUE4LuaBindingNode*
		{
			auto CurBinding = Node->GetBinding();
			auto CurBindingClass = CurBinding->GetBindingClass();

			check(CurBindingClass != nullptr);

			if (InClass == CurBindingClass)
			{
				return Node;
			}

			if (InClass->IsChildOf(CurBindingClass))
			{
				for (auto Child : Node->GetChildren())
				{
					if (auto Ret = FindBindingNode(Child))
					{
						return Ret;
					}
				}

				return Node;
			}

			return nullptr;
		};

		FSUE4LuaBindingNode* BindingNode = FindBindingNode(RootBindingNode);
		check(BindingNode);

		FName BindingFunctionNamesToFind[2] =
		{
			FName(*FString::Printf(TEXT("Lua%s"), *InFunctionName.ToString())),
			FName(*FString::Printf(TEXT("LuaStatic%s"), *InFunctionName.ToString())),
		};

		const UFunction* BindingFunction = nullptr;
		while (BindingNode != nullptr)
		{
			for (int32 i = 0; i < 2; ++i)
			{
				BindingFunction = BindingNode->GetBinding()->GetClass()->FindFunctionByName(BindingFunctionNamesToFind[i]);
				if (BindingFunction != nullptr)
				{
					return BindingFunction;
				}
			}

			BindingNode = BindingNode->GetParent();
		}
	}

	// 바인딩 트리에는 해당 이름의 함수가 없으므로 UClass에서 찾습니다.
	UFunction* ClassFunction = InClass->FindFunctionByName(InFunctionName);

	// 찾는 함수가 없다면 K2_를 붙여서 찾아봅니다(예: K2_GetActorLocation)
	if (ClassFunction == nullptr)
	{
		static const FString K2Prefix(TEXT("K2_"));
		FString K2FunctionName = K2Prefix + InFunctionName.ToString();

		ClassFunction = InClass->FindFunctionByName(FName(*K2FunctionName));
	}

	return ClassFunction;
}

SUE4LuaStaticBindingHandler FSUE4LuaBindingRegistry::FindFunctionStaticBindingHandler(const UFunction* Function) const
{
	if (bStaticBindingEnabled)
	{
		const SUE4LuaStaticBindingHandler* Handler = FunctionStaticBindingHandlers.Find(Function);
		if (Handler != nullptr)
		{
			return *Handler;
		}
	}

	return nullptr;
}

SUE4LuaStaticBindingHandler FSUE4LuaBindingRegistry::FindPropertyGetterStaticBindingHandler(const UProperty* Property) const
{
	if (bStaticBindingEnabled)
	{
		const auto* Handlers = PropertyStaticBindingHandlers.Find(Property);
		if (Handlers != nullptr)
		{
			return (*Handlers).Get<0>();
		}
	}

	return nullptr;
}

SUE4LuaStaticBindingHandler FSUE4LuaBindingRegistry::FindPropertySetterStaticBindingHandler(const UProperty* Property) const
{
	if (bStaticBindingEnabled)
	{
		const auto* Handlers = PropertyStaticBindingHandlers.Find(Property);
		if (Handlers != nullptr)
		{
			return (*Handlers).Get<1>();
		}
	}

	return nullptr;
}

void FSUE4LuaBindingRegistry::AddReferencedObjects(FReferenceCollector& InCollector)
{
	// 지연된 등록 처리
	for (int32 Index = DeferredBindings.Num() - 1; 0 <= Index; --Index)
	{
		auto Binding = DeferredBindings[Index];

		InCollector.AddReferencedObject(Binding);
		if (Binding == nullptr)
		{
			// 파괴되었으므로 등록하지 않습니다.
			DeferredBindings.RemoveAt(Index);
		}
	}

	if (RootBindingNode != nullptr)
	{
		TFunction<void(FSUE4LuaBindingNode*)> Traverse = [&](FSUE4LuaBindingNode* Node)
		{
			// 노드가 파되될 수 있으므로 사본을 만들고 먼저 처리
			TArray<FSUE4LuaBindingNode*> Children = Node->GetChildren();
			for (auto Child : Children)
			{
				Traverse(Child);
			}

			auto Binding = Node->GetBinding();

			InCollector.AddReferencedObject(Binding);
			if (Binding == nullptr)
			{
				// 파괴되었으므로 바인딩 객체를 버려야 합니다.
				Node->Purge();
			}
		};
		Traverse(RootBindingNode);
	}
}

void FSUE4LuaBindingRegistry::GetAllBindings(TArray<USUE4LuaBinding*>& OutBindings) const
{
	if (RootBindingNode != nullptr)
	{
		TFunction<void(FSUE4LuaBindingNode*)> Traverse = [&](FSUE4LuaBindingNode* Node)
		{
			OutBindings.Add(Node->GetBinding());

			for (auto Child : Node->GetChildren())
			{
				Traverse(Child);
			}
		};
		Traverse(RootBindingNode);
	}
}

void FSUE4LuaBindingRegistry::PurgeBinding(USUE4LuaBinding* Binding)
{
	auto Node = FindNode(Binding);
	if (Node != nullptr)
	{
		Node->Purge();
	}
}

bool FSUE4LuaBindingRegistry::VerifyBindingConventions(USUE4LuaBinding* Binding) const
{
	UClass* Class = Binding->GetClass();
	UClass* BindingClass = Binding->GetBindingClass();

	if (BindingClass == nullptr)
	{
		// 바인딩 클래스가 null이면 조용히 무시합니다.
		return true;
	}

	UE_LOG(LogSUE4L, Verbose, TEXT("Verifying binding conventions: %s(%s)..."), *Class->GetName(), *BindingClass->GetName());

	bool bValid = true;

	for (TFieldIterator<UFunction> FuncIt(Class); FuncIt; ++FuncIt)
	{
		const UFunction* Function = *FuncIt;
		const FString FunctionName = Function->GetName();

		if (!Function->GetOwnerClass()->IsChildOf(USUE4LuaBinding::StaticClass()))
		{
			continue;
		}

		const UProperty* FirstParamProperty = nullptr;
		for (TFieldIterator<UProperty> FuncParamIt(Function); FuncParamIt; ++FuncParamIt)
		{
			const UProperty* ParamProperty = *FuncParamIt;			

			if (ParamProperty->HasAnyPropertyFlags(CPF_ReturnParm))
			{
				continue;
			}

			if (FirstParamProperty == nullptr)
			{
				FirstParamProperty = ParamProperty;
				break;
			}
		}

		// 규칙: 이름이 'Lua'로 시작해야 합니다.
		if (!FunctionName.StartsWith(TEXT("Lua")))
		{
			UE_LOG(LogSUE4L, Error, TEXT("U%s::%s: Name must start with 'Lua'."), *Class->GetName(), *FunctionName);
			bValid = false;
		}

		// 규칙: 바인딩 함수는 static으로 선언되어 있어야 합니다.
		if (!Function->HasAnyFunctionFlags(EFunctionFlags::FUNC_Static))
		{
			UE_LOG(LogSUE4L, Error, TEXT("U%s::%s: Function must be declared static."), *Class->GetName(), *FunctionName);
			bValid = false;
		}

		// 규칙:	이름이 'LuaStatic'으로 시작하면 바인딩 대상 함수가 정적 함수임을 의미합니다.
		//		그렇지 않은 경우, 첫번째 인자의 타입과 이름은 반드시 바인딩클래스 포인터형의 'InSelf'이어야 합니다.
		//		예: USceneComponent* InSelf
		const bool bLuaStatic = FunctionName.StartsWith(TEXT("LuaStatic"));
		const bool bFirstParamTypeIsUObject = FirstParamProperty != nullptr && FirstParamProperty->IsA<UObjectProperty>();
		const bool bFirstParamTypeIsValid = 
			FirstParamProperty != nullptr &&
			FirstParamProperty->IsA<UObjectProperty>() &&
			Cast<UObjectProperty>(FirstParamProperty)->PropertyClass == BindingClass;
		const bool bFirstParamNameIsSelf = FirstParamProperty != nullptr && FirstParamProperty->GetName().StartsWith(TEXT("InSelf"));
		const bool bHasSelfParam = bFirstParamTypeIsUObject && bFirstParamNameIsSelf;
		if (bLuaStatic && bHasSelfParam)
		{
			UE_LOG(LogSUE4L, Error, TEXT("U%s::%s: static binding must not have 'InSelf' parameter."), *Class->GetName(), *FunctionName);
			bValid = false;
		}
		if (!bLuaStatic)
		{
			if (!bHasSelfParam)
			{
				UE_LOG(LogSUE4L, Error, TEXT("U%s::%s: non-static binding must have 'InSelf' as a first parameter."), *Class->GetName(), *FunctionName);
				bValid = false;
			}
			if (bHasSelfParam && !bFirstParamTypeIsValid)
			{
				UE_LOG(LogSUE4L, Error, TEXT("U%s::%s: 'InSelf' parameter type must be the binding type(%s)."), *Class->GetName(), *FunctionName, *BindingClass->GetName());
				bValid = false;
			}
		}

		// 규칙: 이름이 같은 정적함수와 멤버함수가 있어서는 안된다.
		if (bLuaStatic && Class->FindFunctionByName(*FString::Printf(TEXT("Lua%s"), *FunctionName.Mid(9))))
		{
			UE_LOG(LogSUE4L, Error, TEXT("U%s::%s: non-static binding with the same name exists."), *Class->GetName(), *FunctionName);
			bValid = false;
		}
	}

	return bValid;
}

FSUE4LuaBindingNode* FSUE4LuaBindingRegistry::FindNode(USUE4LuaBinding* Binding) const
{
	TFunction<FSUE4LuaBindingNode*(FSUE4LuaBindingNode*)> Find = [&](FSUE4LuaBindingNode* Node) -> FSUE4LuaBindingNode*
	{
		if (Node->GetBinding() == Binding)
		{
			return Node;
		}

		for (auto Child : Node->GetChildren())
		{
			if (auto Ret = Find(Child))
			{
				return Ret;
			}
		}

		return nullptr;
	};
	return RootBindingNode != nullptr ? Find(RootBindingNode) : nullptr;
}