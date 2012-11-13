/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/3D/PHSphereMesh.h>
#include <Porkholt/Core/PHGLVertexArrayObject.h>
#include <Porkholt/Core/PHGLVertexBufferObject.h>
#include <Porkholt/Core/PHGameManager.h>

#define INIT closeDist(-INFINITY), farDist(INFINITY), gm(_gm)

PHSphereMesh::PHSphereMesh(PHGameManager * _gm) : INIT
{
    vaos[0] = vaos[1] = vaos[2] = NULL;
    setResolutionForLOD(PHMesh::defaultLOD, PHSPHEREMESH_DEFAULT_LAT, PHSPHEREMESH_DEFAULT_LONG);
}

PHSphereMesh::PHSphereMesh(PHGameManager * _gm, int latitude, int longitude) : INIT
{
    vaos[0] = vaos[1] = vaos[2] = NULL;
    setResolutionForLOD(PHMesh::defaultLOD, latitude, longitude);
}

PHSphereMesh::~PHSphereMesh()
{
    releaseLOD(0);
    releaseLOD(1);
    releaseLOD(2);
}

PHGLVertexArrayObject * PHSphereMesh::vaoForLevelOfDetail(int lod)
{
    if (vaos[lod]) return vaos[lod];
    if (vaos[0]) return vaos[0];
    if (vaos[1]) return vaos[1];
    if (vaos[2]) return vaos[2];
    return NULL;
}

bool PHSphereMesh::usesLevelsOfDetail()
{
    int c = 0; 
    if (vaos[0]) c++;
    if (vaos[1]) c++;
    if (vaos[2]) c++;
    return (c>1);
}

void PHSphereMesh::setResolutionForLOD(int lod, int latitude, int longitude)
{
    releaseLOD(lod);
    int i, j;
    GLfloat * buf = new GLfloat[(longitude+1)*(latitude)*8*2];
    int k = 0;
    for(i = 0; i < latitude; i++) 
    {
        double lat0f = (double) i / latitude;
        double lat0 = M_PI * (-0.5 + lat0f);
        double z0  = sin(lat0);
        double zr0 =  cos(lat0);
        
        double lat1f = (double) (i+1) / latitude;
        double lat1 = M_PI * (-0.5 + lat1f);
        double z1 = sin(lat1);
        double zr1 = cos(lat1);
        
        for(j = 0; j <= longitude; j++) 
        {
            double lngf = (double) (j) / longitude;
            double lng = 2 * M_PI * lngf;
            double x = -cos(lng);
            double y = sin(lng);
            
            buf[k++] = x * zr1; 
            buf[k++] = z1;
            buf[k++] = y * zr1; 
            //--
            buf[k++] = x * zr1; 
            buf[k++] = z1;
            buf[k++] = y * zr1; 
            //--
            buf[k++] = lngf;
            buf[k++] = lat1f;
            //---
            buf[k++] = x * zr0; 
            buf[k++] = z0;
            buf[k++] = y * zr0; 
            //--
            buf[k++] = x * zr0; 
            buf[k++] = z0;
            buf[k++] = y * zr0; 
            //--
            buf[k++] = lngf;
            buf[k++] = lat0f;
        }        
    }
    vaos[lod] = new PHGLVertexArrayObject(gm);
    vaos[lod]->bindToEdit();
    PHGLVBO * vbo = new PHGLVertexBufferObject(gm);
    vbo->bindToArrayBuffer();
    vbo->setData(buf, (longitude+1)*(latitude)*8*2*sizeof(GLfloat), PHGLVBO::staticDraw);
    vaos[lod]->vertexPointer(PHIMAGEATTRIBUTE_POS, 3, GL_FLOAT, false, sizeof(GLfloat)*8, 0, vbo);
    vaos[lod]->vertexPointer(PHIMAGEATTRIBUTE_NRM, 3, GL_FLOAT, false, sizeof(GLfloat)*8, sizeof(GLfloat)*3, vbo);
    vaos[lod]->vertexPointer(PHIMAGEATTRIBUTE_TXC, 2, GL_FLOAT, false, sizeof(GLfloat)*8, sizeof(GLfloat)*6, vbo);
    vaos[lod]->setDrawArrays(GL_TRIANGLE_STRIP, 0, (longitude+1)*(latitude)*2);
    vbo->unbind();
    vbo->release();
    vaos[lod]->unbind();
    delete[] buf;
}

void PHSphereMesh::releaseLOD(int lod)
{
    if (vaos[lod])
    {
        vaos[lod]->release();
        vaos[lod] = NULL;
    }
}

PHSphereMesh * PHSphereMesh::sphere(PHGameManager * gm)
{
    static map<PHGameManager*,PHSphereMesh*> map;
    PHSphereMesh * sm = map[gm];
    if (!sm)
        map[gm] = sm = new PHSphereMesh(gm);
    return sm;
}
