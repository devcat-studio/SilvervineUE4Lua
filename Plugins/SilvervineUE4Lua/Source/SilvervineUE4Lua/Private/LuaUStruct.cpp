// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaUStruct.h"

#include "CoreUObject.h"
#include "Launch/Resources/Version.h"

#include "LuaAPI.h"
#include "LuaLog.h"
#include "LuaNativeValue.h"
#include "LuaStack.h"
#include "LuaUObject.h"
#include "LuaUProperty.h"
#include "LuaUserData.h"


namespace SUE4LuaUStruct
{
	static FString GetAuthoredNameForField(const UStruct* Struct, const UField* Field)
	{
#if 23 <= ENGINE_MINOR_VERSION
		return Struct->GetAuthoredNameForField(Field);
#else
		return Struct->PropertyNameToDisplayName(Field->GetFName());
#endif
	}

	//=============================================================================================================================
	// FUStructUserData
	//
	// USTRUCT() 구조체에 대한 유저 데이터 구현
	//=============================================================================================================================
	struct FUStructUserData : public FSUE4LuaUserData
	{
		// 구조체 타입 클래스
		const UScriptStruct* Struct = nullptr;
		// 구조체가 들어있는 메모리 주소
		void* ValueAddress = nullptr;
		// 구조체의 메모리를 소유하고 있는 객체
		TRefCountPtr<FSUE4LuaNativeValue> Owner;

		static FUStructUserData* New(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::NewUserData<FUStructUserData>(L);
			// Stack: UserData
		
			// 메타 테이블 설정
			{
				// Stack: UserData
				lua_getglobal(L, "SUE4LuaBinding");
				lua_getfield(L, -1, "ustructProxyMT");
				// Stack: UserData, SUE4LuaBinding, ustructProxyMT
				lua_setmetatable(L, -3);
				lua_pop(L, 1);
				// Stack: UserData
			}

			// Stack: UserData
			return UserData;
		}

		static bool IsUStructUserData(lua_State* L, int32 StackIndex)
		{
			bool bUStructUserData = false;

			if (lua_isuserdata(L, StackIndex))
			{
				lua_getmetatable(L, StackIndex);
				// Stack: MT

				if (lua_istable(L, -1))
				{
					lua_getfield(L, -1, "isUStructProxy");
					if (lua_isboolean(L, -1) && lua_toboolean(L, -1))
					{
						bUStructUserData = true;
					}
					lua_pop(L, 1);
				}

				lua_pop(L, 1);
				// Stack: (empty)
			}

			return bUStructUserData;
		}

		// UStruct* getType(self)
		static int32 LuaGetType(lua_State* L)
		{
			if (!IsUStructUserData(L, 1))
			{
				SUE4LVM_ERROR(L, TEXT("UStruct.getType(): Invalid self"));

				lua_pushnil(L);
				return 1;
			}

			auto UserData = FSUE4LuaUserData::ToUserData<FUStructUserData>(L, 1);

			if (!UserData->IsValid())
			{
				return 0;
			}

			SUE4LuaUObject::PushRef(L, const_cast<UScriptStruct*>(UserData->Struct));

			return 1;
		}

		// UObject* getObjectProperty(self, PropertyName)
		// 주어진 이름의 UObject 속성을 lua 스택에 푸시합니다. USUE4LuaValue를 UObject로 스택에 푸시하고 싶을 때 사용합니다.
		static int32 LuaGetObjectProperty(lua_State* L)
		{
			if (!IsUStructUserData(L, 1))
			{
				SUE4LVM_ERROR(L, TEXT("UStruct.getObjectProperty(): Invalid self"));

				lua_pushnil(L);
				return 1;
			}

			auto UserData = FSUE4LuaUserData::ToUserData<FUStructUserData>(L, 1);

			auto PropertyName = FSUE4LuaStack::To<FName>(L, 2);
			if (PropertyName.IsNone())
			{
				SUE4LVM_ERROR(L, TEXT("UStruct.getObjectProperty(): Invalid arg2"));

				lua_pushnil(L);
				return 1;
			}

			if (!UserData->IsValid())
			{
				return 0;
			}

			if (const UProperty* Property = UserData->FindPropertyByName(PropertyName))
			{
				if (auto ObjectProperty = Cast<UObjectPropertyBase>(Property))
				{
					SUE4LuaUObject::PushRef(L, ObjectProperty->GetObjectPropertyValue_InContainer(UserData->ValueAddress));
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
			auto UserData = FSUE4LuaUserData::ToUserData<FUStructUserData>(L, 1);
			UserData->Dispose();

			return 0;
		}

		static int32 LuaIndex(lua_State* L)
		{
			// MT에 있는 값이면 그대로 반환
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

			auto UserData = FSUE4LuaUserData::ToUserData<FUStructUserData>(L, 1);
			if (!UserData->IsValid())
			{
				return 0;
			}

			return UserData->Index(L, 2);
		}

		static int32 LuaNewIndex(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::ToUserData<FUStructUserData>(L, 1);
			if (!UserData->IsValid())
			{
				return 0;
			}

			UserData->NewIndex(L, 2);

			return 0;
		}

		static int32 LuaIterator(lua_State* L);
		static int32 LuaPairs(lua_State* L);

		// 메타테이블을 만들고 lua 스택에 푸시합니다.
		static void MakeProxyMetaTable(lua_State* L)
		{
			lua_newtable(L);

			lua_pushboolean(L, true);
			lua_setfield(L, -2, "isUStructProxy");

			lua_pushcfunction(L, &LuaGetType);
			lua_setfield(L, -2, "getType");

			lua_pushcfunction(L, &LuaGetObjectProperty);
			lua_setfield(L, -2, "getObjectProperty");

			lua_pushstring(L, "__gc");
			lua_pushcfunction(L, &LuaGC);
			lua_rawset(L, -3);

			lua_pushstring(L, "__index");
			lua_pushcfunction(L, &LuaIndex);
			lua_rawset(L, -3);

			lua_pushstring(L, "__newindex");
			lua_pushcfunction(L, &LuaNewIndex);
			lua_rawset(L, -3);

			lua_pushstring(L, "__pairs");
			lua_pushcfunction(L, &LuaPairs);
			lua_rawset(L, -3);
		}

		static void Register(lua_State* L)
		{
			lua_getglobal(L, "SUE4LuaBinding");

			// 메타테이블 등록
			MakeProxyMetaTable(L);
			lua_setfield(L, -2, "ustructProxyMT");

			// Stack: SUE4LuaBinding
			lua_pop(L, 1);
			// Stack: (empty)
		}

		bool IsValid()
		{
			return Owner->IsValid();
		}
			   
		int32 Index(lua_State* L, int32 StackIndex)
		{
			if (!IsValid())
			{
				return 0;
			}

			FName KeyName = FSUE4LuaStack::To<FName>(L, StackIndex);

			if (const UProperty* MemberProperty = FindPropertyByName(KeyName))
			{
				SUE4LuaUProperty::Push(L, MemberProperty, MemberProperty->ContainerPtrToValuePtr<void>(ValueAddress), Owner);

				return 1;
			}
			else
			{
				SUE4LVM_ERROR(L, TEXT("'%s': is not a member of '%s'"), *KeyName.ToString(), *Struct->GetName());

				return 0;
			}
		}

		void NewIndex(lua_State* L, int32 StackIndex)
		{
			if (!IsValid())
			{
				return;
			}

			FName KeyName = FSUE4LuaStack::To<FName>(L, StackIndex);

			if (const UProperty* MemberProperty = FindPropertyByName(KeyName))
			{
				SUE4LuaUProperty::CopyToNative(L, StackIndex + 1, MemberProperty, MemberProperty->ContainerPtrToValuePtr<void>(ValueAddress));
			}
			else
			{
				SUE4LVM_ERROR(L, TEXT("'%s': is not a member of '%s'"), *KeyName.ToString(), *Struct->GetName());
			}
		}

		const UProperty* FindPropertyByName(FName Name)
		{
			if (Struct->IsNative())
			{
				return Struct->FindPropertyByName(Name);
			}
			else
			{
				const FString NameStr = Name.ToString();

				// BP 스트럭트의 속성 이름은 {DisplayName}_{index}_{hashcode}와 같이 이상하게 설정되어 있어서 빠르게 찾을 수 없습니다...
				for (TFieldIterator<UProperty> PropIt(Struct); PropIt; ++PropIt)
				{
					const UProperty* FieldProperty = *PropIt;
					const FString FieldPropertyName = GetAuthoredNameForField(Struct, FieldProperty);

					// 속성 이름이 FName으로 저장되므로 이름을 비교할 때 반드시 대소문자를 무시해야 합니다.
					if (FieldPropertyName.Equals(NameStr, ESearchCase::IgnoreCase))
					{
						return FieldProperty;
					}
				}

				return nullptr;
			}
		}
	};

	//=============================================================================================================================
	// FStructIteratorUserData
	//
	// FUStructUserData를 pairs()로 돌릴 때 사용하는 반복자 클래스
	//=============================================================================================================================
	struct FStructIteratorUserData : public FSUE4LuaUserData
	{
		TFieldIterator<UProperty> MemberPropIt;

		static int32 LuaGC(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::ToUserData<FStructIteratorUserData>(L, 1);
			UserData->Dispose();

			return 0;
		}

		static FStructIteratorUserData* New(lua_State* L, const UScriptStruct* Struct)
		{
			auto NewIteratorUserData = FSUE4LuaUserData::NewUserData<FStructIteratorUserData>(L, Struct);
			// Stack: NewIteratorUserData

			lua_newtable(L);
			lua_pushcfunction(L, &LuaGC);
			lua_setfield(L, -2, "__gc");
			lua_setmetatable(L, -2);

			// Stack: NewIteratorUserData
			return NewIteratorUserData;
		}

		FStructIteratorUserData(const UScriptStruct* Struct)
			: MemberPropIt(Struct)
		{
		}

		int32 Next(lua_State* L, FUStructUserData* StructUserData)
		{
			if (!StructUserData->IsValid())
			{
				return 0;
			}

			if (MemberPropIt)
			{
				const UProperty* MemberProperty = *MemberPropIt;
				++MemberPropIt;

				if (StructUserData->Struct->IsNative())
				{
					FSUE4LuaStack::Push(L, MemberProperty->GetFName());
				}
				else
				{
					FSUE4LuaStack::Push(L, GetAuthoredNameForField(StructUserData->Struct, MemberProperty));
				}
				SUE4LuaUProperty::Push(L, MemberProperty, MemberProperty->ContainerPtrToValuePtr<void>(StructUserData->ValueAddress), StructUserData->Owner);

				return 2;
			}

			return 0;
		}
	};

	int32 FUStructUserData::LuaIterator(lua_State* L)
	{
		auto UserData = FSUE4LuaUserData::ToUserData<FUStructUserData>(L, lua_upvalueindex(1));
		auto IteratorUserData = FSUE4LuaUserData::ToUserData<FStructIteratorUserData>(L, 1);

		return IteratorUserData->Next(L, UserData);
	}

	int32 FUStructUserData::LuaPairs(lua_State* L)
	{
		auto UserData = FSUE4LuaUserData::ToUserData<FUStructUserData>(L, 1);

		lua_pushvalue(L, 1);
		lua_pushcclosure(L, &LuaIterator, 1);
		auto IteratorUserData = FStructIteratorUserData::New(L, UserData->Struct);

		// Stack: LuaPairsIterator, IteratorUserData
		return 2;
	}

	//=============================================================================================================================
	// SUE4LuaUStruct
	//=============================================================================================================================
	bool IsUStruct(lua_State* L, int32 StackIndex)
	{
		return FUStructUserData::IsUStructUserData(L, StackIndex);
	}

	void* GetRef(lua_State* L, int32 StackIndex, const UScriptStruct* Struct)
	{
		check(Struct != nullptr);

		if (!FUStructUserData::IsUStructUserData(L, StackIndex))
		{
			SUE4LVM_WARNING(L, TEXT("%s: expected UStruct(%s)."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, StackIndex))), *Struct->GetStructCPPName());
			return nullptr;
		}

		auto UserData = FSUE4LuaUserData::ToUserData<FUStructUserData>(L, StackIndex);
		if (!UserData->IsValid())
		{
			return nullptr;
		}

		if (UserData->Struct != Struct)
		{
			SUE4LVM_ERROR(L, TEXT("%s: struct type mismatch(%s -> %s)"), *UserData->Struct->GetName(), *UserData->Struct->GetName(), *Struct->GetName());
			return nullptr;
		}
		
		return UserData->ValueAddress;
	}

	void PushValue(lua_State* L, const UScriptStruct* Struct, const void* ValueAddress)
	{
		check(Struct != nullptr);

		auto UserData = FUStructUserData::New(L);
		UserData->Struct = Struct;
		UserData->Owner = new FSUE4LuaUStructValue(Struct);
		UserData->ValueAddress = UserData->Owner->GetValueAddress();

		if (ValueAddress != nullptr)
		{
			Struct->CopyScriptStruct(UserData->ValueAddress, ValueAddress);
		}
	}

	void PushRef(lua_State* L, const UScriptStruct* Struct, const void* ValueAddress, FSUE4LuaNativeValue* Owner)
	{
		check(Struct != nullptr);
		check(ValueAddress != nullptr);
		check(Owner != nullptr);

		auto UserData = FUStructUserData::New(L);
		UserData->Struct = Struct;
		UserData->Owner = Owner;
		UserData->ValueAddress = const_cast<void*>(ValueAddress);
	}

	bool CopyToNative(lua_State* L, int32 StackIndex, const UScriptStruct* Struct, void* ValueAddress)
	{
		check(Struct != nullptr);
		check(ValueAddress != nullptr);

		const void* SrcValueAddress = GetRef(L, StackIndex, Struct);
		if (SrcValueAddress != nullptr)
		{
			Struct->CopyScriptStruct(ValueAddress, SrcValueAddress);
		}

		return true;
	}

	bool TryCopyToLua(lua_State* L, int32 StackIndex, const UScriptStruct* SrcStruct, const void* SrcValueAddress)
	{
		check(SrcStruct != nullptr);
		check(SrcValueAddress != nullptr);

		if (!FUStructUserData::IsUStructUserData(L, StackIndex))
		{
			return false;
		}

		auto UserData = FSUE4LuaUserData::ToUserData<FUStructUserData>(L, StackIndex);
		if (!UserData->IsValid())
		{
			return false;
		}

		if (UserData->Struct != SrcStruct)
		{
			SUE4LVM_ERROR(L, TEXT("%s: copying to incompatible type(%s -> %s)"), *UserData->Struct->GetName(), *SrcStruct->GetName(), *UserData->Struct->GetName());
			return false;
		}

		UserData->Struct->CopyScriptStruct(UserData->ValueAddress, SrcValueAddress);

		return true;
	}

	void Register(lua_State* L)
	{
		FUStructUserData::Register(L);
	}
}