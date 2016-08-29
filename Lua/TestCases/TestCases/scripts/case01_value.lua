--------------------------------------------------------------------
-- Case 01
--
-- Value & Type in Lua
--
-- Lua is a dynamically typed language. This means that variables do not have types; only values do. 
-- There are no type definitions in the language. All values carry their own type.
-- 
-- All values in Lua are first-class values. 
-- This means that all values can be stored in variables, passed as arguments to other functions, and returned as results.
--------------------------------------------------------------------

local iVal = 10
local fVal = 10.2
local strVal = "Hello, World!"
local funVal = print
local bVal = true
local nilVal = nil
local tbVal = {}

print(">>> value type")

if type(iVal) == "number" then
    print(tostring(iVal) .. " is a number")
end

if type(fVal) == "number" then
    print(tostring(fVal) .. " is a number")
end

if type(strVal) == "string" then
    print(strVal .. " is a string")
end

if type(funVal) == "function" then
    print(tostring(funVal) .. " is a function")
end

if type(bVal) == "boolean" then
    print(tostring(bVal) .. " is a boolean")
end

if type(nilVal) == "nil" then
    print(tostring(nilVal) .. " is a nil")
end

if type(tbVal) == "table" then
    print(tostring(tbVal) .. " is a table")
end

--multi-lines string
print(">>> multi-lines string")

local page = [[
     <html xmlns="http://www.w3.org/1999/xhtml">
          <head>
               <title>xxxx</title>
          </head>
          <body>
          </body>
     </html>
]]
print(page)

-- table
-- The type table implements associative arrays, that is, arrays that can be indexed not only with numbers, but with any Lua value except nil and NaN.
print(">>> table")

local a = {}  -- create a table, and store it's refenerce to a
a["x"] = 10

local b = a -- b have same refenerce with a
print(b["x"])
b["x"] = 20
print(a["x"])

b = nil -- only a refenerce to table
print(a["x"])
a = nil -- no refenerce

a = {1, 5, 10, 15, 20, 25}
a.x = 2 -- same with a["x"]
print(a["x"])
print(a[a.x]) --print a[2], it should be 5
print(#a)

a = {}
a[100] = 1
print(#a) --lua use nil as end of array. since a[1] = nil, so length of a is 0