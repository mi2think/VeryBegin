--------------------------------------------------------------------
-- Case 06
--
-- Compilation, Execution and Errors in Lua
--------------------------------------------------------------------
local f = load("print(1 + 2)") -- separate compilation
f()

i = 5
local i = 1
local f = load("i = i + 1; return i") -- load compile chunk in global environment
print(i)   --1
print(f()) --6

print("load case05_iterators")
local f = load(io.lines("case05_iterators.lua", "*L"))
f()

print("enter a function to plotted(with var 'x')")
local l = io.read()
local f = load("return " .. l)
for i = 1, 10 do
	x = i
	print(string.rep("*", f()))
end

print("enter a function to plotted(with var 'x')")
local l = io.read()
local f = load("local x = ...; return ".. l)
for i = 1, 10 do
	print(string.rep("*", f(i)))
end