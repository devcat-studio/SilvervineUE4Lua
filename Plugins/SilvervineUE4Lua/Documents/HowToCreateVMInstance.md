
How to Create VM Assets in C++
==============================

This guide describes how to create a VM in C++.

1. Include "LuaVirtualMachine.h".

If you get a compile error, check [this](HowToSetupGameModule.md) out.

2. Write the following code.

```cpp
auto VM = FSUE4LuaVirtualMachine::Create();
if (VM.IsValid())
{
    // ok
}
```

The return type is `TSharedPtr<FSUE4LuaVirtualMachine>`.

To create a VM with debugging enabled, pass `true` as an argument.

```cpp
auto VM = FSUE4LuaVirtualMachine::Create(true);
if (VM.IsValid())
{
    // ok
}
```

Note:
* The VM instance created by uasset has debugging enabled.

------------------------------------------------
[Back to Programming Guide](ProgrammingGuide.md)
