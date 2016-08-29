--------------------------------------------------------------------
-- Case 03
--
-- function in Lua
--------------------------------------------------------------------
function add(a, b)
    return a + b
end

function minus(a, b)
    return a - b
end

function multiply(a, b)
    return a * b
end

function divide(a, b)
    return a / b
end

function numOp(a, op, b)
    local func = nil
    if op == "+" then
        func = add
    elseif op == "-" then
        func = minus
    elseif op == "*" then
        func = multiply
    elseif op == "/" then
        func = divide
    end
    if func ~= nil then
        return func(a, b)
    end
    return nil
end

--number operation function
print(numOp(1, "+", 2))
print(numOp(1, "-", 2))
print(numOp(1, "*", 2))
print(numOp(1, "/", 2))

--multi return function
function fourNumOp(a, b)
    return add(a, b), minus(a, b), multiply(a, b), divide(a, b)
end

local n1, n2, n3, n4 = fourNumOp(2, 3)
print(n1.." "..n2.." "..n3.." "..n4)

local n5 = fourNumOp(2, 3)
print(n5)

local t = { fourNumOp(2, 3) }
for _, v in ipairs(t) do print(v) end

local t = {fourNumOp(2, 3), 1}
for _, v in ipairs(t) do print(v) end

--put function call in (), get one result
local n6 = ((fourNumOp(2, 3)))
print(n6) -- 5

--var agrs
print("var agrs")

function VarArguments(...)
	for _, v in ipairs{...} do  --{...} means an array of ...
		print(v)
	end
end
VarArguments(fourNumOp(2, 3))


--may be ... contain nil
VarArguments(1, 2, nil, 4, 5)

function VarArguments2(...)
	for i = 1, select('#', ...) do
		local arg = select(i, ...)
		print(arg)
	end
end
VarArguments2(1, 2, nil, 4, 5)

--
myadd = function(a, b) return a + b end
print(myadd(1, 2))

--nested function
function Counter()
	local i = 0
	return function() --anonymous function
		i = i + 1
		return i
	end
end
local c1 = Counter()
print(c1()) -- 1
print(c1()) -- 2

function Counter(i)
	return function()
		i = i + 1
		return i
	end
end
local c2 = Counter(10)
print(c2()) -- 11
print(c2()) -- 12

--local function
lib = {}
lib.add = add
lib.minus = minus
print(lib.add(1, 0.5))

local f = function(...)
	print(...)
end
f("a", "b", "c")

local function f2(...)
	print(...)
end
f2("a", "b", "c")

--recursive function
function fact(n)
	if n == 0 then
		return 1
	else
		return n * fact(n - 1)
	end
end
print(fact(5))