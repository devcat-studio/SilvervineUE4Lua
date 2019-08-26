
Working with UEnum in Lua
=========================

Like C++, lua treats UEnum in integer type(_integer_). 
The `SUE4Lua.GetEnumTable()` function can be used to improve the readability of code that uses UEnum.

```lua
local EHorizontalAlignment = SUE4Lua.GetEnumTable("EHorizontalAlignment")

SUE4Lua.Log("EHorizontalAlignment.HAlign_Center", EHorizontalAlignment.HAlign_Center)
-- EHorizontalAlignment.HAlign_Center  2
```

Use pairs() for enumerating a EnumTable.
```lua
for k, v in pairs(EHorizontalAlignment) do
    SUE4Lua.Log(k, v)
end
-- HAlign_Right   3
-- HAlign_Left    1
-- HAlign_Fill    0
-- HAlign_Center  2
``` 

You can also convert a UEnum value to a string.
```lua
SUE4Lua.Log(EHorizontalAlignment.HAlign_Fill, EHorizontalAlignment:ToString(EHorizontalAlignment.HAlign_Fill))
-- 0  HAlign_Fill
```

Converting a string to a UEnum value is very simple.
```lua
SUE4Lua.Log(EHorizontalAlignment.HAlign_Fill, EHorizontalAlignment["HAlign_Fill"])
-- 0  0
```

The entire code is [here](../Source/SilvervineUE4Lua/Private/Scripts/Tests/UEnumTest.lua).

------------------------------------------------
[Back to Programming Guide](ProgrammingGuide.md)