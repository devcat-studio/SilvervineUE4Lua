-- SilvervineUE4Lua / devCAT studio
-- Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

--=============================================================================================================================
-- CaseInsensitiveTable
-- 스트링 키의 대소문자를 구별하지 않는 테이블
-- 스트링 키는 모두 소문자로 저장됩니다.
--=============================================================================================================================
CaseInsensitiveTable = {}

local mt = {}
function CaseInsensitiveTable.new()
	return setmetatable({}, mt)
end

mt.__index = function (t, k)
	if type(k) == 'string' then
		k = k:lower()
	end
	return rawget(t, k)
end

mt.__newindex  = function (t, k, v)
	if type(k) == 'string' then
		k = k:lower()
	end
	rawset(t, k, v)
end
