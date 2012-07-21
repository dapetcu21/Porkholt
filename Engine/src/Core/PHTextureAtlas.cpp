/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHTextureAtlas.h>
#include <Porkholt/IO/PHFile.h>
#include <Porkholt/IO/PHDirectory.h>
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHImage.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHAnimatorPool.h>
#include <Porkholt/Core/PHGLTexture.h>

lua_State * PHTextureAtlas::staticL = NULL;
PHMutex * PHTextureAtlas::luaMutex = new PHMutex;

#define INIT gm(gameManager), loaded(false), ploaded(false), _textures(NULL), _texCoords(NULL), mutex(new PHMutex)

PHTextureAtlas::PHTextureAtlas(PHGameManager * gameManager, PHDirectory * dir) : INIT
{
    luaMutex->lock();
    if (!staticL)
    {
        staticL = lua_open();
        luaL_openlibs(staticL);
        PHLuaSeedRandom(staticL);
    }
    try {
        loadFromDir(dir, staticL);
    } catch (string ex) {
        luaMutex->unlock();
        throw ex;
    }
    luaMutex->unlock();
}

PHTextureAtlas::PHTextureAtlas(PHGameManager * gameManager, PHDirectory * dir, lua_State * L) : INIT
{
    loadFromDir(dir, L);
}
 
void PHTextureAtlas::load()
{
    if (!ploaded)
    {
        mutex->lock();
        if (!ploaded)
        {
#ifdef PHIMAGE_ORDERED_LOADING
            PHImage::loadingMutex->lock();
#endif
            loadInMemory();
#ifdef PHIMAGE_ORDERED_LOADING
            PHImage::loadingMutex->unlock();
#endif
        }
        mutex->unlock();
    }
    if (!loaded)
        loadInVRAM();
}

void PHTextureAtlas::asyncLoad()
{
    mutex->lock();
#ifdef PHIMAGE_ORDERED_LOADING
    PHImage::loadingMutex->lock();
#endif
    loadInMemory();
#ifdef PHIMAGE_ORDERED_LOADING
    PHImage::loadingMutex->unlock();
#endif
    mutex->unlock();
    gm->mainAnimatorPool()->scheduleAction(PHInv(this, PHTextureAtlas::loadInVRAM, NULL));
}

struct PHTextureAtlas::loadStruct
{
    vector<string> maps;
    vector< pair< int, PHRect > > imgs;
    struct params
    {
        uint8_t * data;
        size_t w,h,bw,bh,sz;
        bool aa;
        PHRect rect;
        PHGLTexture::pixelFormat fmt;
        ~params() {
            delete[] data;
        }
    };
    vector<params> mp;
    PHDirectory * dir;
    ~loadStruct()
    {
        dir->release();
    }
};

void PHTextureAtlas::loadFromDir(PHDirectory * dir, lua_State * L)
{
    lua_getglobal(L, "textureMaps");
    ls = NULL;
    if (lua_istable(L, -1))
    {
        ls = new PHTextureAtlas::loadStruct;
        ls->dir = dir;
        dir->retain();
        int o = -1;
        lua_pushnil(L);
        while (lua_next(L,-2) != 0) 
        {
            if (lua_istable(L, -1) && lua_isstring(L, -2))
            {
                o++;
                string fn = lua_tostring(L, -2);
                ls->maps.push_back(fn);
                PHLuaForEach(-1)
                    if (lua_istable(L, -1))
                        ls->imgs.push_back(make_pair(o, PHRect::fromLua(L, -1)));
                PHLuaForEach_
            }
            lua_pop(L, 1);
        }
    }

    if (!ls)
        throw string("Nothing. There's nothing in this atlas. No texture. Nothing");

#ifdef PHIMAGE_ASYNCHRONEOUS_LOADING
    PHThread::detachThread(PHInv(this, PHTextureAtlas::asyncLoad, NULL));
#else
    load();
#endif
}

void PHTextureAtlas::loadInMemory()
{
    if (ploaded) return;
    int j=0;
    ls->mp.resize(ls->maps.size());
    bool pot = !PHGLTexture::supportsNPOT(gm);
    for (vector<string>::iterator i = ls->maps.begin(); i!= ls->maps.end(); i++, j++)
    {
        PHFile * f = ls->dir->fileAtPath(*i);
        PHTextureAtlas::loadStruct::params & p = ls->mp[j];
        p.data = PHGLTexture::dataFromFile(f, p.w, p.h, p.bw, p.bh, pot, p.sz, p.fmt);
        f->release();
        p.aa = ls->dir->fileExists((*i) + ".aa");
    }
    ploaded = true;
}

void PHTextureAtlas::loadInVRAM()
{
    if (loaded) return;
    n = ls->imgs.size();
    vector<PHGLTexture2D*> v;
    v.reserve(ls->maps.size());
    for (vector<PHTextureAtlas::loadStruct::params>::iterator i = ls->mp.begin(); i != ls->mp.end(); i++)
    {
        PHGLTexture2D * tx = new PHGLTexture2D(gm);
        i->rect = tx->loadFromData(i->data, i->w, i->h, i->bw, i->bh, i->fmt, i->aa);
        v.push_back(tx);
    }

    int j = 0;
    _textures = new PHGLTexture2D * [n];
    _texCoords = new PHRect[n];
    for(vector< pair< int, PHRect > >::iterator i = ls->imgs.begin(); i != ls->imgs.end(); i++, j++)
    {
        PHTextureAtlas::loadStruct::params & p = ls->mp[i->first];
        _texCoords[j] = PHRect(i->second.x / (p.w-1), i->second.y / (p.h-1), i->second.width / (p.w-1), i->second.height / (p.h-1)) * p.rect;
        _textures[j] = v[i->first];
        v[i->first]->retain();
    }

    for(vector<PHGLTexture2D*>::iterator i = v.begin(); i!= v.end(); i++)
        (*i)->release();

    delete ls;
    loaded = true;
}

PHTextureAtlas::~PHTextureAtlas()
{
    if (_textures)
        delete [] _textures;
    if (_texCoords)
        delete [] _texCoords;
    if (ls)
        delete ls;
    mutex->release();
}
