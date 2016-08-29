--------------------------------------------------------------------
-- Complex module
--
--------------------------------------------------------------------
local complex = {}

function complex.add(c1, c2)
    return complex.new(c1.r + c2.r, c1.i + c2.i)
end

function complex.sub(c1, c2)
    return complex.new(c1.r - c2.r, c1.i - c2.i)
end

function complex.tostring(c)
    local s = ""
    if c.i < 0 then
        s = c.r .. c.i .. "i"
    elseif c.i == 0 then
        s = c.r .. ""
    else
        s = c.r .. "+" ..c.i .. "i"
    end
    return s 
end

local __mt = { __add = complex.add, __sub = complex.sub, __tostring = complex.tostring }

function complex.new(r, i)
    local t = {r = r, i = i}
    setmetatable(t, __mt)
    return t
end


return complex