
C++에서 VM 인스턴스를 생성하기
============================

이 문서에서는 C++에서 VM을 만드는 방법에 대해서 설명합니다.

1. "LuaVirtualMachine.h"을 인클루드 합니다.

이 단계에서 컴파일 오류가 발생할 경우 [게임 모듈 설정하기](HowToSetupGameModule.md)를 하지 않았는지 확인해보세요.

2. 다음과 같은 코드를 작성합니다.

```cpp
auto VM = FSUE4LuaVirtualMachine::Create();
if (VM.IsValid())
{
    // ok
}
```

반환 타입은 `TSharedPtr<FSUE4LuaVirtualMachine>` 입니다.

디버깅이 활성화된 VM을 생성하려면 인자로 `true`를 전달합니다.

```cpp
auto VM = FSUE4LuaVirtualMachine::Create(true);
if (VM.IsValid())
{
    // ok
}
```

참고:
* VM 어셋이 생성한 VM 인스턴스는 디버깅이 활성화되어 있습니다.

----------------------------------------------------
[프로그래밍 가이드](ProgrammingGuide_ko.md)로 돌아가기
