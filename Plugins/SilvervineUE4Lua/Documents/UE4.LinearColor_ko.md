# UE4 LinearColor Library

SilvervineUE4Lua에서 제공하는 LinearColor 함수 라이브러리입니다.

## operator + | - (LC1, LC2)

두 선형색상 LC1, LC2를 덧셈 | 뺄셈한 결과를 가져옵니다.

```lua
local LinearColor1 = UE4.LinearColor.new(0.2, 0.2, 0.2, 0.2)
local LinearColor2 = UE4.LinearColor.new(0.1, 0.1, 0.1, 0.1)

local Return1 = LinearColor1 + LinearColor2 -- LinearColor(0.3, 0.3, 0.3, 0.3)
local Return2 = LinearColor1 - LinearColor2 -- LinearColor(0.1, 0.1, 0.1, 0.1)
```

## operator * | / (LC, Scale)

선형색상 LC의 각 구성 요소를 Scale 값으로 곱셈 | 나눗셈한 결과를 가져옵니다.

```lua
local LinearColor = UE4.LinearColor.new(0.2, 0.2, 0.2, 0.2)
local Scale = 2.0

local Return1 = LinearColor * Scale -- LinearColor(0.4, 0.4, 0.4, 0.4)
local Return2 = LinearColor / Scale -- LinearColor(0.1, 0.1, 0.1, 0.1)
```

## operator * | / (LC1, LC2)

선형색상 LC1을 다른 선형색상 LC2의 구성 요소별로 곱셈 | 나눗셈한 결과를 가져옵니다.

```lua
local LinearColor1 = UE4.LinearColor.new(0.1, 0.1, 0.1, 0.1)
local LinearColor2 = UE4.LinearColor.new(0.2, 0.2, 0.2, 0.2)

local Return1 = LinearColor1 * LinearColor2 -- LinearColor(0.02, 0.02, 0.02, 0.02)
local Return2 = LinearColor1 / LinearColor2 -- LinearColor(0.5, 0.5, 0.5, 0.5)
```

## operator == (LC1, LC2)

두 선형색상 LC1, LC2가 동일한지 확인합니다.

```lua
local LinearColor1 = UE4.LinearColor.new(0.1, 0.1, 0.1, 0.1)
local LinearColor2 = UE4.LinearColor.new(0.1, 0.1, 0.1, 0.1)

local Return = LinearColor1 == LinearColor2 -- true
```

## UE4.LinearColor.Equals(LC1, LC2, Tolerance?)

지정된 허용 오차 내에서 두 선형색상 LC1, LC2가 동일한지 확인합니다.

```lua
local LinearColor1 = UE4.LinearColor.new(0.1, 0.1, 0.1, 0.1)
local LinearColor2 = UE4.LinearColor.new(0.2, 0.2, 0.2, 0.2)
local Tolerance = 0.2

local Return1 = UE4.LinearColor.Equals(LinearColor1, LinearColor2) -- false
local Return2 = LinearColor1:Equals(LinearColor2) -- false
local Return3 = UE4.LinearColor.Equals(LinearColor1, LinearColor2, Tolerance) -- true
local Return4 = LinearColor1:Equals(LinearColor2, Tolerance) -- true
```

## UE4.LinearColor.IsAlmostBlack(LC)

입력 선형색상의 각 요소가 0에 가까워 졌는지 확인합니다.

```lua
local LinearColor = UE4.LinearColor.new(0.000001, 0.000001, 0.000001, 0.000001) -- smaller then DELTA(0.00001)

local Return1 = UE4.LinearColor.IsAlmostBlack(LinearColor) -- true
local Return2 = LinearColor:IsAlmostBlack() -- true
```

## UE4.LinearColor.Clamp(LC, Min, Max)

선형색상의 크기를 최소값과 최대값으로 클램프하여 복사본을 만듭니다. (범위 0~1) 

```lua
local LinearColor = UE4.LinearColor.new(0.1, 0.2, 0.3, 0.4);
local Min = 0.1;
local Max = 0.2;

local Return1 = UE4.LinearColor.Clamp(LinearColor, Min, Max) -- LinearColor(0.1, 0.2, 0.2, 0.2)
local Return2 = LinearColor:Clamp(Min, Max) -- LinearColor(0.1, 0.2, 0.2, 0.2)
```

## UE4.LinearColor.Lerp(LC1, LC2, Alpha)

알파를 기준으로 LC1와 LC2를 선형적으로 보간합니다. (알파 = 0인 경우 A의 100%, 알파 = 1인 경우 B의 100%)

```lua
local LinearColor1 = UE4.LinearColor.new(0.1, 0.2, 0.3, 0.4)
local LinearColor2 = UE4.LinearColor.new(0.5, 0.6, 0.7, 0.8)
local Alpha = 0.5

local Return1 = UE4.LinearColor.Lerp(LinearColor1, LinearColor2, Alpha) -- LinearColor(0.3, 0.4, 0.5, 0.6)
local Return2 = LinearColor1:Lerp(LinearColor2, Max) -- LinearColor(0.3, 0.4, 0.5, 0.6)
```

## UE4.LinearColor.LerpUsingHSV(LC1, LC2, Alpha)

알파를 기준으로 LC1와 LC2를 선형적으로 HSV 보간합니다. (알파 = 0인 경우 A의 100%, 알파 = 1인 경우 B의 100%)

보간은 새로운 색상의 색조에 대한 최단 경로를 취하는 HSV 색상 공간에서 수행됩니다. 이것은 Lerp보다 더 나은 결과를 줄 수 있지만 더 비쌉니다.

```lua
local LinearColor1 = UE4.LinearColor.new(0.1, 0.2, 0.3, 0.4)
local LinearColor2 = UE4.LinearColor.new(0.5, 0.6, 0.7, 0.8)
local Alpha = 0.5

local Return1 = UE4.LinearColor.LerpUsingHSV(LinearColor1, LinearColor2, Alpha) -- LinearColor(0.2619, 0.3809, 0.5, 0.6)
local Return2 = LinearColor1:LerpUsingHSV(LinearColor2, Max) -- LinearColor(0.2619, 0.3809, 0.5, 0.6)
```

## UE4.LinearColor.Desaturate(LC, Desaturation)

채도가 없는 색을 반환합니다. Desaturation은 0~1의 값이며 1일 경우 채도가 완전히 사라집니다.

```lua
local LinearColor = UE4.LinearColor.new(0.1, 0.2, 0.3, 0.4)
local Desaturation = 0.5

local Return1 = UE4.LinearColor.Desaturate(LinearColor, Desaturation) -- LinearColor(0.1404, 0.1904, 0.2405, 0.2)
local Return2 = LinearColor:Desaturate(Desaturation) -- LinearColor(0.1404, 0.1904, 0.2405, 0.2)
```

## UE4.LinearColor.Distance(LC1, LC2)

두 선형색상 사이의 유클리드 거리를 반환합니다.

```lua
local LinearColor1 = UE4.LinearColor.new(0.1, 0.2, 0.3, 0.4)
local LinearColor2 = UE4.LinearColor.new(0.5, 0.6, 0.7, 0.8)

local Return1 = UE4.LinearColor.Distance(LinearColor1, LinearColor2) -- 0.8
local Return2 = LinearColor1:Distance(LinearColor2) -- 0.8
```

## UE4.LinearColor.GetMin(LC)

선형색상 구성 요소 중 최소값을 가져옵니다.

```lua
local LinearColor = UE4.LinearColor.new(0.1, 0.2, 0.3, 0.4)

local Return1 = UE4.LinearColor.GetMin(LinearColor) -- 0.1
local Return2 = LinearColor:GetMin() -- 0.1
```

## UE4.LinearColor.GetMax(LC)

선형색상 구성 요소 중 최대값을 가져옵니다.

```lua
local LinearColor = UE4.LinearColor.new(0.1, 0.2, 0.3, 0.4)

local Return1 = UE4.LinearColor.GetMax(LinearColor) -- 0.4
local Return2 = LinearColor:GetMax() -- 0.4
```

## UE4.LinearColor.GetLuminance(LC)

선형색상의 휘도를 반환합니다.

```lua
local LinearColor = UE4.LinearColor.new(0.1, 0.2, 0.3, 0.4)

local Return1 = UE4.LinearColor.GetLuminance(LinearColor) -- 0.1809
local Return2 = LinearColor:GetLuminance() -- 0.1809
```

## UE4.LinearColor.ToColor(LC, bSRGB)

선형 색상을 샘플링하여 FColor로 결과를 반환합니다. bSRGB가 true일 경우 sRGB 변환을 합니다.

```lua
local LinearColor = UE4.LinearColor.new(0.1, 0.2, 0.3, 0.4)

local Return1 = UE4.LinearColor.ToColor(LinearColor, false) -- Color(25, 51, 76, 102)
local Return2 = LinearColor:ToColor(false) -- Color(25, 51, 76, 102)
local Return1 = UE4.LinearColor.ToColor(LinearColor, true) -- Color(89, 124, 149, 102)
local Return2 = LinearColor:ToColor(true) -- Color(89, 124, 149, 102)
```

------------------------------------
[라이브러리](Library_ko.md)로 돌아가기