
디스패치 핸들러 팩토리 설정하기
============================

디스패치할 클래스가 사용하는 Lua 파일 이름을 반환해주는 Lua 함수를 _디스패치 핸들러 팩토리_ 라고 부릅니다. 

디스패치 핸들러 팩토리는 다음과 같이 구현한 후에 VM에 연결해주세요. Game/Main.lua와 같은 가장 먼저 실행되는 Lua파일에 작성하시는 것이 좋습니다.
```lua
SUE4Lua.SetDispatchHandlerFactory(function (CallerClass)
    local ClassName = CallerClass:GetName()

    SUE4Lua.Log("DispatchHandlerFactory: ", ClassName)

    local Filenames = {
        Sedan_C         = "Game/Sedan.lua",
        VehicleHUD_C    = "Game/VehicleHUD.lua",
    }

    return Filenames[ClassName] or ""
end)
```

참고:
* 로딩한 디스패치 핸들러는 VM 내부에서 캐싱됩니다. 디스패치 핸들러 팩토리는 캐시에 없는 클래스를 발견했을 때에 호출됩니다. 

----------------------------------------------------
[프로그래밍 가이드](ProgrammingGuide_ko.md)로 돌아가기