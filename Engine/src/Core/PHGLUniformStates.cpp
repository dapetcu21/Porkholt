/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHGLUniformStates.h>
#include <Porkholt/Core/PHGLShaderProgram.h>
#include <Porkholt/Core/PHGLTexture.h>
#include <Porkholt/Core/PHNormalImage.h>
#include <Porkholt/Core/PHGameManager.h>

GLint PHGLUniformStates::uniform::location(PHGLShaderProgram * shader)
{
    GLint loc;
    map<PHGLShaderProgram*,int>::iterator i = shaders.find(shader);
    if (i == shaders.end())
    {
        loc = PHGL::glGetUniformLocation(shader->programID(), name.c_str());
        shaders.insert(make_pair<PHGLShaderProgram*,int>(shader,loc));
    } else
        loc = i->second;
    return loc;
}

void PHGLUniformStates::uniform::apply(PHGLShaderProgram * shader, int * tc)
{
    if (!active) return;
    GLint loc = location(shader);
    if (loc==-1) return;
    switch  (type)
    {
    case float3Type:
        PHGL::glUniform3f(loc, floatValue[0], floatValue[1], floatValue[2]);
        break;
    case float4Type:
        PHGL::glUniform4f(loc, floatValue[0], floatValue[1], floatValue[2], floatValue[3]);
        break;
    case matrixType:
        PHGL::glUniformMatrix4fv(loc, 1, GL_FALSE, floatValue);
        break;
    case floatType:
        PHGL::glUniform1f(loc, floatValue[0]);
        break;
    case float2Type:
        PHGL::glUniform2f(loc, floatValue[0], floatValue[1]);
        break;
    case imageType:
    {
        if (img)
        {
            int c = tc?((*tc)++):0;
            img->texture()->bind(c);
            PHGL::glUniform1i(loc, c);
        }
        break;
    }
    case textureType:
    {
        if (tex)
        {
            int c = tc?((*tc)++):0;
            tex->bind(c);
            PHGL::glUniform1i(loc, c);
        }
        break;
    }
    case intType:
        PHGL::glUniform1i(loc, intValue[0]);
        break;
    case int3Type:
        PHGL::glUniform3i(loc, intValue[0], intValue[1], intValue[2]);
        break;
    case int4Type:
        PHGL::glUniform4i(loc, intValue[0], intValue[1], intValue[2], intValue[3]);
        break;
    case int2Type:
        PHGL::glUniform2i(loc, intValue[0], floatValue[1]);
        break;
    };
}

void PHGLUniformStates::dump()
{
    map<string,uniform*>::iterator i;
    PHLog("Uniform %x:", int(size_t(this)));
    for (i = stringMap.begin(); i!=stringMap.end(); i++)
    {
        const string & name = i->first;
        uniform * u = i->second;
        switch  (u->type)
        {
            case float3Type:
                PHLog("%s = vec3(%f, %f, %f)",name.c_str(), u->floatValue[0], u->floatValue[1], u->floatValue[2]);
                break;
            case float4Type:
                PHLog("%s = vec4(%f, %f, %f, %f)",name.c_str(), u->floatValue[0], u->floatValue[1], u->floatValue[2], u->floatValue[3]);
                break;
            case matrixType:
                PHLog("%s = mat4(\n  %f, %f, %f, %f,\n  %f, %f, %f, %f,\n  %f, %f, %f, %f,\n  %f, %f, %f, %f )",name.c_str(),
                    u->floatValue[0],
                    u->floatValue[1],
                    u->floatValue[2],
                    u->floatValue[3],
                    u->floatValue[4],
                    u->floatValue[5],
                    u->floatValue[6],
                    u->floatValue[7],
                    u->floatValue[8],
                    u->floatValue[9],
                    u->floatValue[10],
                    u->floatValue[11],
                    u->floatValue[12],
                    u->floatValue[13],
                    u->floatValue[14],
                    u->floatValue[15]
                );
                break;
            case floatType:
                PHLog("%s = %f",name.c_str(), u->floatValue[0]);
                break;
            case float2Type:
                PHLog("%s = vec2(%f, %f)",name.c_str(), u->floatValue[0], u->floatValue[1]);
                break;
            case intType:
                PHLog("%s = %d",name.c_str(), u->intValue[0]);
                break;
            case int3Type:
                PHLog("%s = vec3i(%d, %d, %d)",name.c_str(), u->intValue[0], u->intValue[1], u->intValue[2]);
                break;
            case int4Type:
                PHLog("%s = vec4i(%d, %d, %d, %d)",name.c_str(), u->intValue[0], u->intValue[1], u->intValue[2], u->intValue[3]);
                break;
            case int2Type:
                PHLog("%s = vec2i(%d, %d)",name.c_str(), u->intValue[0], u->floatValue[1]);
                break;
            case textureType:
                PHLog("%s = texture", name.c_str());
                break;
            case imageType:
                PHLog("%s = image", name.c_str());
                break;
        };

    }
}

void PHGLUniformStates::apply(PHGLShaderProgram * shader, int * tmuCount)
{
    shader->use();
    int t = 0;
    if (!tmuCount)
        tmuCount = &t;
    for (map<string,uniform*>::iterator i = stringMap.begin(); i!=stringMap.end(); i++)
        i->second->apply(shader, tmuCount);
    if (_linked)
        _linked->apply(shader, tmuCount);
}

PHGLUniformStates::uniform & PHGLUniformStates::operator[] (const string & name)
{
    map<string,uniform*>::iterator i = stringMap.find(name);
    if (i != stringMap.end())
        return *(i->second);

    uniform * u = new uniform;
    u->name = name;
    u->type = nullType;
    u->active = true;
    stringMap.insert(make_pair<string,uniform*>(name,u));
    return *u;
}

void PHGLUniformStates::erase(const PHGLUniformStates::uniform & u)
{
    stringMap.erase(u.name);
    delete &u;
}

PHGLUniformStates::PHGLUniformStates() : _linked(NULL) {};
PHGLUniformStates::~PHGLUniformStates()
{
    if (_linked)
        _linked->release();
    for (map<string,uniform*>::iterator i = stringMap.begin(); i!=stringMap.end(); i++)
        delete i->second;
}

void PHGLUniformStates::uniform::setValue(PHNormalImage * i)
{
    if (i)
        i->retain();
    clear();
    type = imageType;
    img = i; 
}

void PHGLUniformStates::uniform::setValue(PHGLTexture * t)
{
    if (t)
        t->retain();
    clear();
    type = textureType;
    tex = t;
}

void PHGLUniformStates::uniform::clear()
{
    if (tex && ((type == textureType) || (type == imageType)))
        tex->release();
    type = nullType;
}

void PHGLUniformStates::setNextStates(PHGLUniformStates * states)
{
    if (states)
        states->retain();
    if (_linked)
        _linked->release();
    _linked = states;
}

PHGLUniformStates::uniform::~uniform()
{
    clear();
}
