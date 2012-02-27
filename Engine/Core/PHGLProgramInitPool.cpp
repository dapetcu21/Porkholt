//
//  PHGLProgramInitPool.cpp
//  Porkholt
//
//  Created by Marius Petcu on 2/5/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#include "PHGLProgramInitPool.h"
#include "PHGLShaderProgram.h"

const string PHGLProgramInitPool::shaderDirectory()
{
    return "./rsrc/shaders/";
}

PHGLShaderProgram* PHGLProgramInitPool::shaderProgramNamed(const string & name)
{
    PHGLShaderProgram * shd;
    map<string,PHGLShaderProgram*>::iterator i = shaders.find(name);
	if (i==shaders.end())
	{
		try {
            shd = new PHGLShaderProgram(gameManager(), shaderDirectory() + name + ".lua");
		} catch (string ex)
		{
			PHLog(ex);
			return NULL;
		}
		shaders[name] = shd;
	} else {
		shd = i->second;
	}
	return shd;
}


PHGLProgramInitPool::~PHGLProgramInitPool()
{
    for (map<string,PHGLShaderProgram*>::iterator i = shaders.begin(); i!=shaders.end(); i++)
        i->second->release();
}