
Working with LuaValue
=====================

Use _LuaValue_(`USUE4LuaValue`) when you want to store lua values outside of lua like C++ or blueprint. 

For example,

![](Images/LuaValue.png)

After you add LuaValue to the blueprint, you can set any lua value in it.
```lua
self.LuaValue = true              -- boolean
self.LuaValue = 123               -- integer
self.LuaValue = 123.456           -- number
self.LuaValue = 'test'            -- string
self.LuaValue = function () end   -- function
self.LuaValue = {}                -- table 
```

In particular, if you set a function, you can bind the lua function to a delegate or call it directly in C++.
```lua
self.LuaValue = function () end
self.Delegate:Bind(self:getObjectProperty("LuaValue"))
```

The function `getObjectProperty()` is the SUE4Lua function that returns the property of a given name as UObject type.

This example helps you understand it.
```lua
self.LuaValue = function () end
self.Delegate:Bind(self.LuaValue)
-- LogSUE4L: Warning: [LuaVM] [...] function: expected UObject type.
-- LogSUE4L: Error: [LuaVM] [...] Delegate.Bind(): Invalid arg1
```

Note:
* Saved lua values are not garbage collected until the LuaValue is destroyed.
* We haven't implemented (yet) the functionality to access LuaValue with a blueprint.
* It is recommended that you name the LuaValue variable as _LuaContext_ and store all lua values only in it. This allows you to isolate lua state from others, which is very helpful for code management. 
```lua
self.LuaContext = {
    MargineLeft = 0.2,
    MargineTop = 0.2,
    MargineRight = 0.8,
    MargineBottom = 0.8
}
```

------------------------------------------------
[Back to Programming Guide](ProgrammingGuide.md)
