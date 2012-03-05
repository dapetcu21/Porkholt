//
//  PHGLShaderProgram.cpp
//  Porkholt
//
//  Created by Marius Petcu on 2/5/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#include "PHGLShaderProgram.h"
#include "PHGLShader.h"
#include "PHLua.h"
#include "PHGameManager.h"

PHGLShaderProgram::PHGLShaderProgram(PHGameManager * gm, const string & path)
{
    lua_State * L = lua_open();
    PHLuaLoadFile(L, path);
 
    int n = path.size();
    int i,j;
    for (i = n-1; i>=0; i--)
        if (path[i]=='/')
            break;
    i++;
    for (j=i; j<n; j++)
        if (path[j]=='.')
            break;
    
    string dir = path.substr(0,i);
    string pname = path.substr(i,j-i);
    string vshader = pname+".vsh";
    string fshader = pname+".fsh";
    
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    PHLuaGetStringField(vshader, "vertexShader");
    PHLuaGetStringField(fshader, "fragmentShader");
    lua_pop(L, 1);
    
    vShader = fShader = NULL;
    try {
        vShader = new PHGLShader(gm->glslHeader, dir+vshader,PHGLShader::vertexShader);
        fShader = new PHGLShader(gm->glslHeader, dir+fshader,PHGLShader::fragmentShader);
    }
    catch (string ex)
    {
        if (vShader)
            vShader->release();
        if (fShader)
            fShader->release();
        lua_close(L);
        throw ex;
    }
    
    identifier = glCreateProgram();
    glAttachShader(identifier, vShader->shaderID());
    glAttachShader(identifier, fShader->shaderID());
    
    lua_getglobal(L,"attributeBindings");
    if (lua_istable(L, -1))
    {
        lua_pushnil(L);
        while (lua_next(L, -2) != 0)
        {
            if (lua_isstring(L, -2) && lua_isnumber(L, -1))
                glBindAttribLocation(identifier, (GLint)lua_tonumber(L, -1), lua_tostring(L, -2));
            lua_pop(L,1);
        }
    }
    lua_pop(L, 1);
    lua_close(L);
    
    glLinkProgram(identifier);
#ifdef PH_DEBUG
    GLint logLength;
    glGetProgramiv(identifier, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar * log = new GLchar[logLength];
        glGetProgramInfoLog(identifier, logLength, &logLength, log);
        PHLog("GL: Program link log: %s",log);
        delete [] log;
    }
#endif
    
    glDetachShader(identifier, vShader->shaderID());
    glDetachShader(identifier, fShader->shaderID());
    vShader->release();
    fShader->release();
    
    GLint status;
    glGetProgramiv(identifier, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        glDeleteProgram(identifier);
        throw "OpenGL Shader program linking failed: " + path;
    }

#ifdef PH_DEBUG
    if (!validate())
    {
        glDeleteProgram(identifier);
        throw "OpenGL Shader program validation failed: " + path;
    }
#endif
    
#ifdef PH_DEBUG
    PHLog("Shader program \"%s\" compiled and linked",pname.c_str());
#endif
}

bool PHGLShaderProgram::validate()
{
    glValidateProgram(identifier);
    
    GLint logLength;
    glGetProgramiv(identifier, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar * log = new GLchar[logLength];
        glGetProgramInfoLog(identifier, logLength, &logLength, log);
        PHLog("GL: Program validate log: %s",log);
        delete [] log;
    }
    
    GLint status;
    glGetProgramiv(identifier, GL_VALIDATE_STATUS, &status);
    return (status==GL_TRUE);
}
