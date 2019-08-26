
How to Implement a C++ Function with Lua
========================================

The C++ function can also be [Dispatched](WorkWithBlueprintDispatch.md). This feature is called _Native Dispatch_

Getting Started
---------------

Like blueprint dispatch, native dispatch needs the lua bridge. Initialize the lua bridge as follows:

```cpp
#include "SilvervineUE4LuaBridge.h"

UCLASS()
class UMyClass : public UObject
{
	GENERATED_BODY()

    // ...

protected:
	UPROPERTY()
	class USUE4LuaBridge* LuaBridge;
```
```cpp
#include "SilvervineUE4LuaVirtualMachine.h"

UMyClass::UMyClass(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<USUE4LuaVirtualMachine> RefLuaVM(TEXT("/Game/BP/LuaVMs/DhLuaVM_Default"));
	LuaBridge = CreateDefaultSubobject<USUE4LuaBridge>(TEXT("LuaBridge"));
	LuaBridge->NativeInitialize(RefLuaVM.Object);
}
```

Setting a native dispatch handler is the same as the blueprint dispatch. 

Note:
* Declaring a lua bridge in C++ and initializing it in a derived blueprint eliminates hardcoded asset path strings.

How to Dispatch
---------------

Implement the function to dispatch as follows. It does not have to be a UFunction.

```cpp
void UMyClass::NewFunction()
{
	SUE4LUA_DISPATCH(LuaBridge);
}
```

The implementation is similar to blueprint dispatch, but the prefix 'Native' is required.
```lua
function MyClass:NativeNewFunction(Params)
    SUE4Lua.Log("Hello, World")
end
```

Note:
* We force 'Native' prefix to distinguish it from the blueprint function. 
* Do not attach 'Native' to the functions whose names begin with _Native_.

    C++ `OnClick` -> Lua `NativeOnClick`
    
    C++ `NativeOnClick` -> Lua `NativeOnClick`

Passing parameters
------------------

Unlike blueprint dispatch, you have to write the parameter passing codes at hand for yourself.
```cpp
void UMyClass::NewFunction(int32 P1, int32 P2, int32 P3)
{
	SUE4LUA_DISPATCH(LuaBridge, P1, P2, P3);
}
```
```lua
function MyClass:NativeNewFunction(Params)
    -- Input
    local P1 = Params[1]
    local P2 = Params[2]
    local P3 = Params[3]
end
```

Because it's not a UFunction, lua doesn't know the name of the parameter. So, we pass the parameters as an array.

The way of the return value passing is similar to blueprint dispatch.
```cpp
void UMyClass::NewFunction(int32 P1, int32 P2, int32 P3, int32& O1, FVector& O2)
{
	SUE4LUA_DISPATCH(LuaBridge, P1, P2, P3, O1, O2);
}
```
```lua
function MyClass:NativeNewFunction(Params)
    -- Input
    local P1 = Params[1]
    local P2 = Params[2]
    local P3 = Params[3]

    -- Output
    Params[4] = ...
    Params[5] = ...
end
```

Note:
* Non const reference type parameters are treated as output.
* You cannot pass containers such as TArray as parameters. If necessary, wrap it in UStruct.

------------------------------------------------
[Back to Programming Guide](ProgrammingGuide.md)