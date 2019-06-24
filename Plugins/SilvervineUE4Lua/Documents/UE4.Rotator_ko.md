# UE4 Rotator Library

SilvervineUE4Lua에서 제공하는 Rotator 함수 라이브러리입니다.

## operator + | - (R1, R2)

두 로테이터 R1, R2를 덧셈 | 뺄셈한 결과를 가져옵니다.

```lua
local Rotator1 = UE4.Rotator.new(6.0, 5.0, 4.0)
local Rotator2 = UE4.Rotator.new(3.0, 2.0, 1.0)

local Return1 = Rotator1 + Rotator2 -- Rotator(9.0, 7.0, 5.0)
local Return2 = Rotator1 - Rotator2 -- Rotator(3.0, 3.0, 3.0)
```

## operator * (R, Scale)

로테이터 R의 각 구성 요소를 Scale 값으로 곱셈한 결과를 가져옵니다.

```lua
local Rotator = UE4.Rotator.new(6.0, 4.0, 2.0)
local Scale = 2.0

local Return1 = Rotator * Scale -- Rotator(12.0, 8.0, 4.0)
```

## operator == (R1, R2)

두 로테이터 R1, R2가 동일한지 확인합니다.

```lua
local Rotator1 = UE4.Rotator.new(3.0, 2.0, 1.0)
local Rotator2 = UE4.Rotator.new(3.0, 2.0, 1.0)

local Return = Rotator1 == Rotator2 -- true
```

## UE4.Rotator.Equals(R1, R2, Tolerance?)

지정된 허용 오차 내에서 두 로테이터 R1, R2가 동일한지 확인합니다.

```lua
local Rotator1 = UE4.Rotator.new(6.0, 5.0, 4.0)
local Rotator2 = UE4.Rotator.new(3.0, 2.0, 1.0)
local Tolerance = 4.0

local Return1 = UE4.Rotator.Equals(Rotator1, Rotator2) -- false
local Return2 = Rotator1:Equals(Rotator2) -- false
local Return3 = UE4.Rotator.Equals(Rotator1, Rotator2, Tolerance) -- true
local Return4 = Rotator1:Equals(Rotator2, Tolerance) -- true
```

## UE4.Rotator.IsNearlyZero(R, Tolerance?)

지정된 허용 오차 내에서 로테이터가 0에 가까워 졌는지 확인합니다.

```lua
local Rotator1 = UE4.Rotator.new(0.000001, 0.000001, 0.000001) -- KINDA_SMALL_NUMBER
local Rotator2 = UE4.Rotator.new(0.1, 0.2, 0.3)
local Tolerance = 0.4

local Return1 = UE4.Rotator.IsNearlyZero(Rotator1) -- true
local Return2 = Rotator1:IsNearlyZero() -- true
local Return3 = UE4.Rotator.IsNearlyZero(Rotator2) -- false
local Return4 = Rotator2:IsNearlyZero() -- false
local Return5 = UE4.Rotator.IsNearlyZero(Rotator2, Tolerance) -- true
local Return6 = Rotator2:IsNearlyZero(Tolerance) -- true
```

## UE4.Rotator.IsZero(R)

로테이터의 모든 구성 요소가 정확하게 0인지 확인합니다.

```lua
local Rotator = UE4.Rotator.new(0.000001, 0.000001, 0.000001) -- KINDA_SMALL_NUMBER

local Return1 = UE4.Rotator.IsZero(Rotator) -- false
local Return2 = Rotator:IsZero() -- false
```

## UE4.Rotator.Inverse(R)

입력 로테이터의 역을 반환합니다

```lua
local Rotator = UE4.Rotator.new(1.0, 2.0, 3.0)

local Return1 = UE4.Rotator.Inverse(Rotator) -- Rotator(-1.0, -2.0, -3.0)
local Return2 = Rotator:Inverse() -- Rotator(-1.0, -2.0, -3.0)
```

## UE4.Rotator.Lerp(R1, R2, Alpha, bShortestPath)

알파를 기준으로 R1와 R2를 선형적으로 보간합니다. (알파 = 0인 경우 A의 100%, 알파 = 1인 경우 B의 100%)

bShortestPath가 true인 경우, FRotator 대신 Quaternion을 이용하여 보간합니다.

```lua
local Rotator1 = UE4.Rotator.new(0.0, 0.0, 0.0)
local Rotator2 = UE4.Rotator.new(90.0, 90.0, 90.0)
local Alpha = 0.5

local Return1 = UE4.Rotator.Lerp(Rotator1, Rotator2, Alpha, false) -- Rotator(45.0, 45.0, 45.0)
local Return2 = Rotator1:Lerp(Rotator2, Alpha, false) -- Rotator(45.0, 45.0, 45.0)
local Return3 = UE4.Rotator.Lerp(Rotator1, Rotator2, Alpha, true) -- Rotator(45.0, 0.0, 0.0)
local Return4 = Rotator1:Lerp(Rotator2, Alpha, true) -- Rotator(45.0, 0.0, 0.0)
```

## UE4.Rotator.Delta(R1, R2)

정규화 된 R1-R2 를 반환합니다.

```lua
local Rotator1 = UE4.Rotator.new(45.0, 45.0, 45.0)
local Rotator2 = UE4.Rotator.new(450.0, 450.0, 450.0)

local Return1 = UE4.Rotator.Delta(Rotator1, Rotator2) -- Rotator(-45.0, -45.0, -45.0)
local Return2 = Rotator1:Delta(Rotator2) -- Rotator(-45.0, -45.0, -45.0)
```

## UE4.Rotator.GetAxisX(R)

X축 벡터를 가져옵니다.

```lua
local Rotator = UE4.Rotator.new(90.0, 90.0, 90.0)

local Return1 = UE4.Rotator.GetAxisX(Rotator) -- Vector(0, 0, 1)
local Return2 = Rotator:GetAxisX() -- Vector(0, 0, 1)
```


## UE4.Rotator.GetAxisY(R)

Y축 벡터를 가져옵니다.

```lua
local Rotator = UE4.Rotator.new(90.0, 90.0, 90.0)

local Return1 = UE4.Rotator.GetAxisY(Rotator) -- Vector(0, 1, 0)
local Return2 = Rotator:GetAxisY() -- Vector(0, 1, 0)

```

## UE4.Rotator.GetAxisZ(R)

Z축 벡터를 가져옵니다.

```lua
local Rotator = UE4.Rotator.new(90.0, 90.0, 90.0)

local Return1 = UE4.Rotator.GetAxisZ(Rotator) -- Vector(-1.0, 0.0, 0.0)
local Return2 = Rotator:GetAxisZ() -- Vector(-1.0, 0.0, 0.0)
```

## UE4.Rotator.GetNormal(R)

로테이터 R의 복사본을 생성하고 정규화합니다. 모든 와인딩을 제거하고 최단 경로 회전을 만듭니다.

```lua
local Rotator = UE4.Rotator.new(450.0, 450.0, 450.0)

local Return1 = UE4.Rotator.GetNormal(Rotator) -- Rotator(90.0, 90.0, 90.0)
local Return2 = Rotator:GetNormal() -- Rotator(90.0, 90.0, 90.0)
```

## UE4.Rotator.MakeRotFromX(X)

XAxis만 주어진 로테이터를 만듭니다. Y와 Z는 불특정하지만 orthonormal입니다. XAxis는 normalized될 필요가 없습니다.

```lua
local Vector = UE4.Rotator.new(1.0, 1.0, 1.0)

local Return = UE4.Rotator.MakeRotFromX(Vector) -- Rotator(90.0, 90.0, 90.0)
```

## UE4.Rotator.MakeRotFromY(Y)

YAxis만 주어진 로테이터를 만듭니다. X와 Z는 불특정하지만 orthonormal입니다. YAxis는 normalized될 필요가 없습니다.

```lua
local Vector = UE4.Rotator.new(1.0, 1.0, 1.0)

local Return = UE4.Rotator.MakeRotFromY(Vector) -- Rotator(90.0, 90.0, 90.0)
```

## UE4.Rotator.MakeRotFromZ(Z)

ZAxis만 주어진 로테이터를 만듭니다. X와 Y는 불특정하지만 orthonormal입니다. ZAxis는 normalized될 필요가 없습니다.

```lua
local Vector = UE4.Rotator.new(1.0, 1.0, 1.0)

local Return = UE4.Rotator.MakeRotFromZ(Vector) -- Rotator(90.0, 90.0, 90.0)
```

## UE4.Rotator.ToQuat(R)

회전을 쿼터니언으로 가져옵니다.

```lua
local Rotator = UE4.Rotator.new(90.0, 90.0, 90.0)

local Return1 = UE4.Rotator.ToQuat(Rotator) -- Quat(0.0, -0.7071, -2.9802, 0.7071)
local Return2 = Rotator:ToQuat() -- Quat(0.0, -0.7071, -2.9802, 0.7071)
```

------------------------------------
[라이브러리](Library_ko.md)로 돌아가기