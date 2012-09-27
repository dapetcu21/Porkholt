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

self_p = strip_n(exec('readlink "'..arg[0]..'"'))
if (#self_p == 0) then
    self_p = arg[0]
end
script_path = strip_n(exec('dirname "'..self_p..'"'))
engine_path = script_path..'/..'
if arg[2] then
    file = io.open(arg[2], 'r')
end
if file then
    head = strip_n(file:read("*a"))..'\n'
    file:close()
else
    head = "/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */\n"
end

class = arg[1]
base = string.sub(class, string.find(class, "%w*$"))
file = io.open(engine_path..'/src/'..class..'.cpp', 'w')
file:write(head.."#include <Porkholt/"..class..".h>\n")
file:close()

macro = string.upper(base).."_H"
file = io.open(engine_path..'/include/Porkholt/'..class..'.h', 'w')
file:write(head..'#ifndef '..macro..'\n#define '..macro..'\n\n#include <Porkholt/Core/PHMain.h>\n\nclass '..base..' : public PHObject\n{\n};\n\n#endif\n')
file:close()

fname = script_path..'/Porkholt_Files.cmake'
file = io.open(fname, "r")
files = file:read("*a")
file:close()

if (not string.find(files, class)) then
    files = string.gsub(files, "(set%(PH_ENGINE_SRCS[ \t\n{}%$/%.%w_]*\n)(([ \t]*)%))", "%1%3${PH_ENGINE_PATH}/src/"..class..".cpp\n%2", 1)
    files = string.gsub(files, "(set%(PH_ENGINE_HEADERS[ \t\n{}%$/%.%w_]*\n)(([ \t]*)%))", "%1%3${PH_ENGINE_PATH}/include/Porkholt/"..class..".h\n%2", 1)
    file = io.open(fname, "w")
    file:write(files)
    file:close()
end
