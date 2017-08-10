local print_obj_pool = function(weak_obj_pool)
	for k, v in pairs(weak_obj_pool) do
		print(v.hold_obj.name .. " " .. v.actual_obj.name)
	end
end

print("0.------->create weak obj, then gc")

local weak_obj_pool = {}
setmetatable(weak_obj_pool, { __mode = "v" })

weak_obj_pool["one"] = { hold_obj = { name = "<one>hold obj" }, actual_obj = { name = "<one>actual obj" } }
weak_obj_pool["two"] = { hold_obj = { name = "<two>hold obj" }, actual_obj = { name = "<two>actual obj" } }
print_obj_pool(weak_obj_pool)

print("1.------->hold one obj, then gc")

local one_obj = weak_obj_pool["one"]
collectgarbage("collect")
print_obj_pool(weak_obj_pool)

print("2.------->release one obj, then gc")

one_obj = nil
collectgarbage("collect")
print_obj_pool(weak_obj_pool)

print("3.------->recreate two obj")

weak_obj_pool["one"] = { hold_obj = { name = "<one>hold obj" }, actual_obj = { name = "<one>actual obj" } }
weak_obj_pool["two"] = { hold_obj = { name = "<two>hold obj" }, actual_obj = { name = "<two>actual obj" } }
print_obj_pool(weak_obj_pool)

print("4.------->only hold actual obj of one, then gc")

local one_actual_obj = weak_obj_pool["one"].actual_obj
collectgarbage("collect")
print_obj_pool(weak_obj_pool)

print("5.------->recreate weak obj pool")
local weak_obj_pool3_path2obj = {}
local weak_obj_pool3_obj2holder = {}

setmetatable(weak_obj_pool3_path2obj, {__mode = "v"})
setmetatable(weak_obj_pool3_obj2holder, {__mode = "k"})

local actual_obj = { name = "<one>actual obj" }
local obj_holder = { name = "<one>hold obj" }

weak_obj_pool3_path2obj["one"] = actual_obj
weak_obj_pool3_obj2holder[actual_obj] = obj_holder

actual_obj = nil
obj_holder = nil

local get_actual_obj = weak_obj_pool3_path2obj["one"]
get_actual_obj = nil

collectgarbage("collect")

	for k, v in pairs(weak_obj_pool3_path2obj) do
		print("path:" .. k .. " - obj:" .. v.name)
	end
	for k, v in pairs(weak_obj_pool3_obj2holder) do
		print("holder:" .. v.name)
	end

