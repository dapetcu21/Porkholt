//
//  PHGLShaderProgram.h
//  Porkholt
//
//  Created by Marius Petcu on 2/5/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#ifndef PHGLSHADERPROGRAM_H
#define PHGLSHADERPROGRAM_H

#include "PHMain.h"

class PHGLShader;
class PHGameManager;

class PHGLShaderProgram : public PHObject
{
protected:
    GLint identifier;
    PHGLShader * vShader, * fShader;
public:
    PHGLShaderProgram(PHGameManager * gameManager, const string & path);
    PHGLShader * vertexShader() { return vShader; }
    PHGLShader * fragmentShader() { return fShader; }
    bool validate();
    GLint programID() { return identifier; }
    void use() { glUseProgram(identifier); }
};

#endif
