// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/RefCounting.h"
#include "UObject/StructOnScope.h"
#include "UObject/WeakObjectPtr.h"


//
// lua에서 UObject, UStruct 등을 참조할 때 사용합니다.
//
class FSUE4LuaNativeValue : public FRefCountedObject
{
public:
	FSUE4LuaNativeValue();
	virtual ~FSUE4LuaNativeValue();

	FORCEINLINE bool IsValid()
	{
		return GetValueAddress() != nullptr;
	}

	// 값의 주소를 반환한다. 
	// UObject*처럼 약참조되는 값들은 null을 반환할 수 있다.
	virtual void* GetValueAddress() = 0;

public:
	static int32 GetInstanceCount() { return InstanceCount; }

	static int32 InstanceCount;
};

//
// UObject에 대한 FSUE4LuaNativeValue 구현
// 대상 UObject에 대한 약참조를 갖고 있습니다.
//
class FSUE4LuaUObjectValue : public FSUE4LuaNativeValue
{
public:
	FSUE4LuaUObjectValue(UObject* InTargetUObject);

public:
	// Begin FSUE4LuaNativeValue Interface
	virtual void* GetValueAddress() override;
	// End FSUE4LuaNativeValue Interface

private:
	TWeakObjectPtr<UObject> TargetUObject;
};

//
// 동적으로 생성한 UStruct에 대한 FSUE4LuaNativeValue 구현
//
class FSUE4LuaUStructValue : public FSUE4LuaNativeValue
{
public:
	FSUE4LuaUStructValue(const UStruct* InStruct);

public:
	// Begin FSUE4LuaNativeValue Interface
	virtual void* GetValueAddress() override;
	// End FSUE4LuaNativeValue Interface

private:
	FStructOnScope StructInstance;
};

//
// 동적으로 생성한 UProperty에 대한 FSUE4LuaNativeValue 구현. 
// UStructProperty는 이 클래스 대신 FSUE4LuaUStructValue를 사용합니다.
//
class FSUE4LuaUPropertyValue : public FSUE4LuaNativeValue
{
public:
	FSUE4LuaUPropertyValue(const UProperty* InProperty);
	virtual ~FSUE4LuaUPropertyValue();

public:
	// Begin FSUE4LuaNativeValue Interface
	virtual void* GetValueAddress() override;
	// End FSUE4LuaNativeValue Interface

private:
	const UProperty* Property = nullptr;
	void* Buffer = nullptr;
};
