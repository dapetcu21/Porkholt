/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/3D/PHSphereMesh.h>
#include <Porkholt/Core/PHGLVertexArrayObject.h>
#include <Porkholt/Core/PHGLVertexBufferObject.h>
#include <Porkholt/Core/PHGameManager.h>

#define INIT closeDist(-INFINITY), farDist(INFINITY), gm(_gm)

map<PHGameManager*,PHSphereMesh*> PHSphereMesh::spheres;

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
    int vertices = ((longitude+2)*(latitude)-1)*2;
    GLfloat * buf = new GLfloat[vertices*8];
    int k = -8*2;
    for(i = 0; i < latitude; i++) 
    {
        GLfloat lat0f = (GLfloat) i / latitude;
        GLfloat lat0 = M_PI * (-0.5 + lat0f);
        GLfloat z0  = sin(lat0);
        GLfloat zr0 =  cos(lat0);
        
        GLfloat lat1f = (GLfloat) (i+1) / latitude;
        GLfloat lat1 = M_PI * (-0.5 + lat1f);
        GLfloat z1 = sin(lat1);
        GLfloat zr1 = cos(lat1);

        if (i)
            memcpy(buf+k, buf+k-8, sizeof(GLfloat)*8);
        k+=2*8;
        int last_k = k;
        
        for(j = 0; j <= longitude; j++) 
        {
            GLfloat lngf = (GLfloat) (j) / longitude;
            GLfloat lng = 2 * M_PI * lngf;
            GLfloat x = -cos(lng);
            GLfloat y = sin(lng);
            
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

        if (i)
            memcpy(buf+last_k-8, buf+last_k, sizeof(GLfloat)*8);
    }
    vaos[lod] = new PHGLVertexArrayObject(gm);
    vaos[lod]->bindToEdit();
    PHGLVBO * vbo = new PHGLVertexBufferObject(gm);
    vbo->bindToArrayBuffer();
    vbo->setData(buf, vertices*8*sizeof(GLfloat), PHGLVBO::staticDraw);
    vaos[lod]->vertexPointer(PHIMAGEATTRIBUTE_POS, 3, GL_FLOAT, false, sizeof(GLfloat)*8, 0, vbo);
    vaos[lod]->vertexPointer(PHIMAGEATTRIBUTE_NRM, 3, GL_FLOAT, false, sizeof(GLfloat)*8, sizeof(GLfloat)*3, vbo);
    vaos[lod]->vertexPointer(PHIMAGEATTRIBUTE_TXC, 2, GL_FLOAT, false, sizeof(GLfloat)*8, sizeof(GLfloat)*6, vbo);
    vaos[lod]->setDrawArrays(GL_TRIANGLE_STRIP, 0, vertices);
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
    PHSphereMesh * sm = spheres[gm];
    if (!sm)
    {
        spheres[gm] = sm = new PHSphereMesh(gm);
        gm->deallocMessage()->addListener(PHInvN(sm, PHSphereMesh::gameManagerQuits));
    }
    return sm;
}

void PHSphereMesh::gameManagerQuits(PHGameManager * gm)
{
    map<PHGameManager*,PHSphereMesh*>::iterator i = spheres.find(gm);
    if (i != spheres.end())
    {
        i->second->release();
        spheres.erase(i);
    }
}

