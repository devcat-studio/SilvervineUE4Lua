-- SilvervineUE4Lua / devCAT studio
-- Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.


--=============================================================================================================================
-- strictness
-- https://github.com/lua-stdlib/strict
--=============================================================================================================================
do
	local declared = { loadstring = true, setfenv = true }

	local mt = {}
	setmetatable(_G, mt)

	--- What kind of variable declaration is this?
	-- @treturn string 'C', 'Lua' or 'main'
	local function what()
	   local d = debug.getinfo(3, 'S')
	   return d and d.what or 'C'
	end

	mt.__newindex = function (t, k, v)
		if not declared[k] then
			local w = what()
			if w ~= 'main' and w ~= 'C' then
				error("assignment to undeclared variable '" .. k .. "'", 2)
			end

			declared[k] = true
		end

		rawset(t, k, v)
	end

	mt.__index = function (t, k)
		if not declared[k] and what() ~= 'C' then
			error("variable '" .. k .. "' is not declared", 2)
		end

		return rawget(t, k)
	end
end

--=============================================================================================================================
-- Class
-- 간단한 클래스 구현
--=============================================================================================================================
do
	-- 모든 클래스의 기본 클래스
	local Object = {}
	Object.isClass = true

	function Class(super)
		local c = setmetatable({}, { __index = super or Object })
		local mt = { __index = c }

		c.metaTable = mt
		c.new = function (...)
			local obj = setmetatable({}, mt)
			if obj.init ~= nil then obj:init(...) end
			return obj
		end

		return c
	end
end