-- SilvervineUE4Lua / devCAT studio
-- Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.


-- SUE4Lua
assert(type(SUE4Lua) == 'table')

-- SUE4Lua.VM에는 FSUE4LuaVirtualMachine*가 lightuserdata 타입으로 들어있습니다.
-- c++에서 lua_State로부터 FSUE4LuaVirtualMachine을 얻을 때 사용합니다.
assert(type(SUE4Lua.VM) == 'userdata')

-- xpcall() 등에서 사용할 수 있는 에러 핸들러
assert(type(SUE4Lua.ErrorHandler) == 'function')

-- WorldContextObject 입니다. 주로 UE4.NewObject()할 때 Outer로 사용합니다.
assert(SUE4Lua.WorldContextObject == nil or type(SUE4Lua.WorldContextObject) == 'userdata')

-- 로그 함수
assert(type(SUE4Lua.Log) == 'function')
assert(type(SUE4Lua.Warning) == 'function')
assert(type(SUE4Lua.Error) == 'function')

-- 빌드 정보
do
	assert(type(SUE4Lua.Build) == 'table')

	assert(type(SUE4Lua.Build.WITH_EDITOR) == 'boolean')
end

-- SUE4Lua.Log() 등으로 디버그 텍스트를 출력했을 때 호출됩니다.
function SUE4Lua.OnDebugPrint(text)
end

-- 엔진 틱 이벤트시 호출됩니다.
function SUE4Lua.OnTick(DeltaTime)
end

-- 개발빌드 전용: 파일이 변경되었을 때 호출됩니다. 
function SUE4Lua.OnFileModifiled(Filename)
end

