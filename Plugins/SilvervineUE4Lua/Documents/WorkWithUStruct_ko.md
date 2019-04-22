
Lua에서 UStruct 사용하기
=======================

Lua에서 UStruct를 사용하는 방법은 UObject와 같습니다. [여기](WorkWithUObject_ko.md)를 참고하세요.

UStruct 생성하기
---------------

`UE4.NewStruct()` 함수를 사용해서 새 UStruct 인스턴스를 생성할 수 있습니다. `UE4.NewObject()`와 달리 인자로 클래스 대신 이름을 사용합니다.
```lua
local Struct = UE4.NewStruct("SlateColor")
Struct.SpecifiedColor = UE4.LinearColor.new(0.66, 0.66, 0.66, 1.0)
```

----------------------------------------------------
[프로그래밍 가이드](ProgrammingGuide_ko.md)로 돌아가기