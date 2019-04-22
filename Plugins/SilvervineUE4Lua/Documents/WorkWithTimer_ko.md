
Lua에서 Timer 사용하기
=====================

SUE4Lua가 UE4의 타이머에 대한 추가 기능을 제공하지는 않습니다.
하지만 `UKismetSystemLibrary`를 사용해서 Blueprint와 비슷한 방식으로 타이머를 사용할 수 있습니다.

먼저 `UKismetSystemLibrary` 클래스를 얻습니다.
```lua
local UKismetSystemLibrary = UE4.FindClass("KismetSystemLibrary")
```

타이머 생성 함수는 `SetTimer()` 입니다.
```lua
UKismetSystemLibrary.SetTimer(Obj, TimerFuncName, 1, false)
```

위의 코드가 실행되면 엔진은 Obj의 TimerFuncName 값과 같은 이름의 함수를 1초 후에 실행합니다. 

타이머로 호출할 함수는 반드시 UFunction이어야 합니다. 당연히 Blueprint 함수도 호출할 수 있습니다.

나머지 타이머 함수의 사용 방법은 아래 코드를 참고하세요.
```lua
if UKismetSystemLibrary.TimerExists(Obj, TimerFuncName) then
    SUE4Lua.Log("TimerExists() returns true.")

    if UKismetSystemLibrary.IsTimerActive(Obj, TimerFuncName) then
        SUE4Lua.Log("IsTimerActive() returns true.")

        UKismetSystemLibrary.PauseTimer(Obj, TimerFuncName)
    end

    if UKismetSystemLibrary.IsTimerPaused(Obj, TimerFuncName) then
        SUE4Lua.Log("IsTimerPaused() returns true.")

        UKismetSystemLibrary.UnPauseTimer(Obj, TimerFuncName)
    end

    SUE4Lua.Log("ElapsedTime", UKismetSystemLibrary.GetTimerElapsedTime(Obj, TimerFuncName))
    SUE4Lua.Log("RemainingTime", UKismetSystemLibrary.GetTimerRemainingTime(Obj, TimerFuncName))

    UKismetSystemLibrary.ClearTimer(Obj, TimerFuncName)
end

SUE4Lua.Log("TimerExists()", UKismetSystemLibrary.TimerExists(Obj, TimerFuncName))

-- TimerExists() returns true.
-- IsTimerActive() returns true.
-- IsTimerPaused() returns true.
-- ElapsedTime    0.0
-- RemainingTime  1.0
-- TimerExists()  false
```

테스트 코드 전체는 [여기](../Source/SilvervineUE4Lua/Private/Scripts/Tests/TimerTest.lua)에 있습니다.

----------------------------------------------------
[프로그래밍 가이드](ProgrammingGuide_ko.md)로 돌아가기