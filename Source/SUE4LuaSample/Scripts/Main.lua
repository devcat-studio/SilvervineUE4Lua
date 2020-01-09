-- SilvervineUE4Lua / devCAT studio
-- Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.


SUE4Lua.Log("Main.lua was called.")

SUE4Lua.SetDispatchHandlerFactory(function (CallerClass)
    local ClassName = CallerClass:GetName()

    SUE4Lua.Log("DispatchHandlerFactory: ", ClassName)

    local Filenames = {
        Sedan_C         = "Game/Sedan.lua",
        VehicleHUD_C    = "Game/VehicleHUD.lua",
    }

    return Filenames[ClassName] or ""
end)

if SUE4Lua.IsDebuggable() then
    SUE4Lua.ExecuteFile("SUE4Lua/Debug/dkjson.lua")

    local debuggee = SUE4Lua.LoadFile("SUE4Lua/Debug/vscode-debuggee.lua")()

    local addUE4Var = function(userdata, addVarFunction)
        if SUE4Lua.IsFixedArray(userdata) or SUE4Lua.IsArray(userdata) or SUE4Lua.IsMap(userdata) or SUE4Lua.IsSet(userdata) then
            for key, value in pairs(userdata) do
                addVarFunction(key, value)
            end
        else
            local curType = userdata:getType()
            while curType do
                addVarFunction('['..curType:GetName()..']', "", true)
                for _, name in pairs(curType:GetPropertyNames()) do
                    addVarFunction(name, userdata[name])
                end
                curType = curType:GetSuperStruct()
            end
        end
    end

    if debuggee.start(json, { addUserdataVar = addUE4Var }) then
        SUE4Lua.OnDebugPrint = function (Text)
            local category = 'log'
            if Text:find("%[Log%]") then
                category = 'log' -- white
            elseif Text:find("%[Warning%]") then
                category = 'warning'-- yellow
            elseif Text:find("%[Error%]") then
                category = 'error'-- red
            end
            debuggee.print(category, Text)
        end
        SUE4Lua.OnTick = function (DeltaTime) debuggee.poll() end
    end
end

SUE4Lua.OnFileModifiled = function (Filename)
    SUE4Lua.Log("OnFileModified:", Filename)
end

SUE4Lua.ExecuteFile("SUE4Lua/Tests/AllTests.lua")