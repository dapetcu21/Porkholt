/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHGLLIGHT_H
#define PHGLLIGHT_H

#include <Porkholt/Core/PHMain.h>

class PHGLLight : public PHObject
{
public:
    enum lightType {
        ambientLight,
        pointLight,
        directionalLight
    } type;
    PHVector3 position; //if directionalLight, then this must be normalized and it represents direction
    PHColor diffuse;
    PHColor specular;
    ph_float intensity;
    
    PHGLLight(enum lightType tp, const PHVector3 & pos, const PHColor & dif, const PHColor & spec, ph_float inten) : type(tp), position(pos), diffuse(dif), specular(spec), intensity(inten) {}
    PHGLLight(enum lightType tp, const PHVector3 & pos, const PHColor & dif, ph_float inten) : type(tp), position(pos), diffuse(dif), specular(PHWhiteColor), intensity(inten) {}
    PHGLLight(enum lightType tp, const PHVector3 & pos, ph_float inten) : type(tp), position(pos), diffuse(PHWhiteColor), specular(PHWhiteColor), intensity(inten) {}
};

#endif
