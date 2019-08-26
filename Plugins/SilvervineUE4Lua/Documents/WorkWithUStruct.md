
Working with UStruct in Lua
===========================

How to use UStruct in lua is the same as for UObject. See [this](WorkWithUObject.md).

Creating a UStruct Instance
---------------------------

You can create a new UStruct instance using the `UE4.NewStruct()`. Unlike `UE4.NewObject()`, it takes a class name instead of a class object as an argument.
```lua
local Struct = UE4.NewStruct("SlateColor")
Struct.SpecifiedColor = UE4.LinearColor.new(0.66, 0.66, 0.66, 1.0)
```

------------------------------------------------
[Back to Programming Guide](ProgrammingGuide.md)