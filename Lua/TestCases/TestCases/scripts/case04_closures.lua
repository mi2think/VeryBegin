--------------------------------------------------------------------
-- Case 04
--
-- closures in Lua
--
-- a closure is a function plus all it needs to access non-local variables
--------------------------------------------------------------------
function newCounter()
	local i = 0
	return function()
		i = i + 1  -- i is non-local variable(also call upvalues)
		return i
	end
end

local c1 = newCounter()
print(c1()) -- 1
print(c1()) -- 2

local c2 = newCounter()
print(c2()) -- 1
print(c1()) -- 3
print(c2()) -- 2

function yal()
	local i = 5
	
	function inyal1()
		print(i)
	end

	function inyal2()
		i = i + 5
	end

	return inyal1, inyal2
end

-- in nested function, upvalue is shared
local a, b = yal()
a() -- 5
b()
a() -- 10
