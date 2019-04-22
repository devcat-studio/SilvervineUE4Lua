# UE4 TextFormatter Library

UE4의 TextFormatter 관련된 기능을 지원하는 라이브러리입니다.

## UE4.TextFormatter.Format(Text, ...)

FText::Format 혹은 블루프린트의 Format Text와 유사한 기능을 제공합니다.

2번째 인자로 테이블 혹은 가변인자를 입력할 수 있습니다.

```lua
local Return1 = UE4.TextFormatter.Format("You currently have {CurrentHealth} health left.", { CurrentHealth = 100 })
-- "You currently have 100 health left."
local Return2 = UE4.TextFormatter.Format("{0} / {1}", 100, 200)
-- "100 / 200"
```

포매팅에 대한 자세한 사항은 [UE4 문서](http://api.unrealengine.com/KOR/Gameplay/Localization/Formatting/)를 참고하세요

------------------------------------
[라이브러리](Library_ko.md)로 돌아가기