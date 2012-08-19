/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHMATERIAL_H
#define PHMATERIAL_H

#include <Porkholt/Core/PHMain.h>

class PHGLVertexArrayObject;
class PHGLUniformStates;

class PHMaterial : public PHObject
{
    public:
        virtual void renderVAO(PHGLVertexArrayObject * vao, PHGLUniformStates * additionalUniforms = NULL) = 0; 
};

#endif
