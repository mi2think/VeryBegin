--------------------------------------------------------------------
-- Case 12
--
-- OO in Lua
-------------------------------------------------------------------- 
local t1 = {a = 1, b = 2}
local t2 = {a = 1, b = 2}
if t1 == t2 then
    print("t1 == t2")
else
    print("t1 ~= t2") -- yes
end

--oo 0
Account = { balance = 0 }
function Account.withDraw(v)  -- this function work just for name Account
    Account.balance = Account.balance - v
end

Account.withDraw(10)
print(Account.balance)

--oo 1
Account = { balance = 0 }
function Account.withDraw(self, v)
    self.balance = self.balance - v
end

a = Account
Account = nil
a.withDraw(a, 100)
print(a.balance)

--oo 2
Account = { balance = 0 }
function Account:withDraw(v) -- : can omit self in param
    self.balance = self.balance - v
end

a = Account
Account = nil
a:withDraw(1000) -- alos need:
print(a.balance)

-- prototype
local Account = { value = 0}
function Account:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
  --  self.__newindex = self
    return o
end

function Account:display()         -- just as Account = { value = 0, new = function() ... end, display = function() ... end }
    self.value = self.value + 100
    print(self.value)
end

local a = Account:new()
-- 1. first Lua look for 'display' field in table a
-- 2. not find it, so it look for meta table
-- 3. find metatable, then use __index to find 'display' method
a:display()
a:display()
print(a.value)
print(Account.value) --if enable 'self.__newindex = self', then a.value is same value with Account.value

--single inherit
local CA = { value = 0 }
function CA:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function CA:display()
    print(self.value)
end

function CA:addValue(v)
    self.value = self.value + v
end


--CLittleA is object of CA, also is a prototype
--CLittleA's metatable is CA
local CLittleA = CA:new() 
--s's metatable is CLittleA
local s = CLittleA:new{ value = 10 }
s:display()

--mulit inherit
local CA = {}
function CA:new(o)
    o = o or {}
    setmetatable(o,self)
    self.__index = self
    return o
end

function CA:setName(name)
    self.name = name
end

local CB = {}
function CB:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function CB:getName()
    return self.name
end

function createClass(...)
    local c = {}
    local args = {...}

    setmetatable(c, {
        __index = function(t, k)
	        for i = 1, #args do
		        local p = args[i]
                local v = p[k]
                if v then
                    return v
                end
	        end
        end
    })

    c.__index = c

    function c:new(o)
        o = o or {}
        -- self.__index = self  ; no need set here
        setmetatable(o, self)
        return o
    end

    return c
end

local CC = createClass(CA, CB) -- new class CC, inherits from CA and CB
local oc = CC:new({name = "Hi"})
print(oc:getName())
oc:setName("World")
print(oc:getName())