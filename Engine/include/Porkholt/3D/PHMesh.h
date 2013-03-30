/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHMESH_H
#define PHMESH_H

#include <Porkholt/Core/PHMain.h>

class PHGLVertexArrayObject;

class PHMesh : public PHObject
{
public:
    enum lods
    {
        lowLOD = 0,
        defaultLOD = 1,
        highLOD = 2
    };
    
    virtual PHGLVertexArrayObject * vaoForLevelOfDetail(int lod) = 0;
    PHGLVertexArrayObject * vao() { return vaoForLevelOfDetail(defaultLOD); }
    virtual int lodForDistance(float dist) { return defaultLOD; }
    virtual bool usesLevelsOfDetail() { return false; }
};

#endif
