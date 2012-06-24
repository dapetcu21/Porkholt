/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHGLProgramInitPool.h>
#include <Porkholt/Core/PHGLShaderProgram.h>
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
        PHFile * file = NULL;
		try {
            file = shaderDirectory()->fileAtPath(shn + ".lua");
            shd = new PHGLShaderProgram(gameManager(), shaderDirectory(), file, ops);
            file->release();
		} catch (string ex)
		{
            if (file)
                file->release();
			PHLog("Could not load shader \"%s\": %s",name.c_str(), ex.c_str());
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
