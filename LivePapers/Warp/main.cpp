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
    float starWidth;
};

extern "C" {

void WPLoadSettings(WarpSettings & s);
void WPDefaultSettings(WarpSettings & s);

void WPDefaultSettings(WarpSettings & s)
{
    s.speed = 10.0f;
    s.starSpread = 1.0f;
    s.starWidth = 1.0f;
    s.starLength = 50.0f;
}

#ifndef PH_LIVEPAPERS
void WPLoadSettings(WarpSettings & s)
{
    WPDefaultSettings(s);
}
#endif

}

#define NEAR_PLANE 0.5f
#define FAR_PLANE 50.0f

class WarpMesh : public PHMesh
{
    protected:
        PHGameManager * gm;
        PHGLVertexArrayObject * vao;
        PHGLVertexBufferObject * vbo, * elements;
        struct point
        {
            float z;
            PHPoint up, down;
        };
        float distanceLeft;
        list<point> points;
        WarpSettings s;
    public:
        WarpMesh(PHGameManager * _gm) : gm(_gm), distanceLeft(0)
        {
            vao = new PHGLVertexArrayObject(gm);
            vbo = new PHGLVertexBufferObject(gm);
            elements = new PHGLVertexBufferObject(gm);
            vao->bindToEdit();
            vao->vertexPointer(PHIMAGEATTRIBUTE_POS, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*5, 0, vbo);
            vao->vertexPointer(PHIMAGEATTRIBUTE_TXC, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*5, sizeof(GLfloat)*3, vbo);
            vao->setElementArrayVBO(elements);
            vao->unbind();

            loadSettings();
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
            WPLoadSettings(s);
        }

        void advanceAnimation(float timeElapsed)
        {
            float distance = timeElapsed * s.speed;
            float radius = max(1.0f, gm->screenWidth() / gm->screenHeight());
            PHLog("Animated: %f", timeElapsed);

            while (true)
            {
                point & p = points.front();
                if (p.z - s.starLength < -NEAR_PLANE)
                    break;
                points.pop_back();
            }

            distanceLeft -= distance;
            while (distanceLeft <= 0)
            {
                point p;
                p.z = -FAR_PLANE - distanceLeft;
                float r = frand(0.5f, 1.0f) * radius;
                float angle = frand(0.0f, M_PI * 2.0f);
                float sinv = sinf(angle);
                float cosv = cosf(angle);
                p.up = PHPoint(r, s.starWidth).rotated(angle);
                p.down = PHPoint(r, -s.starWidth).rotated(angle);
                distanceLeft += frand(0.5f, 1.0f) * s.starSpread;
            }

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
                p[12] = pt.z - s.starLength;
                p[13] = 1.0f;
                p[14] = 0.0f;

                p[15] = pt.down.x;
                p[16] = pt.down.y;
                p[17] = pt.z - s.starLength;
                p[18] = 1.0f;
                p[19] = 1.0f;

                p+= 20;
            }

            vbo->setDataOptimally(v, vertices * 5 * sizeof(GLfloat), PHGLVBO::dynamicDraw);
            delete[] v;

            GLshort * idx = new GLshort[n * 6 - 2];
            idx[0] = 0;
            idx[1] = 1;
            idx[2] = 2;
            idx[3] = 3;

            for (int i = 4, j = 4; i < (n<<2); i+=4, j+=6)
            {
                idx[j-2] = i-1;
                idx[j-1] = i;
                idx[j] = i;
                idx[j+1] = i+1;
                idx[j+2] = i+2;
                idx[j+3] = i+3;
            }
            elements->setDataOptimally(idx, sizeof(GLshort) * (n*6-2), PHGLVBO::dynamicDraw);
            vao->setDrawElements(GL_TRIANGLE_STRIP, n*6-2, GL_SHORT, 0);
        }
};

#include <Porkholt/3D/PHSphereMesh.h>

void PHGameEntryPoint(PHGameManager * gm)
{    
    PHPerspectiveCamera * cam = new PHPerspectiveCamera();
    cam->setNearClippingPlane(NEAR_PLANE);
    cam->setFarClippingPlane(FAR_PLANE);
    gm->setMainDrawable(cam);

    WarpMesh * mesh = new WarpMesh(gm);
    PHMeshBody * body = new PHMeshBody();
    body->setBlendingEnabled(true);
    body->setBackFaceCullingEnabled(false);
    body->setMaterial(gm->materialNamed("warp"));
    body->setMesh(mesh);
    cam->addChild(body);

    body->release();
    mesh->release();
    cam->release();
}

PHMAIN_DECLARE(PHWDepthBuffer | PHWVSync | PHWResizable | PHWFrameAnimation, &PHGameEntryPoint,NULL);
 
