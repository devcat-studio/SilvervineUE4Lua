
Programming Guide
=================

이 문서에서는 SUE4Lua 플러그인의 초기화 및 작업 환경 설정, Lua와의 데이터 교환, 함수 호출 방법 등을 설명합니다.

플러그인 설치 방법에 대해서는 [README](README_ko.md) 문서를 참고하세요.

코딩 컨벤션
----------
C++, Lua 모두 [UE4 코딩 표준](https://docs.unrealengine.com/en-us/Programming/Development/CodingStandard)을 사용합니다.

시작하기
--------

* [게임 모듈 설정하기](HowToSetupGameModule_ko.md)
* [VM 어셋 생성하기](HowToCreateVMAsset_ko.md)
* [VM 어셋에 게임 인스턴스 등록하기](HowToRegisterGameInstance_ko.md)
* [C++에서 VM 인스턴스를 생성하기](HowToCreateVMInstance_ko.md)

작업 환경 설정
-------------

* [Lua 파일 로더 설정하기](HowToSetupFileLoader_ko.md)
* [Visual Studio Code로 작업하기](WorkWithVSCode_ko.md)
* [유닛 테스트](UnitTest_ko.md)

Lua 프로그래밍 가이드
--------------------

* [라이브러리](Library_ko.md)
* [디버그 텍스트 출력하기](WorkWithDebugPrint_ko.md)
* [Lua에서 UObject 사용하기](WorkWithUObject_ko.md)
* [Lua에서 UStruct 사용하기](WorkWithUStruct_ko.md)
* [Lua에서 UEnum 사용하기](WorkWithUEnum_ko.md)
* [Lua에서 Timer 사용하기](WorkWithTimer_ko.md)
* [Blueprint 함수를 Lua로 구현하기](WorkWithBlueprintDispatch_ko.md) 
* [C++ 함수를 Lua로 구현하기](WorkWithNativeDispatch_ko.md) 
* [디스패치 핸들러 팩토리 설정하기](HowToSetupDispatchHandlerFactory_ko.md)
* [Lua에서 디스패치 핸들러의 함수를 호출하기](WorkWithDirectCall_ko.md) 
* [LuaValue 사용하기](WorkWithLuaValue_ko.md)
* [Lua 파일 핫 리로딩](LuaHotReloading_ko.md)