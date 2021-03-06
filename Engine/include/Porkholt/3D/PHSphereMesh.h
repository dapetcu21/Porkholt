/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHSPHEREMESH_H
#define PHSPHEREMESH_H

#include <Porkholt/3D/PHMesh.h>

//#define PHSPHEREMESH_DEFAULT_LAT 30
//#define PHSPHEREMESH_DEFAULT_LONG 30
#define PHSPHEREMESH_DEFAULT_LAT 30
#define PHSPHEREMESH_DEFAULT_LONG 30

class PHGLVertexArrayObject;
class PHGLVertexBufferObject;
class PHGameManager;
class PHSphereMesh : public PHMesh
{
private:
    PHGLVertexArrayObject * vaos[3];
    float closeDist, farDist;
    PHGameManager * gm;
public:
    PHSphereMesh(PHGameManager * gm);
    PHSphereMesh(PHGameManager * gm, int latitude, int longitude);
    ~PHSphereMesh();
    void setMediumLODInterval(float close, float far)
    {
        closeDist = close; 
        farDist = far;
    }
    
    PHGLVertexArrayObject * vaoForLevelOfDetail(int lod);
    int lodForDistance(float dist) { return (dist<closeDist)?PHMesh::highLOD:((dist>farDist)?PHMesh::lowLOD:PHMesh::defaultLOD); }
    bool usesLevelsOfDetail();
    
    void setResolutionForLOD(int lod, int latitude, int longitude);
    
    static PHSphereMesh * sphere(PHGameManager * gm);
private:
    static map<PHGameManager*,PHSphereMesh*> spheres;
    void gameManagerQuits(PHGameManager * sender);

    void releaseLOD(int lod);
};

#endif
