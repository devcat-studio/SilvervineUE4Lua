
# SUE4Lua Library

SUE4Lua에서 제공하는 필수 라이브러리입니다.

## SUE4Lua.ExecuteString(LuaString)

입력 텍스트의 소스를 불러와 실행합니다. 실행 성공 여부를 반환합니다.

```lua
local bSuccess = SUE4Lua.ExecuteString("local Value = 123")
```

## SUE4Lua.ExecuteFile(LuaFilename)

입력 파일 이름의 소스를 불러와 실행합니다. 실행 성공 여부를 반환합니다.

```lua
local bSuccess = SUE4Lua.ExecuteFile("FilePath/FileName")
```

## SUE4Lua.LoadString(LuaString)

입력 텍스트의 소스를 불러옵니다. 반환값은 함수입니다.

```lua
local Function = SUE4Lua.LoadString("local Value = 123")
```

## SUE4Lua.LoadFile(LuaFilename)

입력 텍스트의 소스를 불러옵니다. 반환값은 함수입니다.

```lua
local Function = SUE4Lua.LoadFile("FilePath/FileName")
```

## SUE4Lua.GetEnumTable(EnumName)

입력 이름의 Enum을 Table로 반환합니다.

```lua
local ESlateVisibility = SUE4Lua.GetEnumTable("ESlateVisibility")

local Enum1 = ESlateVisibility.Visible -- 0
local Enum2 = ESlateVisibility.Collapsed -- 1
local Enum3 = ESlateVisibility.Hidden -- 2
local Enum4 = ESlateVisibility.HitTestInvisible -- 3
local Enum5 = ESlateVisibility.SelfHitTestInvisible -- 4
```

## SUE4Lua.SetDispatchHandlerFactory(Function)

UE4의 클래스에 대응하는 루아 파일을 디스패치하는 핸들러를 등록합니다.

```lua
SUE4Lua.SetDispatchHandlerFactory(function (CallerClass)
    local ClassName = CallerClass:GetName()
	local Filename = ("Game/SomePath/%s.lua"):format(ClassName)
    return Filename
end)
```

## SUE4Lua.IsFixedArray(Userdata)

입력 Userdata가 고정 크기 배열(예 : int32[]) 인지 확인합니다.

## SUE4Lua.IsArray(Userdata)

입력 Userdata가 UE4의 TArray인지 확인합니다.

## SUE4Lua.IsMap(Userdata)

입력 Userdata가 UE4의 TMap인지 확인합니다.

## SUE4Lua.IsSet(Userdata)

입력 Userdata가 UE4의 TSet인지 확인합니다.

## SUE4Lua.IsDebuggable()

디버깅이 가능한 환경인 지 확인합니다.

------------------------------------
[라이브러리](Library_ko.md)로 돌아가기