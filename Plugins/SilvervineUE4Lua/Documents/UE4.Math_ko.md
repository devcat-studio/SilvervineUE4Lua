# UE4 Math Library

SilvervineUE4Lua에서 제공하는 Math 함수 라이브러리입니다.

## UE4.Math.Hypotenuse(Width, Height)

너비와 높이를 입력하여, 직각 삼각형의 빗변을 반환합니다.

```lua
local Width = 3.0
local Height = 4.0

local Return1 = UE4.Math.Hypotenuse(Width, Height) -- 5.0
```

## UE4.Math.GridSnap(Location, GridSize)

가장 가까운 격자에 스냅하는 값을 반환합니다.

```lua
local Location = 5.1
local GridSize = 10.0

local Return1 = UE4.Math.GridSnap(Location, GridSize) -- 10.0
```

## UE4.Math.ClampAngle(AngleDegrees, MinAngleDegrees, MaxAngleDegrees)

임의의 각도를 주어진 각도 사이로 클램프합니다. 가장 가까운 경계로 클램프합니다.

```lua
local MinAngleDegrees = -90.0
local MaxAngleDegrees = 90.0
local AngleDegrees = 180

local Return1 = UE4.Math.ClampAngle(AngleDegrees, MinAngleDegrees, MaxAngleDegrees) -- 90.0
```

## UE4.Math.Clamp(Value, Min, Max)

A와 B 사이에 클램핑 된 값을 반환합니다.

```lua
local Min = 1.0
local Max = 10.0
local Value = 15.0

local Return1 = UE4.Math.Clamp(Value, Min, Max) -- 10.0
```

## UE4.Math.Lerp(F1, F2, Alpha)

알파를 기준으로 F1과 F2를 선형적으로 보간합니다. (알파 = 0인 경우 A의 100%, 알파 = 1인 경우 B의 100%)

```lua    
local Float1 = -10.0
local Float2 = 10.0
local Alpha = 0.6

local Return1 = UE4.Math.Lerp(Float1, Float2, Alpha) -- 2.0
```

## UE4.Math.Round(F)

A를 가장 가까운 정수로 반올림합니다.

```lua
local Float = 1.5

local Return1 = UE4.Math.Round(Float) -- 2
```

## UE4.Math.Truncate(F)

실수를 0으로 잘라내어 정수로 반올림합니다.

```lua
local Float1 = -1.7
local Float2 = 2.8

local Return1 = UE4.Math.Truncate(Float1) -- -1
local Return2 = UE4.Math.Truncate(Float2) -- 2
```

## UE4.Math.TruncateVector(V)

벡터의 각 요소를 정수로 반올림합니다.

```lua
local Vector = UE4.Vector.new(1.5, 2.5, 3.5)
 
local Return1 = UE4.Math.TruncateVector(Vector) -- Vector(1.0, 2.0, 3.0)
```

## UE4.Math.SignOfFloat(F)

어떤 수의 부호를 반환합니다. (F < 0이면 -1을 반환하고, F가 0이면 0을 반환하고, F > 0이면 +1을 반환)

```lua
local Float1 = -1.5
local Float2 = 0.0
local Float3 = 1.5

local Return1 = UE4.Math.SignOfFloat(Float1) -- -1.0
local Return2 = UE4.Math.SignOfFloat(Float2) -- 0.0
local Return3 = UE4.Math.SignOfFloat(Float3) -- 1.0
```

## UE4.Math.NormalizeToRange(Value, RangeMin, RangeMax)

지정된 범위로 정규화된 값을 반환합니다.

```lua
local Value = 20.0
local Min = 10.0
local Max = 50.0

local Return1 = UE4.Math.NormalizeToRange(Value, Min, Max) -- 0.25
```

## UE4.Math.MapRangeUnclamped(Value, InRangeA, InRangeB, OutRangeA, OutRangeB)

한 범위에서 다른 범위로 매핑 된 값을 반환합니다.

```lua
local Value = 20.0
local InRangeA = 10.0
local InRangeB = 50.0
local OutRangeA = 20.0
local OutRangeB = 40.0

local Return1 = UE4.Math.MapRangeUnclamped(Value, InRangeA, InRangeB, OutRangeA, OutRangeB) -- 25.0
```

## UE4.Math.MapRangeClamped(Value, InRangeA, InRangeB, OutRangeA, OutRangeB)

한 범위에서 다른 범위로 매핑 된 값을 반환합니다. 값은 입력 범위에 고정됩니다. 

```lua
local Value = 0.5
local InRangeA = 0.0
local InRangeB = 1.0
local OutRangeA = 0.0
local OutRangeB = 50.0

local Return1 = UE4.Math.MapRangeClamped(Value, InRangeA, InRangeB, OutRangeA, OutRangeB) -- 25.0
```

## UE4.Math.InterpEaseInOut(F1, F2, Alpha, Exponent)

 Ease-in/Ease-out 함수를 적용하여 F1와 F2 사이를 보간합니다. Exponent는 커브의 정도를 제어합니다.

```lua
local Float1 = 10.0
local Float2 = 20.0
local Alpha = 0.5
local Exponent = 4.0

local Return1 = UE4.Math.InterpEaseInOut(Float1, Float2, Alpha, Exponent) -- 15.0
```

## UE4.Math.MakePulsatingValue(InCurrentTime, InPulsesPerSecond, InPhase)

펄스 스칼라 값을 생성하는 간단한 함수입니다. (0.0~1.0)

| Parameter         | Description              |
|-------------------|--------------------------|
| InCurrentTime     | 현재 절대 시간           |
| InPulsesPerSecond | 초당 전체 펄스 수        |
| InPhase           | 임의의 위상값, 0.0 ~ 1.0 |

```lua
local InCurrentTime = 1.0
local InPulsesPerSecond = 2.0
local InPhase = 0.5

local Return1 = UE4.Math.MakePulsatingValue(InCurrentTime, InPulsesPerSecond) -- 1.0
local Return3 = UE4.Math.MakePulsatingValue(InCurrentTime, InPulsesPerSecond, InPhase) -- 0.0
```

## UE4.Math.FixedTurn(InCurrent, InDesired, InDeltaRate)

새로운 회전 요소 값을 반환합니다. (-360~360)

```lua
local InCurrent = 1.0
local InDesired = 2.0
local InDeltaRate = 0.5

local Return1 = UE4.Math.FixedTurn(InCurrent, InDesired, InDeltaRate) -- 1.5
```

## UE4.Math.RandomFloat(Min, Max)

Min과 Max 사이의 임의의 부동소수점을 반환합니다.

## UE4.Math.RandomInteger(Min, Max)

Min과 Max 사이의 임의의 정수를 반환합니다.


## UE4.Math.Sin(Rad)

라디안을 입력하면 사인을 반환합니다.

```lua
local Rad = math.pi

local Return1 = UE4.Math.Sin(Rad) -- 0.0
```

## UE4.Math.Asin(Value)

Value의 사인의 역(arcsin)을 반환합니다 (결과는 라디안 단위입니다).

```lua
local Value = 1.0

local Return1 = UE4.Math.Asin(Value) -- 1.5707
```

## UE4.Math.Cos(Rad)

라디안을 입력하면 코사인을 반환합니다.

```lua
local Rad = math.pi

local Return1 = UE4.Math.Cos(Rad) -- -1.0
```

## UE4.Math.Acos(Value)

Value의 코사인의 역(arccos)을 반환합니다 (결과는 라디안 단위입니다).

```lua
local Value = 1.0

local Return1 = UE4.Math.Acos(Rad) -- 0.0
```

## UE4.Math.Tan(Rad)

라디안을 입력하면 탄젠트를 반환합니다.

```lua
local Rad = math.pi / 4

local Return1 = UE4.Math.Tan(Rad) -- 1.0
```

## UE4.Math.Atan(Value)

Value의 탄젠트의 역(atan)을 반환합니다 (결과는 라디안 단위입니다).

```lua
local Value = 1.0

local Return1 = UE4.Math.Atan(Value) -- 0.7853
```

## UE4.Math.Atan2(A, B)

A/B의 탄젠트의 역(atan2)을 반환합니다 (결과는 라디안 단위입니다).

```lua
local A = 1.0
local B = 2.0

local Return1 = UE4.Math.Atan2(A, B) -- 0.4636
```

## UE4.Math.DegreesToRadians(Degree)

입력 각도에 따라 라디안 값을 반환합니다.

```lua
local Degree = 90.0

local Return1 = UE4.Math.DegreesToRadians(Degree) -- 1.5707
```

## UE4.Math.RadiansToDegrees(Rad)

입력 된 라디안에 따라 각도를 반환합니다.

```lua
local Rad = math.pi

local Return1 = UE4.Math.RadiansToDegrees(Rad) -- 180.0
```

## UE4.Math.IsNearlyZero(Value)

부동 소수점 숫자가 거의 0에 가까운지 점검합니다.

```lua
local Value = 0.00000001

local Return1 = UE4.Math.IsNearlyZero(Value) -- true
```

------------------------------------
[라이브러리](Library_ko.md)로 돌아가기