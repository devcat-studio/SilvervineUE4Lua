# UE4 Color Library

SilvervineUE4Lua에서 제공하는 Color 함수 라이브러리입니다.

## operator + (C1, C2)

두 색상 C1, C2를 덧셈한 결과를 가져옵니다.

```lua
local Color1 = UE4.Color.new(1, 2, 3, 4)
local Color2 = UE4.Color.new(5, 6, 7, 8)

local Return = Color1 + Color2 -- Color(6, 8, 10, 12)
```

## operator == (C1, C2)

두 색상 C1, C2가 동일한지 확인합니다.

```lua
local Color1 = UE4.Color.new(1, 2, 3, 4)
local Color2 = UE4.Color.new(1, 2, 3, 4)

local Return = Color1 == Color2 -- true
```

## UE4.Color.ToLinearColor(C)

색상을 선형색상으로 가져옵니다.

```lua
local Color = UE4.Color.new(255, 0, 255, 0)

local Return1 = UE4.Color.ToLinearColor(Color) -- LinearColor(1.0, 0.0, 1.0, 0.0)
local Return2 = Color:ToLinearColor() -- LinearColor(1.0, 0.0, 1.0, 0.0)
```

------------------------------------
[라이브러리](Library_ko.md)로 돌아가기