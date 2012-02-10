//
//  PHGLShader.h
//  Porkholt
//
//  Created by Marius Petcu on 2/5/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

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
    
    PHGLShader(const string & path, int type);
    ~PHGLShader();
    GLint shaderID() { return identifier; }
};

#endif