
VM 어셋 생성하기
===============

lua 코드를 실행하기 위해서는 먼저 Lua VM(Virtual Machine)을 만들어야 합니다. 
C++ 코드로 VM을 간단하게 생성할 수도 있으나 UE4 어셋(*.uasset)으로 VM을 생성하는 것이 여러 가지로 편리합니다.

C++로 직접 VM 인스턴스를 생성하는 방법은 [여기](HowToCreateVMInstance_ko.md)를 참고하세요.

참고: 
* VM 어셋을 여러 개 만들어도 전혀 문제 없습니다. 하지만 다른 VM으로 Lua 값을 전달할 수 없으니 주의하시기 바랍니다. 

VM 어셋 생성하기
---------------

![](Images/VMAsset.png)

1. 컨텐츠 브라우저에서 우클릭하여 컨텍스트 메뉴를 엽니다.
2. Miscellaneous -> SUE4Lua Virtual Machine 을 선택합니다.
3. 부모 클래스를 선택하는 창이 열리면 SUE4LuaVirtualMachine을 선택합니다.

VM 속성 편집하기
---------------

![](Images/EditingVM.png)

컨텐츠 브라우저에서 VM 어셋을 더블클릭하면 VM 어셋 편집 창이 열립니다.

* Startup File Path

    VM이 시작되면 지정된 파일을 실행합니다. 

----------------------------------------------------
[프로그래밍 가이드](ProgrammingGuide_ko.md)로 돌아가기