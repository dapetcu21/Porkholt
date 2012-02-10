//
//  PHGLLight.h
//  Porkholt
//
//  Created by Marius Petcu on 2/8/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#ifndef PHGLLIGHT_H
#define PHGLLIGHT_H

#include "PHMain.h"

class PHGLLight : public PHObject
{
    
};

class PHGLPointLight : public PHGLLight
{
public:
    PHVector3 position;
    PHColor diffuse;
    PHColor specular;
    ph_float intensity;
    
    PHGLPointLight(const PHVector3 & pos, const PHColor & dif, const PHColor & spec, ph_float inten) : position(pos), diffuse(dif), specular(spec), intensity(inten) {}
    PHGLPointLight(const PHVector3 & pos, const PHColor & dif, ph_float inten) : position(pos), diffuse(dif), specular(PHWhiteColor), intensity(inten) {}
};

#endif
