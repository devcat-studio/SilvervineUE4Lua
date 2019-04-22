
디버그 텍스트 출력하기
====================

Lua에서 디버그 텍스트 출력하기
----------------------------

Lua에서 일반 로그 메시지를 출력하려면 `SUE4Lua.Log()` 함수를 사용합니다. 인자는 `print()`와 동일합니다.
```lua
SUE4Lua.Log("This is a Log message:", 123)
```

경고 메시지는 `SUE4Lua.Warning()` 함수로 출력할 수 있습니다.
```lua
SUE4Lua.Warning("This is a Warning message:", 123)
```

에러 메시지는 `SUE4Lua.Error()` 함수로 출력할 수 있습니다.
```lua
SUE4Lua.Error("This is an Error message:", 123)
```

참고:
* print()는 내부적으로 SUE4Lua.Log()와 동일하게 처리됩니다. 하지만 일관성 있게 print() 대신 SUE4Lua.Log()를 사용하시는 것이 좋습니다.

C++에서 Lua 디버그 텍스트 출력하기
----------------------------

게임 모듈에서 Lua 디버그 텍스트를 출력하는 기능은 제공하지 않습니다.

플러그인 코드를 직접 수정하실 때에는 다음의 매크로를 사용하세요. 형식은 UE_LOG()와 비슷합니다.
```cpp
SUE4LVM_LOG(L, TEXT("This is a Log message: %d"), 123);
SUE4LVM_WARNING(L, TEXT("This is a Warning message: %d"), 123);
SUE4LVM_ERROR(L, TEXT("This is an Error message: %d"), 123);
```

----------------------------------------------------
[프로그래밍 가이드](ProgrammingGuide_ko.md)로 돌아가기
