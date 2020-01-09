// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "CoreUObject.h"
#include "UObject/GCObject.h"

#include "LuaBindingRegistry.generated.h"


class FSUE4LuaBindingNode;
struct lua_State;

// 코드젠된 스태택 바인딩 함수 타입
typedef int32(*SUE4LuaStaticBindingHandler)(lua_State* L, void* Self);

// 바인딩을 등록할 때 사용하는 매크로
// 게임 모듈을 정의하는 cpp에 "SilvervineUE4LuaCodeGen_Game.g.inl"를 인클루드한 후에 다음 매크로 함수를 호출합니다.
//
// 예:
//	#include "SilvervineUE4LuaCodeGen_Game.g.inl"
//	void FMyGameModule::StartupModule()
//	{
//		SUE4LUA_REGISTER_BINDINGS();
//	}
#define SUE4LUA_REGISTER_BINDINGS()												\
	{																			\
		FSUE4LuaBindingRegistry::Get().RegisterBindingsByCDO();					\
		SUE4LuaStaticBinding::RegisterStaticBindings();							\
	}

//
// UFunction이 아닌 함수들에 대한 lua 바인딩을 제공하는 클래스.
// 바인딩 대상 함수는 이름이 'Lua'로 시작해야 합니다. VerifyBindingConventions() 구현을 참고하세요.
//
UCLASS(NotBlueprintType)
class SILVERVINEUE4LUA_API USUE4LuaBinding : public UObject
{
	GENERATED_BODY()
	
public:
	// BeginUObject Interface
	virtual void PostInitProperties() override;
	// End UObject Interface

	// 바인딩 대상 타입을 반환
	virtual UClass* GetBindingClass() const { return nullptr; }
};

//
// FSUE4LuaBinding 등록 및 타입 계층구조를 관리한다.
//
class FSUE4LuaBindingRegistry
{
public:
	FSUE4LuaBindingRegistry();
	~FSUE4LuaBindingRegistry();

	SILVERVINEUE4LUA_API static FSUE4LuaBindingRegistry& Get();

	// 주어진 클래스에 대한 lua 바인딩을 등록합니다.
	// 기존에 등록된 바인딩이 있으면 덮어씁니다.
	SILVERVINEUE4LUA_API void Register(USUE4LuaBinding* Binding, bool bCDO = false);

	// CDO에 의해서 등록된 바인딩을 처리합니다.
	// 외부 모듈에서 바인딩 클래스를 구현했으면 이 함수를 직접 호출해야 합니다.
	SILVERVINEUE4LUA_API void RegisterBindingsByCDO();

	// 스태틱 바인딩을 활성화 합니다.
	bool IsStaticBindingEnabled() const					{ return bStaticBindingEnabled; }
	void SetStaticBindingEnabled(bool bEnabled);

public:
	//
	// 아래는 내부 구현에만 사용됩니다.
	//

	const UFunction* FindFunctionByName(const UClass* InClass, FName InFunctionName) const;
	SUE4LuaStaticBindingHandler FindPropertyGetterStaticBindingHandler(const UProperty* Property) const;
	SUE4LuaStaticBindingHandler FindPropertySetterStaticBindingHandler(const UProperty* Property) const;
	SUE4LuaStaticBindingHandler FindFunctionStaticBindingHandler(const UFunction* Function) const;

	void AddReferencedObjects(FReferenceCollector& InCollector);
	SILVERVINEUE4LUA_API void RegisterPropertyStaticBinding(const class UClass* InClass, const TCHAR* InPropertyName, SUE4LuaStaticBindingHandler InGetterHandler, SUE4LuaStaticBindingHandler InSetterHandler);
	SILVERVINEUE4LUA_API void RegisterFunctionStaticBinding(const class UClass* InClass, const TCHAR* InFunctionName, SUE4LuaStaticBindingHandler InHandler);

	// 등록된 바인딩을 모두 반환합니다. 순서는 pre-order.
	void GetAllBindings(TArray<USUE4LuaBinding*>& OutBindings) const;

	// 바인딩 객체를 강제로 제거합니다. 유닛테스트 등의 개발 코드에서만 사용할 것
	void PurgeBinding(USUE4LuaBinding* Binding);

private:
	bool VerifyBindingConventions(USUE4LuaBinding* Binding) const;

	FSUE4LuaBindingNode* FindNode(USUE4LuaBinding* Binding) const;

private:
	// 스태틱 바인딩이 활성화됨
	// USUE4LuaSettings의 bEnableStaticBinding가 변경되면 이 값도 바뀝니다.
	bool bStaticBindingEnabled = false;

	// 사용하고 있는 UObject들이 GC되지 않도록 합니다.
	TUniquePtr<class FSUE4LuaBindingObjectReferencer> BindingObjectReferencer;

	// 등록된 타입 계층 구조
	FSUE4LuaBindingNode* RootBindingNode = nullptr;

	// 생성한 바인딩 노드들
	TArray<FSUE4LuaBindingNode*> AllBindingNodes;

	// 등록 대기중인 바인딩 객체들
	TArray<USUE4LuaBinding*> DeferredBindings;

	// 등록된 속성 스태틱 바인딩 함수들
	TMap<const UProperty*, TTuple<SUE4LuaStaticBindingHandler, SUE4LuaStaticBindingHandler>> PropertyStaticBindingHandlers;

	// 등록된 함수 스태틱 바인딩 함수들
	TMap<const UFunction*, SUE4LuaStaticBindingHandler> FunctionStaticBindingHandlers;
};