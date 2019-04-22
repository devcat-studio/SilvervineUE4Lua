
C++ 함수를 Lua로 구현하기 
========================

C++ 함수도 [디스패치](WorkWithBlueprintDispatch_ko.md)할 수 있습니다. 이 기능을 _네이티브 디스패치_ 라고 부릅니다.

준비
----

Blueprint 디스패치와 마찬가지로 Lua 브릿지가 필요합니다. 다음과 같이 Lua 브릿지를 초기화 해주세요. 초기화만 할 수 있다만 어떠한 방식이든 상관없습니다.

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

디스패치 핸들러의 준비는 Blueprint 디스패치와 같습니다. 

참고:
* C++에서 Lua 브릿지 속성을 선언만 해 두고 파생 Blueprint에서 초기화하면 지저분한 하드코딩을 제거할 수 있습니다.

디스패치하기
-----------

다음과 같이 디스패치할 함수를 구현합니다. Blueprint와 달리 함수가 UFunction이 아니어도 됩니다.
```cpp
void UMyClass::NewFunction()
{
	SUE4LUA_DISPATCH(LuaBridge);
}
```

디스패치 핸들러 구현은 Blueprint 디스패치와 비슷합니다만 함수 이름 앞에 Native을 붙여주세요.
```lua
function MyClass:NativeNewFunction(Params)
    SUE4Lua.Log("Hello, World")
end
```

참고:
* Blueprint 함수와 구별하기 위해서 Native를 붙이도록 규칙을 정했습니다(다른 이유는 없습니다).
* 이름이 _Native_로 시작하는 C++ 함수의 디스패치 Lua 함수 이름에 Native를 덧붙이지 않습니다. 

    C++ `OnClick` -> Lua `NativeOnClick`
    
    C++ `NativeOnClick` -> Lua `NativeOnClick`

파라메터 전달
------------

Blueprint 디스패치와 다르게 네이티브 디스패치는 수동으로 파라메터를 전달해야 합니다.
디스패치할 C++ 함수의 인자를 그대로 전달해주세요.
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

UFunction이 아니기 때문에 Lua에서 파라메터의 이름을 알 수가 없습니다. 그래서 파라메터를 배열 형태로 Lua에 전달합니다.

출력값을 반환하는 방법은 Blueprint 디스패치와 비슷합니다.
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

참고:
* const가 아닌 참조 형식인 파라메터를 출력값으로 처리합니다. 
* 배열이나 TArray 등의 컨테이너 타입을 파라메터로 전달할 수 없습니다. 필요하다면 UStruct로 감싸서 전달해 주세요.

----------------------------------------------------
[프로그래밍 가이드](ProgrammingGuide_ko.md)로 돌아가기