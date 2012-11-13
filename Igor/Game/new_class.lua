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
engine_path = script_path
head = "/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */\n"

class = arg[1]
super = arg[2]
if (not super or (#super == 0)) then
    super = "IGProp"
end
base = string.sub(class, string.find(class, "%w*$"))
file = io.open(engine_path..'/Classes/'..class..'.cpp', 'w')
file:write(head.."\n#include \""..class..".h\"\n"
..[[#include "IGWorld.h"
#include "IGScripting.h"
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Box2D/Box2D.h>

IGSCRIPTING_REGISTERCLASS("]]..base..[[", ]]..base..[[)

]]..base..[[::]]..base..[[(IGWorld * w) : ]]..super..[[(w)
{
}

]]..base..[[::~]]..base..[[()
{
}

void ]]..base..[[::attachedToWorld()
{
}

void ]]..base..[[::configureDrawable(PHDrawable * d)
{
}

//--- Lua Scripting ---

void ]]..base..[[::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "]]..base..[[");

    lua_pop(L, 1);
}
]])
file:close()

macro = string.upper(base).."_H"
file = io.open(engine_path..'/Classes/'..class..'.h', 'w')
file:write(head..'\n#ifndef '..macro..'\n#define '..macro..'\n\n#include "'..super..'.h"\n\nclass '..base..[[ : public ]]..super..'\n'..
[[{
    protected:
        void configureDrawable(PHDrawable * d);
    public:
        ]]..base..[[(IGWorld * w);
        ~]]..base..[[();

        void attachedToWorld();

        const char * luaClass() { return "]]..base..[["; } 
        static void loadLuaInterface(IGScripting * s);
};

#endif
]])
file:close()

fname = script_path..'/CMakeLists.txt'
file = io.open(fname, "r")
files = file:read("*a")
file:close()

if (not string.find(files, class)) then
    files = string.gsub(files, "(set%(PH_SOURCES[ \t\n{}%$/%.%w_]*\n)(([ \t]*)%))", "%1%3Classes/"..class..".cpp\n%2", 1)
    files = string.gsub(files, "(set%(PH_HEADERS[ \t\n{}%$/%.%w_]*\n)(([ \t]*)%))", "%1%3Classes/"..class..".h\n%2", 1)
    file = io.open(fname, "w")
    file:write(files)
    file:close()
end
