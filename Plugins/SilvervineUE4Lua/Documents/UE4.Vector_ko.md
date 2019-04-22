# UE4 Vector Library

SilvervineUE4Lua에서 제공하는 Vector 함수 라이브러리입니다.

## operator - (V)

벡터의 단항 부정 연산자의 결과를 가져옵니다.

```lua
local Vector = UE4.Vector.new(1.0, 2.0, 3.0)

local Return1 = -Vector -- Vector(-1.0, -2.0, -3.0)
```

## operator + | - (V1, V2)

두 벡터 V1, V2를 덧셈 | 뺄셈한 결과를 가져옵니다.

```lua
local Vector1 = UE4.Vector.new(6.0, 5.0, 4.0)
local Vector2 = UE4.Vector.new(3.0, 2.0, 1.0)

local Return1 = Vector1 + Vector2 -- Vector(9.0, 7.0, 5.0)
local Return2 = Vector1 - Vector2 -- Vector(3.0, 3.0, 3.0)
```

## operator * | / (V, Scale)

벡터 V의 각 구성 요소를 Scale 값으로 곱셈 | 나눗셈한 결과를 가져옵니다.

```lua
local Vector = UE4.Vector.new(6.0, 4.0, 2.0)
local Scale = 2.0

local Return1 = Vector * Scale -- Vector(12.0, 8.0, 4.0)
local Return2 = Vector / Scale -- Vector(3.0, 2.0, 1.0)
```

## operator * | / (V1, V2)

벡터 V1를 다른 벡터 V2 구성 요소별로 곱셈 | 나눗셈한 결과를 가져옵니다.

```lua
local Vector1 = UE4.Vector.new(6.0, 4.0, 2.0)
local Vector2 = UE4.Vector.new(3.0, 2.0, 1.0)

local Return1 = Vector1 * Vector2 -- Vector(18.0, 8.0, 2.0)
local Return2 = Vector1 / Vector2 -- Vector(2.0, 2.0, 2.0)
```

## operator == (V1, V2)

두 벡터 V1, V2가 동일한지 확인합니다.

```lua
local Vector1 = UE4.Vector.new(3.0, 2.0, 1.0)
local Vector2 = UE4.Vector.new(3.0, 2.0, 1.0)

local Return = Vector1 == Vector2 -- true
```

## UE4.Vector.Equals(V1, V2, Tolerance?)

지정된 허용 오차 내에서 두 벡터 V1, V2가 동일한지 확인합니다.

```lua
local Vector1 = UE4.Vector.new(6.0, 5.0, 4.0)
local Vector2 = UE4.Vector.new(3.0, 2.0, 1.0)
local Tolerance = 4.0

local Return1 = UE4.Vector.Equals(Vector1, Vector2) -- false
local Return2 = Vector1:Equals(Vector2) -- false
local Return3 = UE4.Vector.Equals(Vector1, Vector2, Tolerance) -- true
local Return4 = Vector1:Equals(Vector2, Tolerance) -- true
```

## UE4.Vector.AllComponentsEqual(V, Tolerance?)

벡터의 모든 구성 요소가 허용 오차 내에서 동일한 지 확인합니다.

```lua
local Vector = UE4.Vector.new(6.0, 5.0, 4.0)
local Tolerance = 2.0

local Return1 = UE4.Vector.AllComponentsEqual(Vector) -- false
local Return2 = Vector:AllComponentsEqual() -- false
local Return3 = UE4.Vector.AllComponentsEqual(Tolerance) -- true
local Return4 = Vector:AllComponentsEqual(Tolerance) -- true
```

## UE4.Vector.DotProduct(V1, V2)

두 벡터의 내적을 계산합니다.

```lua
local Vector1 = UE4.Vector.new(1.0, 2.0, 3.0)
local Vector2 = UE4.Vector.new(4.0, 5.0, 6.0)

local Return1 = UE4.Vector.DotProduct(Vector1, Vector2) -- 32.0
local Return2 = Vector1:DotProduct(Vector2) -- 32.0
```

## UE4.Vector.CrossProduct(V1, V2)

두 벡터의 외적을 계산합니다.

```lua
local Vector1 = UE4.Vector.new(1.0, 2.0, 3.0)
local Vector2 = UE4.Vector.new(4.0, 5.0, 6.0)

local Return1 = UE4.Vector.CrossProduct(Vector1, Vector2) -- Vector(-3.0, 6.0, -3.0)
local Return2 = Vector1:CrossProduct(Vector2) -- Vector(-3.0, 6.0, -3.0)
```

## UE4.Vector.Size(V)

벡터의 길이(크기)를 가져옵니다.

```lua
local Vector = UE4.Vector.new(1.0, 2.0, 3.0)

local Return1 = UE4.Vector.Size(Vector) -- 3.7416
local Return2 = Vector:Size() -- 3.74165749
```

## UE4.Vector.SizeSquared(V)

벡터의 제곱 길이를 가져옵니다.

```lua
local Vector = UE4.Vector.new(1.0, 2.0, 3.0)

local Return1 = UE4.Vector.SizeSquared(Vector) -- 14.0
local Return2 = Vector:SizeSquared() -- 14.0
```

## UE4.Vector.Size2D(V)

벡터의 2D 구성 요소의 길이를 가져옵니다.

```lua
local Vector = UE4.Vector.new(1.0, 2.0, 3.0)

local Return1 = UE4.Vector.Size2D(Vector) -- 2.2360
local Return2 = Vector:Size2D() -- 2.2360
```

## UE4.Vector.SizeSquared2D(V)

벡터의 2D 구성 요소의 제곱 길이를 가져옵니다.

```lua
local Vector = UE4.Vector.new(1.0, 2.0, 3.0)

local Return1 = UE4.Vector.SizeSquared2D(Vector) -- 5.0
local Return2 = Vector:SizeSquared2D() -- 5.0
```

## UE4.Vector.ClampToSize(V, Min, Max)

벡터의 크기를 최소값과 최대 값으로 클램프하여 복사본을 만듭니다.

```lua
local Vector = UE4.Vector.new(1.0, 2.0, 3.0)
local Min = 1.0
local Max = 3.0

local Return1 = UE4.Vector.ClampToSize(Vector, Min, Max) -- Vector(0.8017, 1.6035, 2.4053)
local Return2 = Vector:ClampToSize(Min, Max) -- Vector(0.8017, 1.6035, 2.4053)
```

## UE4.Vector.Distance(V1, V2)

두 점 사이의 유클리드 거리를 반환합니다.

```lua
local Vector1 = UE4.Vector.new(1.0, 2.0, 3.0)
local Vector2 = UE4.Vector.new(4.0, 5.0, 6.0)

local Return1 = UE4.Vector.Distance(Vector1, Vector2) -- 5.1961
local Return2 = Vector1:Distance(Vector2) -- 5.1961
```

## UE4.Vector.DistanceSquared(V1, V2)

두 점 사이의 제곱 거리를 반환합니다.

```lua
local Vector1 = UE4.Vector.new(1.0, 2.0, 3.0)
local Vector2 = UE4.Vector.new(4.0, 5.0, 6.0)

local Return1 = UE4.Vector.DistanceSquared(Vector1, Vector2) -- 27.0
local Return2 = Vector1:DistanceSquared(Vector2) -- 27.0
```

## UE4.Vector.Distance2D(V1, V2)

XY 평면에서 두 점 사이의 유클리드 거리를 반환합니다. (Z 무시)

```lua
local Vector1 = UE4.Vector.new(1.0, 2.0, 3.0)
local Vector2 = UE4.Vector.new(4.0, 5.0, 6.0)

local Return1 = UE4.Vector.Distance2D(Vector1, Vector2) -- 4.2426
local Return2 = Vector1:Distance2D(Vector2) -- 4.2426
```

## UE4.Vector.DistanceSquared2D(V1, V2)

XY 평면에서 두 점 사이의 제곱 거리를 반환합니다. (Z 무시)

```lua
local Vector1 = UE4.Vector.new(1.0, 2.0, 3.0) -- 18.0
local Vector2 = UE4.Vector.new(4.0, 5.0, 6.0) -- 18.0

local Return1 = UE4.Vector.DistanceSquared2D(Vector1, Vector2)
local Return2 = Vector1:DistanceSquared2D(Vector2)
```

## UE4.Vector.DistancePointToSegment(Point, SegmentStart, SegmentEnd)

점에서 세그멘트의 가장 가까운 점까지의 거리를 반환합니다.

```lua
local Point = UE4.Vector.new(0.0, 0.0, 0.0)
local SegmentStart = UE4.Vector.new(1.0, 2.0, 3.0)
local Segment = UE4.Vector.new(4.0, 5.0, 6.0)

local Return1 = UE4.Vector.DistancePointToSegment(Point, SegmentStart, Segment) -- 3.7416
local Return2 = Point:DistancePointToSegment(SegmentStart, Segment) -- 3.7416
```

## UE4.Vector.DistancePointToLine(Point, LineOrigin, LineDirection)

원점, 방향으로 정의된 주어진 선에 대한 월드 공간에서 주어진 점의 거리를 계산합니다.

```lua
local Point = UE4.Vector.new(0.0, 0.0, 0.0)
local LineOrigin = UE4.Vector.new(1.0, 2.0, 3.0)
local LineDirection = UE4.Vector.new(4.0, 5.0, 6.0)

local Return1 = UE4.Vector.DistancePointToLine(Point, LineOrigin, LineDirection) -- 0.8374
local Return2 = Point:DistancePointToLine(LineOrigin, LineDirection) -- 0.8374
```

## UE4.Vector.IsNearlyZero(V, Tolerance?)

지정된 허용 오차 내에서 벡터가 0에 가까워 졌는지 확인합니다.

```lua
local Vector1 = UE4.Vector.new(0.000001, 0.000001, 0.000001) -- KINDA_SMALL_NUMBER
local Vector2 = UE4.Vector.new(0.1, 0.2, 0.3)
local Tolerance = 0.4

local Return1 = UE4.Vector.IsNearlyZero(Vector1) -- true
local Return2 = Vector1:IsNearlyZero() -- true
local Return3 = UE4.Vector.IsNearlyZero(Vector2) -- false
local Return4 = Vector2:IsNearlyZero() -- false
local Return5 = UE4.Vector.IsNearlyZero(Vector2, Tolerance) -- true
local Return6 = Vector2:IsNearlyZero(Tolerance) -- true
```

## UE4.Vector.IsZero(V)

벡터의 모든 구성 요소가 정확하게 0인지 확인합니다.

```lua
local Vector = UE4.Vector.new(0.000001, 0.000001, 0.000001) -- KINDA_SMALL_NUMBER

local Return1 = UE4.Vector.IsZero(Vector) -- false
local Return2 = Vector:IsZero() -- false
```

## UE4.Vector.IsNormalized(V)

벡터가 정규화되었는지 확인합니다.

```lua
local Vector = UE4.Vector.new(1.0, 0.0, 0.0)

local Return1 = UE4.Vector.IsNormalized(Vector) -- true
local Return2 = Vector:IsNormalized() -- true
```

## UE4.Vector.UnrotateByRotator(V, Rotator)

Rotator의 역에 의해 회전 된 벡터를 돌려줍니다.

```lua
local Vector = UE4.Vector.new(1.0, 2.0, 3.0)
local Rotator = UE4.Rotator.new(-1.0, 2.0, -3.0)

local Return1 = UE4.Vector.UnrotateByRotator(Vector, Rotator) -- Vector(1.0166, 2.1191, 2.9112)
local Return2 = Vector:UnrotateByRotator(Rotator) -- Vector(1.0166, 2.1191, 2.9112)
```

## UE4.Vector.RotateByRotator(V, Rotator)

Rotator에 의해 회전 된 벡터를 돌려줍니다.

```lua  
local Vector = UE4.Vector.new(1.0, 2.0, 3.0)
local Rotator = UE4.Rotator.new(-1.0, 2.0, -3.0)

local Return1 = UE4.Vector.RotateByRotator(Vector, Rotator) -- Vector(0.9890, 1.8759, 3.0826)
local Return2 = Vector:RotateByRotator(Rotator) -- Vector(0.9890, 1.8759, 3.0826)
```

## UE4.Vector.RotateByAngleAxis(V, AngleDeg, Axis)

Axis를 중심으로 회전합니다. (Axis.Size () == 1로 가정)

```lua
local Vector = UE4.Vector.new(1.0, 2.0, 3.0)
local AngleDeg = 180
local Axis = UE4.Vector.new(1.0, 0.0, 0.0)

local Return1 = UE4.Vector.RotateByAngleAxis(Vector, AngleDeg, Axis) -- Vector(1.0, -2.0, -3.0)
local Return2 = Vector:RotateByAngleAxis(AngleDeg, Axis) -- Vector(1.0, -2.0, -3.0)
```

## UE4.Vector.Lerp(V1, V2, Alpha)

알파를 기준으로 V1와 V2를 선형적으로 보간합니다. (알파 = 0인 경우 A의 100%, 알파 = 1인 경우 B의 100%)

```lua
local Vector1 = UE4.Vector.new(1.0, 2.0, 3.0)
local Vector2 = UE4.Vector.new(3.0, 4.0, 5.0)
local Alpha = 0.5

local Return1 = UE4.Vector.Lerp(Vector1, Vector2, Alpha) -- Vector(2.0, 3.0, 4.0)
local Return2 = Vector1:Lerp(Vector2, Alpha) -- Vector(2.0, 3.0, 4.0)
```

## UE4.Vector.Reflection(V, SurfaceNormal)

방향 벡터와 표면 법선이 주어지면 표면 법선에 반사된 벡터를 반환합니다.

```lua
local Vector = UE4.Vector.new(1.0, 2.0, 3.0)
local SurfaceNormal = UE4.Vector.new(1.0, 0.0, 0.0)

local Return1 = UE4.Vector.Reflection(Vector, SurfaceNormal) -- Vector(-1.0, 2.0, 3.0)
local Return2 = Vector:Reflection(SurfaceNormal) -- Vector(-1.0, 2.0, 3.0)
```

## UE4.Vector.FindClosestPointOnSegment(Point, SegmentStart, SegmentEnd)

주어진 점과 가장 가까운 세그먼트 위의 점을 반환합니다.

```lua
local Point = UE4.Vector.new(0.0, 0.0, 0.0)
local SegmentStart = UE4.Vector.new(1.0, 2.0, 3.0)
local Segment = UE4.Vector.new(4.0, 5.0, 6.0)

local Return1 = UE4.Vector.FindClosestPointOnSegment(Point, SegmentStart, Segment) -- Vector(1.0, 2.0, 3.0)
local Return2 = Point:FindClosestPointOnSegment(SegmentStart, Segment) -- Vector(1.0, 2.0, 3.0)
```

## UE4.Vector.FindClosestPointOnLine(Point, LineOrigin, LineDirection)

주어진 점과 가장 가까운 선 위의 점을 반환합니다.

```lua
local Point = UE4.Vector.new(0.0, 0.0, 0.0)
local LineOrigin = UE4.Vector.new(1.0, 2.0, 3.0)
local LineDirection = UE4.Vector.new(4.0, 5.0, 6.0)

local Return1 = UE4.Vector.FindClosestPointOnLine(Point, LineOrigin, LineDirection) -- Vector(-0.6623, -0.0779, 0.5064)
local Return2 = Point:FindClosestPointOnLine(LineOrigin, LineDirection) -- Vector(-0.6623, -0.0779, 0.5064)
```

## UE4.Vector.ProjectVectorOnToVector(V1, V2) 

입력 벡터에 투영된 벡터를 반환합니다.

```lua
local Vector1 = UE4.Vector.new(1.0, 2.0, 3.0)
local Vector2 = UE4.Vector.new(4.0, 5.0, 6.0)

local Return1 = UE4.Vector.ProjectVectorOnToVector(Vector1, Vector2) -- Vector(-1.6623, 2.0779, 2.4935)
local Return2 = Vector1:ProjectVectorOnToVector(Vector2) -- Vector(-1.6623, 2.0779, 2.4935)
```

## UE4.Vector.ProjectPointOnToPlane(Point, PlaneBase, PlaneNormal)

PlaneBase 및 PlaneNormal에 의해 정의된 평면에서 점의 투영을 계산합니다.

```lua
local Point = UE4.Vector.new(0.0, 0.0, 0.0)
local PlaneBase = UE4.Vector.new(1.0, 2.0, 3.0)
local PlaneNormal = UE4.Vector.new(4.0, 5.0, 6.0)

local Return1 = UE4.Vector.ProjectPointOnToPlane(Point, PlaneBase, PlaneNormal) -- Vector(-128.0, 160.0, 192.0)
local Return2 = Point:ProjectPointOnToPlane(PlaneBase, PlaneNormal) -- Vector(-128.0, 160.0, 192.0)
```

## UE4.Vector.ProjectVectorOnToPlane(V, PlaneNormal)

PlaneNormal에 의해 정의된 평면에서 벡터의 투영을 계산합니다.

```lua
local Vector = UE4.Vector.new(1.0, 2.0, 3.0)
local PlaneNormal = UE4.Vector.new(4.0, 5.0, 6.0)

local Return1 = UE4.Vector.ProjectVectorOnToVector(Vector, PlaneNormal)  -- Vector(-1.6623, 2.0779, 2.4935)
local Return2 = Vector:ProjectVectorOnToVector(PlaneNormal)  -- Vector(-1.6623, 2.0779, 2.4935)
```

## UE4.Vector.GetMin(V)

벡터 구성 요소 중 최소값을 가져옵니다.

```lua
local Vector = UE4.Vector.new(1.0, 2.0, 3.0)

local Return1 = UE4.Vector.GetMin(Vector) -- 1.0
local Return2 = Vector:GetMin() -- 1.0
```

## UE4.Vector.GetMax(V)

벡터 구성 요소 중 최대값을 가져옵니다.

```lua
local Vector = UE4.Vector.new(1.0, 2.0, 3.0)

local Return1 = UE4.Vector.GetMax(Vector) -- 3.0
local Return2 = Vector:GetMax() -- 3.0
```

## UE4.Vector.GetNormal(V, Tolerance?)

Vector의 Normal 벡터를 가져옵니다. 길이에 따라 그렇게 하는 것이 안전한지 확인합니다. 벡터 길이가 허용 범위보다 작아서 안전하게 계산할 수 없는 경우 0 벡터를 반환합니다.

```lua
local Vector = UE4.Vector.new(1.0, 1.0, 1.0)

local Return1 = UE4.Vector.GetNormal(Vector) -- Vector(0.5773, 0.5773, 0.5773)
local Return2 = Vector:GetNormal() -- Vector(0.5773, 0.5773, 0.5773)
```

## UE4.Vector.GetUnitDirection(PointStart, PointEnd)

한 위치에서 다른 위치로 단위 방향 벡터를 찾습니다.

```lua
local Point1 = UE4.Vector.new(0.0, 0.0, 0.0)
local Point2 = UE4.Vector.new(1.0, 1.0, 1.0)

local Return1 = UE4.Vector.GetUnitDirection(Point1, Point2) -- Vector(0.5773, 0.5773, 0.5773)
local Return2 = Point1:GetUnitDirection(Point2) -- Vector(0.5773, 0.5773, 0.5773)
```

## UE4.Vector.ToTransform(V)

벡터를 트랜스폼으로 변환합니다. 벡터를 위치로 사용합니다.

```lua
local Vector = UE4.Vector.new(1.0, 2.0, 3.0)

local Return1 = UE4.Vector.ToTransform(Vector) -- Transform(Translation(1.0, 2.0, 3.0))
local Return2 = Vector:ToTransform() -- Transform(Translation(1.0, 2.0, 3.0))
```

------------------------------------
[라이브러리](Library_ko.md)로 돌아가기