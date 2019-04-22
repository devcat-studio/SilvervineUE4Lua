
Lua에서 UObject 사용하기
=======================

UObject는 Lua에서 가장 많이 사용하는 엔진 타입입니다. 
SUE4Lua는 UObject를 _userdata_ 형식의 프록시 객체로 감싸써 Lua에 전달합니다.
이 프록시 객체를 사용하면 마치 Lua 테이블 처럼 UObject를 다룰 수 있어서 코딩할 때 매우 편리합니다.

아래의 모든 테스트 코드는 [여기](../Source/SilvervineUE4Lua/Private/Scripts/Tests/UObjectTest.lua)에 있습니다.

참고:
* UObject의 모든 멤버를 Lua에서 사용할 수 있는 것은 아닙니다. UPROPERTY()와 UFUNCTION()가 붙은 멤버만 Lua에서 사용할 수 있습니다.
* UFUNCTION()은 BlueprintCallable일 필요가 없습니다. `UFUNCTION()` 만 붙어 있어도 Lua에서 사용할 수 있습니다.
* C++의 접근 지정자(private, protected)는 Lua에 영향을 주지 않습니다. private 멤버도 Lua에서 사용할 수 있습니다. 

기본
----

대부분의 C++ 기본형 속성 멤버를 Lua에서 사용할 수 있습니다.
```lua
Obj.IntProperty = 123
SUE4Lua.Log("Obj.IntProperty", Obj.IntProperty) -- Obj.IntProperty	123
```

enum은 정수로 처리됩니다. UEnum은 [여기](WorkWithUEnum_ko.md)를 참고하세요.
```lua
Obj.EnumProperty = 1
SUE4Lua.Log("Obj.EnumProperty", Obj.EnumProperty)   -- Obj.EnumProperty	1
```

`FName`, `FString`, `FText`은 Lua에서 모두 `string` 타입으로 처리됩니다.
```lua
Obj.NameProperty = "name"
SUE4Lua.Log("Obj.NameProperty", Obj.NameProperty)   -- Obj.NameProperty	name
```

UObject 멤버도 사용할 수 있습니다.
```lua
Obj.UObjectProperty = Obj
SUE4Lua.Log("type(Obj.UObjectProperty)", type(Obj.UObjectProperty))
-- type(Obj.UObjectProperty)	userdata
```

구조체 멤버도 사용할 수 있습니다.
```lua
Obj.StructProperty.IntProperty = 123
SUE4Lua.Log("Obj.StructProperty.IntProperty", Obj.StructProperty.IntProperty)
Obj.NestedStructProperty.StructProperty.IntProperty = 123
SUE4Lua.Log("Obj.NestedStructProperty.StructProperty.IntProperty", Obj.NestedStructProperty.StructProperty.IntProperty)
-- Obj.StructProperty.IntProperty	123
-- Obj.NestedStructProperty.StructProperty.IntProperty	123
```

배열은 테이블처럼 다룰 수 있습니다.
```lua
for i = 0, 9 do
    Obj.FixedIntArrayProperty[i] = i * i
end
for k, v in pairs(Obj.FixedIntArrayProperty) do
    SUE4Lua.Log("Obj.FixedIntArrayProperty: k,v = ", k, v)
end
-- Obj.FixedIntArrayProperty: k,v = 	0	0
-- Obj.FixedIntArrayProperty: k,v = 	1	1
-- Obj.FixedIntArrayProperty: k,v = 	2	4
-- Obj.FixedIntArrayProperty: k,v = 	3	9
-- Obj.FixedIntArrayProperty: k,v = 	4	16
-- Obj.FixedIntArrayProperty: k,v = 	5	25
-- Obj.FixedIntArrayProperty: k,v = 	6	36
-- Obj.FixedIntArrayProperty: k,v = 	7	49
-- Obj.FixedIntArrayProperty: k,v = 	8	64
```

참고:
* 배열을 ipairs()로 순회하면 0번 인덱스가 무시되므로 배열에 ipairs()를 사용하지 않는 것이 좋습니다.

`FVector`와 같은 구조체의 배열도 사용할 수 있습니다.
```lua
Obj.FixedVectorArrayProperty[4] = UE4.Vector.new(1, 2, 3)
SUE4Lua.Log("Obj.FixedVectorArrayProperty[4].X", Obj.FixedVectorArrayProperty[4].X)
SUE4Lua.Log("Obj.FixedVectorArrayProperty[4].Y", Obj.FixedVectorArrayProperty[4].Y)
SUE4Lua.Log("Obj.FixedVectorArrayProperty[4].Z", Obj.FixedVectorArrayProperty[4].Z)
-- Obj.FixedVectorArrayProperty[4].X	1.0
-- Obj.FixedVectorArrayProperty[4].Y	2.0
-- Obj.FixedVectorArrayProperty[4].Z	3.0
```

TArray
------

TArray의 Add(), Remove, Insert(), Empty(), Num() 함수를 Lua에서 그대로 사용할 수 있습니다.
```lua
Obj.IntArrayProperty:Empty()
Obj.IntArrayProperty:Add(123)
Obj.IntArrayProperty:Insert(0, 456)
for k, v in pairs(Obj.IntArrayProperty) do
    SUE4Lua.Log("Obj.IntArrayProperty: k,v = ", k, v)
end
Obj.IntArrayProperty:Remove(0)
SUE4Lua.Log("Obj.IntArrayProperty:Num()", Obj.IntArrayProperty:Num())
Obj.IntArrayProperty:Empty()
SUE4Lua.Log("Obj.IntArrayProperty:Num()", Obj.IntArrayProperty:Num())
-- Obj.IntArrayProperty: k,v = 	0	456
-- Obj.IntArrayProperty: k,v = 	1	123
-- Obj.IntArrayProperty:Num()	1
-- Obj.IntArrayProperty:Num()	0
```

원소가 기본형이면 `[]`로 접근할 수 있습니다.
```lua
Obj.IntArrayProperty:Empty()
Obj.IntArrayProperty:Add(123)
SUE4Lua.Log("Obj.IntArrayProperty[0]", Obj.IntArrayProperty[0])
```

TMap
----

TMap의 Num(), Empty(), Add(), Remove() 함수를 그대로 사용할 수 있습니다. 
```lua
Obj.IntMapProperty:Empty()
Obj.IntMapProperty:Add(1, 2)
Obj.IntMapProperty:Add(3, 4)
for k, v in pairs(Obj.IntMapProperty) do
    SUE4Lua.Log("Obj.IntMapProperty: k,v = ", k, v)
end
Obj.IntMapProperty:Remove(3)
SUE4Lua.Log("Obj.IntMapProperty:Num()", Obj.IntMapProperty:Num())
Obj.IntMapProperty:Empty()
SUE4Lua.Log("Obj.IntMapProperty:Num()", Obj.IntMapProperty:Num())
-- Obj.IntMapProperty: k,v = 	1	2
-- Obj.IntMapProperty: k,v = 	3	4
-- Obj.IntMapProperty:Num()	1
-- Obj.IntMapProperty:Num()	0
```

원소가 기본형이면 `[]`로 접근할 수 있습니다.
```lua
Obj.IntMapProperty:Empty()
Obj.IntMapProperty[123] = 456
SUE4Lua.Log("Obj.IntMapProperty[123]", Obj.IntMapProperty[123])
Obj.IntMapProperty[123] = nil
SUE4Lua.Log("Obj.IntMapProperty[123]", Obj.IntMapProperty[123])
-- Obj.IntMapProperty[123]	456
-- Obj.IntMapProperty[123]	nil
```

TSet
----

TSet의 Num(), Empty(), Add(), Remove() 함수를 그대로 사용할 수 있습니다. 
```lua
Obj.IntSetProperty:Empty()
Obj.IntSetProperty:Add(1)
Obj.IntSetProperty:Add(2)
for k, v in pairs(Obj.IntSetProperty) do
    SUE4Lua.Log("Obj.IntSetProperty: k,v = ", k, v)
end
Obj.IntSetProperty:Remove(2)
SUE4Lua.Log("Obj.IntSetProperty:Num()", Obj.IntSetProperty:Num())
Obj.IntSetProperty:Empty()
SUE4Lua.Log("Obj.IntSetProperty:Num()", Obj.IntSetProperty:Num())
-- Obj.IntSetProperty: k,v = 	1	1
-- Obj.IntSetProperty: k,v = 	2	2
-- Obj.IntSetProperty:Num()	1
-- Obj.IntSetProperty:Num()	0
```

원소가 기본형이면 `[]`로 접근할 수 있습니다.
```lua
Obj.IntSetProperty:Empty()
Obj.IntSetProperty[123] = false
SUE4Lua.Log("Obj.IntSetProperty[123]", Obj.IntSetProperty[123])
Obj.IntSetProperty[123] = nil
SUE4Lua.Log("Obj.IntSetProperty[123]", Obj.IntSetProperty[123])
-- Obj.IntSetProperty[123]	123
-- Obj.IntSetProperty[123]	nil
```

UStruct 타입의 TArray, TMap, TSet에서 원소 검색하기
-------------------------------------------------

원소가 UStruct 타입인 TArray은 `[]`로 접근할 수 없습니다. 이 경우에는 GetCopy() 혹은 GetRef()를 사용해야 합니다.
마찬가지로 TMap과 TSet은 FindCopy(), FindRef()를 사용해야 합니다.

```lua
Obj.StructArrayProperty:Empty()
Obj.StructArrayProperty:Add()
Obj.StructArrayProperty:GetCopy(0).IntProperty = 123
SUE4Lua.Log("Obj.StructArrayProperty:GetCopy(0).IntProperty", Obj.StructArrayProperty:GetCopy(0).IntProperty)
Obj.StructArrayProperty:GetRef(0).IntProperty = 123
SUE4Lua.Log("Obj.StructArrayProperty:GetRef(0).IntProperty", Obj.StructArrayProperty:GetRef(0).IntProperty)
-- Obj.StructArrayProperty:GetCopy(0).IntProperty	0
-- Obj.StructArrayProperty:GetRef(0).IntProperty	123
```

이름에서 알 수 있듯이 GetCopy()는 사본을 반환하므로 이 값을 수정해도 원본 값이 바뀌지 않습니다.
GetRef()는 원본의 참조를 반환하므로 수정하면 원본 값이 바뀝니다.

TArray()의 내부 버퍼가 재할당된 후에 이전에 저장해 둔 GetRef() 반환값을 사용하면 **게임이 크래시될 수 있으니 주의하세요**. TMap, TSet도 동일합니다.

참고: 다음 타입들은 예외적으로 UStruct지만 `[]`로 접근할 수 있습니다(noexport 속성의 UStruct는 값 타입으로 취급합니다). 
* FVector, FVector2D, FVector4, FIntPoint, FIntVector, FMatrix, FRotator, FQuad, FTransform
* FColor, FLinearColor
* FBox, FBox2D, FBoxSphereBounds
* FGuid
* FDateTime, FTimecode, FTimespan
* FRandomStream, FFrameRte, FFrameNumber, FFrameTime, FQualifiedFrameTime
* FPrimaryAssetType, FPrimaryAssetId
* FSoftObjectPath
* FFloatRangeBound, FFloatRange, FInt32Interval

딜리게이트
---------

딜리게이트 및 멀티캐스트 딜리게이트의 _일부_ 기능을 Lua에서 사용할 수 있습니다.
Lua에서 브로드캐스팅을 할 수 있다면 코드 흐름을 파악하기 어렵다고 판단하여 일부러 브로드캐스팅 함수를 바인딩하지 않았습니다.

```lua
Obj.ValueProperty = function () end

Obj.IntDelegate:Bind(Obj:getObjectProperty("ValueProperty"))
SUE4Lua.Log("Obj.IntDelegate:IsBound()", Obj.IntDelegate:IsBound())
Obj.IntDelegate:Unbind()
SUE4Lua.Log("Obj.IntDelegate:IsBound()", Obj.IntDelegate:IsBound())
-- Obj.IntDelegate:IsBound()	true
-- Obj.IntDelegate:IsBound()	false

local LuaValue = Obj:getObjectProperty("ValueProperty")
Obj.IntMulticastDelegate:AddUnique(LuaValue)
SUE4Lua.Log("Obj.IntMulticastDelegate:Contains()", Obj.IntMulticastDelegate:Contains(LuaValue))
Obj.IntMulticastDelegate:RemoveAll(LuaValue)
SUE4Lua.Log("Obj.IntMulticastDelegate:Contains()", Obj.IntMulticastDelegate:Contains(LuaValue))
-- Obj.IntMulticastDelegate:Contains()	true
-- Obj.IntMulticastDelegate:Contains()	false
```

바인드할 함수를 전달하는 방법이 이상하게 보일 수 있습니다. 자세한 내용은 [여기](WorkWithLuaValue_ko.md)를 참고하세요.

함수 호출하기
------------

다음과 같은 UFunction이 있을 때,
```cpp
int32 USUE4LuaTestUObject::TestArgs(int32 InIntArg1, int32 InIntArg2, const int32& InIntArg3, int32& OutIntArg1, int32& OutIntArg2)
{
	OutIntArg1 = InIntArg1 + InIntArg2;
	OutIntArg2 = InIntArg3;

	return OutIntArg1 + OutIntArg2;
}
```

Lua에서 다음과 같이 TestArgs()를 호출할 수 있습니다. 
출력값(참조형 파라메터)이 있다면 UFunction의 반환값 다음에 차례대로 Lua에 반환됩니다.

```lua
SUE4Lua.Log("Obj:TestArgs(1, 2, 3, 4, 5)", Obj:TestArgs(1, 2, 3, 4, 5)) -- Obj:TestArgs(1, 2, 3, 4, 5)	6	3   3
```

이름을 사용해서 파라메터를 전달할 수도 있습니다. 이 방식은 파라메터가 많을 때 코드의 가독성을 높혀줍니다.
```lua
SUE4Lua.Log("Obj:TestArgs(Args)", Obj:TestArgs({
    InIntArg1 = 1,
    InIntArg2 = 2,
    InIntArg3 = 3,
    OutIntArg1 = 4,
    OutIntArg2 = 5,
})) -- Obj:TestArgs(Args)	6	3	3
```

출력값 파라메터를 전달하지 않아도 상관없습니다.
```lua
SUE4Lua.Log("Obj:TestArgs(1, 2, 3)", Obj:TestArgs(1, 2, 3))
SUE4Lua.Log("Obj:TestArgs(Args)", Obj:TestArgs({
    InIntArg1 = 1,
    InIntArg2 = 2,
    InIntArg3 = 3,
}))
-- Obj:TestArgs(1, 2, 3)	6	3	3
-- Obj:TestArgs(Args)	6	3	3
```

_이름으로 전달_ 방식으로 함수를 호출했을 경우 전달한 Lua 테이블에 출력값이 저장됩니다.
```lua
local Args = {
    InIntArg1 = 1,
    InIntArg2 = 2,
    InIntArg3 = 3,
}
Obj:TestArgs(Args)
SUE4Lua.Log("Args.OutIntArg1", Args.OutIntArg1) -- Args.OutIntArg1	3
SUE4Lua.Log("Args.OutIntArg2", Args.OutIntArg2) -- Args.OutIntArg2	3
```

UObject 생성하기
---------------

`UE4.NewObject()` 함수를 사용해서 새 UObject 인스턴스를 생성할 수 있습니다.
```lua
local Obj = UE4.NewObject(UE4.FindClass("SUE4LuaTestUObject"))
```
----------------------------------------------------
[프로그래밍 가이드](ProgrammingGuide_ko.md)로 돌아가기