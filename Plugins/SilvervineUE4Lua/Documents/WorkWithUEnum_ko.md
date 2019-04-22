
Lua에서 UEnum 사용하기
=====================

Lua는 C++과 마찬가지로 UEnum을 정수 형식(_integer_)으로 처리합니다. 
`SUE4Lua.GetEnumTable()` 함수를 사용하면 UEnum을 사용하는 코드의 가독성을 높일 수 있습니다.

```lua
local EHorizontalAlignment = SUE4Lua.GetEnumTable("EHorizontalAlignment")

SUE4Lua.Log("EHorizontalAlignment.HAlign_Center", EHorizontalAlignment.HAlign_Center)
-- EHorizontalAlignment.HAlign_Center  2
```

pairs()로 EnumTable을 순회할 수 있습니다.
```lua
for k, v in pairs(EHorizontalAlignment) do
    SUE4Lua.Log(k, v)
end
-- HAlign_Right   3
-- HAlign_Left    1
-- HAlign_Fill    0
-- HAlign_Center  2
``` 

UEnum 값을 스트링으로 바꿀 수도 있습니다.
```lua
SUE4Lua.Log(EHorizontalAlignment.HAlign_Fill, EHorizontalAlignment:ToString(EHorizontalAlignment.HAlign_Fill))
-- 0  HAlign_Fill
```

스트링을 UEnum값으로 바꾸는 방법은 아주 간단합니다.
```lua
SUE4Lua.Log(EHorizontalAlignment.HAlign_Fill, EHorizontalAlignment["HAlign_Fill"])
-- 0  0
```

테스트 코드 전체는 [여기](../Source/SilvervineUE4Lua/Private/Scripts/Tests/UEnumTest.lua)에 있습니다.

----------------------------------------------------
[프로그래밍 가이드](ProgrammingGuide_ko.md)로 돌아가기