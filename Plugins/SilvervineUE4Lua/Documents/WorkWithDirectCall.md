
How to Call Functions of a Dispatch Handler
===========================================

When you are writing the dispatch handler code, you may want to call the lua function of another dispatch handler.
SUE4Lua allows you to call functions of the dispatch handler like a normal lua function.

We call this feature a _Direct Call_. 

```lua
function ClassA:FuncA(...)
    -- DirectCall
    self.ObjB:FuncB(1, 2, 3)
end
```
```lua
function ClassB:FuncB(P1, P2, P3)
    SUE4Lua.Log(P1, P2, P3) -- 1    2   3
end
```

Note: 
* Direct Call is easy to use, but it can also make lua code less understandable because it is difficult to distinguish it from the dispatch handler function.
* Please let us know if you have any ideas that can improve.

------------------------------------------------
[Back to Programming Guide](ProgrammingGuide.md)
