
Lua File Hot Reloading
======================

For development builds only, the local file loader monitors file changes in the associated path.
When a file change is detected, the `SUE4Lua.OnFileModifiled()` function is called. So, you can easily implement a simple hot reloading in it.

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

Note:
* You must create a VM instance to start monitoring. That is, if you haven't started the game, changes will not be detected.
* Dispatch handlers are automatically reloaded by SUE4Lua.
```
LogSUE4L: [LuaVM] [SUE4Lua/Framework/SUE4LuaBinding.lua:151] Dispatch Handler was Hot-Reloaded:   Sedan_C game/Sedan.lua
LogSUE4L: [LuaVM] [Game/Main.lua:59] OnFileModified:  game/Sedan.lua
```

------------------------------------------------
[Back to Programming Guide](ProgrammingGuide.md)