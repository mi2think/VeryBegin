--------------------------------------------------------------------
-- Case 10
--
-- environment in Lua
--
-- setfenv has removed in 5.2, add _ENV
--------------------------------------------------------------------
for n in pairs(_G) do
	print(n)
end

setmetatable(_G, {
	__index = function(_, k)
		error("attempt to read undeclared variable" .. k)
	end,
	__newindex = function (_, k)
		error("attempt to write to undeclared variable" .. k)
	end
})

print("------------------------------")

--N = 1            -- error
rawset(_G, "N", 1) -- skip triger meta method
print(tostring(_G.N))


_ENV = {g = _G}   -- change ENV to table {g = _G}
--print(13)          error, no print
g.print(13) -- 13

local n = { x = 10, y = 20 }
g.print(n.x, n.y)
g.setmetatable(n, {__index = g})
_ENV = n
print(x + 20)   -- print is trigger __index, x is in now table
