
Working with UObject in Lua
===========================

UObject is the most common engine type in lua. 
SUE4Lua wraps a UObject in a proxy object of the _userdata_ type and passes it to lua.
This proxy object allows you to handle UObject like a lua table, which is very convenient for coding.

All test codes below are [here](../Source/SilvervineUE4Lua/Private/Scripts/Tests/UObjectTest.lua).

Note:
* Not all members of a UObject are accessable in lua. Only members with UPROPERTY() and UFUNCTION() can be accessed.
* The function does not need to be BlueprintCallable. `UFUNCTION()` is sufficient.
* C++ access specifiers(private, protected) do not affect. Private members are accessable from lua.

Primitive Types
---------------

Most C++ primitive type members are accessable from lua.
```lua
Obj.IntProperty = 123
SUE4Lua.Log("Obj.IntProperty", Obj.IntProperty) -- Obj.IntProperty	123
```

Enum is treated as integer. For UEnum, see [this](WorkWithUEnum.md).
```lua
Obj.EnumProperty = 1
SUE4Lua.Log("Obj.EnumProperty", Obj.EnumProperty)   -- Obj.EnumProperty	1
```

`FName`, `FString` and `FText` are all treated as string.
```lua
Obj.NameProperty = "name"
SUE4Lua.Log("Obj.NameProperty", Obj.NameProperty)   -- Obj.NameProperty	name
```

You can use UObject members.
```lua
Obj.UObjectProperty = Obj
SUE4Lua.Log("type(Obj.UObjectProperty)", type(Obj.UObjectProperty))
-- type(Obj.UObjectProperty)	userdata
```

You can use UStruct members.
```lua
Obj.StructProperty.IntProperty = 123
SUE4Lua.Log("Obj.StructProperty.IntProperty", Obj.StructProperty.IntProperty)
Obj.NestedStructProperty.StructProperty.IntProperty = 123
SUE4Lua.Log("Obj.NestedStructProperty.StructProperty.IntProperty", Obj.NestedStructProperty.StructProperty.IntProperty)
-- Obj.StructProperty.IntProperty	123
-- Obj.NestedStructProperty.StructProperty.IntProperty	123
```

Arrays can be treated like tables.
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

Note:
* It is not recommended to use ipairs() on an array because iterating the array with ipairs() will ignore index 0.

You can use array of structures like `FVector`.
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

You can use TArray's Add(), Remove, Insert(), Empty() and Num().
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

If the element is a primitive type, it can be accessed with `[]`.
```lua
Obj.IntArrayProperty:Empty()
Obj.IntArrayProperty:Add(123)
SUE4Lua.Log("Obj.IntArrayProperty[0]", Obj.IntArrayProperty[0])
```

TMap
----

You can use TMap's Num(), Empty(), Add() and Remove(). 
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

If the element is a primitive type, it can be accessed with `[]`.
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

You can use TSet's Num(), Empty(), Add() and Remove(). 
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

If the element is a primitive type, it can be accessed with `[]`.
```lua
Obj.IntSetProperty:Empty()
Obj.IntSetProperty[123] = false
SUE4Lua.Log("Obj.IntSetProperty[123]", Obj.IntSetProperty[123])
Obj.IntSetProperty[123] = nil
SUE4Lua.Log("Obj.IntSetProperty[123]", Obj.IntSetProperty[123])
-- Obj.IntSetProperty[123]	123
-- Obj.IntSetProperty[123]	nil
```

Finding elements of UStruct type in TArray, TMap and TSet
---------------------------------------------------------

TArray whose elements are of UStruct type cannot be accessed with `[]`. In this case, you must use GetCopy() or GetRef() for safety. 
Similarly, You must use FindCopy() or FindRef() to access TMap and TSet.

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

As you can know from the name, GetCopy() returns a copy, so modifying it does not change the original value.
GetRef() returns a reference to the original, so modifying it changes the original value.

Note that after TArray's buffer is reallocated, using the previously saved GetRef() value **may cause your game to crash**. The same is true for TMap and TSet.

Note: The following UStruct types can be accessed with `[]`(UStruct with _noexport_ is treated as a value type). 
* FVector, FVector2D, FVector4, FIntPoint, FIntVector, FMatrix, FRotator, FQuad, FTransform
* FColor, FLinearColor
* FBox, FBox2D, FBoxSphereBounds
* FGuid
* FDateTime, FTimecode, FTimespan
* FRandomStream, FFrameRte, FFrameNumber, FFrameTime, FQualifiedFrameTime
* FPrimaryAssetType, FPrimaryAssetId
* FSoftObjectPath
* FFloatRangeBound, FFloatRange, FInt32Interval

Delegate
--------

Some features of delegate and multicast delegate are available in lua.
We think that if lua could broadcast, it would be hard to understand the code flow.

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

The way of binding functions may seem odd. Please see [this](WorkWithLuaValue.md) for details.

Function Call
-------------

If you have the following UFunction,
```cpp
int32 USUE4LuaTestUObject::TestArgs(int32 InIntArg1, int32 InIntArg2, const int32& InIntArg3, int32& OutIntArg1, int32& OutIntArg2)
{
	OutIntArg1 = InIntArg1 + InIntArg2;
	OutIntArg2 = InIntArg3;

	return OutIntArg1 + OutIntArg2;
}
```

You can call TestArgs() as follows.
If there is an output value(reference type parameter), it is returned to lua after the return value of UFunction itself.

```lua
SUE4Lua.Log("Obj:TestArgs(1, 2, 3, 4, 5)", Obj:TestArgs(1, 2, 3, 4, 5)) -- Obj:TestArgs(1, 2, 3, 4, 5)	6	3   3
```

You can also pass parameters by name. This method improves readability when there are many parameters.
```lua
SUE4Lua.Log("Obj:TestArgs(Args)", Obj:TestArgs({
    InIntArg1 = 1,
    InIntArg2 = 2,
    InIntArg3 = 3,
    OutIntArg1 = 4,
    OutIntArg2 = 5,
})) -- Obj:TestArgs(Args)	6	3	3
```

You can omit to pass output parameters.
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

If you cal functions with _pass-by-name_, the output value will be stored in the passed lua table.
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

Creating a UObject Instance
---------------------------

You can create a new UObject instance using the `UE4.NewObject()`.
```lua
local Obj = UE4.NewObject(UE4.FindClass("SUE4LuaTestUObject"))
```
------------------------------------------------
[Back to Programming Guide](ProgrammingGuide.md)