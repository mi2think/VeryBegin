--------------------------------------------------------------------
-- Case 11
--
-- modules and requires in Lua
--
-- function module is deprecated in 5.2
--------------------------------------------------------------------

-- when require"mod", Lua will first search path to find mod
-- if find it, call loadfile

-- print search path
print("module path:")
print(package.path)

-- else Lua can not find, Lua then search c path
-- if find it, call loadlib
print("c path:")
print(package.cpath)

--module complex
local complex = require("complex")
local c1 = complex.new(1, 2)
print(complex.tostring(c1))

local c2 = complex.new(2, -5)
local c3 = c1 + c2
print(c3)

print("----------------")

--package.loaded contain all loaded modules
for n in pairs(package.loaded) do
	print(n)
end