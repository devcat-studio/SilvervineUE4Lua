
LuaValue 사용하기
================

C++이나 Blueprint와 같은 Lua 바깥에 Lua 값을 저장하고 싶을 때 _LuaValue_(`USUE4LuaValue`)를 사용합니다. 

예를 들어,

![](Images/LuaValue.png)

와 같이 Blueprint에 LuaValue를 추가한 후에 Lua 코드에서 아무 Lua값이나 저장할 수 있습니다.
```lua
self.LuaValue = true              -- boolean
self.LuaValue = 123               -- integer
self.LuaValue = 123.456           -- number
self.LuaValue = 'test'            -- string
self.LuaValue = function () end   -- function
self.LuaValue = {}                -- table 
```

특히 함수를 저장해 두면 딜리게이트에 Lua 함수를 바인딩하거나 C++에서 Lua 함수를 바로 호출할 수 있습니다.
```lua
self.LuaValue = function () end
self.Delegate:Bind(self:getObjectProperty("LuaValue"))
```

`getObjectProperty()` 함수는 주어진 이름의 속성을 UObject 타입으로 반환하는 SUE4Lua에 구현된 함수입니다.

이 함수는 Lua에서 LuaValue를 Lua 값이 아닌 UObject로 다루고 싶을 때 사용합니다.

이 예제를 보시면 쉽게 이해하실 수 있습니다.
```lua
self.LuaValue = function () end
self.Delegate:Bind(self.LuaValue)
-- LogSUE4L: Warning: [LuaVM] [...] function: expected UObject type.
-- LogSUE4L: Error: [LuaVM] [...] Delegate.Bind(): Invalid arg1
```

참고:
* 저장된 Lua 값은 LuaValue가 파괴되기 전까지 gc되지 않습니다.
* Blueprint로 LuaValue에 접근하는 기능은 (아직) 구현하지 않았습니다.
* LuaValue 변수 이름을 _LuaContext_ 과 같이 붙이고 해당 객체의 모든 Lua 값을 이 변수에만 저장하시기를 추천합니다. 
이렇게 하면 Lua 상태를 격리시킬 수 있어서 코드 관리에 크게 도움됩니다.
```lua
self.LuaContext = {
    MargineLeft = 0.2,
    MargineTop = 0.2,
    MargineRight = 0.8,
    MargineBottom = 0.8
}
```

----------------------------------------------------
[프로그래밍 가이드](ProgrammingGuide_ko.md)로 돌아가기