# UE4 Vector2D Library

SilevervineUE4Lua에서 제공하는 Vector2D 함수 라이브러리입니다.

## operator - (V)

벡터의 단항 부정 연산자의 결과를 가져옵니다.

```lua
local Vector2D = UE4.Vector2D.new(1.0, 2.0)

local Result1 = -Vector2D -- Vector2D(-1.0, -2.0)
```

## operator + | - (V, Float)

벡터 V의 각 구성 요소에 Float 값으로 덧셈 | 뺄셈한 결과를 가져옵니다.

```lua
local Vector2D = UE4.Vector2D.new(4.0, 3.0)
local Float = 2.0

local Result1 = Vector2D + Float -- Vector2D(6.0, 5.0)
local Result2 = Vector2D - Float -- Vector2D(2.0, 1.0)
```

## operator + | - (V1, V2)

두 벡터 V1, V2를 덧셈 | 뺄셈한 결과를 가져옵니다.

```lua
local Vector2D1 = UE4.Vector2D.new(3.0, 4.0)
local Vector2D2 = UE4.Vector2D.new(1.0, 2.0)

local Result1 = Vector2D1 + Vector2D2 -- Vector2D(4.0, 6.0)
local Result2 = Vector2D1 - Vector2D2 -- Vector2D(2.0, 2.0)
```

## operator * | / (V, Scale)

벡터의 각 구성 요소를 Scale 값으로 곱셈 | 나눗셈한 결과를 가져옵니다.

```lua
local Vector2D = UE4.Vector2D.new(6.0, 4.0)
local Scale = 2.0

local Result1 = Vector2D * Scale -- Vector2D(12.0, 8.0)
local Result2 = Vector2D / Scale -- Vector2D(3.0, 2.0)
```

## operator * | / (V1, V2)

벡터 V1를 다른 벡터 V2 구성 요소별로 곱셈 | 나눗셈한 결과를 가져옵니다.

```lua
local Vector2D1 = UE4.Vector2D.new(6.0, 4.0)
local Vector2D2 = UE4.Vector2D.new(3.0, 2.0)

local Result1 = Vector2D1 * Vector2D2 -- Vector2D(18.0, 8.0)
local Result2 = Vector2D1 / Vector2D2 -- Vector2D(2.0, 2.0)
```

## operator == (V1, V2)

두 벡터 V1, V2가 동일한지 확인합니다.

```lua
local Vector2D1 = UE4.Vector2D.new(3.0, 2.0)
local Vector2D2 = UE4.Vector2D.new(3.0, 2.0)

local Result = Vector2D1 == Vector2D2 -- true
```

## UE4.Vector.Equals(V1, V2, Tolerance?)

지정된 허용 오차 내에서 두 벡터 V1, V2가 동일한지 확인합니다.

```lua
local Vector2D1 = UE4.Vector2D.new(6.0, 5.0)
local Vector2D2 = UE4.Vector2D.new(3.0, 2.0)
local Tolerance = 4.0

local Result1 = UE4.Vector2D.Equals(Vector2D1, Vector2D2) -- false
local Result2 = Vector2D1:Equals(Vector2D2) -- false
local Result3 = UE4.Vector2D.Equals(Vector2D1, Vector2D2, Tolerance) -- true
local Result4 = Vector2D1:Equals(Vector2D2, Tolerance) -- true
```

## UE4.Vector2D.DotProduct(V1, V2)

두 벡터의 내적을 계산합니다.

```lua
local Vector2D1 = UE4.Vector2D.new(1.0, 2.0)
local Vector2D2 = UE4.Vector2D.new(4.0, 5.0)

local Result1 = UE4.Vector2D.DotProduct(Vector2D1, Vector2D2) -- 14.0
local Result2 = Vector2D1:DotProduct(Vector2D2) -- 14.0
```

## UE4.Vector2D.CrossProduct(V1, V2)

두 벡터의 외적을 계산합니다. 반환값은 float 입니다.

```lua
local Vector2D1 = UE4.Vector2D.new(1.0, 2.0)
local Vector2D2 = UE4.Vector2D.new(4.0, 5.0)

local Result1 = UE4.Vector2D.CrossProduct(Vector2D1, Vector2D2) -- -3.0
local Result2 = Vector2D1:CrossProduct(Vector2D2) -- -3.0
```

## UE4.Vector2D.Size(V)

벡터의 길이(크기)를 가져옵니다.

```lua
local Vector2D = UE4.Vector2D.new(1.0, 2.0)

local Result1 = UE4.Vector2D.Size(Vector2D) -- 2.2360
local Result2 = Vector2D:Size() -- 2.2360
```

## UE4.Vector2D.SizeSquared(V)

벡터의 제곱 길이를 가져옵니다.

```lua
local Vector2D = UE4.Vector2D.new(1.0, 2.0)

local Result1 = UE4.Vector2D.SizeSquared(Vector2D) -- 5.0
local Result2 = Vector2D:SizeSquared() -- 5.0
```

## UE4.Vector2D.Distance(V1, V2)

두 점 사이의 유클리드 거리를 반환합니다.

```lua
local Vector2D1 = UE4.Vector2D.new(1.0, 2.0)
local Vector2D2 = UE4.Vector2D.new(3.0, 4.0)

local Result1 = UE4.Vector2D.Distance(Vector2D1, Vector2D2) -- 2.8284
local Result2 = Vector2D1:Distance(Vector2D2) -- 2.8284
```

## UE4.Vector2D.DistanceSquared(V1, V2)

두 점 사이의 제곱 거리를 반환합니다.

```lua
local Vector2D1 = UE4.Vector2D.new(1.0, 2.0)
local Vector2D2 = UE4.Vector2D.new(3.0, 4.0)

local Return1 = UE4.Vector2D.DistanceSquared(Vector2D1, Vector2D2) -- 8.0
local Return2 = Vector2D1:DistanceSquared(Vector2D2) -- 8.0
```

## UE4.Vector2D.IsNearlyZero(V, Tolerance?)

지정된 허용 오차 내에서 벡터가 0에 가까워 졌는지 확인합니다.

```lua
local Vector2D1 = UE4.Vector2D.new(0.000001, 0.000001) -- KINDA_SMALL_NUMBER
local Vector2D2 = UE4.Vector2D.new(0.1, 0.2)
local Tolerance = 0.3

local Result1 = UE4.Vector2D.IsNearlyZero(Vector2D1) -- true
local Result2 = Vector2D1:IsNearlyZero() -- true
local Result3 = UE4.Vector2D.IsNearlyZero(Vector2D2) -- false
local Result4 = Vector2D2:IsNearlyZero() -- false
local Result5 = UE4.Vector2D.IsNearlyZero(Vector2D2, Tolerance) -- true
local Result6 = Vector2D2:IsNearlyZero(Tolerance) -- true
```

## UE4.Vector2D.IsZero(V)

벡터의 모든 구성 요소가 정확하게 0인지 확인합니다.

```lua
local Vector2D = UE4.Vector2D.new(0.000001, 0.000001) -- KINDA_SMALL_NUMBER

local Result1 = UE4.Vector2D.IsZero(Vector2D) -- false
local Result2 = Vector2D:IsZero() -- false
```

## UE4.Vector2D.Rotate(V, AngleDeg)

축 (0,0,1) 주위로 AngleDeg만큼 회전합니다.

```lua
local Vector2D = UE4.Vector2D.new(1.0, 2.0)
local AngleDeg = 180.0

local Result1 = UE4.Vector2D.Rotate(Vector2D, AngleDeg) -- Vector2D(-1.0, -2.0)
local Result2 = Vector2D:Rotate(AngleDeg) -- Vector2D(-1.0, -2.0)-- 
```

## UE4.Vector2D.Lerp(V1, V2, Alpha)

알파를 기준으로 V1와 V2를 선형적으로 보간합니다. (알파 = 0인 경우 A의 100%, 알파 = 1인 경우 B의 100%)

```lua
local Vector2D1 = UE4.Vector2D.new(1.0, 2.0)
local Vector2D2 = UE4.Vector2D.new(3.0, 4.0)
local Alpha = 0.5

local Result1 = UE4.Vector2D.Lerp(Vector2D1, Vector2D2, Alpha) -- Vector2D(2.0, 3.0)
local Result2 = Vector2D1:Lerp(Vector2D2, Alpha) -- Vector2D(2.0, 3.0)
```

## UE4.Vector2D.GetMin(V)

벡터의 컴퍼넌트의 최소값을 가져옵니다.

```lua
local Vector2D = UE4.Vector2D.new(1.0, 2.0)

local Result1 = UE4.Vector2D.GetMin(Vector2D) -- 1.0
local Result2 = Vector2D:GetMin() -- 1.0
```

## UE4.Vector2D.GetMax(V)

벡터의 컴퍼넌트의 최대값을 가져옵니다.

```lua
local Vector2D = UE4.Vector2D.new(1.0, 2.0)

local Result1 = UE4.Vector2D.GetMax(Vector2D) -- 2.0
local Result2 = Vector2D:GetMax() -- 2.0
```

## UE4.Vector2D.GetNormal(V, Tolerance?)

Vector의 Normal 벡터를 가져옵니다. 길이에 따라 그렇게 하는 것이 안전한지 확인합니다. 벡터 길이가 허용 범위보다 작아서 안전하게 계산할 수 없는 경우 0 벡터를 반환합니다.

```lua
local Vector2D = UE4.Vector2D.new(1.0, 1.0)

local Result1 = UE4.Vector2D.GetNormal(Vector2D) -- Vector2D(0.7071, 0.7071)
local Result2 = Vector2D:GetNormal() -- Vector2D(0.7071, 0.7071)
```

## UE4.Vector2D.GetUnitDirection(PointStart, PointEnd)

한 위치에서 다른 위치로 단위 방향 벡터를 찾습니다.

```lua
local Point1 = UE4.Vector2D.new(0.0, 0.0)
local Point2 = UE4.Vector2D.new(1.0, 1.0)

local Result1 = UE4.Vector2D.GetUnitDirection(Point1, Point2) -- Vector2D(0.7071, 0.7071)
local Result2 = Point1:GetUnitDirection(Point2) -- Vector2D(0.7071, 0.7071)
```

------------------------------------
[라이브러리](Library_ko.md)로 돌아가기