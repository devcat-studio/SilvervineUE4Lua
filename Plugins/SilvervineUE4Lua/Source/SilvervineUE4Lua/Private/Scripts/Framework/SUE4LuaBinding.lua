-- SilvervineUE4Lua / devCAT studio
-- Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.


SUE4LuaBinding = {}

-- c++에서 참조하는 lua 값을 보관하는 참조 테이블
-- (void*, luaObj)
SUE4LuaBinding.valuesReferencedByNative = {}

-- UObject에 대한 프록시 객체를 보관하는 약참조 테이블
-- (UObject*, proxy)
SUE4LuaBinding.uobjectProxies = setmetatable({}, { __mode = 'v' })

-- UObject 프록시에 대한 정보를 보관하는 약참조 테이블
SUE4LuaBinding.uobjectProxyDatas = setmetatable({}, { __mode = 'k' })

-- UObject 프록시에 대한 메타테이블 컬렉션
SUE4LuaBinding.uobjectProxyMTs = {}

-- UProperty 프록시에 대한 메타테이블 컬렉션
SUE4LuaBinding.upropertyProxyMTs = {}

-- UStruct 프록시에 대한 메타테이블
SUE4LuaBinding.ustructProxyMT = {}

-- 등록된 디스패치 핸들러들
SUE4LuaBinding.dispatchHandlers = {}

-- c++에서 lua 값에 대한 참조를 보관할 때 호출합니다.
function SUE4LuaBinding.AddReference(nativeObj, targetValue)
	assert(type(nativeObj) == 'userdata')

	if targetValue ~= nil then
		SUE4LuaBinding.valuesReferencedByNative[nativeObj] = targetValue
	end

	--SUE4Lua.Log("AddReference(nativeObj = '" .. tostring(nativeObj) .. "', targetValue = '" .. tostring(targetValue) .. "''")
end

-- c++에서 lua 값에 대한 참조를 해제할 때 호출합니다.
function SUE4LuaBinding.RemoveReference(nativeObj)
	assert(type(nativeObj) == 'userdata')

	local targetValue = SUE4LuaBinding.valuesReferencedByNative[nativeObj]

	if targetValue ~= nil then
		--SUE4Lua.Log("RemoveReference(nativeObj = '" .. tostring(nativeObj) .. "', targetValue = '" .. tostring(targetValue) .. "''")			
		SUE4LuaBinding.valuesReferencedByNative[nativeObj] = nil
	else
		SUE4Lua.Error("RemoveReference() was called with unreferenced value.")
	end
end

-- c++에서 참조하는 lua 값을 lua 스택에 푸시할 때 호출합니다.
function SUE4LuaBinding.GetReferenced(nativeObj)
	assert(type(nativeObj) == 'userdata')

	return SUE4LuaBinding.valuesReferencedByNative[nativeObj]
end

-- uobject 프록시에 대한 정보를 반환합니다.
-- 없으면 새 테이블을 생성.
function SUE4LuaBinding.GetUObjectProxyData(uobj)
	assert(type(uobj) == 'userdata')

	local data = SUE4LuaBinding.uobjectProxyDatas[uobj]
	if data == nil then
		data =
		{
			-- 생성한 uproperty 프록시 캐시
			upropertyProxies = {},
		}
		SUE4LuaBinding.uobjectProxyDatas[uobj] = data
	end

	return data
end

-- 디스패치 핸들러를 등록
function SUE4LuaBinding.RegisterDispatchHandler(dispatchClass, handlerChunk, filename)
	assert(type(dispatchClass) == 'userdata')
	assert(handlerChunk == nil or type(handlerChunk) == 'function')
	assert(type(filename) == 'string')

	local dispatchClassName = dispatchClass:GetName()
	local handler

	if handlerChunk then
		handler = handlerChunk()
		if type(handler) ~= 'table' then
			SUE4Lua.Error("Dispatch Handler files should return a table:", dispatchClassName, filename)
		end
	end

	-- 핫리로딩할 수 있도록 등록만 해둡니다.
	SUE4LuaBinding.dispatchHandlers[dispatchClassName] = {
		handler = handler,
		filenameLower = filename:lower(),
	}

	if handler then
		SUE4Lua.Log("Dispatch Handler Registered: ", dispatchClassName, filename)
	end
end

-- 등록된 디스패치 핸들러를 반환
function SUE4LuaBinding.FindDispatchHandler(functionName, dispatchObject)
	assert(type(functionName) == 'string')
	assert(type(dispatchObject) == 'userdata')

	local class = dispatchObject:GetClass()
	if class == nil then
		SUE4Lua.Error("GetDispatchHandler(): Invalid dispatch object class.")
		return nil
	end

	local className = class:GetName()
	local handlerData = SUE4LuaBinding.dispatchHandlers[className]
	
	if handlerData == nil then
		SUE4Lua.Error("GetDispatchHandler(): Unregistered class name: ", className)
		return nil
	end

	local func = handlerData.handler and handlerData.handler[functionName] or nil
	if func ~= nil then
		if type(func) == 'function' then
			return func
		else
			SUE4Lua.Error("GetDispatchHandler(): Dispatch Handler must be a function type:", className .. '.' .. functionName)
		end
	end

	return nil
end

-- 디스패치 핸들러를 다시 로딩
function SUE4LuaBinding.ReloadDispatchHandler(filename)
	assert(type(filename) == 'string')

	local filenameLower = filename:lower()

	for className, handlerData in pairs(SUE4LuaBinding.dispatchHandlers) do
		if handlerData.filenameLower == filenameLower then
			local handlerChunk = SUE4Lua.LoadFile(filename)
			if handlerChunk then
				local handler = handlerChunk()
				if type(handler) == 'table' then
					handlerData.handler = handler					
					SUE4Lua.Log("Dispatch Handler was Hot-Reloaded: ", className, filename)
				else
					SUE4Lua.Error("Dispatch Handler files should return a table:", className, filename)
				end
			end
		end
	end
end