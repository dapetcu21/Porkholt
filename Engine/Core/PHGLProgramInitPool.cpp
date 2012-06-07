/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHGLProgramInitPool.h"
#include "PHGLShaderProgram.h"

map<string,int> PHGLProgramInitPool::mapping;
int PHGLProgramInitPool::nr = 0;

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
        vector<string> ops;
        size_t s = 0;
        while ((s=name.find('[', s)) != string::npos)
        {
            size_t beg = s;
            s = name.find(']', s);
            ops.push_back(name.substr(beg+1, s-beg-1));
        }
        string shn = name.substr(0, name.find('[', 0));
		try {
            shd = new PHGLShaderProgram(gameManager(), shaderDirectory() + shn + ".lua", ops);
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
