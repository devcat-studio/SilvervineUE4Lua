-- SilvervineUE4Lua / devCAT studio
-- Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.


SUE4Lua.Log("# UEnumTest")

local EHorizontalAlignment = SUE4Lua.GetEnumTable("EHorizontalAlignment")

SUE4Lua.Log("EHorizontalAlignment.HAlign_Center", EHorizontalAlignment.HAlign_Center)

for k, v in pairs(EHorizontalAlignment) do
    SUE4Lua.Log(k, v)
end

SUE4Lua.Log(EHorizontalAlignment.HAlign_Fill, EHorizontalAlignment:ToString(EHorizontalAlignment.HAlign_Fill))
SUE4Lua.Log(EHorizontalAlignment.HAlign_Fill, EHorizontalAlignment["HAlign_Fill"])