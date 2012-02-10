//
//  PHGLUniformStates.cpp
//  Porkholt
//
//  Created by Marius Petcu on 2/5/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#include "PHGLUniformStates.h"
#include "PHGLShaderProgram.h"

GLint PHGLUniformStates::uniform::location(PHGLShaderProgram * shader)
{
    GLint loc;
    map<PHGLShaderProgram*,int>::iterator i = shaders.find(shader);
    if (i == shaders.end())
    {
        loc = glGetUniformLocation(shader->programID(), name.c_str());
        shaders.insert(make_pair<PHGLShaderProgram*,int>(shader,loc));
    } else
        loc = i->second;
    return loc;
}

void PHGLUniformStates::uniform::apply(PHGLShaderProgram * shader)
{
    if (!active) return;
    GLint loc = location(shader);
    if (loc==-1) return;
    switch  (type)
    {
    case float3Type:
        glUniform3f(loc, floatValue[0], floatValue[1], floatValue[2]);
        break;
    case float4Type:
        glUniform4f(loc, floatValue[0], floatValue[1], floatValue[2], floatValue[3]);
        break;
    case matrixType:
        glUniformMatrix4fv(loc, 1, GL_FALSE, floatValue);
        break;
    case floatType:
        glUniform1f(loc, floatValue[0]);
        break;
    case float2Type:
        glUniform2f(loc, floatValue[0], floatValue[1]);
        break;
    case intType:
        glUniform1i(loc, intValue[0]);
        break;
    case int3Type:
        glUniform3i(loc, intValue[0], intValue[1], intValue[2]);
        break;
    case int4Type:
        glUniform4i(loc, intValue[0], intValue[1], intValue[2], intValue[3]);
        break;
    case int2Type:
        glUniform2i(loc, intValue[0], floatValue[1]);
        break;
    };
}

PHGLUniformStates::uniform & PHGLUniformStates::_insert(const string &name, int index)
{
    map<int,uniform*>::iterator i = intMap.find(index);
    if (i != intMap.end())
        erase(*(i->second));
    uniform * u = new uniform;
    u->name = name;
    u->index = index;
    u->type = nullType;
    u->active = true;
    intMap.insert(make_pair<int,uniform*>(index, u));
    stringMap.insert(make_pair<string,uniform*>(name,u));
    return *u;
}

PHGLUniformStates::uniform & PHGLUniformStates::insert(const string &name, int index)
{
    map<string,uniform*>::iterator i = stringMap.find(name);
    if (i != stringMap.end())
        erase(*(i->second));
    return _insert(name,index);
}

void PHGLUniformStates::apply(PHGLShaderProgram * shader)
{
    for (map<int,uniform*>::iterator i = intMap.begin(); i!=intMap.end(); i++)
        i->second->apply(shader);
}

PHGLUniformStates::uniform & PHGLUniformStates::operator[] (const string & name)
{
    map<string,uniform*>::iterator i = stringMap.find(name);
    if (i != stringMap.end())
        return *(i->second);
    return _insert(name, autogen--);
}

PHGLUniformStates::uniform & PHGLUniformStates::operator[] (int index)
{
    return *(intMap[index]);
}

void PHGLUniformStates::erase(const PHGLUniformStates::uniform & u)
{
    intMap.erase(u.index);
    stringMap.erase(u.name);
    delete &u;
}

PHGLUniformStates::PHGLUniformStates() : autogen(-1) {};
PHGLUniformStates::~PHGLUniformStates()
{
    for (map<int,uniform*>::iterator i = intMap.begin(); i!=intMap.end(); i++)
        delete i->second;
}