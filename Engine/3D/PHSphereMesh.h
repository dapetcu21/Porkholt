//
//  PHSphereMesh.h
//  3DDemo
//
//  Created by Marius Petcu on 4/13/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#ifndef PHSPHEREMESH_H
#define PHSPHEREMESH_H

#include "PHMesh.h"

#define PHSPHEREMESH_DEFAULT_LAT 20
#define PHSPHEREMESH_DEFAULT_LONG 20

class PHGLVertexArrayObject;
class PHGLVertexBufferObject;
class PHGameManager;
class PHSphereMesh : public PHMesh
{
private:
    PHGLVertexArrayObject * vaos[3];
    PHGLVertexBufferObject * vbos[3];
    ph_float closeDist, farDist;
    PHGameManager * gm;
public:
    PHSphereMesh(PHGameManager * gm);
    PHSphereMesh(PHGameManager * gm, int latitude, int longitude);
    void setMediumLODInterval(ph_float close, ph_float far)
    {
        closeDist = close; 
        farDist = far;
    }
    
    PHGLVertexArrayObject * vaoForLevelOfDetail(int lod);
    int lodForDistance(ph_float dist) { return (dist<closeDist)?PHMesh::highLOD:((dist>farDist)?PHMesh::lowLOD:PHMesh::defaultLOD); }
    bool usesLevelsOfDetail();
    
    void setResolutionForLOD(int lod, int latitude, int longitude);
    
    static PHSphereMesh * sphere(PHGameManager * gm);
private:
    void releaseLOD(int lod);
};

#endif
