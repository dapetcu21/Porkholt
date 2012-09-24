#!/usr/bin/env lua

function exec(x)
    local file = io.popen(x)
    local s = file:read("*a")
    file:close()
    return s;
end

function strip_n(x)
    if (string.sub(x, -1) == '\n') then
        x = string.sub(x, 1, -2)
    end
    return x
end

map = {}
out = ""

for i,v in ipairs(arg) do
    v = strip_n(exec('readlink -f "'..v..'"'))
    v = strip_n(exec('dirname "'..v..'"'))
    if not map[v] then
        map[v] = true
        out = out..v..' '
    end
end

io.write(out)
