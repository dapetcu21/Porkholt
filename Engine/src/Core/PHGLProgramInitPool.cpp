/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHGLProgramInitPool.h>
#include <Porkholt/Core/PHGLShaderProgram.h>
#include <Porkholt/Core/PHAutoreleasePool.h>
#include <Porkholt/IO/PHDirectory.h>
#include <Porkholt/IO/PHFile.h>

map<string,int> PHGLProgramInitPool::mapping;
int PHGLProgramInitPool::nr = 0;

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
        PHAutoreleasePool ap;
		try {
            PHFile * file = shaderDirectory()->fileAtPath(shn + ".lua");
            shd = new PHGLShaderProgram(gameManager(), shaderDirectory(), file, ops);
		} catch (string ex)
		{
			PHLog("Could not load shader \"%s\": %s",name.c_str(), ex.c_str());
			return NULL;
		}
		shaders[name] = shd;
	} else {
		shd = i->second;
	}
	return shd;
}

void PHGLProgramInitPool::collectGarbageShaderPrograms()
{
	map<string, PHGLShaderProgram*>::iterator i;
	map<string, PHGLShaderProgram*> tmp;
	for (i = shaders.begin(); i!=shaders.end(); i++)
	{
		if ((i->second)->referenceCount()>=2)
			tmp[i->first] = i->second;
		else
			i->second->release();
	}
	shaders = tmp;
    someShaders.clear();
}

PHGLProgramInitPool::~PHGLProgramInitPool()
{
    PHLog("Deallocating shader programs");
    someShaders.clear();
    for (map<string,PHGLShaderProgram*>::iterator i = shaders.begin(); i!=shaders.end(); i++)
        i->second->release();
}
