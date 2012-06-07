/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHMATERIAL_H
#define PHMATERIAL_H

#include "PHMain.h"

class PHMaterial : public PHObject
{
public:
    PHColor diffuse, specular, emissive;
    ph_float shininess;
    PHMaterial() : diffuse(PHWhiteColor), specular(PHWhiteColor), emissive(PHClearColor), shininess(0) {}
    PHMaterial(const PHColor & d) : diffuse(d), specular(PHWhiteColor), emissive(PHClearColor), shininess(0) {}
    PHMaterial(const PHColor & d, const PHColor & s, ph_float shine) : diffuse(d), specular(s), emissive(PHClearColor), shininess(shine) {}
    PHMaterial(const PHColor & d, const PHColor & s, ph_float shine, const PHColor & e) : diffuse(d), specular(s), emissive(e), shininess(shine) {}
};

#endif
