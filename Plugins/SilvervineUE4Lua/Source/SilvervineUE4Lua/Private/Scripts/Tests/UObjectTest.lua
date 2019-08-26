-- SilvervineUE4Lua / devCAT studio
-- Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.


SUE4Lua.Log("# UObjectTest")

local Obj = UE4.NewObject(UE4.FindClass("SUE4LuaTestUObject"))
assert(Obj ~= nil)

--=============================================================================================================================
-- Primitive Types
--=============================================================================================================================

Obj.BoolProperty = true
SUE4Lua.Log("Obj.BoolProperty", Obj.BoolProperty)

Obj.ByteProperty = 123
SUE4Lua.Log("Obj.ByteProperty", Obj.ByteProperty)

Obj.IntProperty = 123
SUE4Lua.Log("Obj.IntProperty", Obj.IntProperty)

Obj.FloatProperty = 123.456
SUE4Lua.Log("Obj.FloatProperty", Obj.FloatProperty)

Obj.EnumProperty = 1
SUE4Lua.Log("Obj.EnumProperty", Obj.EnumProperty)

Obj.NameProperty = "name"
SUE4Lua.Log("Obj.NameProperty", Obj.NameProperty)

Obj.StringProperty = "str"
SUE4Lua.Log("Obj.StringProperty", Obj.StringProperty)

Obj.TextProperty = "text"
SUE4Lua.Log("Obj.TextProperty", Obj.TextProperty)

Obj.UObjectProperty = Obj
SUE4Lua.Log("type(Obj.UObjectProperty)", type(Obj.UObjectProperty))

Obj.StructProperty.IntProperty = 123
SUE4Lua.Log("Obj.StructProperty.IntProperty", Obj.StructProperty.IntProperty)

Obj.NestedStructProperty.StructProperty.IntProperty = 123
SUE4Lua.Log("Obj.NestedStructProperty.StructProperty.IntProperty", Obj.NestedStructProperty.StructProperty.IntProperty)

--=============================================================================================================================
-- FixedArray
--=============================================================================================================================

for i = 0, 9 do
    Obj.FixedIntArrayProperty[i] = i * i
end
for k, v in pairs(Obj.FixedIntArrayProperty) do
    SUE4Lua.Log("Obj.FixedIntArrayProperty: k,v = ", k, v)
end

Obj.FixedVectorArrayProperty[4] = UE4.Vector.new(1, 2, 3)
SUE4Lua.Log("Obj.FixedVectorArrayProperty[4].X", Obj.FixedVectorArrayProperty[4].X)
SUE4Lua.Log("Obj.FixedVectorArrayProperty[4].Y", Obj.FixedVectorArrayProperty[4].Y)
SUE4Lua.Log("Obj.FixedVectorArrayProperty[4].Z", Obj.FixedVectorArrayProperty[4].Z)

--=============================================================================================================================
-- Array
--=============================================================================================================================

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

Obj.IntArrayProperty:Empty()
Obj.IntArrayProperty:Add(123)
SUE4Lua.Log("Obj.IntArrayProperty[0]", Obj.IntArrayProperty[0])
SUE4Lua.Log("Obj.IntArrayProperty:Find(123)", Obj.IntArrayProperty:Find(123))
SUE4Lua.Log("Obj.IntArrayProperty:Find(456)", Obj.IntArrayProperty:Find(456))

Obj.StructArrayProperty:Empty()
Obj.StructArrayProperty:Add()
Obj.StructArrayProperty:GetCopy(0).IntProperty = 123
SUE4Lua.Log("Obj.StructArrayProperty:GetCopy(0).IntProperty", Obj.StructArrayProperty:GetCopy(0).IntProperty)
Obj.StructArrayProperty:GetRef(0).IntProperty = 123
SUE4Lua.Log("Obj.StructArrayProperty:GetRef(0).IntProperty", Obj.StructArrayProperty:GetRef(0).IntProperty)

--=============================================================================================================================
-- Map
--=============================================================================================================================

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

Obj.IntMapProperty:Empty()
Obj.IntMapProperty[123] = 456
SUE4Lua.Log("Obj.IntMapProperty[123]", Obj.IntMapProperty[123])
Obj.IntMapProperty[123] = nil
SUE4Lua.Log("Obj.IntMapProperty[123]", Obj.IntMapProperty[123])

--=============================================================================================================================
-- Set
--=============================================================================================================================

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

Obj.IntSetProperty:Empty()
Obj.IntSetProperty[123] = false
SUE4Lua.Log("Obj.IntSetProperty[123]", Obj.IntSetProperty[123])
Obj.IntSetProperty[123] = nil
SUE4Lua.Log("Obj.IntSetProperty[123]", Obj.IntSetProperty[123])

--=============================================================================================================================
-- Delegates
--=============================================================================================================================
Obj.ValueProperty = function () end
Obj.IntDelegate:Bind(Obj:getObjectProperty("ValueProperty"))
SUE4Lua.Log("Obj.IntDelegate:IsBound()", Obj.IntDelegate:IsBound())
Obj.IntDelegate:Unbind()
SUE4Lua.Log("Obj.IntDelegate:IsBound()", Obj.IntDelegate:IsBound())

Obj.ValueProperty = function () end
do
    local LuaValue = Obj:getObjectProperty("ValueProperty")
    Obj.IntMulticastDelegate:AddUnique(LuaValue)
    SUE4Lua.Log("Obj.IntMulticastDelegate:Contains()", Obj.IntMulticastDelegate:Contains(LuaValue))
    Obj.IntMulticastDelegate:RemoveAll(LuaValue)
    SUE4Lua.Log("Obj.IntMulticastDelegate:Contains()", Obj.IntMulticastDelegate:Contains(LuaValue))
end

--=============================================================================================================================
-- Function Call
--=============================================================================================================================

--[[
int32 USUE4LuaTestUObject::TestArgs(int32 InIntArg1, int32 InIntArg2, const int32& InIntArg3, int32& OutIntArg1, int32& OutIntArg2)
{
	OutIntArg1 = InIntArg1 + InIntArg2;
	OutIntArg2 = InIntArg3;

	return OutIntArg1 + OutIntArg2;
}
]]

SUE4Lua.Log("Obj:TestArgs(1, 2, 3, 4, 5)", Obj:TestArgs(1, 2, 3, 4, 5))

SUE4Lua.Log("Obj:TestArgs(Args)", Obj:TestArgs({
    InIntArg1 = 1,
    InIntArg2 = 2,
    InIntArg3 = 3,
    OutIntArg1 = 4,
    OutIntArg2 = 5,
}))

SUE4Lua.Log("Obj:TestArgs(1, 2, 3)", Obj:TestArgs(1, 2, 3))
SUE4Lua.Log("Obj:TestArgs(Args)", Obj:TestArgs({
    InIntArg1 = 1,
    InIntArg2 = 2,
    InIntArg3 = 3,
}))

do
    local Args = {
        InIntArg1 = 1,
        InIntArg2 = 2,
        InIntArg3 = 3,
    }
    Obj:TestArgs(Args)
    SUE4Lua.Log("Args.OutIntArg1", Args.OutIntArg1)
    SUE4Lua.Log("Args.OutIntArg2", Args.OutIntArg2)
end