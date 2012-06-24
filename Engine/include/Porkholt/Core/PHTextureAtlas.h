/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHTEXTUREATLAS_H
#define PHTEXTUREATLAS_H

#include <Porkholt/Core/PHMain.h>

class PHGLTexture2D;
class PHGameManager;
class PHDirectory;

class PHTextureAtlas : public PHObject
{
protected:
    struct loadStruct;
    PHGLTexture2D ** _textures;
    PHRect * _texCoords;
    PHGameManager * gm;
    int n;
    volatile bool loaded;
    bool ploaded;
    void loadFromDir(PHDirectory * dir, lua_State * L);
    void loadInMemory();
    void loadInVRAM();
    void asyncLoad();
    PHMutex * mutex;
    loadStruct * ls;
    static PHMutex * luaMutex;
    static lua_State * staticL;
public:
    PHTextureAtlas(PHGameManager * gameManager, PHDirectory * dir);
    PHTextureAtlas(PHGameManager * gameManager, PHDirectory * dir, lua_State * L);
    void load();

    PHRect textureCoordinates(int index) { load(); return _texCoords[index]; }
    PHGLTexture2D * texture(int index) { load(); return _textures[index]; }
    int count() { return n; }

    ~PHTextureAtlas();
};

#endif
