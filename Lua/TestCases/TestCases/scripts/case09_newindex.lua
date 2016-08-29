--------------------------------------------------------------------
-- Case 09
--
-- __index and __newindex in Lua
--
-- __index for table access, __newindex for table updates
--------------------------------------------------------------------

-- table using default value
function setDefault(t, defaultVal)
	local mt = { __index = function() return defaultVal end }
	setmetatable(t, mt)
end

local t = { x = 10, y = 20 }
print(t.x, t.z)
setDefault(t, 100) -- every table has a metatable if call setDefault
print(t.x, t.z)

--
local mt = { __index = function(t) return t.___ end }
function setDefault(t, defaultVal)
	t.___ = defaultVal
	setmetatable(t, mt)
end

--
local key = {}
local mt = { __index = function(t) return t[key] end }
function setDefault(t, defaultVal)
	t[key] = defaultVal
	setmetatable(t, mt)
end

local t = { x = 4, y = 5 }
print(t.x, t.z)
setDefault(t, 2) -- every table use a same metatable 
print(t.x, t.z)

--tracing table access
local t = {} -- origin table
local _t = t

t = {} -- create proxy

local mt = {
	__index = function(t, k)
		print("access to element " .. tostring(k))
		return _t[k]
	end,

	__newindex = function(t, k, v)
		print("update of element " .. tostring(k))
		_t[k] = v
	end
}
setmetatable(t, mt)

t.x = 10   -- update of element x
print(t.x) -- access to element x

--tracing table using same metatable
local index = {} -- create unique index
local mt = {
	__index = function(t, k)
		print("access to element " .. tostring(k))
		return t[index][k]
	end,

	__newindex = function(t, k, v)
		print("update of element " .. tostring(k))
		t[index][k] = v
	end	
}
function track(t)
	local proxy = {}
	proxy[index] = t
	setmetatable(proxy, mt)
	return proxy
end

local t = {}
local proxy = track(t)
proxy.x = 10
print(proxy.x)

--readonly table
function readOnly(t)
	local proxy = {}
	local mt = {
		__index = t,
		__newindex = function(t, k, v)
			error("attempt to update a rea-only table", 2)
		end
	}
	setmetatable(proxy, mt)
	return proxy
end

local t = readOnly{1, 2, 3, 4, 5}
print(t[1])
--t[1] = 20