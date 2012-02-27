//
//  PHGLProgramInitPool.h
//  Porkholt
//
//  Created by Marius Petcu on 2/5/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#ifndef PHGLPROGRAMINITPOOL_H
#define PHGLPROGRAMINITPOOL_H

#include "PHMain.h"

class PHGLShaderProgram;
class PHGameManager;

class PHGLProgramInitPool
{
public:
    PHGLShaderProgram * shaderProgramNamed(const string & name);
    virtual const string shaderDirectory();
    ~PHGLProgramInitPool();
    virtual PHGameManager * gameManager() = 0;
private:
    map<string,PHGLShaderProgram*> shaders;
};

#endif
