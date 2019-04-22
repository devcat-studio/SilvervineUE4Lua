# UE4 Quat Library

SilvervineUE4Lua에서 제공하는 Quat 함수 라이브러리입니다.

## operator + | - (Q1, Q2)

두 쿼터니언 Q1, Q2를 덧셈 | 뺄셈한 결과를 가져옵니다.

```lua
local Quat1 = UE4.Quat.new(6.0, 5.0, 4.0, 2.0)
local Quat2 = UE4.Quat.new(3.0, 2.0, 1.0, 1.0)

local Return1 = Quat1 + Quat2 -- Quat(9.0, 7.0, 5.0, 3.0)
local Return2 = Quat1 - Quat2 -- Quat(3.0, 3.0, 3.0, 1.0)
```

## operator * | / (Q, Scale)

쿼터니언 R의 각 구성 요소를 Scale 값으로 곱셈 | 나눗셈한 결과를 가져옵니다.

```lua
local Quat = UE4.Quat.new(6.0, 4.0, 2.0, 2.0)
local Scale = 2.0

local Return1 = Quat * Scale -- Quat(12.0, 8.0, 4.0, 4.0)
local Return2 = Quat / Scale -- Quat(3.0, 2.0, 1.0, 1.0)
```

## operator * (Q1, Q2)

쿼터니언 Q1을 다른 쿼터니언 Q2로 곱셈한 결과를 가져옵니다.

```lua
local Quat1 = UE4.Quat.new(6.0, 5.0, 4.0, 2.0)
local Quat2 = UE4.Quat.new(3.0, 2.0, 1.0, 1.0)

local Return1 = Quat1 * Quat2 -- Quat(9.0, 15.0, 3.0, -30)
```

## operator == (Q1, Q2)

두 쿼터니언 Q1, Q2가 동일한지 확인합니다.

```lua
local Quat1 = UE4.Quat.new(6.0, 5.0, 4.0, 2.0)
local Quat2 = UE4.Quat.new(6.0, 5.0, 4.0, 2.0)

local Return = Quat1 == Quat2 -- true
```

## UE4.Quat.Equals(Q1, Q2, Tolerance?)

지정된 허용 오차 내에서 두 쿼터니언 Q1, Q2가 동일한지 확인합니다.

```lua
local Quat1 = UE4.Quat.new(6.0, 5.0, 4.0, 2.0)
local Quat2 = UE4.Quat.new(3.0, 2.0, 1.0, 1.0)
local Tolerance = 4.0

local Return1 = UE4.Quat.Equals(Quat1, Quat2) -- false
local Return2 = Quat1:Equals(Quat2) -- false
local Return3 = UE4.Quat.Equals(Quat1, Quat2, Tolerance) -- true
local Return4 = Quat1:Equals(Quat2, Tolerance) -- true
```

## UE4.Quat.Size(Q)

쿼터니언의 길이를 가져옵니다.

```lua
local Quat = UE4.Quat.new(6.0, 5.0, 4.0, 2.0)

local Return1 = UE4.Quat.Size(Quat) -- 9.0
local Return2 = Quat:Size() -- 9.0
```

## UE4.Quat.SizeSquared(Q)

쿼터니언의 제곱 길이를 가져옵니다.

```lua
local Quat = UE4.Quat.new(6.0, 5.0, 4.0, 2.0)

local Return1 = UE4.Quat.SizeSquared(Quat) -- 81.0
local Return2 = Quat:SizeSquared() -- 81.0
```

## UE4.Quat.IsNormalized(Q)

쿼터니언이 정규화되었는지 확인합니다.

```lua
local Quat = UE4.Quat.new(6.0, 5.0, 4.0, 2.0)
local QuatNormal = UE4.Quat.new(6.0, 5.0, 4.0, 2.0):GetNormal()

local Return1 = UE4.Quat.IsNormalized(Quat) -- false
local Return2 = Quat:IsNormalized() -- false
local Return3 = UE4.Quat.IsNormalized(QuatNormal) -- true
local Return4 = QuatNormal:IsNormalized() -- true
```

## UE4.Quat.Inverse(Q)

입력 쿼터니언의 역을 반환합니다

```lua
local Quat = UE4.Quat.new(6.0, 5.0, 4.0, 2.0)

local Return1 = UE4.Quat.Inverse(Quat) -- Quat(-6.0, -5.0, -4.0, 2.0)
local Return2 = Quat:Inverse() -- Quat(-6.0, -5.0, -4.0, 2.0)
```

## UE4.Quat.Lerp(Q1, Q2, Alpha)

알파를 기준으로 Q1와 Q2를 선형적으로 보간합니다. (알파 = 0인 경우 A의 100%, 알파 = 1인 경우 B의 100%)

```lua
local Quat1 = UE4.Rotator.new(0.0, 0.0, 0.0):ToQuat()
local Quat2 = UE4.Rotator.new(90.0, 90.0, 90.0):ToQuat()
local Alpha = 0.5

local Return1 = UE4.Quat.Lerp(Quat1, Quat2, Alpha) -- Quat(0.0, -0.3826, 0.0, 0.9238)
local Return2 = Quat1:Lerp(Quat2, Alpha) -- Quat(0.0, -0.3826, 0.0, 0.9238)

local Rotator1 = Return1:ToRotator() -- Rotator(45.0, 0.0, 0.0)
local Rotator2 = Return2:ToRotator() -- Rotator(45.0, 0.0, 0.0)
```

## UE4.Quat.AngularDistance(Q1, Q2)

두 개의 회전 쿼터니언 사이의 각 거리 찾습니다. (라디안)

```lua
local Quat1 = UE4.Rotator.new(0.0, 0.0, 0.0):ToQuat()
local Quat2 = UE4.Rotator.new(90.0, 90.0, 90.0):ToQuat()

local Return1 = UE4.Quat.AngularDistance(Quat1, Quat2) -- 1.5707
local Return2 = Quat1:AngularDistance(Quat2) -- 1.5707
```

## UE4.Quat.GetAngle(Q)

쿼터니온의 각도를 구합니다.

```lua
local Quat = UE4.Rotator.new(90.0, 90.0, 90.0):ToQuat()

local Return1 = UE4.Quat.GetAngle(Quat) -- 1.5707
local Return2 = Quat:GetAngle() -- 1.5707
```

## UE4.Quat.GetRotationAxis(Q)

쿼터니언의 회전 축을 가져옵니다. 이것은 표준 좌표계를 목표 방향으로 변환하기 위해 회전이 발생하는 축입니다.

회전이 없는 단위 쿼터니언의 경우 Vector (1,0,0)가 반환됩니다.

```lua
local Quat = UE4.Rotator.new(90.0, 90.0, 90.0):ToQuat()
local QuatIdentity = UE4.Quat.Identity

local Return1 = UE4.Quat.GetRotationAxis(Quat) -- Vector(0, 1, 0)
local Return2 = Quat:GetRotationAxis() -- Vector(0, 1, 0)
local Return3 = UE4.Quat.GetRotationAxis(QuatIdentity) -- Vector(1, 0, 0)
local Return4 = QuatIdentity:GetRotationAxis() -- Vector(1, 0, 0)
```

## UE4.Quat.GetAxisX(Q)

X축 벡터를 가져옵니다.

```lua
local Quat = UE4.Rotator.new(90.0, 90.0, 90.0):ToQuat()

local Return1 = UE4.Quat.GetAxisX(Quat) -- Vector(0.0, 0.0, 1.0)
local Return2 = Quat:GetAxisX() -- Vector(0.0, 0.0, 1.0)
```


## UE4.Quat.GetAxisY(Q)

Y축 벡터를 가져옵니다.

```lua
local Quat = UE4.Rotator.new(90.0, 90.0, 90.0):ToQuat()

local Return1 = UE4.Quat.GetAxisY(Quat) -- Vector(0.0, 1.0, 0.0)
local Return2 = Quat:GetAxisY() -- Vector(0.0, 1.0, 0.0)

```

## UE4.Quat.GetAxisZ(Q)

Z축 벡터를 가져옵니다.

```lua
local Quat = UE4.Rotator.new(90.0, 90.0, 90.0):ToQuat()

local Return1 = UE4.Quat.GetAxisZ(Quat) -- Vector(-1.0, 0.0, 0.0)
local Return2 = Quat:GetAxisZ() -- Vector(-1.0, 0.0, 0.0)
```

## UE4.Quat.GetNormal(Q)

쿼터니언 Q의 복사본을 생성하고 정규화합니다. 길이가 너무 작을 경우 단위 쿼터니언을 반환합니다.

```lua
local Quat = UE4.Quat.new(1.0, 2.0, 3.0, 1.0)

local Return1 = UE4.Quat.GetNormal(Quat) -- Quat(0.2581, 0.5163, 0.7745, 0.2581)
local Return2 = Quat:GetNormal() -- Quat(0.2581, 0.5163, 0.7745, 0.2581)
```

## UE4.Quat.ToRotator(Q)

회전을 로테이터로 가져옵니다.

```lua
local Quat = UE4.Quat.new(1.0, 2.0, 3.0, 1.0)

local Return1 = UE4.Quat.ToRotator(Quat) -- Rotator(68.1986, 158.1985, 90.0)
local Return2 = Quat:ToRotator() -- Rotator(68.1986, 158.1985, 90.0)
```

------------------------------------
[라이브러리](Library_ko.md)로 돌아가기