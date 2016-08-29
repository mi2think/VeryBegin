--------------------------------------------------------------------
-- Case 05
--
-- iterators in Lua
--
-- an iterator is any construction the allows you to iterate over elements
-- of a collection
--------------------------------------------------------------------
function values(t)
	local i = 0
	return function()
		i = i + 1
		return t[i]
	end
end

local N = {10, 20, 30}
for v in values(N) do
	print(v)
end

-- the drawback of values is create clouser every loop


--[[
generic for:
for <var-list> in <exp-list> do
    <body>
end

semantics:
for var1, ..., varn in <exp-list> do
    <body>
end

equals

do
	-- _f: iterator function
	-- _s: invariant state
	-- _var: initial value for control variable
	local _f, _s, _var = <exp-list>
	while true do
		local var1, ..., varn = _f(_s, _var)
		_var = var1
		if _var == nil then break end
		<body>
		end
	end
end
--]]

-- stateless iterators
function iter(a, i)
	i = i + 1
	local v = a[i]
	if v then
		return i, v
	end
end

function myipairs(a)
	return iter, a, 0
end

for v in myipairs(N) do
	print(v)
end

-- exercises 7.1
--
-- for i in fromto(n, m) do <body> end
-- equals
-- for i =n, m do <body> end

function fromto_iter(m, i)
	i = i + 1
	if i <= m then
		return i
	end
end

function fromto(n, m)
	return fromto_iter, m, n - 1
end

local sum = 0
for i in fromto(1, 10) do
	sum = sum + i
end
print(sum)