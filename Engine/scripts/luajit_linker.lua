#!/usr/bin/env lua

linker = arg[1]
args = {}
archs = {}
archspec = { x86_64={ "-pagezero_size", "10000", "-image_base", "100000000" } }
local i = 2
while true do
    local v = arg[i]
    if v == nil then
        break
    end
    if v == "-o" then
        i = i + 1
        out = arg[i]
        table.insert(args, "-o@")
    elseif v == "-arch" then
        i = i + 1
        table.insert(archs, arg[i])
    else
        table.insert(args, arg[i])
    end
    i = i + 1
end

cl = ""
filelist = ""

if not next(archs) then
    table.insert(archs, "native")
end

for i,arch in pairs(archs) do
    cl = cl..'"'..linker..'" '
    if arch ~= "native" then
        cl = cl..'-arch "'..arch..'" '
    end
    local t = archspec[arch] or {}
    for j,s in pairs(t) do
        cl = cl..'"'..s..'" '
    end
    for j,s in pairs(args) do
        if s == "-o@" then
            local s = out..'.'..arch
            cl = cl..'-o "'..s..'" '
            filelist = filelist .. '"' .. s .. '" '
        else
            cl = cl..'"'..s..'" '
        end
    end
    cl = cl .. ';\\\n '
end

if out then
    cl = cl .. 'lipo -create '..filelist..'-o "'..out..'"; rm -f '..filelist
end

print(cl)
os.execute(cl)
