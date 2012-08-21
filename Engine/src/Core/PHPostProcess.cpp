/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#include <Porkholt/Core/PHPostProcess.h>
#include <Porkholt/Core/PHMaterial.h>
#include <Porkholt/Core/PHGLUniformStates.h>
#include <Porkholt/Core/PHGameManager.h>

PHPostProcess::PHPostProcess(PHGameManager * gm) : PHTextureCanvas(gm), mat(NULL), us(new PHGLUniformStates), resMat(false)
{
}

PHPostProcess::~PHPostProcess()
{
    if (us)
        us->release();
    if (mat)
        mat->release();
}

void PHPostProcess::setMaterial(PHMaterial * m)
{
    if (m)
        m->retain();
    if (mat)
        mat->release();
    mat = m;
}

void PHPostProcess::render()
{
    PHTextureCanvas::render();
    if (resMat)
    {
        PHMatrix om = gm->modelViewMatrix();
        PHMatrix op = gm->projectionMatrix();
        gm->setModelViewMatrix(PHIdentityMatrix);
        gm->setProjectionMatrix(PHIdentityMatrix);
        draw();
        gm->setModelViewMatrix(om);
        gm->setProjectionMatrix(op);
    } else {
        draw();
    }
}

void PHPostProcess::draw()
{
    if (!mat) return;
    gm->setGLStates(PHGLBlending);
    mat->renderVAO(gm->fullScreenVAO(), us);
}
