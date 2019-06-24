# UE4 Timespan Library

SilvervineUE4Lua에서 제공하는 Timespan 함수 라이브러리입니다.

## operator - (T)

시간범위의 단항 부정 연산자의 결과를 가져옵니다.

```lua
local Timespan = UE4.Timespan.new(100)

local Return = -Timespan -- Timespan(-100)
```

## operator + | - (T1, T2)

두 시간범위 T1, T2를 덧셈 | 뺄셈한 결과를 가져옵니다.

```lua
local Timespan1 = UE4.Timespan.new(200)
local Timespan2 = UE4.Timespan.new(100)

local Return = Timespan1 + Timespan2 -- Timespan(300)
local Return = Timespan1 - Timespan2 -- Timespan(100)
```

## operator * | / (T), Scale)

시간범위 T를 Scale 값으로 곱셈 | 나눗셈한 결과를 가져옵니다.

```lua
local Timespan = UE4.Timespan.new(200)
local Scale = 2.0

local Return1 = Timespan * Scale -- Timespan(400)
local Return2 = Timespan / Scale -- Timespan(200)
```

## operator == (T1, T2)

두 시간범위가 동일한지 확인합니다.

```lua
local Timespan1 = UE4.Timespan.new(100)
local Timespan2 = UE4.Timespan.new(100)

local Return = Timespan1 == Timespan2 -- true
```

## UE4.Timespan.GetDays(T)

시간범위 T의 일 구성 요소를 가져옵니다.

```lua
local Timespan = UE4.Timespan.new(10000000000000)

local Return1 = UE4.Timespan.GetDays(Timespan) -- 11
local Return2 = Timespan:GetDays() -- 11
```

## UE4.Timespan.GetDuration(T)

시간범위 T의 절대값을 가져옵니다.

```lua
local Timespan = UE4.Timespan.new(-10000000000000)

local Return1 = UE4.Timespan.GetDuration(Timespan) -- Timespan(10000000000000)
local Return2 = Timespan:GetDuration() -- Timespan(10000000000000)
```

## UE4.Timespan.GetHours(T)

시간범위 T의 시간 구성 요소를 반환합니다.

```lua
local Timespan = UE4.Timespan.new(10000000000000)

local Return1 = UE4.Timespan.GetHours(Timespan) -- 13
local Return2 = Timespan:GetHours() -- 13
```

## UE4.Timespan.GetMilliseconds(T)

시간범위 T의 밀리 초 구성 요소를 반환합니다.

```lua
local Timespan = UE4.Timespan.new(10000000000000)

local Return1 = UE4.Timespan.GetMilliseconds(Timespan) -- 0
local Return2 = Timespan:GetMilliseconds() -- 0
```

## UE4.Timespan.GetMinutes(T)

시간범위 T의 분 구성 요소를 반환합니다.

```lua
local Timespan = UE4.Timespan.new(10000000000000)

local Return1 = UE4.Timespan.GetMinutes(Timespan) -- 46
local Return2 = Timespan:GetMinutes() -- 46
```

## UE4.Timespan.GetSeconds(T)

시간범위 T의 초 구성 요소를 반환합니다.

```lua
local Timespan = UE4.Timespan.new(10000000000000)

local Return1 = UE4.Timespan.GetSeconds(Timespan) -- 40
local Return2 = Timespan:GetSeconds() -- 40
```

## UE4.Timespan.GetTotalDays(T)

시간범위 T의 총 일수를 반환합니다.

```lua
local Timespan = UE4.Timespan.new(10000000000000)

local Return1 = UE4.Timespan.GetTotalDays(Timespan) -- 11.5740
local Return2 = Timespan:GetTotalDays() -- 11.5740
```

## UE4.Timespan.GetTotalHours(T)

시간범위 T의 총 시간 수를 반환합니다.

```lua
local Timespan = UE4.Timespan.new(10000000000000)

local Return1 = UE4.Timespan.GetTotalHours(Timespan) -- 277.7777
local Return2 = Timespan:GetTotalHours() -- 277.7777
```

## UE4.Timespan.GetTotalMilliseconds(T)

시간범위 T의 총 밀리 초 수를 반환합니다.

```lua
local Timespan = UE4.Timespan.new(10000000000000) -- 2019/05/22 04:34:39.939

local Return1 = UE4.Timespan.GetTotalMilliseconds(Timespan) -- 1000000000.0
local Return2 = Timespan:GetTotalMilliseconds() -- 1000000000.0(164199390000)
```

## UE4.Timespan.GetTotalMinutes(T)

시간범위 T의 총 분 수를 반환합니다.

```lua
local Timespan = UE4.Timespan.new(10000000000000)

local Return1 = UE4.Timespan.GetTotalMinutes(Timespan) -- 16666.6666
local Return2 = Timespan:GetTotalMinutes() -- 16666.6666
```

## UE4.Timespan.GetTotalSeconds(T)

시간범위 T의 총 초 수를 반환합니다.

```lua
local Timespan = UE4.Timespan.new(10000000000000)

local Return1 = UE4.Timespan.GetTotalSeconds(Timespan) -- 1000000.0
local Return2 = Timespan:GetTotalSeconds() -- 1000000.0
```

## UE4.Timespan.FromDays(Days)

지정된 일 수를 나타내는 시간범위를 반환합니다.

```lua
local Days = 10

local Return1 = UE4.Timespan.FromDays(Days) -- Timespan(8640000000000)
```

## UE4.Timespan.FromHours(Hours)

지정된 시간 수를 나타내는 시간범위를 반환합니다.

```lua
local Hours = 10

local Return1 = UE4.Timespan.FromHours(Hours) -- Timespan(360000000000)
```

## UE4.Timespan.FromMilliseconds(Milliseconds)

지정된 밀리 초를 나타내는 시간범위를 반환합니다.

```lua
local Milliseconds = 10

local Return1 = UE4.Timespan.FromMilliseconds(Milliseconds) -- Timespan(100000)
```

## UE4.Timespan.FromMinutes(Minutes)

지정된 분 수를 나타내는 시간범위를 반환합니다.

```lua
local Minutes = 10

local Return1 = UE4.Timespan.FromMinutes(Minutes) -- Timespan(6000000000)
```

## UE4.Timespan.FromSeconds(Seconds)

지정된 초 수를 나타내는 시간범위를 반환합니다.

```lua
local Seconds = 10

local Return1 = UE4.Timespan.FromSeconds(Seconds) -- Timespan(100000000)
```

## UE4.Timespan.MaxValue()

최대 시간범위 값을 반환합니다.

```lua
local Return1 = UE4.Timespan.MaxValue() -- Timespan(9223372036854775807)
```

## UE4.Timespan.MinValue()

최소 시간범위 값을 반환합니다.

```lua
local Return1 = UE4.Timespan.MinValue() -- Timespan(-9223372036854775808)
```

## UE4.Timespan.Ratio(T1, T2)

두 시간범위 (T1 / T2) 사이의 비율을 반환하고, 0 값을 처리합니다.

```lua
local Timespan1 = UE4.Timespan.new(200)
local Timespan2 = UE4.Timespan.new(100)

local Return1 = UE4.Timespan.Ratio(Timespan1, Timespan2) -- 2.0
local Return2 = Timespan1:Ratio(Timespan2) -- 2.0
```

## UE4.Timespan.ZeroValue()

0 시간범위 값을 반환합니다.

```lua
local Return1 = UE4.Timespan.ZeroValue() -- Timespan(0)
```

------------------------------------
[라이브러리](Library_ko.md)로 돌아가기