--------------------------------------------------------------------
-- Case 08
--
-- metatable and metafunction in Lua
--------------------------------------------------------------------
local a = {1, 2}
print(getmetatable(a)) -- getmetatable

local b = {}
setmetatable(a, b) -- setmetatable
print(getmetatable(a))

-- can just set metatable of table in lua
-- for other types, except string, have no metatable in default 
print(getmetatable("hello world"))
print(getmetatable("ok, got it"))
print(getmetatable(10))

--function of metatable call metafunction
--we can define metafunction in table
-- __add(a, b)    a + b
-- __sub(a, b)    a - b
-- __mul(a, b)    a * b
-- __div(a, b)    a / b
-- __mod(a, b)    b % b
-- __pow(a, b)    a ^ b
-- __unm(a)      -a
-- __concat(a,b)  a .. b
-- __len(a)       length of a
-- __eq(a, b)     a == b
-- __lt(a, b)     a < b
-- __le(a, b)     a <=b
-- __index(a, b)  index of a in b
-- __newindex(a, b, c)
-- __call(a, ...) call function a
-- __tostring(a)  to string
-- __metatable
 
 Set = {}
 local mt = {}

 function Set.new(l)
    local set = {}
    setmetatable(set, mt)
    for _, v in pairs(l) do
        set[v] = true
    end
    --no one can set metatable angin after we set it
    mt.__metatable = "can not modify metatable"
    return set
 end

 function Set.union(a, b)
    local retSet = Set.new{} -- Set.new({})
    for v in pairs(a) do
        retSet[v] = true
    end
    for v in pairs(b) do
        retSet[v] = true
    end
    return retSet
 end

 function Set.intersection(a, b)
    local retSet = Set.new{}
    for v in pairs(a) do
        retSet[v] = b[v]
    end
    return retSet
 end

 function Set.toString(a)
    local t = {}
    for v in pairs(a) do
        t[#t + 1] = v
    end
    return "{" .. table.concat(t, ", ") .. "}"
 end

 function Set.print(a)
    print(Set.toString(a))
 end

 local s1 = Set.new({1, 2, 3})
 local s2 = Set.new({5, 6})
 print(getmetatable(s1))
 print(getmetatable(s2))

 mt.__add = Set.union

 -- 1. if s1 have metatable, and has __add, then use it
 -- 2. else if s2 have metatable, and has __add, then use it
 -- 3. error
 local s3 = s1 + s2
 Set.print(s3)

 mt.__tostring = Set.toString
 print(s3)

 -- metatable has been protected using __metatable in Set.new
 print(getmetatable(s3))
 --setmetatable(s3, {})

 -- __index metafunction, maybe a table

 -- 1. when access filed in table, return it if table have
 -- 2. else Lua will look up __index, then call __index, return val
 -- 3. else return nil
Windows = {}
Windows.default = {x = 0, y = 0, width = 100, height = 100}
Windows.mt = {}

function Windows.new(o)
    setmetatable(o, Windows.mt)
    return o
end

Windows.mt.__index = function(table, key)
    return Windows.default[key]
end

local win = Windows.new({x = 10, y = 10})
print(win.x)     -- own valuue of win
print(win.width) -- look for __index, then call it found default value

Windows.mt.__index = Windows.default

print(win.x)
print(win.width)

-- __newindex metafunction

-- 1. Lua first check whether have element in table
-- 2. look for __newindex if have metatable
-- 3. if can not find it, Lua add index then assign
-- 4. else find __newindex, if it's a function ,then call it
-- 5. else __index is a table, assgin it in this table

-- error code
local t1 = {}
local t2 = {}

t1.__newindex = t2
t2.__newindex = t1
setmetatable(t1, t2)
setmetatable(t2, t1)

--t1.x = 10 
--rawset(t1, x, 111)
--print(t1.x)
