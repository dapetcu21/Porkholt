/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHWindowing.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHPerspectiveCamera.h>
#include <Porkholt/Core/PHGLVertexArrayObject.h>
#include <Porkholt/Core/PHGLVertexBufferObject.h>
#include <Porkholt/Core/PHAnimator.h>
#include <Porkholt/3D/PHMesh.h>
#include <Porkholt/3D/PHMeshBody.h>
#include <dlfcn.h>

float frand(float f)
{
    return rand() * f * (1.0f / RAND_MAX);
}

float frand(float s, float f)
{
    return rand() * (f - s) * (1.0f / RAND_MAX) + s;
}

struct WarpSettings
{
    float speed;
    float starSpread;
    float starLength;
};

class WarpMesh : public PHMesh
{
    protected:
        PHGameManager * gm;
        PHGLVertexArrayObject * vao;
        PHGLVertexBufferObject * vbo, * elements;
        struct point
        {
            PHVector3 pos;
            float sin, cos;
        };
        list<point> points;
        WarpSettings s;
    public:
        WarpMesh(PHGameManager * _gm) : gm(_gm)
        {
            vao = new PHGLVertexArrayObject(gm);
            vbo = new PHGLVertexBufferObject(gm);
            elements = new PHGLVertexBufferObject(gm);
            vao->bindToEdit();
            vao->vertexPointer(PHIMAGEATTRIBUTE_POS, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*5, 0, vbo);
            vao->vertexPointer(PHIMAGEATTRIBUTE_TXC, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*5, sizeof(GLfloat)*3, vbo);
            vao->setElementArrayVBO(elements);
            vao->unbind();
        }

        ~WarpMesh()
        {
            vao->release();
            vbo->release();
        }
        PHGLVertexArrayObject * vaoForLevelOfDetail(int lod)
        {
            return vao;
        }

        void animate(float timeElapsed)
        {
            float distance = timeElapsed * s.speed;
            PHLog("Animated: %f", timeElapsed);
        }
};

#include <Porkholt/3D/PHSphereMesh.h>

void PHGameEntryPoint(PHGameManager * gm)
{    
    PHPerspectiveCamera * cam = new PHPerspectiveCamera();
    cam->setNearClippingPlane(0.5f);
    cam->setFarClippingPlane(50.0f);
    gm->setMainDrawable(cam);

    WarpMesh * mesh = new WarpMesh(gm);
    PHMeshBody * body = new PHMeshBody();
    body->setBlendingEnabled(true);
    body->setMaterial(gm->materialNamed("warp"));
    body->setMesh(mesh);
    cam->addChild(body);

    body->release();
    mesh->release();
    cam->release();
}

PHMAIN_DECLARE(PHWDepthBuffer | PHWVSync | PHWResizable | PHWFrameAnimation, &PHGameEntryPoint,NULL);
 
