/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGRipples.h"
#include "IGWorld.h"
#include "IGScripting.h"
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHTextureCanvas.h>
#include <Porkholt/Core/PHPostProcess.h>
#include <Porkholt/Core/PHGLTexture.h>
#include <Porkholt/Core/PHGLUniformStates.h>
#include <Box2D/Box2D.h>
#include <Porkholt/Core/PHTime.h>

IGSCRIPTING_REGISTERCLASS("IGRipples", IGRipples)

IGRipples::IGRipples(IGWorld * w) : IGProp(w), disv(NULL), difv(NULL)
{
    disperse[0] = disperse[1] = displace[0] = displace[1] = displace[2] = NULL;

}

IGRipples::~IGRipples()
{
    if (disperse[0])
        disperse[0]->release();
    if (disperse[1])
        disperse[1]->release();
    if (displace[0])
        displace[0]->release();
    if (displace[1])
        displace[1]->release();
    if (displace[2])
        displace[2]->release();
    if (disv)
        disv->release();
    if (difv)
        difv->release();
    if (distort)
        distort->release();
}

void IGRipples::onFrame()
{
    PHGLTexture2D * aux;
    difv->setColorTexture(disperse[0]);
    aux = difv->colorTexture();
    if (!disperse[0]) 
        aux->retain();
    disperse[0] = aux;

    umask->setValue(disperse[0]);
    uoldmask->setValue(disperse[1]);

    disv->setColorTexture(aux = displace[0]);

    utex->setValue(aux);
    umap->setValue(displace[2]);
    umap2->setValue(displace[1]);


    aux = disperse[0]; 
    disperse[0] = disperse[1];
    disperse[1] = aux;

    aux = displace[0];
    displace[0] = displace[1];
    displace[1] = displace[2];
    displace[2] = aux;

//    PHTime::sleep(0.2);
}

PHDrawable * IGRipples::loadDrawable()
{
    PHGameManager * gm = world->gameManager();
    for (int i = 0; i < 3; i++)
    {
        PHGLTexture2D * t = new PHGLTexture2D(gm);
        t->setWrapS(PHGLTexture::clampToEdge);
        t->setWrapT(PHGLTexture::clampToEdge);
        t->setMinFilter(PHGLTexture::linear);
        t->setMagFilter(PHGLTexture::linear);
        //uint16_t v[4] = {1<<15, 1<<15, 1<<15, 1<<15};
        //uint8_t v[4] = {128, 128, 128, 128};
        //t->setData((uint8_t*)&v, 1, 1, PHGLTexture::R16f);
        displace[i] = t;
    }
    
    disv = new PHPostProcess(gm);
    disv->retain();
    disv->setColorFormat(PHGLTexture::R16f);
    disv->setMaterial(gm->materialNamed("displacement"));
    disv->setOnRenderCallback(PHInvBind(this, IGRipples::onFrame, NULL));

    difv = new PHPostProcess(gm);
    difv->setColorFormat(PHGLTexture::R16f);
    difv->setMaterial(gm->materialNamed("dispersion"));
    PHGLUniformStates * us = difv->additionalUniforms();
    PHGLUniformStates * uv = disv->additionalUniforms();
    umask = &(us->at("mask"));
    uoldmask = &(us->at("oldmask"));
    umap = &(us->at("map"));
    umap2 = &(us->at("map2"));
    utex = &(uv->at("map"));

    us->at("screenW").setValue(gm->screenWidth());
    us->at("screenH").setValue(gm->screenHeight());
    uv->at("screenWInv").setValue(1.0f/gm->screenWidth());
    uv->at("screenHInv").setValue(1.0f/gm->screenHeight());

    disv->addChild(difv);
    difv->setRenderMode(666); 
    difv->addChild(world->view()->newProxy());

    distort = new PHTextureCanvas(gm);
    distort->setColorFormat(PHGLTexture::RGBA8);
    distort->setClearColor(PHColor((float)0xd9/0xff, (float)0x68/0xff, (float)0x68/0xff));
    disv->addChild(distort);
    uv->at("texture").setValue(distort->colorTexture());

    return disv;
}

void IGRipples::addToDistortion(IGObject * o)
{
    PHDrawable * v = o->getDrawable();
    if (v)
        distort->addChild(v);
}

//--- Lua Scripting ---

int IGRipples_addToDistortion(lua_State * L)
{
    IGRipples * r = (IGRipples*) PHLuaThisPointer(L, 1);
    IGRipples * o = (IGRipples*) PHLuaThisPointer(L, 2);
    if (r && o)
        r->addToDistortion(o);
    return 0;
}

void IGRipples::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGRipples");
    
    PHLuaAddMethod(IGRipples, addToDistortion);

    lua_pop(L, 1);
}
