-- SilvervineUE4Lua / devCAT studio
-- Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.


SUE4Lua.Log("# TimerTest")

local Outer = SUE4Lua.WorldContextObject
assert(Outer ~= nil)

local Obj = UE4.NewObject(UE4.FindClass("SUE4LuaTestUObject"), Outer)
assert(Obj ~= nil)

local UKismetSystemLibrary = UE4.FindClass("KismetSystemLibrary")
local TimerFuncName = "TestTimer"

UKismetSystemLibrary.SetTimer(Obj, TimerFuncName, 1, false)

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




