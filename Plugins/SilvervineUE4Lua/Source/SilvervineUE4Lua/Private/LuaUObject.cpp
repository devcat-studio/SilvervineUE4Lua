// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaUObject.h"

#include "CoreUObject.h"

#include "LuaAPI.h"
#include "LuaBindingRegistry.h"
#include "LuaLog.h"
#include "LuaNativeValue.h"
#include "LuaStack.h"
#include "LuaUFunction.h"
#include "LuaUProperty.h"
#include "LuaUserData.h"
#include "LuaVirtualMachine.h"


namespace SUE4LuaUObject
{
	struct FUObjectUserData : public FSUE4LuaUserData
	{
		TWeakObjectPtr<UObject> TargetUObject = nullptr;
		TRefCountPtr<FSUE4LuaUObjectValue> Owner;

		struct FCachedField
		{
			const UProperty* Property = nullptr;
			const UFunction* Function = nullptr;
			SUE4LuaStaticBindingHandler PropertyGetter = nullptr;
			SUE4LuaStaticBindingHandler PropertySetter = nullptr;
		};
		TMap<FName, FCachedField> CachedFields;

		FCachedField* FindOrAddCachedField(FName KeyName)
		{
			if (TargetUObject == nullptr)
			{
				// GC됨
				return nullptr;
			}

			FCachedField* CachedField = CachedFields.Find(KeyName);

			if (CachedField == nullptr)
			{
				CachedField = &CachedFields.Add(KeyName);

				const UClass* Class = TargetUObject->GetClass();

				// 함수부터 찾아봅니다.
				// #todo: 속성부터 찾는 것이 더 빠를 수 있으니 테스트 필요
				if (CachedField->Property == nullptr)
				{
					// UKismetSystemLibrary와 같은 타입의 함수를 간편하게 호출하기 위해서 UClass 타입만 예외적으로 자신의 함수를 찾아봅니다.
					if (auto ClassObject = Cast<UClass>(TargetUObject))
					{
						CachedField->Function = FSUE4LuaBindingRegistry::Get().FindFunctionByName(ClassObject, KeyName);
					}

					if (CachedField->Function == nullptr)
					{
						CachedField->Function = FSUE4LuaBindingRegistry::Get().FindFunctionByName(Class, KeyName);
					}
				}

				// 속성을 찾아봅니다.
				if (CachedField->Function == nullptr)
				{
					CachedField->Property = Class->FindPropertyByName(KeyName);

					// 스태틱 바인딩 함수가 있으면 사용
					if (CachedField->Property != nullptr)
					{
						CachedField->PropertyGetter = FSUE4LuaBindingRegistry::Get().FindPropertyGetterStaticBindingHandler(CachedField->Property);
						CachedField->PropertySetter = FSUE4LuaBindingRegistry::Get().FindPropertySetterStaticBindingHandler(CachedField->Property);
					}
				}
			}

			return CachedField;
		}
	};

	static bool IsUObjectUserData(lua_State* L, int32 Index)
	{
		bool bUObjectUserData = false;

		if (lua_isuserdata(L, Index))
		{
			lua_getmetatable(L, Index);
			// Stack: MT

			if (lua_istable(L, -1))
			{
				lua_getfield(L, -1, "isUObjectProxy");
				if (lua_isboolean(L, -1) && lua_toboolean(L, -1))
				{
					bUObjectUserData = true;
				}
				lua_pop(L, 1);
			}

			lua_pop(L, 1);
			// Stack: (empty)
		}

		return bUObjectUserData;
	}

	void PushRef(lua_State* L, UObject* TargetUObject)
	{
		// null이면 바로 nil을 푸시합니다.
		if (TargetUObject == nullptr)
		{
			lua_pushnil(L);
			return;
		}

		// 이미 프록시 객체가 있으면 그대로 반환
		{
			lua_getglobal(L, "SUE4LuaBinding");
			lua_getfield(L, -1, "uobjectProxies");
			lua_remove(L, -2);
			lua_pushlightuserdata(L, TargetUObject);
			lua_rawget(L, -2);
			lua_remove(L, -2);
			if (!lua_isnil(L, -1))
			{
				// UObject가 이전에 사용했던 메모리에 그대로 할당된 경우, 저장된 프록시 객체가 유효하지 않을 수 있습니다
				auto UserData = FSUE4LuaUserData::ToUserData<FUObjectUserData>(L, -1);
				if (UserData->TargetUObject.IsValid())
				{
					return;
				}
			}
			lua_pop(L, 1);
		}

		// full userdata 생성하고 UObject*를 저장
		{
			auto UserData = FSUE4LuaUserData::NewUserData<FUObjectUserData>(L);
			UserData->TargetUObject = TargetUObject;
			UserData->Owner = new FSUE4LuaUObjectValue(TargetUObject);
		}

		// 메타 테이블 설정
		{
			// Stack: TargetUObject
			lua_getglobal(L, "SUE4LuaBinding");
			lua_getfield(L, -1, "uobjectProxyMTs");
			lua_getfield(L, -1, "UObject");
			// Stack: TargetUObject, SUE4LuaBinding, uobjectProxyMTs, MT
			lua_setmetatable(L, -4);
			lua_pop(L, 2);
			// Stack: TargetUObject
		}

		// 프록시 테이블에 등록
		{
			lua_getglobal(L, "SUE4LuaBinding");
			lua_getfield(L, -1, "uobjectProxies");
			lua_remove(L, -2);
			lua_pushlightuserdata(L, TargetUObject);
			lua_pushvalue(L, -3);
			lua_rawset(L, -3);
			lua_pop(L, 1);
		}
	}

	UObject* ToRef(lua_State* L, int32 Index)
	{
		if (lua_isnil(L, Index))
		{
			return nullptr;
		}

		if (!IsUObjectUserData(L, Index))
		{
			SUE4LVM_WARNING(L, TEXT("%s: expected UObject type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));
			return nullptr;
		}

		auto UserData = FSUE4LuaUserData::ToUserData<FUObjectUserData>(L, Index);

		return UserData->TargetUObject.Get();
	}

	UObject* TryToRef(lua_State* L, int32 Index)
	{
		if (lua_isnil(L, Index))
		{
			return nullptr;
		}

		if (!IsUObjectUserData(L, Index))
		{
			return nullptr;
		}

		auto UserData = FSUE4LuaUserData::ToUserData<FUObjectUserData>(L, Index);

		return UserData->TargetUObject.Get();
	}

	UObject* PopRef(lua_State* L)
	{
		UObject* TargetUObject = ToRef(L, -1);
		
		lua_pop(L, 1);
		return TargetUObject;
	}

	// UStruct* getType(self)
	static int32 LuaGetType(lua_State* L)
	{
		if (!IsUObjectUserData(L, 1))
		{
			SUE4LVM_ERROR(L, TEXT("UObject.getType(): Invalid self"));

			lua_pushnil(L);
			return 1;
		}

		auto UserData = FSUE4LuaUserData::ToUserData<FUObjectUserData>(L, 1);

		UObject* TargetUObject = UserData->TargetUObject.Get();
		if (TargetUObject == nullptr)
		{
			// GC됨
			lua_pushnil(L);
			return 1;
		}

		SUE4LuaUObject::PushRef(L, TargetUObject->GetClass());

		return 1;
	}

	// UObject* getObjectProperty(self, PropertyName)
	// 주어진 이름의 UObject 속성을 lua 스택에 푸시합니다. USUE4LuaValue를 UObject로 스택에 푸시하고 싶을 때 사용합니다.
	static int32 LuaGetObjectProperty(lua_State* L)
	{
		if (!IsUObjectUserData(L, 1))
		{
			SUE4LVM_ERROR(L, TEXT("UObject.getObjectProperty(): Invalid self"));

			lua_pushnil(L);
			return 1;
		}

		auto UserData = FSUE4LuaUserData::ToUserData<FUObjectUserData>(L, 1);

		auto PropertyName = FSUE4LuaStack::To<FName>(L, 2);
		if (PropertyName.IsNone())
		{
			SUE4LVM_ERROR(L, TEXT("UObject.getObjectProperty(): Invalid arg2"));

			lua_pushnil(L);
			return 1;
		}

		UObject* TargetUObject = UserData->TargetUObject.Get();
		if (TargetUObject == nullptr)
		{
			// GC됨
			lua_pushnil(L);
			return 1;
		}

		const UClass* Class = TargetUObject->GetClass();

		if (const UProperty* Property = Class->FindPropertyByName(PropertyName))
		{
			if (auto ObjectProperty = Cast<UObjectPropertyBase>(Property))
			{
				PushRef(L, ObjectProperty->GetObjectPropertyValue_InContainer(TargetUObject));
			}
			else
			{
				SUE4LVM_WARNING(L, TEXT("%s(%s): expected UObject* type"), *PropertyName.ToString(), *Property->GetCPPType());

				lua_pushnil(L);
			}
		}
		
		return 1;
	}

	static int32 LuaGC(lua_State* L)
	{
		auto UserData = FSUE4LuaUserData::ToUserData<FUObjectUserData>(L, 1);
		UserData->Dispose();

		return 0;
	}

	static int32 LuaIndex(lua_State* L)
	{
		// MT에 있는 값으면 그대로 반환
		{
			// Stack: (empty)
			lua_getmetatable(L, 1);
			lua_pushvalue(L, 2);
			// Stack: MT, Key
			lua_gettable(L, -2);
			// Stack: MT, Value
			lua_remove(L, -2);
			// Stack: Value
			if (!lua_isnil(L, -1))
			{
				return 1;
			}
			lua_pop(L, 1);
			// Stack: (empty)
		}

		auto UserData = FSUE4LuaUserData::ToUserData<FUObjectUserData>(L, 1);
		FName KeyName = FSUE4LuaStack::To<FName>(L, 2);

		UObject* TargetUObject = UserData->TargetUObject.Get();
		if (TargetUObject == nullptr)
		{
			// GC됨
			return 0;
		}

		auto* CachedField = UserData->FindOrAddCachedField(KeyName);
		check(CachedField != nullptr);

		// 함수가 있으면 호출
		if (CachedField->Function != nullptr)
		{
			SUE4LuaUFunction::PushClosure(L, CachedField->Function);
			return 1;
		}

		// 속성이 있으면 호출
		if (CachedField->PropertyGetter != nullptr)
		{
			return CachedField->PropertyGetter(L, TargetUObject);
		}
		else if (CachedField->Property != nullptr)
		{
			// 생성한 프록시가 담긴 테이블을 찾습니다.
			{
				lua_getglobal(L, "SUE4LuaBinding");
				lua_getfield(L, -1, "GetUObjectProxyData");
				lua_remove(L, -2);
				lua_pushvalue(L, 1);
				lua_pcall(L, 1, 1, 0);
				// Stack: UObjectProxyData
				lua_getfield(L, -1, "upropertyProxies");
				lua_remove(L, -2);
				// Stack: upropertyProxies
			}

			// 이미 생성한 적이 있으면 그대로 반환
			{
				FSUE4LuaStack::Push(L, KeyName);
				lua_gettable(L, -2);
				// Stack: upropertyProxies, upropertyProxies[KeyName]
				if (!lua_isnil(L, -1))
				{
					lua_remove(L, -2);
					// Stack: upropertyProxies[KeyName]
					return 1;
				}
				lua_pop(L, 1);
				// Stack: upropertyProxies
			}

			SUE4LuaUProperty::Push(L, CachedField->Property, CachedField->Property->ContainerPtrToValuePtr<void>(TargetUObject), UserData->Owner);
			// Stack: upropertyProxies, propertyValue

			// 프록시를 캐시에 저장
			// UObject*는 내용이 바뀔 수 있으므로 캐시하지 않습니다.
			if (lua_isuserdata(L, -1) && !CachedField->Property->IsA<UObjectPropertyBase>())
			{
				FSUE4LuaStack::Push(L, KeyName);
				lua_pushvalue(L, -2);
				lua_settable(L, -4);
			}

			lua_remove(L, -2);
			// Stack: propertyValue
			return 1;
		}

		// LuaDirectCall: 디스패치 핸들러에서 해당 함수를 찾아서 푸시합니다.
		// lua 코드에서 디스패치 핸들러 lua 함수를 바로 호출할 수 있습니다.
		{
			if (auto VM = FSUE4LuaVirtualMachine::PinVM(L))
			{
				if (VM->PushDispatchHandler(L, TargetUObject, KeyName.ToString()))
				{
					return 1;
				}
			}
		}
		
		SUE4LVM_ERROR(L, TEXT("'%s': is not a member of '%s'."), *KeyName.ToString(), *TargetUObject->GetClass()->GetName());

		return 0;
	}

	static int32 LuaNewIndex(lua_State* L)
	{
		auto UserData = FSUE4LuaUserData::ToUserData<FUObjectUserData>(L, 1);
		FName KeyName = FSUE4LuaStack::To<FName>(L, 2);

		UObject* TargetUObject = UserData->TargetUObject.Get();
		if (TargetUObject == nullptr)
		{
			// GC됨
			return 0;
		}

		auto* CachedField = UserData->FindOrAddCachedField(KeyName);
		check(CachedField != nullptr);

		if (CachedField->PropertySetter != nullptr)
		{
			return CachedField->PropertySetter(L, TargetUObject);
		}
		else if (CachedField->Property != nullptr)
		{
			SUE4LuaUProperty::CopyToNative(L, 3, CachedField->Property, CachedField->Property->ContainerPtrToValuePtr<void>(TargetUObject));
			return 0;
		}

		SUE4LVM_ERROR(L, TEXT("'%s': is not a member of '%s'."), *KeyName.ToString(), *TargetUObject->GetClass()->GetName());

		return 0;
	}

	// 메타테이블을 만들고 lua 스택에 푸시
	static void MakeProxyMetaTable(lua_State* L)
	{
		// 프록시의 메타테이블을 만든다.
		{
			lua_newtable(L);

			lua_pushboolean(L, true);
			lua_setfield(L, -2, "isUObjectProxy");

			lua_pushcfunction(L, &LuaGetType);
			lua_setfield(L, -2, "getType");

			lua_pushcfunction(L, &LuaGetObjectProperty);
			lua_setfield(L, -2, "getObjectProperty");

			// __gc = &LuaGC
			lua_pushstring(L, "__gc");
			lua_pushcfunction(L, &LuaGC);
			lua_rawset(L, -3);

			// __index = &LuaIndex
			lua_pushstring(L, "__index");
			lua_pushcfunction(L, &LuaIndex);
			lua_rawset(L, -3);

			// __newindex = &LuaNewIndex
			lua_pushstring(L, "__newindex");
			lua_pushcfunction(L, &LuaNewIndex);
			lua_rawset(L, -3);
		}
	}

	void Register(lua_State* L)
	{
		lua_getglobal(L, "SUE4LuaBinding");
		lua_getfield(L, -1, "uobjectProxyMTs");
		// Stack: SUE4LuaBinding, uobjectProxyMTs

		// UObject의 메타테이블 등록
		{
			MakeProxyMetaTable(L);
			lua_setfield(L, -2, "UObject");
		}

		// Stack: SUE4LuaBinding, uobjectProxyMTs
		lua_pop(L, 2);
		// Stack: (empty)
	}
}