/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHGLPROGRAMINITPOOL_H
#define PHGLPROGRAMINITPOOL_H

#include <Porkholt/Core/PHMain.h>

class PHGLShaderProgram;
class PHDirectory;
class PHGameManager;

class PHGLProgramInitPool
{
public:
    PHGLShaderProgram * shaderProgramNamed(const string & name);
    virtual PHDirectory * shaderDirectory() = 0;
    ~PHGLProgramInitPool();
    virtual PHGameManager * gameManager() = 0;
    
    template <const char * S>
    PHGLShaderProgram * shaderProgramNamed() {
        static int pos = -1;
        if (pos==-1)
        {
            string s(S);
            map<string,int>::iterator i = mapping.find(s);
            if (i == mapping.end())
            {
                mapping[s] = pos = nr++;
            } else
                pos = i->second;
        }
        if (someShaders.size()<=pos)
            someShaders.resize(pos+1);
        if (!someShaders[pos])
            someShaders[pos] = shaderProgramNamed(string(S));
        return someShaders[pos];
    }
    
private:
    map<string,PHGLShaderProgram*> shaders;
    vector<PHGLShaderProgram*> someShaders;
    static map<string,int> mapping;
    static int nr;
};

#endif
