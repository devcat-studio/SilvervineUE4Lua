# UE4 DateTime Library

SilvervineUE4Lua에서 제공하는 DateTime 함수 라이브러리입니다.

## operator + (DT, Timespan)

날짜시간과 시간범위를 덧셈한 결과를 가져옵니다. 결과값은 DateTime입니다.

```lua
local DateTime = UE4.DateTime.new(100)
local Timespan = UE4.Timespan.new(100)

local Return = DateTime + Timespan -- DateTime(200)
```

## operator - (DT1, DT2)

두 날짜시간 DT1, DT2를 뺄셈한 결과를 가져옵니다. 결과값은 시간범위입니다.

```lua
local DateTime1 = UE4.DateTime.new(200)
local DateTime2 = UE4.DateTime.new(100)

local Return = DateTime1 - DateTime2 -- Timespan(100)
```

## operator - (DT, Timespan)

날짜시간 DT와 시간범위 Timespan을 뺄셈한 결과를 가져옵니다. 결과값은 날짜시간입니다.

```lua
local DateTime = UE4.DateTime.new(200)
local Timespan = UE4.Timespan.new(100)

local Return = DateTime - Timespan -- DateTime(100)
```

## operator == (DT1, DT2)

두 날짜시간 DT1과 DT2가 동일한지 확인합니다.

```lua
local DateTime1 = UE4.DateTime.new(200)
local DateTime2 = UE4.DateTime.new(200)

local Return = DateTime1 == DateTime2 -- true
```

## UE4.DateTime.GetDate(DT)

날짜시간 DT의 날짜 부분을 가져옵니다. 시간 부분은 잘려 00:00:00.000이 됩니다.

```lua
local DateTime = UE4.DateTime.new(636940964199390000) -- 2019/05/22 04:34:39.939

local Return1 = UE4.DateTime.GetDate(DateTime) -- DateTime(636940800000000000) 2019/05/22 00:00:00.000
local Return2 = DateTime:GetDate() -- DateTime(636940800000000000) 2019/05/22 00:00:00.000
```

## UE4.DateTime.GetDay(DT)

날짜시간 DT의 날짜 구성 요소 반환합니다. (1 ~ 31)

```lua
local DateTime = UE4.DateTime.new(636940964199390000) -- 2019/05/22 04:34:39.939

local Return1 = UE4.DateTime.GetDay(DateTime) -- 22
local Return2 = DateTime:GetDay() -- 22
```

## UE4.DateTime.GetDayOfYear(DT)

날짜시간 DT의 그 해의 날을 돌려줍니다.

```lua
local DateTime = UE4.DateTime.new(636940964199390000) -- 2019/05/22 04:34:39.939

local Return1 = UE4.DateTime.GetDayOfYear(DateTime) -- 142
local Return2 = DateTime:GetDayOfYear() -- 142
```

## UE4.DateTime.GetHour(DT)

날짜시간 DT의 시간 구성 요소를 반환합니다. (24h 형식)

```lua
local DateTime = UE4.DateTime.new(636940964199390000) -- 2019/05/22 04:34:39.939

local Return1 = UE4.DateTime.GetHour(DateTime) -- 4
local Return2 = DateTime:GetHour() -- 4
```

## UE4.DateTime.GetHour12(DT)

날짜시간 DT의 시간 구성 요소를 반환합니다. (12h 형식)

```lua
local DateTime = UE4.DateTime.new(636940964199390000) -- 2019/05/22 04:34:39.939

local Return1 = UE4.DateTime.GetHour12(DateTime) -- 4
local Return2 = DateTime:GetHour12() -- 4
```

## UE4.DateTime.GetMillisecond(DT)

날짜시간 DT의 밀리 초 구성 요소를 반환합니다.

```lua
local DateTime = UE4.DateTime.new(636940964199390000) -- 2019/05/22 04:34:39.939

local Return1 = UE4.DateTime.GetMillisecond(DateTime) -- 939
local Return2 = DateTime:GetMillisecond() -- 939
```

## UE4.DateTime.GetMinute(DT)

날짜시간 DT의 분 구성 요소를 반환합니다.

```lua
local DateTime = UE4.DateTime.new(636940964199390000) -- 2019/05/22 04:34:39.939

local Return1 = UE4.DateTime.GetMinute(DateTime) -- 33
local Return2 = DateTime:GetMinute() -- 33
```

## UE4.DateTime.GetMonth(DT)

날짜시간 DT의 달 구성 요소를 반환합니다.

```lua
local DateTime = UE4.DateTime.new(636940964199390000) -- 2019/05/22 04:34:39.939

local Return1 = UE4.DateTime.GetMonth(DateTime) -- 5
local Return2 = DateTime:GetMonth() -- 5
```

## UE4.DateTime.GetSecond(DT)

날짜시간 DT의 초 구성 요소를 반환합니다.

```lua
local DateTime = UE4.DateTime.new(636940964199390000) -- 2019/05/22 04:34:39.939

local Return1 = UE4.DateTime.GetSecond(DateTime) -- 39
local Return2 = DateTime:GetSecond() -- 39
```

## UE4.DateTime.GetTimeOfDay(DT)

날짜시간 DT의 자정 이후 경과한 시간 Timespan을 반환합니다.

```lua
local DateTime = UE4.DateTime.new(636940964199390000) -- 2019/05/22 04:34:39.939

local Return1 = UE4.DateTime.GetTimeOfDay(DateTime) -- Timespan(164199390000)
local Return2 = DateTime:GetTimeOfDay() -- Timespan(164199390000)
```

## UE4.DateTime.GetYear(DT)

날짜시간 DT의 해 구성 요소를 반환합니다.

```lua
local DateTime = UE4.DateTime.new(636940964199390000) -- 2019/05/22 04:34:39.939

local Return1 = UE4.DateTime.GetYear(DateTime) -- 2019
local Return2 = DateTime:GetYear() -- 2019
```

## UE4.DateTime.IsAfternoon(DT)

날짜시간 DT의 시간이 오후에 있는지 여부를 반환합니다.

```lua
local DateTime = UE4.DateTime.new(636940964199390000) -- 2019/05/22 04:34:39.939

local Return1 = UE4.DateTime.IsAfternoon(DateTime) -- false
local Return2 = DateTime:IsAfternoon() -- false
```

## UE4.DateTime.IsMorning(DT)

날짜시간 DT의 시간이 오전에 있는지 여부를 반환합니다.

```lua
local DateTime = UE4.DateTime.new(636940964199390000) -- 2019/05/22 04:34:39.939

local Return1 = UE4.DateTime.IsMorning(DateTime) -- true
local Return2 = DateTime:IsMorning() -- true
```

## UE4.DateTime.DaysInYear(Year)

주어진 해의 일수를 반환합니다.

```lua
local Year = 2000

local Return = UE4.DateTime.DaysInYear(Year) -- 366
```

## UE4.DateTime.DaysInMonth(Year, Month)

주어진 해와 달의 일수를 반환합니다.

```lua
local Year = 2000
local Month = 12

local Return = UE4.DateTime.DaysInMonth(Year, Month) -- 31
```

## UE4.DateTime.IsLeapYear(Year)

주어진 해가 윤년인지 여부를 반환합니다.

```lua
local Year = 2000

local Return = UE4.DateTime.IsLeapYear(2000) -- true
```

## UE4.DateTime.MaxValue()

최대 날짜 및 시간 값을 반환합니다.

```lua
local Return = UE4.DateTime.MaxValue() -- DateTime(3155378975999999999) 2019/12/31 23:59:59.999
```

## UE4.DateTime.MinValue()

최소 날짜 및 시간 값을 반환합니다.

```lua
local Return = UE4.DateTime.MinValue() -- DateTime(0) 2019/01/01 00:00:00.000
```

## UE4.DateTime.Now()

이 컴퓨터의 현지 날짜 및 시간을 반환합니다.

## UE4.DateTime.Today()

이 컴퓨터의 현지 날짜를 반환합니다.

## UE4.DateTime.UtcNow()

이 컴퓨터에서 UTC 날짜 및 시간을 반환합니다.

------------------------------------
[라이브러리](Library_ko.md)로 돌아가기