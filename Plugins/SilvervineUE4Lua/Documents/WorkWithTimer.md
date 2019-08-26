
Working with Timers in Lua
==========================

SUE4Lua does not provide any functionality for UE4 timers. But, using the `UKismetSystemLibrary`, you can use the timer in a way similar to the blueprint.

First, you need the `UKismetSystemLibrary` class.
```lua
local UKismetSystemLibrary = UE4.FindClass("KismetSystemLibrary")
```

You can create timer using the function `SetTimer()`.
```lua
UKismetSystemLibrary.SetTimer(Obj, TimerFuncName, 1, false)
```

When the above code is executed, the engine executes a function of Obj with the same name as TimerFuncName in 1 second.

The function to be called by timer must be UFunction. You can also call a blueprint function.

See the code below for how to use timer functions.
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

The entire test code is [here](../Source/SilvervineUE4Lua/Private/Scripts/Tests/TimerTest.lua).

------------------------------------------------
[Back to Programming Guide](ProgrammingGuide.md)