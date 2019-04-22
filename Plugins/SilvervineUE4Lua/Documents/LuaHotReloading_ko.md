
Lua File Hot Reloading
======================

개발 빌드 한정으로 로컬 파일로더는 연결된 경로의 파일 변경점을 모니터링 합니다.
파일 변경이 감지되면 `SUE4Lua.OnFileModifiled()` 함수가 호출되는데 이 함수를 구현해서 간단한 핫 리로딩 기능을 만들 수 있습니다.

```lua
local AdditionalExecuteFilenames = {
    "Game/Utility.lua",
    "Game/UI.lua",
    "Game/DispatchHandlerFactory.lua",
}

SUE4Lua.OnFileModifiled = function (Filename)
    SUE4Lua.Log("OnFileModifiled:", Filename)

    for _, ExecutedFilename in pairs(AdditionalExecuteFilenames) do
        if Filename:lower() == ExecutedFilename:lower() then
            SUE4Lua.Log("Re-Execute", Filename)
            SUE4Lua.ExecuteFile(Filename)
            break
        end
    end
end
```

참고:
* VM 인스턴스를 생성해야 모니터링을 시작합니다. 즉, 게임을 시작하지 않았다면 변경점을 감지하지 않습니다.
* 디스패치 핸들러는 SUE4Lua가 자동으로 리로딩해줍니다.
```
LogSUE4L: [LuaVM] [SUE4Lua/Framework/SUE4LuaBinding.lua:151] Dispatch Handler was Hot-Reloaded:   Sedan_C game/Sedan.lua
LogSUE4L: [LuaVM] [Game/Main.lua:59] OnFileModified:  game/Sedan.lua
```

----------------------------------------------------
[프로그래밍 가이드](ProgrammingGuide_ko.md)로 돌아가기