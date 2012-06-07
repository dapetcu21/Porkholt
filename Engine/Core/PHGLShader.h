/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHGLSHADER_H
#define PHGLSHADER_H

#include "PHMain.h"

class PHGLShader : public PHObject
{
private:
    GLint identifier;
public:
    enum types
    {
        vertexShader = 0,
        fragmentShader = 1
    };
    
    PHGLShader(const string & header, const string & path, int type);
    ~PHGLShader();
    GLint shaderID() { return identifier; }
};

#endif
