# UE4 QualifiedFrameTime Library

SilvervineUE4Lua에서 제공하는 QualifiedFrameTime 함수 라이브러리입니다.

## UE4.QualifiedFrameTime.AsSeconds(QFT)

프레임시간 QFT를 초 단위의 값으로 변환합니다.

```lua
local FrameTime = UE4.FrameTime.new(UE4.FrameNumber.new(10), 0.1)
local FrameRate = UE4.FrameRate.new(10, 20)
local QualifiedFrameTime = UE4.QualifiedFrameTime.new(FrameTime, FrameRate)

local Return1 = UE4.QualifiedFrameTime.AsSeconds(QualifiedFrameTime) -- 20.2
local Return2 = QualifiedFrameTime:AsSeconds() -- 20.2
```

## UE4.QualifiedFrameTime.ConvertTo(QFT, DesiredFrameRate)

프레임시간 QFT를 프레임속도 DesiredFrameRate를 이용하여 변환합니다.

```lua
local FrameTime = UE4.FrameTime.new(UE4.FrameNumber.new(10), 0.1)
local FrameRate = UE4.FrameRate.new(10, 20)
local QualifiedFrameTime = UE4.QualifiedFrameTime.new(FrameTime, FrameRate)
local DesiredFrameRate = UE4.FrameRate.new(30, 40)

local Return1 = UE4.QualifiedFrameTime.ConvertTo(QualifiedFrameTime, DesiredFrameRate) -- FrameTime(FrameNumber(15), 0.15)
local Return2 = QualifiedFrameTime:ConvertTo(DesiredFrameRate) -- FrameTime(FrameNumber(15), 0.15)
```

------------------------------------
[라이브러리](Library_ko.md)로 돌아가기