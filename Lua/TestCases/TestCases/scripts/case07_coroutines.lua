--------------------------------------------------------------------
-- Case 07
--
-- coroutines in Lua
--
-- coroutine is similar to thread: it's a line of execution, with own stack, 
-- and own local variables and own instruction pointer.
-- coroutines are collabortive. at any given time only one coroutine is running.
--
-- status of coroutine
-- 1. suspended     - not end(call yeild or not start runing)
-- 2. running       - when resume
-- 3. dead
-- 4. normal        - when coroutine a resume coroutine b
--------------------------------------------------------------------
local co = coroutine.create(function() print("Hello, Coroutine") end)
print(co) -- thread
print(coroutine.status(co)) -- suspended
coroutine.resume(co)
print(coroutine.status(co)) -- dead

local co = coroutine.create(function()
	for i = 1, 2 do
		print("co", i)
		coroutine.yield()
	end
end)
print(coroutine.status(co))
coroutine.resume(co)
print(coroutine.status(co))
coroutine.resume(co)
print(coroutine.status(co))
coroutine.resume(co)
print(coroutine.status(co))

--
local co = coroutine.create(function(a, b, c)
	print("co", a, b, c + 2)
end)
coroutine.resume(co, 1, 2, 3)

local co = coroutine.create(function(a, b)
	coroutine.yield(a + b, a - b)
end)
print(coroutine.resume(co, 20, 10))

-- 1. first resume, extra param is pass to coroutine's function
-- 2. in coroutine, param of yield will return to the resume call of main thread
-- 3. resume agin, the extra param will as return of last yield
function foo(a)
	print("foo", a) -- foo 2
	return coroutine.yield(2 * a)
end

print("--------------")
local co = coroutine.create(function(a, b)
	print("co-body", a, b) --co-body 1 10
	local r = foo(a + 1)

	print("co-body2", r)
	local r, s = coroutine.yield(a + b, a - b)

	print("co-body3", r, s)
	return b, "end"
end)
print("main", coroutine.resume(co, 1, 10))
print("--------------")
print("main", coroutine.resume(co, "r"))
print("--------------")
print("main", coroutine.resume(co, "x", "y"))
print("--------------")
print("main", coroutine.resume(co, "x", "y"))

-- producer-consumer

-- 1. first consumer resume producer, producer produce a value, then yield
-- 2. consumer get param of yield, as return value of last resume, print value
-- 3. consumer resume producer again, then loop until end
local newProducer

function producer()
	local i = 0
	while i < 10 do
		i = i + 1
		send(i)
	end
end

function consumer()
	local k = 0
	while k < 10 do
		local i = receive()
		print(i)
		k = k + 1
	end
end

function send(i)
	coroutine.yield(i)
end

function receive()
	local status, value = coroutine.resume(newProducer)
	return value
end

newProducer = coroutine.create(producer)
consumer()
print(coroutine.status(newProducer))
coroutine.resume(newProducer)
print(coroutine.status(newProducer))