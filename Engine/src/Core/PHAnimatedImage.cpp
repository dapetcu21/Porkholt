/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHAnimatedImage.h>
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/IO/PHFileManager.h>
#include <Porkholt/Core/PHImageAnimator.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHAnimatorPool.h>
#include <Porkholt/IO/PHDirectory.h>
#include <Porkholt/IO/PHFile.h>
#include <Porkholt/Core/PHTextureAtlas.h>

#define PNG_DEBUG 4
#include <png.h>
#include <sstream>

lua_State * PHAnimatedImage::L = NULL;
PHMutex * PHAnimatedImage::luaMutex = new PHMutex;

#define PHLuaAnimationsLuaAPI \
"function section(name) \
    if (not nametable) then \
        nametable = {} \
    end \
    if ((not anim) or (not(anim.n))) then \
        anim = { n=0 } \
    end \
    pntr = { n=0, nm = name } \
    anim[anim.n] = pntr \
    anim.n = anim.n + 1 \
    nametable[name] = pntr \
end \
\
function frame(frameno,duration) \
    if (not pntr) then \
        section(\"default\",false) \
    end \
    local frame = { fno=frameno, d=duration } \
    pntr[pntr.n] = frame \
    pntr.n = (pntr.n or 0) + 1 \
end \
\
function fade(frameno,duration) \
    if (not pntr) then \
        section(\"default\",false) \
    end \
    local frame = { fno=frameno, d=duration, fd = true} \
    pntr[pntr.n] = frame \
    pntr.n = (pntr.n or 0) + 1 \
end \
\
function jump(section,frameno) \
    frame(frameno,section) \
end \
\
function default(name) \
    def = nametable[name] \
end"

PHAnimatedImage::PHAnimatedImage(PHGameManager * gameManager, PHDirectory * dir) : PHImage(gameManager), defaultSection(0)
{
    luaMutex->lock();
    if (!L)
    {
        L = lua_open();
        luaL_openlibs(L);
        PHLuaSeedRandom(L);
        PHLuaLoadString(L, PHLuaAnimationsLuaAPI);
    }
    PHFile * initFile = NULL;
    try {
        initFile = dir->fileAtPath("info.lua");
    } catch (string ex)
    {
        luaMutex->unlock();
        throw ex;
    }
    if (!PHLuaLoadFile(L, initFile))
    {
        initFile->release();
        cleanupLua();
        luaMutex->unlock();
        throw PHLuaError;
    }
    initFile->release();
    try {
        atl = new PHTextureAtlas(gameManager, dir, L);
    } catch (string ex)
    {
        PHLog("meow");
        cleanupLua();
        luaMutex->unlock();
        throw ex;
    }
    loadFromLua();
    cleanupLua();
    luaMutex->unlock();
}

void PHAnimatedImage::loadFromLua()
{
    lua_newtable(L);
    PHLuaSetHardRef(L, this);
    int functions = 0;
    
    lua_getglobal(L, "def");
    if (lua_istable(L, -1))
    {
        lua_pushboolean(L, true);
        lua_setfield(L, -2, "default");
    }
    lua_pop(L, 1);
    
    int max = -1;
    
    lua_getglobal(L, "anim");
    if (lua_istable(L, -1))
    {
        int n = 0;
        PHLuaGetNumberField(n, "n");
        for (int i=0; i<n; i++)
        {
            lua_pushnumber(L, i);
            lua_gettable(L, -2);
            if (lua_istable(L, -1))
            {
                lua_getfield(L, -1, "default");
                if (lua_isboolean(L, -1) && lua_toboolean(L, -1))
                    defaultSection = i;
                lua_pop(L,1);
                
                section * sec = new section;

                PHLuaGetStringField(sec->name, "nm");
                
                int m = 0;
                PHLuaGetNumberField(m, "n");
                for (int j=0; j<m; j++)
                {
                    lua_pushnumber(L, j);
                    lua_gettable(L, -2);
                    if (lua_istable(L, -1))
                    {
                        frame frm;
                        frm.frameno = 0;
                        frm.duration = 1.0f;
                        frm.type = 0;
                        frm.fade = false;
                        
                        PHLuaGetNumberField(frm.frameno, "fno");
                        PHLuaGetBoolField(frm.fade, "fd");
                        
                        lua_getfield(L, -1, "d");
                        if (lua_isnumber(L, -1))
                            frm.duration = lua_tonumber(L, -1);
                        else
                        if (lua_isfunction(L, -1))
                        {
                            frm.type = 1;
                            frm.fnumber = functions;
                            PHLuaGetHardRef(L, this);
                            lua_pushnumber(L, functions++);
                            lua_pushvalue(L, -3);
                            lua_settable(L, -3);
                            lua_pop(L,1);
                        }
                        else
                        if (lua_isstring(L, -1))
                        {
                            frm.type = 2;
                            frm.sectionName = new string(lua_tostring(L,-1));
                        }
                        lua_pop(L,1);
                        
                        if (frm.frameno > max)
                            max = frm.frameno;
                        
                        sec->frames.push_back(frm);
                    }
                    lua_pop(L, 1);
                }
                sections.push_back(sec);
            }
            lua_pop(L,1);
        }
    }
    lua_pop(L,1);
    
    for (size_t i=0; i<sections.size(); i++)
    {
        section * sec = sections[i];
        for (size_t i=0; i<sec->frames.size(); i++)
        {
            if (sec->frames[i].type == 2)
            {
                string * s = sec->frames[i].sectionName;
                sec->frames[i].section = sectionNo(*s);
                delete s;
            }
        }
        frame f = sec->frames.back();
        f.duration = 0;
        f.fade = false;
        f.type = 0;
        sec->frames.push_back(f);
    }
}

void PHAnimatedImage::cleanupLua()
{
    lua_pushnil(L);
    lua_setglobal(L, "nametable");
    lua_pushnil(L);
    lua_setglobal(L, "anim");
    lua_pushnil(L);
    lua_setglobal(L, "pntr");
    lua_pushnil(L);
    lua_setglobal(L, "def");
//    lua_gc(L, LUA_GCCOLLECT, 0); not quite necessary
}

int PHAnimatedImage::sectionNo(const string & sectionName)
{
    for (size_t i=0; i<sections.size(); i++)
        if (sections[i]->name == sectionName)
            return i;
    return -1;
}
    
PHAnimatedImage::~PHAnimatedImage()
{
    PHLuaDeleteHardRef(L, this);
    for (size_t i=0; i<sections.size(); i++)
        delete sections[i];
}

PHImageAnimator * PHAnimatedImage::newAnimator()
{
    return new PHImageAnimator(this);
}

PHImageAnimator * PHAnimatedImage::newAnimator(PHAnimatorPool * p)
{
    return new PHImageAnimator(this,p);
}

void PHAnimatedImage::_load()
{
    if (atl)
        atl->load();
    loaded = true; 
}

void PHAnimatedImage::setAtlas(PHTextureAtlas * a)
{
    if (a)
        a->retain();
    if (atl)
        atl->release();
    atl = a;
    loaded = false;
}
