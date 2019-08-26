
How to Setup a Dispatch Handler Factory
=======================================

The _Dispatch Handler Factory_ is a lua function that returns the name of the lua file used by the class to be dispatched. 

Implement the dispatch handler factory as follows and connect it to the VM. It is recommended that you write to the first running lua file, such as Game/Main.lua.

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

Note:
* The loaded dispatch handlers are cached inside the VM. The dispatch handler factory is called whenever a class not in the cache is found. 

------------------------------------------------
[Back to Programming Guide](ProgrammingGuide.md)