
How to Print a Debug Text
=========================

Printing a Debug Text in Lua
----------------------------

Use the function `SUE4Lua.Log()` to print log messages in lua. The parameter format is the same as `print()`.
```lua
SUE4Lua.Log("This is a Log message:", 123)
```

Use the function `SUE4Lua.Warning()` to print warning messages.
```lua
SUE4Lua.Warning("This is a Warning message:", 123)
```

Use the function `SUE4Lua.Error()` to print error messages.
```lua
SUE4Lua.Error("This is an Error message:", 123)
```

Note:
* print() is internally the same as SUE4Lua.Log(). However, we recommend that you use SUE4Lua.Log() instead of print() for consistency.

Printing a Debug Text in C++
----------------------------

SUE4Lua do not provide the functionality to print lua debug texts in the game module.

Use the following macros to modify the plugin codes. The parameter format is similar to UE_LOG().
```cpp
SUE4LVM_LOG(L, TEXT("This is a Log message: %d"), 123);
SUE4LVM_WARNING(L, TEXT("This is a Warning message: %d"), 123);
SUE4LVM_ERROR(L, TEXT("This is an Error message: %d"), 123);
```

------------------------------------------------
[Back to Programming Guide](ProgrammingGuide.md)
