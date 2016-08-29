--------------------------------------------------------------------
-- Case 02
--
-- Statements in Lua
--------------------------------------------------------------------
local a, b = 2, 4
print(a)
print(b)

a, b = b, a -- in multi-assignment, lua eval all right statments first
print(a)
print(b)

local c, d = 10
print(c)
print(d) -- d is nil

local x = 5, 10
print(x) -- x is 5, 10 is ignored

--block
m = 5
do
    local m = 10
    print(m) -- local m is 10
end
print(m) -- global m is 5

--control
do
    --if
    local a = 1.5
    if a < 1 then
        a = 1
    elseif a < 2 then
        a = 2
    end
    print(a)

    --while
    local x = 0
    while x < 100 do
        x = x + 1
    end
    print(x)

    --for
    
    --number for
    for var = 1, 10 do
        print(var)
    end
    for var = 10, 1, -1 do
        print(var)
    end

    --generics for
    local array = {1, 3, 5, 7, 9}
    for k,v in ipairs(array) do
        print(k .. " " .. v)
    end

    local map = {"hi", 2, "world"}
    map["entry"] = "main map"
    for k,v in pairs(map) do
        print(k .. " " .. v)
    end
end

--number operator
local x1 = 2.0
print("x1 + 1:" .. x1 + 1)
print("x1 - 1:" .. x1 + 1)
print("x1 ^ 2:" .. x1 ^ 2)
print("x1 ^ 0.5:" .. x1 ^ 0.5)
print("x1 / 5:" .. x1 / 5)
print("x1 * 3:" .. x1 * 3)

print(nil and 5)
print(nil or 3)
print(3 or 4)
print(not nil)

print(1 ~= 1)
print(1 == 1)

local m = {x = 1, y = 2}
local n = {["x"] = 1, ["y"] = 2}
print("x = " .. m.x .. "," .. "y = " .. m.y )
print("x = " .. n.x .. "," .. "y = " .. n.y )