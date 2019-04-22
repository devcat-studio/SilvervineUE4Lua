# UE4 Library

UE4의 기본적인 기능을 지원하는 라이브러리입니다.

## UE4.FindClass(ClassName)

입력 이름과 일치하는 UClass를 찾아서 반환합니다.

```lua
local Class = UE4.FindClass("Actor")
```

## UE4.LoadObject(Class, Outer, Name)

입력 정보와 일치하는 UObject를 불러옵니다.

```lua
local Class = UE4.FindClass("LevelSequence")
local ObjectName = "/Game/ObjectName.ObjectName_C"

local Object = UE4.LoadObject(Class, nil, ObjectName)
```

## UE4.NewObject(Class, Outer)

입력 정보와 일치하는 UObject를 생성하여 반환합니다.

```lua
local Class = UE4.FindClass("LevelSequence")

local Object = UE4.NewObject(Class, Outer)
```

## UE4.NewStruct(StructName)

입력 정보와 일치하는 UStruct를 생성하여 반환합니다.

```lua
local Struct = UE4.NewStruct("SlateColor")
Struct.SpecifiedColor = UE4.LinearColor.new(0.66, 0.66, 0.66, 1.0)
```

## UE4.CreateWidget(OwningObject, Class)

입력 정보와 일치하는 위젯을 생성하여 반환합니다.

```lua
local UObject = UE4.FindClass("Object")
local WidgetName = "/Game/WidgetName.WidgetName_C"
local WidgetClass = UE4.LoadObject(UObject, nil, WidgetName)

local Widget = UE4.CreateWidget(self:GetWorld(), WidgetClass)
Widget:AddToViewport(0)
```

## UE4.IsValid(UObject)

입력 UObject가 유효한 지(non null and not garbage collected) 확인합니다.

------------------------------------
[라이브러리](Library_ko.md)로 돌아가기