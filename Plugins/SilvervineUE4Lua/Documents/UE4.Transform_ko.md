# UE4 Transform Library

SilvervineUE4Lua에서 제공하는 Transform 함수 라이브러리입니다.


## operator +  (T1, T2)

두 트랜스폼 T1, T2를 덧셈한 결과를 가져옵니다.

```lua
local Transform1 = UE4.Transform.new(UE4.Quat.new(1.0, 1.0, 1.0, 1.0), UE4.Vector.new(1.0, 2.0, 3.0), UE4.Vector.new(1.0, 2.0, 3.0))
local Transform2 = UE4.Transform.new(UE4.Quat.new(2.0, 2.0, 2.0, 2.0), UE4.Vector.new(2.0, 3.0, 4.0), UE4.Vector.new(2.0, 3.0, 4.0))

local Return = Transform1 + Transform2

local Rotation = Return.Rotation -- Quat(3.0, 3.0, 3.0, 3.0)
local Translation = Return.Translation -- Vector(3.0, 5.0, 7.0)
local Scale3D = Return.Scale3D -- Vector(3.0, 5.0, 7.0)
```

## operator * (T1, T2)

트랜스폼 T1을 다른 트랜스폼 T2 구성 요소별로 곱셈한 결과를 가져옵니다.

```lua
local Transform1 = UE4.Transform.new(UE4.Quat.new(1.0, 1.0, 1.0, 1.0), UE4.Vector.new(1.0, 2.0, 3.0), UE4.Vector.new(1.0, 2.0, 3.0))
local Transform2 = UE4.Transform.new(UE4.Quat.new(2.0, 2.0, 2.0, 2.0), UE4.Vector.new(2.0, 3.0, 4.0), UE4.Vector.new(2.0, 3.0, 4.0))

local Return = Transform1 * Transform2

local Rotation = Return.Rotation -- Quat(-4.0, 4.0, 4.0, 4.0)
local Translation = Return.Translation -- Vector(164.0, -55.0, -80.0)
local Scale3D = Return.Scale3D -- Vector(2.0, 6.0, 12.0)
```

## operator == (T1, T2)

두 트랜스폼 T1, T2가 동일한지 확인합니다.

```lua
local Transform1 = UE4.Transform.new(UE4.Quat.new(1.0, 1.0, 1.0, 1.0), UE4.Vector.new(1.0, 2.0, 3.0), UE4.Vector.new(1.0, 2.0, 3.0))
local Transform2 = UE4.Transform.new(UE4.Quat.new(1.0, 1.0, 1.0, 1.0), UE4.Vector.new(1.0, 2.0, 3.0), UE4.Vector.new(1.0, 2.0, 3.0))

local Return = Transform1 == Transform2 -- true
```

## UE4.Transform.Equals(T1, T2, Tolerance?)

지정된 허용 오차 내에서 두 트랜스폼 T1, T2가 동일한지 확인합니다.

```lua
local Transform1 = UE4.Transform.new(UE4.Quat.new(1.0, 1.0, 1.0, 1.0), UE4.Vector.new(1.0, 2.0, 3.0), UE4.Vector.new(1.0, 2.0, 3.0))
local Transform2 =UE4.Transform.new(UE4.Quat.new(2.0, 2.0, 2.0, 2.0), UE4.Vector.new(2.0, 3.0, 4.0), UE4.Vector.new(2.0, 3.0, 4.0))
local Tolerance = 4.0

local Return1 = UE4.Transform.Equals(Transform1, Transform2) -- false
local Return2 = Transform1:Equals(Transform2) -- false
local Return3 = UE4.Transform.Equals(Transform1, Transform2, Tolerance) -- true
local Return4 = Transform1:Equals(Transform2, Tolerance) -- true
```

## UE4.Transform.TransformLocation(T, Location)

입력 트랜스폼을 사용하여 위치를 변형합니다.

```lua
local Transform = UE4.Transform.new(UE4.Rotator.new(90.0, 90.0, 90.0):ToQuat(), UE4.Vector.new(1.0, 2.0, 3.0), UE4.Vector.new(2.0, 2.0, 2.0))
local Location = UE4.Vector.new(1.0, 2.0, 3.0)

local Return1 = UE4.Transform.TransformLocation(Transform, Location) -- Vector(-5.0, 6.0, 5.0)
local Return2 = Transform:TransformLocation(Location) -- Vector(-5.0, 6.0, 5.0)
```

## UE4.Transform.TransformDirection(T, Direction)

입력 트랜스폼에 따라 방향 벡터를 변환합니다. Scale은 적용하지 않습니다.

```lua
local Transform = UE4.Transform.new(UE4.Rotator.new(90.0, 90.0, 90.0):ToQuat(), UE4.Vector.new(1.0, 2.0, 3.0), UE4.Vector.new(1.0, 2.0, 3.0))
local Direction = UE4.Vector.new(1.0, 2.0, 3.0)

local Return1 = UE4.Transform.TransformDirection(Transform, Direction) -- Vector(-2.0, 4.0, 4.0)
local Return2 = Transform:TransformDirection(Direction) -- Vector(-2.0, 4.0, 4.0)
```

## UE4.Transform.TransformRotation(T, Rotation)

입력 트랜스폼에 의해 회전자를 변환합니다.

```lua
local Transform = UE4.Transform.new(UE4.Rotator.new(90.0, 90.0, 90.0):ToQuat(), UE4.Vector.new(1.0, 2.0, 3.0), UE4.Vector.new(1.0, 2.0, 3.0))
local Rotation = UE4.Rotator.new(90.0, 90.0, 90.0)

local Return1 = UE4.Transform.TransformRotation(Transform, Rotation) -- Rotator(0.0, 180.0, -180.0)
local Return2 = Transform:TransformRotation(Rotation) -- Rotator(0.0, 180.0, -180.0)
```

## UE4.Transform.InverseTransformLocation(T, Location)

입력 트랜스폼의 역을 사용하여 위치를 변형합니다.

```lua
local Transform = UE4.Transform.new(UE4.Rotator.new(90.0, 90.0, 90.0):ToQuat(), UE4.Vector.new(1.0, 2.0, 3.0), UE4.Vector.new(2.0, 2.0, 2.0))
local Location = UE4.Vector.new(0.0, 0.0, 0.0)

local Return1 = UE4.Transform.InverseTransformLocation(Transform, Location) -- Vector(-1.5, -1.0, 0.5)
local Return2 = Transform:InverseTransformLocation(Location) -- Vector(-1.5, -1.0, 0.5)
```

## UE4.Transform.InverseTransformDirection(T, Direction)

입력 트랜스폼의 역에 따라 방향 벡터를 변환합니다. Scale은 적용하지 않습니다.

```lua
local Transform = UE4.Transform.new(UE4.Rotator.new(90.0, 90.0, 90.0):ToQuat(), UE4.Vector.new(1.0, 2.0, 3.0), UE4.Vector.new(2.0, 2.0, 2.0))
local Direction = UE4.Vector.new(1.0, 1.0, 1.0)

local Return1 = UE4.Transform.InverseTransformDirection(Transform, Direction) -- Vector(-2.0, -1.0, 0.0)
local Return2 = Transform:InverseTransformDirection(Direction) -- Vector(-2.0, -1.0, 0.0)
```

## UE4.Transform.InverseTransformRotation(T, Rotation)

입력 트랜스폼의 역에 의해 회전자를 변환합니다.

```lua
local Transform = UE4.Transform.new(UE4.Rotator.new(90.0, 90.0, 90.0):ToQuat(), UE4.Vector.new(1.0, 2.0, 3.0), UE4.Vector.new(2.0, 2.0, 2.0))
local Rotation = UE4.Rotator.new(180.0, 90.0, 0.0)

local Return1 = UE4.Transform.InverseTransformRotation(Transform, Rotation) -- Rotator(0.0, -90.0, -90.0)
local Return2 = Transform:InverseTransformRotation(Rotation) -- Rotator(0.0, -90.0, -90.0)
```

## UE4.Transform.Inverse(T)

입력 트랜스폼의 역을 반환합니다.

```lua
local Transform = UE4.Transform.new(UE4.Quat.new(1.0, 1.0, 1.0, 1.0), UE4.Vector.new(1.0, 2.0, 3.0), UE4.Vector.new(1.0, 2.0, 3.0))

local Return1 = UE4.Transform.Inverse(Transform)
local Return2 = Transform:Inverse()

local Rotation = Return1.Rotation -- Quat(-1.0, -1.0, -1.0, 1.0)
local Translation = Return1.Translation -- Vector(-1.0, -1.0, -1.0)
local Scale3D = Return1.Scale3D -- Vector(1.0, 0.5, 0.3333)
```

## UE4.Transform.Lerp(T1, T2, Alpha)

알파를 기준으로 T1와 T2를 선형적으로 보간합니다. (알파 = 0인 경우 A의 100%, 알파 = 1인 경우 B의 100%).

Quaternion interpolation만 지원합니다.

```lua
local Transform1 = UE4.Transform.new(UE4.Rotator.new(0.0, 0.0, 0.0):ToQuat(), UE4.Vector.new(1.0, 2.0, 3.0), UE4.Vector.new(1.0, 2.0, 3.0))
local Transform2 = UE4.Transform.new(UE4.Rotator.new(90.0, 90.0, 90.0):ToQuat(), UE4.Vector.new(2.0, 3.0, 4.0), UE4.Vector.new(2.0, 3.0, 4.0))
local Alpha = 0.5

local Return1 = UE4.Transform.Lerp(Transform1, Transform2, Alpha)
local Return2 = Transform1:Lerp(Transform2, Alpha)

local Rotator = Return1.Rotation:ToRotator() -- Rotator(45.0, 0.0, 0.0)
local Translation = Return1.Translation -- Vector(1.5, 2.5, 3.5)
local Scale3D = Return1.Scale3D -- Vector(1.5, 2.5, 3.5)
```

------------------------------------
[라이브러리](Library_ko.md)로 돌아가기