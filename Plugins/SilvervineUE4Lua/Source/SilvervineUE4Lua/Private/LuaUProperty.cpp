// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaUProperty.h"

#include "Misc/QualifiedFrameTime.h"

#include "LuaAPI.h"
#include "LuaLog.h"
#include "LuaStack.h"
#include "LuaUObject.h"
#include "LuaUserData.h"
#include "LuaUStruct.h"
#include "SilvervineUE4LuaValue.h"


namespace SUE4LuaUProperty
{
#define DEFINE_VALUETYPE_NAME(Name)		static FName NAME_##Name(#Name)
	DEFINE_VALUETYPE_NAME(Vector);
	DEFINE_VALUETYPE_NAME(Vector2D);
	DEFINE_VALUETYPE_NAME(Vector4);
	DEFINE_VALUETYPE_NAME(IntPoint);
	DEFINE_VALUETYPE_NAME(IntVector);
	DEFINE_VALUETYPE_NAME(Plane);
	DEFINE_VALUETYPE_NAME(Matrix);
	DEFINE_VALUETYPE_NAME(Rotator);
	DEFINE_VALUETYPE_NAME(Quat);
	DEFINE_VALUETYPE_NAME(LinearColor);
	DEFINE_VALUETYPE_NAME(Color);
	DEFINE_VALUETYPE_NAME(Transform);
	DEFINE_VALUETYPE_NAME(Box);
	DEFINE_VALUETYPE_NAME(Box2D);
	DEFINE_VALUETYPE_NAME(BoxSphereBounds);
	DEFINE_VALUETYPE_NAME(Guid);
	DEFINE_VALUETYPE_NAME(DateTime);
	DEFINE_VALUETYPE_NAME(Timecode);
	DEFINE_VALUETYPE_NAME(Timespan);
	DEFINE_VALUETYPE_NAME(RandomStream);
	DEFINE_VALUETYPE_NAME(FrameRate);
	DEFINE_VALUETYPE_NAME(FrameNumber);
	DEFINE_VALUETYPE_NAME(FrameTime);
	DEFINE_VALUETYPE_NAME(QualifiedFrameTime);
	DEFINE_VALUETYPE_NAME(PrimaryAssetType);
	DEFINE_VALUETYPE_NAME(PrimaryAssetId);
	DEFINE_VALUETYPE_NAME(SoftObjectPath);
	DEFINE_VALUETYPE_NAME(FloatRangeBound);
	DEFINE_VALUETYPE_NAME(FloatRange);
	DEFINE_VALUETYPE_NAME(Int32Interval);
#undef DEFINE_VALUETYPE_NAME

	static void PushNonFixedArray(lua_State* L, const UProperty* Property, const void* ValueAddress, FSUE4LuaNativeValue* Owner);
	static bool CopyNonFixedArrayToNative(lua_State* L, int32 StackIndex, const UProperty* DestProperty, void* DestValueAddress);
}

namespace SUE4LuaUProperty
{
	//=============================================================================================================================
	// FUPropertyUserData
	// 
	// 고정 크기 배열, TArray, TMap, TSet, Delegate, MulticastDelegate 속성에 대한 유저데이터 기본 클래스
	//=============================================================================================================================
	class FUPropertyUserData : public FSUE4LuaUserData
	{
	public:
		~FUPropertyUserData()
		{
		}

		void Initialize(const UProperty* InProperty, const void* InValueAddress, FSUE4LuaNativeValue* InOwner)
		{
			check(InProperty != nullptr);
			check(InValueAddress != nullptr);

			Property = InProperty;

			if (InOwner != nullptr)
			{
				Owner = InOwner;
				ValueAddress = const_cast<void*>(InValueAddress);
			}
			else
			{
				Owner = new FSUE4LuaUPropertyValue(Property);
				ValueAddress = Owner->GetValueAddress();

				Property->CopySingleValue(ValueAddress, InValueAddress);
			}

			OnInitialized();
		}

		// 컨테이너에서 인덱싱할 수 있는 원소 타입이면 true를 반환합니다. 접근 시 값 복사가 일어나는 값 타입이나 UObject* 등이 이에 해당됩니다.
		// 고정 크기 배열을 제외하고 나머지 속성들은 내부 버퍼가 동적으로 할당되기 때문에 인덱싱된 원소에 대한 메모리가 나중에 해제될 수 있습니다.
		// 인덱싱한 원소를 참조하는 곳에서는 메모리가 해제되었는지 알 수 있는 방법이 없으므로 인덱싱이 발생하면 참조 대신 값을 복사해서 전달합니다.
		static bool IsIndexableElementType(const UProperty* Property)
		{
			const bool bStruct = Property->IsA<UStructProperty>();
			const bool bDelegate = Property->IsA<UDelegateProperty>() || Property->IsA<UMulticastDelegateProperty>();

			bool bValueTypeStruct = false;
			if (bStruct)
			{
				auto StructProperty = Cast<UStructProperty>(Property);

				bValueTypeStruct =
					StructProperty->Struct->GetFName() == NAME_Vector ||
					StructProperty->Struct->GetFName() == NAME_Vector2D ||
					StructProperty->Struct->GetFName() == NAME_Vector4 ||
					StructProperty->Struct->GetFName() == NAME_IntPoint ||
					StructProperty->Struct->GetFName() == NAME_IntVector ||
					StructProperty->Struct->GetFName() == NAME_Plane ||
					StructProperty->Struct->GetFName() == NAME_Matrix ||
					StructProperty->Struct->GetFName() == NAME_Rotator ||
					StructProperty->Struct->GetFName() == NAME_Quat ||
					StructProperty->Struct->GetFName() == NAME_LinearColor ||
					StructProperty->Struct->GetFName() == NAME_Color ||
					StructProperty->Struct->GetFName() == NAME_Transform ||
					StructProperty->Struct->GetFName() == NAME_Box ||
					StructProperty->Struct->GetFName() == NAME_Box2D ||
					StructProperty->Struct->GetFName() == NAME_BoxSphereBounds ||
					StructProperty->Struct->GetFName() == NAME_Guid ||
					StructProperty->Struct->GetFName() == NAME_DateTime ||
					StructProperty->Struct->GetFName() == NAME_Timecode ||
					StructProperty->Struct->GetFName() == NAME_Timespan ||
					StructProperty->Struct->GetFName() == NAME_RandomStream ||
					StructProperty->Struct->GetFName() == NAME_FrameRate ||
					StructProperty->Struct->GetFName() == NAME_FrameNumber ||
					StructProperty->Struct->GetFName() == NAME_FrameTime ||
					StructProperty->Struct->GetFName() == NAME_QualifiedFrameTime ||
					StructProperty->Struct->GetFName() == NAME_PrimaryAssetType ||
					StructProperty->Struct->GetFName() == NAME_PrimaryAssetId ||
					StructProperty->Struct->GetFName() == NAME_SoftObjectPath ||
					StructProperty->Struct->GetFName() == NAME_FloatRangeBound ||
					StructProperty->Struct->GetFName() == NAME_FloatRange ||
					StructProperty->Struct->GetFName() == NAME_Int32Interval;
			}

			return !(bStruct || bDelegate) || bValueTypeStruct;
		}

		const UProperty* GetProperty() const
		{
			return Property;
		}

		void* GetValueAddress() const
		{
			return ValueAddress;
		}

		FSUE4LuaNativeValue* GetOwner() const
		{
			return Owner;
		}

		bool IsValid() const
		{
			return Owner->IsValid();
		}

		virtual const char* GetTypeName() const = 0;
		virtual void OnInitialized() = 0;

		// 속성 전체를 대상에 복사합니다. 예를 들어 배열이면 배열 전체를 대상에 복사합니다.
		virtual bool CopyComplete(lua_State* L, const UProperty* SrcProperty, const void* SrcValueAddress, const UProperty* DestProperty, void* DestValueAddress) const
		{
			SUE4LVM_ERROR(L, TEXT("'%s': copying value of this type(%s) is not supported."), *Property->GetName(), ANSI_TO_TCHAR(GetTypeName()));

			return false;
		}

		// lua 스택에서 키 값을 읽고 해당하는 필드를 lua 스택에 푸시합니다.
		virtual int32 Index(lua_State* L, int32 StackIndex)
		{
			SUE4LVM_ERROR(L, TEXT("'%s': access a property of type '%s'"), *Property->GetName(), ANSI_TO_TCHAR(GetTypeName()));
			return 0;
		}

		// lua 스택에서 키, 밸류를 읽어서 속성에 기록합니다.
		virtual void NewIndex(lua_State* L, int32 StackIndex)
		{
			SUE4LVM_ERROR(L, TEXT("'%s': access a property of type '%s'"), *Property->GetName(), ANSI_TO_TCHAR(GetTypeName()));
		}

		// pairs()에서 사용할 반복자를 lua 스택에 푸시합니다.
		virtual void PushIterator(lua_State* L)
		{
			lua_pushnil(L);
		}

	protected:
		// 속성 타입 클래스
		const UProperty* Property = nullptr;
		// 속성 값이 들어있는 메모리 주소
		void* ValueAddress = nullptr;
		// 속성의 메모리를 소유하고 있는 객체
		TRefCountPtr<FSUE4LuaNativeValue> Owner;
	};

	//=============================================================================================================================
	// FPropertyIteratorUserData
	//
	// FUPropertyUserData를 pairs()로 돌릴 때 사용하는 반복자들의 기본 클래스
	//=============================================================================================================================
	class FPropertyIteratorUserData : public FSUE4LuaUserData
	{
	public:
		static int32 LuaGC(lua_State* L)
		{
			auto IteratorUserData = FSUE4LuaUserData::ToUserData<FPropertyIteratorUserData>(L, 1);
			IteratorUserData->Dispose();

			return 0;
		}

		template<typename T>
		static T* New(lua_State* L)
		{
			auto NewIteratorUserData = FSUE4LuaUserData::NewUserData<T>(L);
			// Stack: NewIteratorUserData

			// #todo: 메타테이블 매번 만들지말고 캐싱

			lua_newtable(L);
			lua_pushcfunction(L, &LuaGC);
			lua_setfield(L, -2, "__gc");
			lua_setmetatable(L, -2);

			// Stack: NewIteratorUserData
			return NewIteratorUserData;
		}
		
		virtual int32 Next(lua_State* L, FUPropertyUserData* PropertyUserData) = 0;
	};

	//=============================================================================================================================
	// FFixedArrayUserData
	//
	// int32[123]과 같은 고정 크기 배열에 대한 구현
	//=============================================================================================================================
	struct FFixedArrayUserData : public FUPropertyUserData
	{
		const UProperty* FixedArrayProperty = nullptr;

		static const char* StaticGetTypeName()
		{
			return "FixedArray";
		}

		static void RegisterLibrary(lua_State* L)
		{	
		}

		// Begin FUPropertyUserData Interface
		virtual const char* GetTypeName() const override
		{
			return StaticGetTypeName();
		}

		virtual void OnInitialized() override
		{
			FixedArrayProperty = Property;
		}

		virtual bool CopyComplete(lua_State* L, const UProperty* SrcProperty, const void* SrcValueAddress, const UProperty* DestProperty, void* DestValueAddress) const override
		{
			if (!IsValid())
			{
				return false;
			}

			return StaticCopyComplete(L, Property->GetName(), SrcProperty, SrcValueAddress, DestProperty, DestValueAddress);
		}

		static bool StaticCopyComplete(lua_State* L, const FString& PropertyName, const UProperty* SrcProperty, const void* SrcValueAddress, const UProperty* DestProperty, void* DestValueAddress)
		{
			if (SrcProperty->GetClass() != DestProperty->GetClass())
			{
				SUE4LVM_ERROR(L, TEXT("%s: copying to incompatible type(%s -> %s)"), *PropertyName, *SrcProperty->GetCPPType(), *DestProperty->GetCPPType());
				return false;
			}

			if (SrcProperty->ArrayDim != DestProperty->ArrayDim)
			{
				SUE4LVM_ERROR(L, TEXT("%s: array sizes are different(%d -> %d)"), *PropertyName, SrcProperty->ArrayDim, DestProperty->ArrayDim);
				return false;
			}

			DestProperty->CopyCompleteValue(DestValueAddress, SrcValueAddress);

			return true;
		}

		virtual int32 Index(lua_State* L, int32 StackIndex) override
		{
			if (!IsValid())
			{
				return 0;
			}

			if (!lua_isinteger(L, StackIndex))
			{
				SUE4LVM_ERROR(L, TEXT("'%s': access an array with a non integer value"), *FixedArrayProperty->GetName());
				return 0;
			}

			int32 ArrayIndex = FSUE4LuaStack::To<int32>(L, StackIndex);
			if (0 <= ArrayIndex && ArrayIndex < FixedArrayProperty->ArrayDim)
			{
				PushRef(L, ArrayIndex);
				return 1;
			}
			else
			{
				lua_pushnil(L);
				return 1;
			}
		}

		virtual void NewIndex(lua_State* L, int32 StackIndex) override
		{
			if (!IsValid())
			{
				return;
			}

			if (!lua_isinteger(L, StackIndex))
			{
				SUE4LVM_ERROR(L, TEXT("'%s': access an array with a non integer value"), *FixedArrayProperty->GetName());
				return;
			}

			int32 ArrayIndex = FSUE4LuaStack::To<int32>(L, StackIndex);
			if (0 <= ArrayIndex && ArrayIndex < FixedArrayProperty->ArrayDim)
			{
				CopyNonFixedArrayToNative(L, StackIndex + 1, FixedArrayProperty, reinterpret_cast<uint8*>(ValueAddress) + FixedArrayProperty->ElementSize * ArrayIndex);
			}
			else
			{
				SUE4LVM_ERROR(L, TEXT("'%s': index out of range(%d)"), *FixedArrayProperty->GetName(), ArrayIndex);
			}
		}

		virtual void PushIterator(lua_State* L) override
		{
			struct FFixedArrayIteratorUserData : public FPropertyIteratorUserData
			{
				int32 Index = 0;

				virtual int32 Next(lua_State* L, FUPropertyUserData* PropertyUserData) override
				{
					auto FixedArrayUserData = static_cast<FFixedArrayUserData*>(PropertyUserData);
					if (!FixedArrayUserData->IsValid())
					{
						return 0;
					}

					int32 CurIndex = Index;
					++Index;

					if (CurIndex < FixedArrayUserData->FixedArrayProperty->ArrayDim)
					{
						FSUE4LuaStack::Push(L, CurIndex);
						FixedArrayUserData->PushRef(L, CurIndex);
						return 2;
					}

					return 0;
				}
			};

			auto IteratorUserData = FPropertyIteratorUserData::New<FFixedArrayIteratorUserData>(L);
		}
		// End FUPropertyUserData Interface

		// ArrayIndex에 해당하는 값(int32 등) 혹은 참조(UStruct 등)를 lua 스택에 푸시합니다.
		// 고정 크기 배열은 소유자 객체가 살아 있는 한 메모리 주소가 변하지 않기 때문에 안전하게 원소에 대한 참조를 만들 수 있습니다.
		int32 PushRef(lua_State* L, int32 ArrayIndex)
		{
			if (!IsValid())
			{
				return 0;
			}

			if (0 <= ArrayIndex && ArrayIndex < FixedArrayProperty->ArrayDim)
			{
				PushNonFixedArray(L, FixedArrayProperty, reinterpret_cast<uint8*>(ValueAddress) + FixedArrayProperty->ElementSize * ArrayIndex, Owner);
				return 1;
			}
			else
			{
				lua_pushnil(L);
				return 1;
			}
		}
	};

	//=============================================================================================================================
	// FArrayUserData
	//
	// TArray에 대한 유저 데이터 구현
	//=============================================================================================================================
	struct FArrayUserData : public FUPropertyUserData
	{
		const UArrayProperty* ArrayProperty = nullptr;
		void* ItemBuffer = nullptr;		// Find()에서 사용
		bool bIndexable = false;

		static const char* StaticGetTypeName()
		{
			return "Array";
		}

		// integer Num(self)
		static int32 LuaNum(lua_State* L)
		{
			int32 Num = 0;

			auto UserData = FSUE4LuaUserData::ToUserData<FArrayUserData>(L, 1);
			if (UserData != nullptr)
			{
				if (UserData->IsValid())
				{
					Num = UserData->GetScriptHelper().Num();
				}
			}
			else
			{
				SUE4LVM_ERROR(L, TEXT("Array.Num(): Invalid self"));
			}

			FSUE4LuaStack::Push(L, Num);

			return 1;
		}

		// void Empty(self)
		static int32 LuaEmpty(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::ToUserData<FArrayUserData>(L, 1);
			if (UserData != nullptr)
			{
				if (UserData->IsValid())
				{
					UserData->GetScriptHelper().EmptyValues();
				}
			}
			else
			{
				SUE4LVM_ERROR(L, TEXT("Array.Empty(): Invalid self"));
			}

			return 0;
		}

		// void Add(self, value?)
		static int32 LuaAdd(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::ToUserData<FArrayUserData>(L, 1);
			if (UserData != nullptr)
			{
				if (UserData->IsValid())
				{
					auto ScriptHelper = UserData->GetScriptHelper();

					ScriptHelper.AddValue();
					if (1 < lua_gettop(L))
					{
						SUE4LuaUProperty::CopyToNative(L, 2, UserData->ArrayProperty->Inner, ScriptHelper.GetRawPtr(ScriptHelper.Num() - 1));
					}
				}
			}
			else
			{
				SUE4LVM_ERROR(L, TEXT("Array.Add(): Invalid self"));
			}

			return 0;
		}

		// void Insert(self, index, value?)
		static int32 LuaInsert(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::ToUserData<FArrayUserData>(L, 1);
			if (UserData == nullptr)
			{
				SUE4LVM_ERROR(L, TEXT("Array.Insert(): Invalid self"));
				return 0;
			}

			if (!lua_isinteger(L, 2))
			{
				SUE4LVM_ERROR(L, TEXT("Array.Insert(): Invalid index"));
				return 0;
			}

			int32 ArrayIndex = FSUE4LuaStack::To<int32>(L, 2);

			if (UserData->IsValid())
			{
				auto ScriptHelper = UserData->GetScriptHelper();

				if (0 <= ArrayIndex && ArrayIndex <= ScriptHelper.Num())
				{
					ScriptHelper.InsertValues(ArrayIndex);
					if (2 < lua_gettop(L))
					{
						SUE4LuaUProperty::CopyToNative(L, 3, UserData->ArrayProperty->Inner, ScriptHelper.GetRawPtr(ArrayIndex));
					}
				}
				else
				{
					SUE4LVM_ERROR(L, TEXT("Array.Insert(): Index out of range(%d)"), ArrayIndex);
				}
			}

			return 0;
		}

		// void Remove(self, index)
		static int32 LuaRemove(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::ToUserData<FArrayUserData>(L, 1);
			if (UserData == nullptr)
			{
				SUE4LVM_ERROR(L, TEXT("Array.Remove(): Invalid self"));
				return 0;
			}

			if (!lua_isinteger(L, 2))
			{
				SUE4LVM_ERROR(L, TEXT("Array.Remove(): Invalid index"));
				return 0;
			}

			int32 ArrayIndex = FSUE4LuaStack::To<int32>(L, 2);

			if (UserData->IsValid())
			{
				auto ScriptHelper = UserData->GetScriptHelper();

				if (ScriptHelper.IsValidIndex(ArrayIndex))
				{
					ScriptHelper.RemoveValues(ArrayIndex);
				}
				else
				{
					SUE4LVM_ERROR(L, TEXT("Array.Remove(): Index out of range(%d)"), ArrayIndex);
				}
			}

			return 0;
		}

		// integer Find(self, value)
		static int32 LuaFind(lua_State* L)
		{
			int32 ResultIndex = INDEX_NONE;

			auto UserData = FSUE4LuaUserData::ToUserData<FArrayUserData>(L, 1);
			if (UserData != nullptr)
			{
				if (UserData->IsValid())
				{
					if (UserData->CopyItemFromStack(L, 2))
					{
						auto ScriptHelper = UserData->GetScriptHelper();
						auto InnerProp = UserData->ArrayProperty->Inner;

						for (int32 Index = 0; Index < ScriptHelper.Num(); ++Index)
						{
							if (InnerProp->Identical(UserData->ItemBuffer, ScriptHelper.GetRawPtr(Index)))
							{
								ResultIndex = Index;
								break;
							}
						}
					}
				}
			}
			else
			{
				SUE4LVM_ERROR(L, TEXT("Array.Find(): Invalid self"));
			}

			FSUE4LuaStack::Push(L, ResultIndex);
			return 1;
		}

		// value GetCopy(self, index)
		static int32 LuaGetCopy(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::ToUserData<FArrayUserData>(L, 1);
			if (UserData == nullptr)
			{
				SUE4LVM_ERROR(L, TEXT("Array.GetCopy(): Invalid self"));
				return 0;
			}

			return UserData->Get(L, 2, false);
		}

		// value GetRef(self, index)
		static int32 LuaGetRef(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::ToUserData<FArrayUserData>(L, 1);
			if (UserData == nullptr)
			{
				SUE4LVM_ERROR(L, TEXT("Array.GetRef(): Invalid self"));
				return 0;
			}

			return UserData->Get(L, 2, true);
		}

		// void Set(self, index, value)
		static int32 LuaSet(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::ToUserData<FArrayUserData>(L, 1);
			if (UserData == nullptr)
			{
				SUE4LVM_ERROR(L, TEXT("Array.Set(): Invalid self"));
				return 0;
			}

			UserData->Set(L, 2);

			return 0;
		}

		static void RegisterLibrary(lua_State* L)
		{
			lua_pushcfunction(L, &LuaNum);
			lua_setfield(L, -2, "Num");

			lua_pushcfunction(L, &LuaEmpty);
			lua_setfield(L, -2, "Empty");

			lua_pushcfunction(L, &LuaAdd);
			lua_setfield(L, -2, "Add");

			lua_pushcfunction(L, &LuaInsert);
			lua_setfield(L, -2, "Insert");

			lua_pushcfunction(L, &LuaRemove);
			lua_setfield(L, -2, "Remove");

			lua_pushcfunction(L, &LuaFind);
			lua_setfield(L, -2, "Find");

			lua_pushcfunction(L, &LuaGetCopy);
			lua_setfield(L, -2, "GetCopy");

			lua_pushcfunction(L, &LuaGetRef);
			lua_setfield(L, -2, "GetRef");

			lua_pushcfunction(L, &LuaSet);
			lua_setfield(L, -2, "Set");
		}

		virtual ~FArrayUserData()
		{
			if (ItemBuffer != nullptr)
			{
				auto InnerProp = ArrayProperty->Inner;

				InnerProp->DestroyValue(ItemBuffer);
				FMemory::Free(ItemBuffer);
				ItemBuffer = nullptr;
			}
		}

		FScriptArrayHelper GetScriptHelper()
		{
			return FScriptArrayHelper(ArrayProperty, ValueAddress);
		}

		// Begin FUPropertyUserData Interface
		virtual const char* GetTypeName() const override
		{
			return StaticGetTypeName();
		}

		virtual void OnInitialized() override
		{
			ArrayProperty = static_cast<const UArrayProperty*>(Property);
			bIndexable = IsIndexableElementType(ArrayProperty->Inner);
		}

		virtual bool CopyComplete(lua_State* L, const UProperty* SrcProperty, const void* SrcValueAddress, const UProperty* DestProperty, void* DestValueAddress) const override
		{
			if (!IsValid())
			{
				return false;
			}

			return StaticCopyComplete(L, Property->GetName(), SrcProperty, SrcValueAddress, DestProperty, DestValueAddress);
		}

		static bool StaticCopyComplete(lua_State* L, const FString& PropertyName, const UProperty* SrcProperty, const void* SrcValueAddress, const UProperty* DestProperty, void* DestValueAddress)
		{
			auto SrcArrayProperty = Cast<UArrayProperty>(SrcProperty);
			auto DestArrayProperty = Cast<UArrayProperty>(DestProperty);

			if (SrcArrayProperty && DestArrayProperty)
			{
				if (SrcArrayProperty->Inner->GetClass() == DestArrayProperty->Inner->GetClass())
				{
					DestProperty->CopySingleValue(DestValueAddress, SrcValueAddress);
					return true;
				}
				else
				{
					SUE4LVM_ERROR(L, TEXT("%s: copying to incompatible element type(%s -> %s)"),
						*PropertyName,
						*SrcArrayProperty->Inner->GetCPPType(),
						*DestArrayProperty->Inner->GetCPPType());
					return false;
				}
			}
			else
			{
				SUE4LVM_ERROR(L, TEXT("%s: copying to incompatible type(%s -> %s)"),
					*PropertyName,
					*SrcProperty->GetCPPType(),
					*DestProperty->GetCPPType());
				return false;
			}
		}

		virtual int32 Index(lua_State* L, int32 StackIndex) override
		{
			if (!bIndexable)
			{
				SUE4LVM_WARNING(L, TEXT("'%s': indexing an array of type '%s' via brackets is not safe. Use 'GetCopy()' or 'GetRef()' instead."), *ArrayProperty->GetName(), *ArrayProperty->Inner->GetCPPType());
			}

			return Get(L, StackIndex, true);
		}

		virtual void NewIndex(lua_State* L, int32 StackIndex) override
		{
			if (!bIndexable)
			{
				SUE4LVM_WARNING(L, TEXT("'%s': indexing an array of type '%s' via brackets is not safe. Use 'Set()' instead."), *ArrayProperty->GetName(), *ArrayProperty->Inner->GetCPPType());
			}

			Set(L, StackIndex);
		}

		virtual void PushIterator(lua_State* L) override
		{
			struct FArrayIteratorUserData : public FPropertyIteratorUserData
			{
				int32 Index = 0;

				virtual int32 Next(lua_State* L, FUPropertyUserData* PropertyUserData) override
				{
					auto ArrayUserData = static_cast<FArrayUserData*>(PropertyUserData);
					if (!ArrayUserData->IsValid())
					{
						return 0;
					}

					auto ScriptHelper = ArrayUserData->GetScriptHelper();

					int32 CurIndex = Index;
					++Index;

					if (CurIndex < ScriptHelper.Num())
					{
						FSUE4LuaStack::Push(L, CurIndex);
						SUE4LuaUProperty::Push(L, ArrayUserData->ArrayProperty->Inner, ScriptHelper.GetRawPtr(CurIndex), nullptr);
						return 2;
					}

					return 0;
				}
			};

			auto IteratorUserData = FPropertyIteratorUserData::New<FArrayIteratorUserData>(L);
		}
		// End FUPropertyUserData Interface

		bool CopyItemFromStack(lua_State* L, int32 StackIndex)
		{
			auto InnerProp = ArrayProperty->Inner;

			if (ItemBuffer == nullptr)
			{
				ItemBuffer = FMemory::Malloc(InnerProp->GetSize(), InnerProp->GetMinAlignment());
				InnerProp->InitializeValue(ItemBuffer);
			}

			return SUE4LuaUProperty::CopyToNative(L, StackIndex, InnerProp, ItemBuffer);
		}

		void Set(lua_State* L, int32 StackIndex) 
		{
			if (!IsValid())
			{
				return;
			}

			if (!lua_isinteger(L, StackIndex))
			{
				SUE4LVM_ERROR(L, TEXT("'%s': access an array with a non integer value"), *ArrayProperty->GetName());
				return;
			}

			int32 ArrayIndex = FSUE4LuaStack::To<int32>(L, StackIndex);
			auto ScriptHelper = GetScriptHelper();

			if (ScriptHelper.IsValidIndex(ArrayIndex))
			{
				SUE4LuaUProperty::CopyToNative(L, StackIndex + 1, ArrayProperty->Inner, ScriptHelper.GetRawPtr(ArrayIndex));
			}
			else
			{
				SUE4LVM_ERROR(L, TEXT("'%s': index out of range(%d)"), *ArrayProperty->GetName(), ArrayIndex);
			}
		}

		int32 Get(lua_State* L, int32 StackIndex, bool bPassByRef)
		{
			if (!IsValid())
			{
				return 0;
			}

			if (!lua_isinteger(L, StackIndex))
			{
				SUE4LVM_ERROR(L, TEXT("'%s': access an array with a non integer value"), *ArrayProperty->GetName());
				return 0;
			}

			int32 ArrayIndex = FSUE4LuaStack::To<int32>(L, StackIndex);
			auto ScriptHelper = GetScriptHelper();

			if (ScriptHelper.IsValidIndex(ArrayIndex))
			{
				SUE4LuaUProperty::Push(L, ArrayProperty->Inner, ScriptHelper.GetRawPtr(ArrayIndex), bPassByRef ? Owner : nullptr);
				return 1;
			}
			else
			{
				lua_pushnil(L);
				return 1;
			}
		}
	};
	
	//=============================================================================================================================
	// FMapUserData
	//
	// TMap에 대한 유저 데이터 구현
	//=============================================================================================================================
	struct FMapUserData : public FUPropertyUserData
	{
		const UMapProperty* MapProperty = nullptr;
		void* KeyBuffer = nullptr;		// 키 값을 잠시 저장할 때 사용
		bool bIndexable = false;

		static const char* StaticGetTypeName()
		{
			return "Map";
		}

		// integer Num(self)
		static int32 LuaNum(lua_State* L)
		{
			int32 Num = 0;

			auto UserData = FSUE4LuaUserData::ToUserData<FMapUserData>(L, 1);
			if (UserData != nullptr)
			{
				if (UserData->IsValid())
				{
					Num = UserData->GetScriptHelper().Num();
				}
			}
			else
			{
				SUE4LVM_ERROR(L, TEXT("Map.Num(): Invalid self"));
			}

			FSUE4LuaStack::Push(L, Num);

			return 1;
		}

		// void Empty(self)
		static int32 LuaEmpty(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::ToUserData<FMapUserData>(L, 1);
			if (UserData != nullptr)
			{
				if (UserData->IsValid())
				{
					UserData->GetScriptHelper().EmptyValues();
				}
			}
			else
			{
				SUE4LVM_ERROR(L, TEXT("Map.Empty(): Invalid self"));
			}
		
			return 0;
		}

		// void Add(self, key, value?)
		static int32 LuaAdd(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::ToUserData<FMapUserData>(L, 1);
			if (UserData == nullptr)
			{
				SUE4LVM_ERROR(L, TEXT("Map.Add(): Invalid self"));
				return 0;
			}

			UserData->Add(L, 2);

			return 0;
		}

		// void Remove(self, key)
		static int32 LuaRemove(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::ToUserData<FMapUserData>(L, 1);
			if (UserData == nullptr)
			{
				SUE4LVM_ERROR(L, TEXT("Map.Remove(): Invalid self"));
				return 0;
			}

			UserData->Remove(L, 2);

			return 0;
		}

		// value FindCopy(self, key)
		static int32 LuaFindCopy(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::ToUserData<FMapUserData>(L, 1);
			if (UserData == nullptr)
			{
				SUE4LVM_ERROR(L, TEXT("Map.FindCopy(): Invalid self"));
				return 0;
			}

			return UserData->Find(L, 2, false);
		}

		// value FindRef(self, key)
		static int32 LuaFindRef(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::ToUserData<FMapUserData>(L, 1);
			if (UserData == nullptr)
			{
				SUE4LVM_ERROR(L, TEXT("Map.FindRef(): Invalid self"));
				return 0;
			}

			return UserData->Find(L, 2, true);
		}

		static void RegisterLibrary(lua_State* L)
		{
			lua_pushcfunction(L, &LuaNum);
			lua_setfield(L, -2, "Num");

			lua_pushcfunction(L, &LuaEmpty);
			lua_setfield(L, -2, "Empty");

			lua_pushcfunction(L, &LuaAdd);
			lua_setfield(L, -2, "Add");

			lua_pushcfunction(L, &LuaRemove);
			lua_setfield(L, -2, "Remove");

			lua_pushcfunction(L, &LuaFindCopy);
			lua_setfield(L, -2, "FindCopy");

			lua_pushcfunction(L, &LuaFindRef);
			lua_setfield(L, -2, "FindRef");
		}

		virtual ~FMapUserData()
		{
			if (KeyBuffer != nullptr)
			{
				auto KeyProp = MapProperty->KeyProp;

				KeyProp->DestroyValue(KeyBuffer);
				FMemory::Free(KeyBuffer);
				KeyBuffer = nullptr;
			} 
		}

		FScriptMapHelper GetScriptHelper()
		{
			return FScriptMapHelper(MapProperty, ValueAddress);
		}

		// Begin FUPropertyUserData Interface
		virtual const char* GetTypeName() const override
		{
			return StaticGetTypeName();
		}

		virtual void OnInitialized() override
		{
			MapProperty = static_cast<const UMapProperty*>(Property);
			bIndexable = IsIndexableElementType(MapProperty->ValueProp);
		}

		virtual bool CopyComplete(lua_State* L, const UProperty* SrcProperty, const void* SrcValueAddress, const UProperty* DestProperty, void* DestValueAddress) const override
		{
			if (!IsValid())
			{
				return false;
			}

			return StaticCopyComplete(L, Property->GetName(), SrcProperty, SrcValueAddress, DestProperty, DestValueAddress);
		}

		static bool StaticCopyComplete(lua_State* L, const FString& PropertyName, const UProperty* SrcProperty, const void* SrcValueAddress, const UProperty* DestProperty, void* DestValueAddress)
		{
			auto SrcMapProperty = Cast<UMapProperty>(SrcProperty);
			auto DestMapProperty = Cast<UMapProperty>(DestProperty);

			if (SrcMapProperty && DestMapProperty)
			{
				if (SrcMapProperty->KeyProp->GetClass() == DestMapProperty->KeyProp->GetClass() &&
					SrcMapProperty->ValueProp->GetClass() == DestMapProperty->ValueProp->GetClass())
				{
					DestProperty->CopySingleValue(DestValueAddress, SrcValueAddress);
					return true;
				}
				else
				{
					SUE4LVM_ERROR(L, TEXT("%s: copying to incompatible key-value type([%s,%s] -> [%s,%s])"),
						*PropertyName,
						*SrcMapProperty->KeyProp->GetCPPType(),
						*SrcMapProperty->ValueProp->GetCPPType(),
						*DestMapProperty->KeyProp->GetCPPType(),
						*DestMapProperty->ValueProp->GetCPPType());
					return false;
				}
			}
			else
			{
				SUE4LVM_ERROR(L, TEXT("%s: copying to incompatible type(%s -> %s)"),
					*PropertyName,
					*SrcProperty->GetCPPType(),
					*DestProperty->GetCPPType());
				return false;
			}
		}

		virtual int32 Index(lua_State* L, int32 StackIndex) override
		{
			if (!bIndexable)
			{
				SUE4LVM_WARNING(L, TEXT("'%s': indexing a map of type '%s' via brackets is not safe. Use 'FindCopy()' or 'FindRef()' instead."), *MapProperty->GetName(), *MapProperty->ValueProp->GetCPPType());
			}

			return Find(L, StackIndex, true);
		}

		virtual void NewIndex(lua_State* L, int32 StackIndex) override
		{
			if (!bIndexable)
			{
				SUE4LVM_WARNING(L, TEXT("'%s': indexing a map of type '%s' via brackets is not safe. Use 'Add()' or 'Remove()' instead."), *MapProperty->GetName(), *MapProperty->ValueProp->GetCPPType());
			}

			if (lua_isnil(L, StackIndex + 1))
			{
				Remove(L, StackIndex);
			}
			else
			{
				Add(L, StackIndex);
			}
		}

		virtual void PushIterator(lua_State* L) override
		{
			struct FMapIteratorUserData : public FPropertyIteratorUserData
			{
				int32 Index = 0;

				virtual int32 Next(lua_State* L, FUPropertyUserData* PropertyUserData) override
				{
					auto MapUserData = static_cast<FMapUserData*>(PropertyUserData);
					if (!MapUserData->IsValid())
					{
						return 0;
					}

					auto ScriptHelper = MapUserData->GetScriptHelper();

					while (Index < ScriptHelper.GetMaxIndex())
					{
						int32 CurIndex = Index;
						++Index;

						if (ScriptHelper.IsValidIndex(CurIndex))
						{
							SUE4LuaUProperty::Push(L, ScriptHelper.GetKeyProperty(), ScriptHelper.GetKeyPtr(CurIndex), nullptr);
							SUE4LuaUProperty::Push(L, ScriptHelper.GetValueProperty(), ScriptHelper.GetValuePtr(CurIndex), nullptr);
							return 2;
						}
					}

					return 0;
				}
			};

			auto IteratorUserData = FPropertyIteratorUserData::New<FMapIteratorUserData>(L);
		}
		// End FUPropertyUserData Interface

		bool CopyKeyFromStack(lua_State* L, int32 StackIndex)
		{
			auto KeyProp = MapProperty->KeyProp;

			if (KeyBuffer == nullptr)
			{
				KeyBuffer = FMemory::Malloc(KeyProp->GetSize(), KeyProp->GetMinAlignment());
				KeyProp->InitializeValue(KeyBuffer);
			}

			return SUE4LuaUProperty::CopyToNative(L, StackIndex, KeyProp, KeyBuffer);
		}

		void Add(lua_State* L, int32 StackIndex)
		{
			if (!IsValid())
			{
				return;
			}

			if (!CopyKeyFromStack(L, StackIndex))
			{
				return;
			}

			FScriptMapHelper ScriptHelper = GetScriptHelper();

			void* ValuePtr = ScriptHelper.FindValueFromHash(KeyBuffer);
			if (ValuePtr == nullptr)
			{
				// 키가 없으므로 새로 추가
				{
					int32 MapIndex = ScriptHelper.AddDefaultValue_Invalid_NeedsRehash();
					ScriptHelper.KeyProp->CopySingleValue(ScriptHelper.GetKeyPtr(MapIndex), KeyBuffer);
					ValuePtr = ScriptHelper.GetValuePtr(MapIndex);
				}

				ScriptHelper.Rehash();
			}

			if (StackIndex < lua_gettop(L))
			{
				SUE4LuaUProperty::CopyToNative(L, StackIndex + 1, ScriptHelper.ValueProp, ValuePtr);
			}
			else
			{
				ScriptHelper.ValueProp->ClearValue(ValuePtr);
			}
		}

		void Remove(lua_State* L, int32 StackIndex)
		{
			if (!IsValid())
			{
				return;
			}

			if (!CopyKeyFromStack(L, StackIndex))
			{
				return;
			}

			FScriptMapHelper ScriptHelper = GetScriptHelper();

			if (!ScriptHelper.RemovePair(KeyBuffer))
			{
				SUE4LVM_ERROR(L, TEXT("'%s': key was not found."), *MapProperty->GetName());
				return;
			}
		}

		int32 Find(lua_State* L, int32 StackIndex, bool bPassByRef)
		{
			if (!IsValid())
			{
				return 0;
			}

			if (!CopyKeyFromStack(L, StackIndex))
			{
				return 0;
			}

			FScriptMapHelper ScriptHelper = GetScriptHelper();

			const void* ValuePtr = ScriptHelper.FindValueFromHash(KeyBuffer);
			if (ValuePtr != nullptr)
			{
				SUE4LuaUProperty::Push(L, ScriptHelper.ValueProp, ValuePtr, bPassByRef ? Owner : nullptr);
				return 1;
			}
			else
			{
				lua_pushnil(L);
				return 1;
			}
		}
	};

	//=============================================================================================================================
	// FSetUserData
	//
	// TSet에 대한 유저 데이터 구현
	//=============================================================================================================================
	struct FSetUserData : public FUPropertyUserData
	{
		const USetProperty* SetProperty = nullptr;
		void* ElementBuffer = nullptr;		// 원소 값을 잠시 저장할 때 사용
		bool bIndexable = false;

		static const char* StaticGetTypeName()
		{
			return "Set";
		}

		// integer Num(self)
		static int32 LuaNum(lua_State* L)
		{
			int32 Num = 0;

			auto UserData = FSUE4LuaUserData::ToUserData<FSetUserData>(L, 1);
			if (UserData != nullptr)
			{
				if (UserData->IsValid())
				{
					Num = UserData->GetScriptHelper().Num();
				}
			}
			else
			{
				SUE4LVM_ERROR(L, TEXT("Set.Num(): Invalid self"));
			}

			FSUE4LuaStack::Push(L, Num);

			return 1;
		}

		// void Empty(self)
		static int32 LuaEmpty(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::ToUserData<FSetUserData>(L, 1);
			if (UserData != nullptr)
			{
				if (UserData->IsValid())
				{
					UserData->GetScriptHelper().EmptyElements();
				}
			}
			else
			{
				SUE4LVM_ERROR(L, TEXT("Set.Empty(): Invalid self"));
			}

			return 0;
		}

		// void Add(self, value?)
		static int32 LuaAdd(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::ToUserData<FSetUserData>(L, 1);
			if (UserData == nullptr)
			{
				SUE4LVM_ERROR(L, TEXT("Set.Add(): Invalid self"));
				return 0;
			}

			UserData->Add(L, 2);

			return 0;
		}

		// void Remove(self, value)
		static int32 LuaRemove(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::ToUserData<FSetUserData>(L, 1);
			if (UserData == nullptr)
			{
				SUE4LVM_ERROR(L, TEXT("Set.Remove(): Invalid self"));
				return 0;
			}

			UserData->Remove(L, 2);

			return 0;
		}

		// value FindCopy(self, value)
		static int32 LuaFindCopy(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::ToUserData<FSetUserData>(L, 1);
			if (UserData == nullptr)
			{
				SUE4LVM_ERROR(L, TEXT("Set.FindCopy(): Invalid self"));
				return 0;
			}

			return UserData->Find(L, 2, false);
		}

		// value FindRef(self, value)
		static int32 LuaFindRef(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::ToUserData<FSetUserData>(L, 1);
			if (UserData == nullptr)
			{
				SUE4LVM_ERROR(L, TEXT("Set.FindRef(): Invalid self"));
				return 0;
			}

			return UserData->Find(L, 2, true);
		}

		static void RegisterLibrary(lua_State* L)
		{
			lua_pushcfunction(L, &LuaNum);
			lua_setfield(L, -2, "Num");

			lua_pushcfunction(L, &LuaEmpty);
			lua_setfield(L, -2, "Empty");

			lua_pushcfunction(L, &LuaAdd);
			lua_setfield(L, -2, "Add");

			lua_pushcfunction(L, &LuaRemove);
			lua_setfield(L, -2, "Remove");

			lua_pushcfunction(L, &LuaFindCopy);
			lua_setfield(L, -2, "FindCopy");

			lua_pushcfunction(L, &LuaFindRef);
			lua_setfield(L, -2, "FindRef");
		}

		~FSetUserData()
		{
			if (ElementBuffer != nullptr)
			{
				auto ElementProp = SetProperty->ElementProp;

				ElementProp->DestroyValue(ElementBuffer);
				FMemory::Free(ElementBuffer);
				ElementBuffer = nullptr;
			}
		}

		FScriptSetHelper GetScriptHelper()
		{
			return FScriptSetHelper(SetProperty, ValueAddress);
		}

		// Begin FUPropertyUserData Interface
		virtual const char* GetTypeName() const override
		{
			return StaticGetTypeName();
		}
		
		virtual void OnInitialized() override
		{
			SetProperty = static_cast<const USetProperty*>(Property);
			bIndexable = IsIndexableElementType(SetProperty->ElementProp);
		}

		virtual bool CopyComplete(lua_State* L, const UProperty* SrcProperty, const void* SrcValueAddress, const UProperty* DestProperty, void* DestValueAddress) const override
		{
			if (!IsValid())
			{
				return false;
			}

			return StaticCopyComplete(L, Property->GetName(), SrcProperty, SrcValueAddress, DestProperty, DestValueAddress);
		}

		static bool StaticCopyComplete(lua_State* L, const FString& PropertyName, const UProperty* SrcProperty, const void* SrcValueAddress, const UProperty* DestProperty, void* DestValueAddress)
		{
			auto SrcSetProperty = Cast<USetProperty>(SrcProperty);
			auto DestSetProperty = Cast<USetProperty>(DestProperty);

			if (SrcSetProperty && DestSetProperty)
			{
				if (SrcSetProperty->ElementProp->GetClass() == DestSetProperty->ElementProp->GetClass())
				{
					DestProperty->CopySingleValue(DestValueAddress, SrcValueAddress);
					return true;
				}
				else
				{
					SUE4LVM_ERROR(L, TEXT("%s: copying to incompatible element type(%s -> %s)"),
						*PropertyName,
						*SrcSetProperty->ElementProp->GetCPPType(),
						*DestSetProperty->ElementProp->GetCPPType());
					return false;
				}
			}
			else
			{
				SUE4LVM_ERROR(L, TEXT("%s: copying to incompatible type(%s -> %s)"),
					*PropertyName,
					*SrcProperty->GetCPPType(),
					*DestProperty->GetCPPType());
				return false;
			}
		}

		virtual int32 Index(lua_State* L, int32 StackIndex) override
		{
			if (!bIndexable)
			{
				SUE4LVM_WARNING(L, TEXT("'%s': indexing a set of type '%s' via brackets is not safe. Use 'FindCopy()' or 'FindRef()' instead."), *SetProperty->GetName(), *SetProperty->ElementProp->GetCPPType());
			}

			return Find(L, StackIndex, true);
		}

		virtual void NewIndex(lua_State* L, int32 StackIndex) override
		{
			if (!bIndexable)
			{
				SUE4LVM_WARNING(L, TEXT("'%s': indexing a set of type '%s' via brackets is not safe. Use 'Add()' or 'Remove()' instead."), *SetProperty->GetName(), *SetProperty->ElementProp->GetCPPType());
			}

			if (lua_isnil(L, StackIndex + 1))
			{
				Remove(L, StackIndex);
			}
			else
			{
				Add(L, StackIndex);
			}
		}

		virtual void PushIterator(lua_State* L) override
		{
			struct FSetIteratorUserData : public FPropertyIteratorUserData
			{
				int32 Index = 0;

				virtual int32 Next(lua_State* L, FUPropertyUserData* PropertyUserData) override
				{
					auto SetUserData = static_cast<FSetUserData*>(PropertyUserData);
					if (!SetUserData->IsValid())
					{
						return 0;
					}

					auto ScriptHelper = SetUserData->GetScriptHelper();

					while (Index < ScriptHelper.GetMaxIndex())
					{
						int32 CurIndex = Index;
						++Index;

						if (ScriptHelper.IsValidIndex(CurIndex))
						{
							SUE4LuaUProperty::Push(L, ScriptHelper.ElementProp, ScriptHelper.GetElementPtr(CurIndex), nullptr);
							lua_pushvalue(L, -1);
							return 2;
						}
					}

					return 0;
				}
			};

			auto IteratorUserData = FPropertyIteratorUserData::New<FSetIteratorUserData>(L);
		}
		// End FUPropertyUserData Interface
		
		bool CopyElementFromStack(lua_State* L, int32 StackIndex)
		{
			auto ElementProp = SetProperty->ElementProp;

			if (ElementBuffer == nullptr)
			{
				ElementBuffer = FMemory::Malloc(ElementProp->GetSize(), ElementProp->GetMinAlignment());
				ElementProp->InitializeValue(ElementBuffer);
			}

			return SUE4LuaUProperty::CopyToNative(L, StackIndex, ElementProp, ElementBuffer);
		}

		void Add(lua_State* L, int32 StackIndex)
		{
			if (!IsValid())
			{
				return;
			}

			auto ScriptHelper = GetScriptHelper();

			if (StackIndex <= lua_gettop(L))
			{
				if (!CopyElementFromStack(L, StackIndex))
				{
					return;
				}

				int32 ElementIndex = ScriptHelper.FindElementIndexFromHash(ElementBuffer);
				if (ElementIndex == INDEX_NONE)
				{
					ElementIndex = ScriptHelper.AddDefaultValue_Invalid_NeedsRehash();
					ScriptHelper.Rehash();
				}

				ScriptHelper.ElementProp->CopySingleValue(ScriptHelper.GetElementPtr(ElementIndex), ElementBuffer);
			}
			else
			{
				ScriptHelper.AddDefaultValue_Invalid_NeedsRehash();
				ScriptHelper.Rehash();
			}
		}

		void Remove(lua_State* L, int32 StackIndex)
		{
			if (!IsValid())
			{
				return;
			}

			if (!CopyElementFromStack(L, StackIndex))
			{
				return;
			}

			auto ScriptHelper = GetScriptHelper();

			if (!ScriptHelper.RemoveElement(ElementBuffer))
			{
				SUE4LVM_ERROR(L, TEXT("'%s': element was not found."), *SetProperty->GetName());
				return;
			}
		}

		int32 Find(lua_State* L, int32 StackIndex, bool bPassByRef)
		{
			if (!IsValid())
			{
				return 0;
			}

			if (!CopyElementFromStack(L, StackIndex))
			{
				return 0;
			}

			auto ScriptHelper = GetScriptHelper();

			int32 ElementIndex = ScriptHelper.FindElementIndexFromHash(ElementBuffer);
			if (ElementIndex != INDEX_NONE)
			{
				SUE4LuaUProperty::Push(L, ScriptHelper.ElementProp, ScriptHelper.GetElementPtr(ElementIndex), bPassByRef ? Owner : nullptr);
				return 1;
			}
			else
			{
				lua_pushnil(L);
				return 1;
			}
		}
	};

	//=============================================================================================================================
	// FDelegateUserData
	//
	// 다이나믹 딜리게이트에 대한 유저 데이터 구현
	//=============================================================================================================================
	struct FDelegateUserData : public FUPropertyUserData
	{
		const UDelegateProperty* DelegateProperty = nullptr;

		static const char* StaticGetTypeName()
		{
			return "Delegate";
		}

		// void Bind(self, UObject, FuncName)
		static int32 LuaBind(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::ToUserData<FDelegateUserData>(L, 1);
			if (UserData == nullptr)
			{
				SUE4LVM_ERROR(L, TEXT("Delegate.Bind(): Invalid self"));
				return 0;
			}

			auto Object = FSUE4LuaStack::To<UObject*>(L, 2);
			if (Object == nullptr)
			{
				SUE4LVM_ERROR(L, TEXT("Delegate.Bind(): Invalid arg1"));
				return 0;
			}

			if (!UserData->IsValid())
			{
				return 0;
			}

			FScriptDelegate& Delegate = UserData->GetPropertyValue();

			if (auto LuaValueObj = Cast<USUE4LuaValue>(Object))
			{
				LuaValueObj->Bind(L, Delegate, UserData->GetSignatureFunction());
			}
			else
			{
				auto FuncName = FSUE4LuaStack::To<FName>(L, 3);
				if (FuncName.IsNone())
				{
					SUE4LVM_ERROR(L, TEXT("Delegate.Bind(): Invalid arg2"));
					return 0;
				}

				Delegate.BindUFunction(Object, FuncName);
			}

			return 0;
		}

		// bool IsBound(self)
		static int32 LuaIsBound(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::ToUserData<FDelegateUserData>(L, 1);
			if (UserData == nullptr)
			{
				SUE4LVM_ERROR(L, TEXT("Delegate.IsBound(): Invalid self"));
				return 0;
			}

			if (!UserData->IsValid())
			{
				return 0;
			}

			FSUE4LuaStack::Push(L, UserData->GetPropertyValue().IsBound());

			return 1;
		}

		// void Unbind(self)
		static int32 LuaUnbind(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::ToUserData<FDelegateUserData>(L, 1);
			if (UserData == nullptr)
			{
				SUE4LVM_ERROR(L, TEXT("Delegate.Unbind(): Invalid self"));
				return 0;
			}

			if (!UserData->IsValid())
			{
				return 0;
			}

			if (UserData->GetPropertyValue().IsBound())
			{
				UserData->GetPropertyValue().Unbind();
			}

			return 0;
		}

		static void RegisterLibrary(lua_State* L)
		{
			lua_pushcfunction(L, &LuaBind);
			lua_setfield(L, -2, "Bind");

			lua_pushcfunction(L, &LuaIsBound);
			lua_setfield(L, -2, "IsBound");

			lua_pushcfunction(L, &LuaUnbind);
			lua_setfield(L, -2, "Unbind");
		}

		FScriptDelegate& GetPropertyValue() const
		{
			check(IsValid());

			return *DelegateProperty->GetPropertyValuePtr(ValueAddress);
		}

		UFunction* GetSignatureFunction() const
		{
			check(IsValid());

			return DelegateProperty->SignatureFunction;
		}

		// Begin FUPropertyUserData Interface
		virtual const char* GetTypeName() const override
		{
			return StaticGetTypeName();
		}

		virtual void OnInitialized() override
		{
			DelegateProperty = static_cast<const UDelegateProperty*>(Property);
		}
		// End FUPropertyUserData Interface
	};

	//=============================================================================================================================
	// FMulticastDelegateUserData
	//
	// 다이나믹 멀티캐스트 딜리게이트에 대한 유저 데이터 구현
	//=============================================================================================================================
	struct FMulticastDelegateUserData : public FUPropertyUserData
	{
		const UMulticastDelegateProperty* MulticastDelegateProperty = nullptr;

		static const char* StaticGetTypeName()
		{
			return "MulticastDelegate";
		}

		// void AddUnique(self, UObject*, FuncName)
		static int32 LuaAddUnique(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::ToUserData<FMulticastDelegateUserData>(L, 1);
			if (UserData == nullptr)
			{
				SUE4LVM_ERROR(L, TEXT("MulticastDelegate.AddUnique(): Invalid self"));
				return 0;
			}

			auto Object = FSUE4LuaStack::To<UObject*>(L, 2);
			if (Object == nullptr)
			{
				SUE4LVM_ERROR(L, TEXT("MulticastDelegate.AddUnique(): Invalid arg1"));
				return 0;
			}

			if (!UserData->IsValid())
			{
				return 0;
			}

			FMulticastScriptDelegate& MulticastDelegate = UserData->GetPropertyValue();

			if (auto LuaValueObj = Cast<USUE4LuaValue>(Object))
			{
				LuaValueObj->AddUnique(L, MulticastDelegate, UserData->GetSignatureFunction());
			}
			else
			{
				auto FuncName = FSUE4LuaStack::To<FName>(L, 3);
				if (FuncName.IsNone())
				{
					SUE4LVM_ERROR(L, TEXT("MulticastDelegate.AddUnique(): Invalid arg2"));
					return 0;
				}

				FScriptDelegate Delegate;
				Delegate.BindUFunction(Object, FuncName);

				MulticastDelegate.AddUnique(Delegate);
			}

			return 0;
		}

		// bool Contains(self, UObject, FuncName?)
		static int32 LuaContains(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::ToUserData<FMulticastDelegateUserData>(L, 1);
			if (UserData == nullptr)
			{
				SUE4LVM_ERROR(L, TEXT("MulticastDelegate.Contains(): Invalid self"));
				return 0;
			}

			auto Object = FSUE4LuaStack::To<UObject*>(L, 2);
			if (Object == nullptr)
			{
				SUE4LVM_ERROR(L, TEXT("MulticastDelegate.Contains(): Invalid arg1"));
				return 0;
			}

			if (!UserData->IsValid())
			{
				return 0;
			}

			FMulticastScriptDelegate& MulticastDelegate = UserData->GetPropertyValue();
			FName FuncName;

			if (Object->IsA<USUE4LuaValue>())
			{
				FuncName = GET_FUNCTION_NAME_CHECKED(USUE4LuaValue, DummyFunc);
			}
			else
			{
				FuncName = FSUE4LuaStack::To<FName>(L, 3);
				if (FuncName.IsNone())
				{
					SUE4LVM_ERROR(L, TEXT("MulticastDelegate.Contains(): Invalid arg2"));
					return 0;
				}
			}

			FSUE4LuaStack::Push(L, MulticastDelegate.Contains(Object, FuncName));
			return 1;
		}

		// void RemoveAll(self, UObject)
		static int32 LuaRemoveAll(lua_State* L)
		{
			auto UserData = FSUE4LuaUserData::ToUserData<FMulticastDelegateUserData>(L, 1);
			if (UserData == nullptr)
			{
				SUE4LVM_ERROR(L, TEXT("MulticastDelegate.Remove(): Invalid self"));
				return 0;
			}

			auto Object = FSUE4LuaStack::To<UObject*>(L, 2);
			if (Object == nullptr)
			{
				SUE4LVM_ERROR(L, TEXT("MulticastDelegate.Remove(): Invalid arg1"));
				return 0;
			}

			if (!UserData->IsValid())
			{
				return 0;
			}

			FMulticastScriptDelegate& MulticastDelegate = UserData->GetPropertyValue();

			MulticastDelegate.RemoveAll(Object);

			return 0;
		}

		static void RegisterLibrary(lua_State* L)
		{
			lua_pushcfunction(L, &LuaAddUnique);
			lua_setfield(L, -2, "AddUnique");

			lua_pushcfunction(L, &LuaContains);
			lua_setfield(L, -2, "Contains");

			lua_pushcfunction(L, &LuaRemoveAll);
			lua_setfield(L, -2, "RemoveAll");
		}

		FMulticastScriptDelegate& GetPropertyValue() const
		{
			check(IsValid());

			return *reinterpret_cast<FMulticastScriptDelegate*>(ValueAddress);
		}

		UFunction* GetSignatureFunction() const
		{
			check(IsValid());

			return MulticastDelegateProperty->SignatureFunction;
		}

		// Begin FUPropertyUserData Interface
		virtual const char* GetTypeName() const override
		{
			return StaticGetTypeName();
		}

		virtual void OnInitialized() override
		{
			MulticastDelegateProperty = static_cast<const UMulticastDelegateProperty*>(Property);
		}
		// End FUPropertyUserData Interface
	};

	//=============================================================================================================================
	// SUE4LuaUProperty
	//=============================================================================================================================
	template<typename UserDataType>
	static UserDataType* NewUserData(lua_State* L, const UProperty* Property, const void* ValueAddress, FSUE4LuaNativeValue* Owner)
	{
		UserDataType* UserData = FSUE4LuaUserData::NewUserData<UserDataType>(L);
		UserData->Initialize(Property, ValueAddress, Owner);

		// 메타 테이블 설정
		{
			// Stack: UserData
			lua_getglobal(L, "SUE4LuaBinding");
			lua_getfield(L, -1, "upropertyProxyMTs");
			lua_getfield(L, -1, UserData->GetTypeName());
			// Stack: UserData, SUE4LuaBinding, upropertyProxyMTs, upropertyProxyMT
			lua_setmetatable(L, -4);
			lua_pop(L, 2);
			// Stack: UserData
		}

		// Stack: UserData
		return UserData;
	}

	static bool TryPushPrimitiveValueToStack(lua_State* L, const UProperty* Property, const void* ValueAddress)
	{
		#define IMPLEMENT_TryPushPrimitiveValueToStack(PropertyType)												\
		if (auto CastedType = Cast<PropertyType>(Property))															\
		{																											\
			PropertyType::TCppType Value = {};																		\
			CastedType->SetPropertyValue(&Value, *reinterpret_cast<const PropertyType::TCppType*>(ValueAddress));	\
			FSUE4LuaStack::Push(L, Value);																			\
			return true;																							\
		}
		IMPLEMENT_TryPushPrimitiveValueToStack(UBoolProperty);
		IMPLEMENT_TryPushPrimitiveValueToStack(UInt8Property);
		IMPLEMENT_TryPushPrimitiveValueToStack(UByteProperty);
		IMPLEMENT_TryPushPrimitiveValueToStack(UInt16Property);
		IMPLEMENT_TryPushPrimitiveValueToStack(UUInt16Property);
		IMPLEMENT_TryPushPrimitiveValueToStack(UIntProperty);
		IMPLEMENT_TryPushPrimitiveValueToStack(UUInt32Property);
		IMPLEMENT_TryPushPrimitiveValueToStack(UInt64Property);
		IMPLEMENT_TryPushPrimitiveValueToStack(UUInt64Property);
		IMPLEMENT_TryPushPrimitiveValueToStack(UFloatProperty);
		IMPLEMENT_TryPushPrimitiveValueToStack(UDoubleProperty);
		IMPLEMENT_TryPushPrimitiveValueToStack(UStrProperty);
		IMPLEMENT_TryPushPrimitiveValueToStack(UNameProperty);
		IMPLEMENT_TryPushPrimitiveValueToStack(UTextProperty);
		#undef IMPLEMENT_TryPushPrimitiveValueToStack

		if (auto EnumProperty = Cast<UEnumProperty>(Property))
		{
			return TryPushPrimitiveValueToStack(L, EnumProperty->GetUnderlyingProperty(), ValueAddress);
		}

		return false;
	}

	static bool TryPushUObjectToStack(lua_State* L, const UProperty* Property, const void* ValueAddress)
	{
		if (auto ObjectProperty = Cast<UObjectPropertyBase>(Property))
		{
			UObject* Value = ObjectProperty->GetObjectPropertyValue(ValueAddress);

			if (auto LuaValueObj = Cast<USUE4LuaValue>(Value))
			{
				LuaValueObj->Push(L);
			}
			else
			{
				SUE4LuaUObject::PushRef(L, Value);				
			}

			return true;
		}

		return false;
	}

	static bool TryPushFixedArrayToStack(lua_State* L, const UProperty* Property, const void* ValueAddress, FSUE4LuaNativeValue* Owner)
	{
		if (1 < Property->ArrayDim)
		{
			auto UserData = NewUserData<FFixedArrayUserData>(L, Property, ValueAddress, Owner);
			
			// Stack: UserData
			return true;
		}

		return false;
	}

	static bool TryPushArrayToStack(lua_State* L, const UProperty* Property, const void* ValueAddress, FSUE4LuaNativeValue* Owner)
	{
		if (auto ArrayProperty = Cast<UArrayProperty>(Property))
		{
			auto UserData = NewUserData<FArrayUserData>(L, Property, ValueAddress, Owner);

			// Stack: UserData
			return true;
		}

		return false;
	}

	static bool TryPushMapToStack(lua_State* L, const UProperty* Property, const void* ValueAddress, FSUE4LuaNativeValue* Owner)
	{
		if (auto MapProperty = Cast<UMapProperty>(Property))
		{
			auto UserData = NewUserData<FMapUserData>(L, Property, ValueAddress, Owner);

			// Stack: UserData
			return true;
		}

		return false;
	}

	static bool TryPushSetToStack(lua_State* L, const UProperty* Property, const void* ValueAddress, FSUE4LuaNativeValue* Owner)
	{
		if (auto SetProperty = Cast<USetProperty>(Property))
		{
			auto UserData = NewUserData<FSetUserData>(L, Property, ValueAddress, Owner);

			// Stack: UserData
			return true;
		}

		return false;
	}

	static bool TryPushStructToStack(lua_State* L, const UProperty* Property, const void* ValueAddress, FSUE4LuaNativeValue* Owner)
	{
		if (Property->IsA<UStructProperty>())
		{
			auto StructProperty = Cast<UStructProperty>(Property);

			#define IMPLEMENT_TryPushStructToStack(TypeName)				\
			if (StructProperty->Struct->GetFName() == NAME_##TypeName)		\
			{																\
				F##TypeName Value;											\
				Property->CopySingleValue(&Value, ValueAddress);			\
				FSUE4LuaStack::Push(L, Value);								\
				return true;												\
			}
			IMPLEMENT_TryPushStructToStack(Vector);
			IMPLEMENT_TryPushStructToStack(Vector2D);
			IMPLEMENT_TryPushStructToStack(Vector4);
			IMPLEMENT_TryPushStructToStack(IntPoint);
			IMPLEMENT_TryPushStructToStack(IntVector);
			IMPLEMENT_TryPushStructToStack(Plane);
			IMPLEMENT_TryPushStructToStack(Matrix);
			IMPLEMENT_TryPushStructToStack(Rotator);
			IMPLEMENT_TryPushStructToStack(Quat);
			IMPLEMENT_TryPushStructToStack(LinearColor);
			IMPLEMENT_TryPushStructToStack(Color);
			IMPLEMENT_TryPushStructToStack(Transform);
			IMPLEMENT_TryPushStructToStack(Box);
			IMPLEMENT_TryPushStructToStack(Box2D);
			IMPLEMENT_TryPushStructToStack(BoxSphereBounds);
			IMPLEMENT_TryPushStructToStack(Guid);
			IMPLEMENT_TryPushStructToStack(DateTime);
			IMPLEMENT_TryPushStructToStack(Timecode);
			IMPLEMENT_TryPushStructToStack(Timespan);
			IMPLEMENT_TryPushStructToStack(RandomStream);
			IMPLEMENT_TryPushStructToStack(FrameRate);
			IMPLEMENT_TryPushStructToStack(FrameNumber);
			IMPLEMENT_TryPushStructToStack(FrameTime);
			IMPLEMENT_TryPushStructToStack(QualifiedFrameTime);
			IMPLEMENT_TryPushStructToStack(PrimaryAssetType);
			IMPLEMENT_TryPushStructToStack(PrimaryAssetId);
			IMPLEMENT_TryPushStructToStack(SoftObjectPath);
			IMPLEMENT_TryPushStructToStack(FloatRangeBound);
			IMPLEMENT_TryPushStructToStack(FloatRange);
			IMPLEMENT_TryPushStructToStack(Int32Interval);
			#undef IMPLEMENT_TryPushStructToStack

			// 일반 구조체 처리
			if (Owner != nullptr)
			{
				SUE4LuaUStruct::PushRef(L, StructProperty->Struct, ValueAddress, Owner);
			}
			else
			{
				SUE4LuaUStruct::PushValue(L, StructProperty->Struct, ValueAddress);
			}

			// Stack: UserData
			return true;
		}

		return false;
	}

	static bool TryPushDelegateToStack(lua_State* L, const UProperty* Property, const void* ValueAddress, FSUE4LuaNativeValue* Owner)
	{
		if (auto DelegateProperty = Cast<UDelegateProperty>(Property))
		{
			auto UserData = NewUserData<FDelegateUserData>(L, Property, ValueAddress, Owner);

			// Stack: UserData
			return true;
		}

		return false;
	}

	static bool TryPushMulticastDelegateToStack(lua_State* L, const UProperty* Property, const void* ValueAddress, FSUE4LuaNativeValue* Owner)
	{
		if (auto MulticastDelegateProperty = Cast<UMulticastDelegateProperty>(Property))
		{
			auto UserData = NewUserData<FMulticastDelegateUserData>(L, Property, ValueAddress, Owner);

			// Stack: UserData
			return true;
		}

		return false;
	}

	// 고정크기배열이 아닌 프로퍼티를 스택에 푸시
	static void PushNonFixedArray(lua_State* L, const UProperty* Property, const void* ValueAddress, FSUE4LuaNativeValue* Owner)
	{
		if (TryPushPrimitiveValueToStack(L, Property, ValueAddress))
		{
			return;
		}

		if (TryPushUObjectToStack(L, Property, ValueAddress))
		{
			return;
		}

		if (TryPushStructToStack(L, Property, ValueAddress, Owner))
		{
			return;
		}

		if (TryPushArrayToStack(L, Property, ValueAddress, Owner))
		{
			return;
		}

		if (TryPushMapToStack(L, Property, ValueAddress, Owner))
		{
			return;
		}

		if (TryPushSetToStack(L, Property, ValueAddress, Owner))
		{
			return;
		}

		if (TryPushDelegateToStack(L, Property, ValueAddress, Owner))
		{
			return;
		}

		if (TryPushMulticastDelegateToStack(L, Property, ValueAddress, Owner))
		{
			return;
		}

		SUE4LVM_ERROR(L, TEXT("%s: type '%s' is not supported."), *Property->GetName(), *Property->GetCPPType());

		// nil이라도 넣어야 스택이 안꼬임
		lua_pushnil(L);
	}

	static bool IsUPropertyUserData(lua_State* L, int32 StackIndex)
	{
		bool bUPropertyUserData = false;

		if (lua_isuserdata(L, StackIndex))
		{
			lua_getmetatable(L, StackIndex);
			// Stack: MT

			if (lua_istable(L, -1))
			{
				lua_getfield(L, -1, "isUPropertyProxy");
				if (lua_isboolean(L, -1) && lua_toboolean(L, -1))
				{
					bUPropertyUserData = true;
				}
				lua_pop(L, 1);
			}

			lua_pop(L, 1);
			// Stack: (empty)
		}

		return bUPropertyUserData;
	}

	bool IsFixedArray(lua_State* L, int32 StackIndex)
	{
		if (IsUPropertyUserData(L, StackIndex))
		{
			auto PropertyUserData = FSUE4LuaUserData::ToUserData<FUPropertyUserData>(L, StackIndex);
			if (PropertyUserData != nullptr)
			{
				return PropertyUserData->GetTypeName() == FFixedArrayUserData::StaticGetTypeName();
			}
		}

		return false;
	}

	bool IsArray(lua_State* L, int32 StackIndex)
	{
		if (IsUPropertyUserData(L, StackIndex))
		{
			auto PropertyUserData = FSUE4LuaUserData::ToUserData<FUPropertyUserData>(L, StackIndex);
			if (PropertyUserData != nullptr)
			{
				return PropertyUserData->GetTypeName() == FArrayUserData::StaticGetTypeName();
			}
		}

		return false;
	}

	bool IsMap(lua_State* L, int32 StackIndex)
	{
		if (IsUPropertyUserData(L, StackIndex))
		{
			auto PropertyUserData = FSUE4LuaUserData::ToUserData<FUPropertyUserData>(L, StackIndex);
			if (PropertyUserData != nullptr)
			{
				return PropertyUserData->GetTypeName() == FMapUserData::StaticGetTypeName();
			}
		}

		return false;
	}

	bool IsSet(lua_State* L, int32 StackIndex)
	{
		if (IsUPropertyUserData(L, StackIndex))
		{
			auto PropertyUserData = FSUE4LuaUserData::ToUserData<FUPropertyUserData>(L, StackIndex);
			if (PropertyUserData != nullptr)
			{
				return PropertyUserData->GetTypeName() == FSetUserData::StaticGetTypeName();
			}
		}

		return false;
	}

	void Push(lua_State* L, const UProperty* Property, const void* ValueAddress, FSUE4LuaNativeValue* Owner)
	{
		check(Property != nullptr);
		check(ValueAddress != nullptr);

		// 고정크기배열은 기본타입과 프로퍼티 클래스가 같기 때문에 먼저 처리해야 합니다.
		if (TryPushFixedArrayToStack(L, Property, ValueAddress, Owner))
		{
			return;
		}

		PushNonFixedArray(L, Property, ValueAddress, Owner);
	}

	bool CopyNonFixedArrayToNative(lua_State* L, int32 StackIndex, const UProperty* DestProperty, void* DestValueAddress)
	{
		check(DestProperty != nullptr);
		check(DestValueAddress != nullptr);

		if (DestProperty->IsA<UBoolProperty>())
		{
			auto Value = FSUE4LuaStack::To<bool>(L, StackIndex);

			DestProperty->CopySingleValue(DestValueAddress, &Value);
			return true;
		}
		else if (auto NumericProperty = Cast<UNumericProperty>(DestProperty))
		{
			if (NumericProperty->IsInteger())
			{
				auto Value = FSUE4LuaStack::To<int64>(L, StackIndex);

				NumericProperty->SetIntPropertyValue(DestValueAddress, Value);
				return true;
			}
			else if (NumericProperty->IsFloatingPoint())
			{
				auto Value = FSUE4LuaStack::To<double>(L, StackIndex);

				NumericProperty->SetFloatingPointPropertyValue(DestValueAddress, Value);
				return true;
			}
		}
		else if (auto EnumProperty = Cast<UEnumProperty>(DestProperty))
		{
			auto Value = FSUE4LuaStack::To<int64>(L, StackIndex);

			EnumProperty->GetUnderlyingProperty()->SetIntPropertyValue(DestValueAddress, Value);
			return true;
		}
		else if (DestProperty->IsA<UNameProperty>())
		{
			auto Value = FSUE4LuaStack::To<FName>(L, StackIndex);

			DestProperty->CopySingleValue(DestValueAddress, &Value);
			return true;
		}
		else if (DestProperty->IsA<UStrProperty>())
		{
			auto Value = FSUE4LuaStack::To<FString>(L, StackIndex);

			DestProperty->CopySingleValue(DestValueAddress, &Value);
			return true;
		}
		else if (DestProperty->IsA<UTextProperty>())
		{
			auto Value = FSUE4LuaStack::To<FText>(L, StackIndex);

			DestProperty->CopySingleValue(DestValueAddress, &Value);
			return true;
		}
		else if (auto ObjectProperty = Cast<UObjectPropertyBase>(DestProperty))
		{
			if (ObjectProperty->PropertyClass->IsChildOf<USUE4LuaValue>())
			{
				auto Value = USUE4LuaValue::CreateFromStack(L, StackIndex);

				ObjectProperty->SetObjectPropertyValue(DestValueAddress, Value);
			}
			else
			{
				auto Value = FSUE4LuaStack::To<UObject*>(L, StackIndex);

				ObjectProperty->SetObjectPropertyValue(DestValueAddress, Value);
			}

			return true;
		}
		else if (auto StructProperty = Cast<UStructProperty>(DestProperty))
		{
			#define IMPLEMENT_To(TypeName)																		\
			if (StructProperty->Struct->GetFName() == NAME_##TypeName)											\
			{																									\
				auto Value = FSUE4LuaStack::To<F##TypeName>(L, StackIndex);										\
				DestProperty->CopySingleValue(DestValueAddress, &Value);										\
				return true;																					\
			}
			IMPLEMENT_To(Vector);
			IMPLEMENT_To(Vector2D);
			IMPLEMENT_To(Vector4);
			IMPLEMENT_To(IntPoint);
			IMPLEMENT_To(IntVector);
			IMPLEMENT_To(Plane);
			IMPLEMENT_To(Matrix);
			IMPLEMENT_To(Rotator);
			IMPLEMENT_To(Quat);
			IMPLEMENT_To(LinearColor);
			IMPLEMENT_To(Color);
			IMPLEMENT_To(Transform);
			IMPLEMENT_To(Box);
			IMPLEMENT_To(Box2D);
			IMPLEMENT_To(BoxSphereBounds);
			IMPLEMENT_To(Guid);
			IMPLEMENT_To(DateTime);
			IMPLEMENT_To(Timecode);
			IMPLEMENT_To(Timespan);
			IMPLEMENT_To(RandomStream);
			IMPLEMENT_To(FrameRate);
			IMPLEMENT_To(FrameNumber);
			IMPLEMENT_To(FrameTime);
			IMPLEMENT_To(QualifiedFrameTime);
			IMPLEMENT_To(PrimaryAssetType);
			IMPLEMENT_To(PrimaryAssetId);
			IMPLEMENT_To(SoftObjectPath);
			IMPLEMENT_To(FloatRangeBound);
			IMPLEMENT_To(FloatRange);
			IMPLEMENT_To(Int32Interval);
			#undef IMPLEMENT_To

			// 일반 구조체 처리
			return SUE4LuaUStruct::CopyToNative(L, StackIndex, StructProperty->Struct, DestValueAddress);
		}
		else
		{
			const char* ProxyTypeName = nullptr;
			
			if (DestProperty->IsA<UArrayProperty>())
			{
				ProxyTypeName = FArrayUserData::StaticGetTypeName();
			}
			else if (DestProperty->IsA<UMapProperty>())
			{
				ProxyTypeName = FMapUserData::StaticGetTypeName();
			}
			else if (DestProperty->IsA<USetProperty>())
			{
				ProxyTypeName = FSetUserData::StaticGetTypeName();
			}
			else if (DestProperty->IsA<UDelegateProperty>())
			{
				ProxyTypeName = FDelegateUserData::StaticGetTypeName();
			}
			else if (DestProperty->IsA<UMulticastDelegateProperty>())
			{
				ProxyTypeName = FMulticastDelegateUserData::StaticGetTypeName();
			}
			else
			{
				SUE4LVM_ERROR(L, TEXT("%s: type '%s' is not supported."), *DestProperty->GetName(), *DestProperty->GetCPPType());
				return false;
			}

			if (IsUPropertyUserData(L, StackIndex))
			{
				auto PropertyUserData = FSUE4LuaUserData::ToUserData<FUPropertyUserData>(L, StackIndex);
				if (PropertyUserData != nullptr)
				{
					return PropertyUserData->CopyComplete(L, PropertyUserData->GetProperty(), PropertyUserData->GetValueAddress(), DestProperty, DestValueAddress);
				}
			}
			else
			{
				SUE4LVM_WARNING(L, TEXT("%s: expected %s."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, StackIndex))), ANSI_TO_TCHAR(ProxyTypeName));
				return false;
			}
		}		

		return false;
	}

	bool CopyToNative(lua_State* L, int32 StackIndex, const UProperty* DestProperty, void* DestValueAddress)
	{
		check(DestProperty != nullptr);
		check(DestValueAddress != nullptr);

		// 고정타입 배열은 타입을 보는 것이 아니고 배열 크기를 봐야 하므로 가장 먼저 검사해야 합니다.
		if (1 < DestProperty->ArrayDim)
		{
			if (IsUPropertyUserData(L, StackIndex))
			{
				auto PropertyUserData = FSUE4LuaUserData::ToUserData<FUPropertyUserData>(L, StackIndex);
				if (PropertyUserData != nullptr)
				{
					return PropertyUserData->CopyComplete(L, PropertyUserData->GetProperty(), PropertyUserData->GetValueAddress(), DestProperty, DestValueAddress);
				}
			}
			else
			{
				const char* ProxyTypeName = FFixedArrayUserData::StaticGetTypeName();

				SUE4LVM_WARNING(L, TEXT("%s: expected %s."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, StackIndex))), ANSI_TO_TCHAR(ProxyTypeName));
				return false;
			}
		}
		else
		{
			return CopyNonFixedArrayToNative(L, StackIndex, DestProperty, DestValueAddress);
		}

		return false;
	}

	bool TryCopyToLua(lua_State* L, int32 StackIndex, const UProperty* SrcProperty, const void* SrcValueAddress)
	{
		check(SrcProperty != nullptr);
		check(SrcValueAddress != nullptr);

		if (IsUPropertyUserData(L, StackIndex))
		{
			auto PropertyUserData = FSUE4LuaUserData::ToUserData<FUPropertyUserData>(L, StackIndex);
			return PropertyUserData->CopyComplete(L, SrcProperty, SrcValueAddress, PropertyUserData->GetProperty(), PropertyUserData->GetValueAddress());
		}
		else if (auto StructProperty = Cast<UStructProperty>(SrcProperty))
		{
			return SUE4LuaUStruct::TryCopyToLua(L, StackIndex, StructProperty->Struct, SrcValueAddress);
		}

		return false;
	}

	static int32 LuaGC(lua_State* L)
	{
		auto PropertyUserData = FSUE4LuaUserData::ToUserData<FUPropertyUserData>(L, 1);
		PropertyUserData->Dispose();

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

		auto PropertyUserData = FSUE4LuaUserData::ToUserData<FUPropertyUserData>(L, 1);
		if (!PropertyUserData->IsValid())
		{
			return 0;
		}

		return PropertyUserData->Index(L, 2);
	}

	static int32 LuaNewIndex(lua_State* L)
	{
		auto PropertyUserData = FSUE4LuaUserData::ToUserData<FUPropertyUserData>(L, 1);
		if (!PropertyUserData->IsValid())
		{
			return 0;
		}

		PropertyUserData->NewIndex(L, 2);

		return 0;
	}

	static int32 LuaIterator(lua_State* L)
	{
		auto PropertyUserData = FSUE4LuaUserData::ToUserData<FUPropertyUserData>(L, lua_upvalueindex(1));
		auto PropertyIteratorUserData = FSUE4LuaUserData::ToUserData<FPropertyIteratorUserData>(L, 1);
		
		return PropertyIteratorUserData->Next(L, PropertyUserData);
	}

	static int32 LuaPairs(lua_State* L)
	{
		auto PropertyUserData = FSUE4LuaUserData::ToUserData<FUPropertyUserData>(L, 1);

		lua_pushvalue(L, 1);
		lua_pushcclosure(L, &LuaIterator, 1);
		PropertyUserData->PushIterator(L);
		// Stack: LuaPairsIterator, PropertyIteratorUserData

		return 2;
	}

	// 메타테이블을 만들고 lua 스택에 푸시합니다.
	static void MakeProxyMetaTable(lua_State* L)
	{
		lua_newtable(L);

		lua_pushboolean(L, true);
		lua_setfield(L, -2, "isUPropertyProxy");

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

	template<typename UserDataType>
	static void RegisterProxyMetaTable(lua_State* L)
	{
		MakeProxyMetaTable(L);
		UserDataType::RegisterLibrary(L);
		lua_setfield(L, -2, UserDataType::StaticGetTypeName());
	}

	void Register(lua_State* L)
	{
		lua_getglobal(L, "SUE4LuaBinding");
		lua_getfield(L, -1, "upropertyProxyMTs");
		lua_remove(L, -2);
		// Stack: upropertyProxyMTs

		// 각 프록시 타입 별 메타테이블 등록
		{
			RegisterProxyMetaTable<FFixedArrayUserData>(L);
			RegisterProxyMetaTable<FArrayUserData>(L);
			RegisterProxyMetaTable<FMapUserData>(L);
			RegisterProxyMetaTable<FSetUserData>(L);
			RegisterProxyMetaTable<FDelegateUserData>(L);
			RegisterProxyMetaTable<FMulticastDelegateUserData>(L);
		}

		// Stack: upropertyProxyMTs
		lua_pop(L, 1);
		// Stack: (empty)
	}
}