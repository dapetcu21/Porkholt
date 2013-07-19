/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHWindowing.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHPerspectiveCamera.h>
#include <Porkholt/Core/PHGLVertexArrayObject.h>
#include <Porkholt/Core/PHGLVertexBufferObject.h>
#include <Porkholt/Core/PHAnimator.h>
#include <Porkholt/3D/PHMesh.h>
#include <Porkholt/3D/PHMeshBody.h>
#include <Porkholt/Core/PHAnimatorPool.h>
#include <Porkholt/Core/PHGLPolyElementVBO.h>
#include <Porkholt/Core/PHEvent.h>
#include "WSettings.h"
#include <dlfcn.h>

float frand(float f)
{
    return rand() * f * (1.0f / RAND_MAX);
}

float frand(float s, float f)
{
    return rand() * (f - s) * (1.0f / RAND_MAX) + s;
}

void WarpSettings::loadDefaults()
{
    speed = 8.0f;
    starsPerSecond = 100;
    starWidth = 0.1f;
    starLength = 2.5f;
}

#undef PH_LIVEPAPERS
#ifndef PH_LIVEPAPERS
void WarpSettings::load()
{
    loadDefaults();
}
#endif

#define NEAR_PLANE 0.5f
#define FAR_PLANE 20.0f

class WarpMesh : public PHMesh, public PHDrawableInputDelegate 
{
    protected:
        PHGameManager * gm;
        PHGLVertexArrayObject * vao;
        PHGLVertexBufferObject * vbo;
        PHGLPolyElementVBO * elements;
        bool touched;
        float touchedDist;
        struct point
        {
            float z;
            PHPoint up, down;
        };
        float distanceLeft;
        float offset;
        float starSpread;
        int chunk;
        list<point> points;
        WarpSettings s;
        PHMeshBody * _body;
    public:
        WarpMesh(PHGameManager * _gm, PHMeshBody * body) : gm(_gm), distanceLeft(0), _body(body), offset(0), touched(false), touchedDist(0)
        {
            vao = new PHGLVertexArrayObject(gm);
            vbo = new PHGLVertexBufferObject(gm);
            elements = new PHGLPolyElementVBO(gm);
            vao->bindToEdit();
            vao->vertexPointer(PHIMAGEATTRIBUTE_POS, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*5, 0, vbo);
            vao->vertexPointer(PHIMAGEATTRIBUTE_TXC, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*5, sizeof(GLfloat)*3, vbo);
            vao->setElementArrayVBO(elements);
            vao->unbind();

            loadSettings();

            body->setMesh(this);
            body->setInputDelegate(this);
            gm->animatorPool()->scheduleAction(PHInvBind(this, WarpMesh::advanceAnimation, NULL), 0.0, true);
        }

        ~WarpMesh()
        {
            vao->release();
            vbo->release();
            elements->release();
        }
        PHGLVertexArrayObject * vaoForLevelOfDetail(int lod)
        {
            return vao;
        }

        void loadSettings()
        {
            s.load();
            starSpread = s.speed / s.starsPerSecond;
            chunk = max((int)(0.25f * s.starsPerSecond), 1);
        }

        void drawableRecievedEvent(PHDrawable * d, PHEvent * evt)
        {
            switch (evt->type())
            {
                case PHEvent::touchDown:
                    if (!touched)
                    {
                        touched = true;
                        evt->setHandled(true);
                    }
                    break;
                case PHEvent::touchCancelled:
                case PHEvent::touchUp:
                    touched = false;
                    break;
            }
        }

        void advanceAnimation()
        {
            float timeElapsed = gm->elapsedTime();
            float radius = max(1.0f, gm->screenWidth() / gm->screenHeight());

            if (touched)
            {
                if (touchedDist <= s.starLength * 2)
                    touchedDist += timeElapsed * s.speed;
            } else
                PHLowPassFilter(touchedDist, 0.0f, timeElapsed, 5);
            float ss = s.starLength + touchedDist;

            float distance = timeElapsed * s.speed * (ss / s.starLength);

            while (!points.empty())
            {
                point & p = points.front();
                if (p.z - ss < -NEAR_PLANE)
                    break;
                points.pop_front();
            }

            bool reshape = touchedDist > 0.05;
            offset += distance;
            distanceLeft -= distance;
            while (distanceLeft <= 0)
            {
                for (int i = 0; i < chunk; i ++)
                {
                    point p;
                    p.z = -FAR_PLANE - distanceLeft - offset;
                    float r = frand(0.5f, 2.0f) * radius;
                    float angle = frand(0.0f, M_PI * 2.0f);
                    p.up = PHPoint(r, s.starWidth).rotated(angle);
                    p.down = PHPoint(r, -s.starWidth).rotated(angle);
                    distanceLeft += frand(0.5f, 1.0f) * starSpread;
                    points.push_back(p);
                }
                reshape = true;
            }


            if (reshape)
            {
                size_t n = points.size();
                if (!n)
                {
                    vao->disableDrawing();
                    return;
                }
                size_t vertices = n * 4;
                GLfloat * v = new GLfloat[vertices * 5];
                GLfloat * p = v;
                for (list<point>::iterator i = points.begin(); i != points.end(); i++)
                {
                    point & pt = *i;
                    pt.z += offset;

                    p[0] = pt.up.x;
                    p[1] = pt.up.y;
                    p[2] = pt.z;
                    p[3] = 0.0f;
                    p[4] = 0.0f;

                    p[5] = pt.down.x;
                    p[6] = pt.down.y;
                    p[7] = pt.z;
                    p[8] = 0.0f;
                    p[9] = 1.0f;

                    p[10] = pt.up.x;
                    p[11] = pt.up.y;
                    p[12] = pt.z - ss;
                    p[13] = 1.0f;
                    p[14] = 0.0f;

                    p[15] = pt.down.x;
                    p[16] = pt.down.y;
                    p[17] = pt.z - ss;
                    p[18] = 1.0f;
                    p[19] = 1.0f;

                    //PHLog("%f %f %f %f  %f %f", pt.up.x, pt.up.y, pt.down.x, pt.down.y, pt.z, pt.z - s.starLength);

                    p+= 20;
                }
                offset = 0.0f;

                PHGLCheckError();
                vbo->setDataOptimally(v, vertices * 5 * sizeof(GLfloat), PHGLVBO::dynamicDraw);
                delete[] v;

                elements->resize(n, vao);
            }

            PHVector3 pos = _body->position();
            pos.z = offset;
            _body->setPosition(pos);
        }
};

#include <Porkholt/3D/PHSphereMesh.h>

void PHGameEntryPoint(PHGameManager * gm)
{    
    PHPerspectiveCamera * cam = new PHPerspectiveCamera();
    cam->setNearClippingPlane(NEAR_PLANE);
    cam->setFarClippingPlane(FAR_PLANE);
    gm->setMainDrawable(cam);

    PHMeshBody * body = new PHMeshBody();
    body->setUserInput(true);
    body->setBlendingEnabled(true);
    body->setZTestingEnabled(false);
    body->setBackFaceCullingEnabled(false);
    body->setMaterial(gm->materialNamed("warp"));
    WarpMesh * mesh = new WarpMesh(gm, body);
    body->setPosition(PHVector3(0, 0, -5));
    cam->addChild(body);

    body->release();
    mesh->release();
    cam->release();
}

PHMAIN_DECLARE(PHWDepthBuffer | PHWVSync | PHWResizable | PHWFrameAnimation, &PHGameEntryPoint,NULL);
 
