
VM 어셋에 게임 인스턴스 등록하기
==============================

게임에서 VM 어셋을 사용하려면 게임 시작 후에 VM 어셋에 게임 인스턴스를 등록해야 합니다.
이 과정이 누락되면 VM 어셋을 통한 모든 함수 호출이 동작하지 않습니다.

C++에서 직접 VM을 생성했다면 아래 과정을 생략해도 상관 없습니다.

편의상 Blueprint로 구현했습니다만 C++도 거의 유사한 방법으로 구현할 수 있습니다.

1. 에디터에서 게임 인스턴스 Blueprint를 열고 LuaVM 변수를 추가합니다.

![](Images/AddLuaVMToGameInstance.png)

* 변수 형식은 SUE4Lua Virtual Machine 입니다.
* 기본값은 만들어두신 VM 어셋으로 설정하시면 됩니다.

2. Init, Shutdown 이벤트에 다음과 같은 코드를 추가합니다.

![](Images/RegisterGameInstance.png)

----------------------------------------------------
[프로그래밍 가이드](ProgrammingGuide_ko.md)로 돌아가기