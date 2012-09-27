/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHGLShaderProgram.h>
#include <Porkholt/Core/PHGLShader.h>
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHAutoreleasePool.h>
#include <Porkholt/IO/PHDirectory.h>
#include <Porkholt/IO/PHFile.h>

PHGLShaderProgram::PHGLShaderProgram(PHGameManager * gameManager, PHDirectory * shdDir, PHFile * file, const vector<string> & ops)
{
    init(gameManager, shdDir, file, &ops);
}

PHGLShaderProgram::PHGLShaderProgram(PHGameManager * gm, PHDirectory * shdDir, PHFile * file)
{
    init(gm, shdDir, file, NULL);
}

void PHGLShaderProgram::init(PHGameManager * gm, PHDirectory * shdDir, PHFile * file, const vector<string> * ops)
{
    PHAutoreleasePool ap;

    this->gm = gm;
    lua_State * L = lua_open();
    if (ops)
    {
        for (vector<string>::const_iterator i = ops->begin(); i!= ops->end(); i++)
        {
            lua_pushboolean(L, 1);
            lua_setglobal(L, i->c_str());
        }
    }
    PHLuaLoadFile(L, file);
 
    string nm = file->name();
    string pname = file->basename();
    string vshader = pname+".vsh";
    string fshader = pname+".fsh";
    
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    PHLuaGetStringField(vshader, "vertexShader");
    PHLuaGetStringField(fshader, "fragmentShader");
    lua_pop(L, 1);
    
    string * header = &(gm->glslHeader);
    string s;
    if (ops)
    {
        s = gm->glslHeader;
        for (vector<string>::const_iterator i = ops->begin(); i!= ops->end(); i++)
            s += "#define " + (*i) + "\n";
        header = &s;
    }
    
    vShader = fShader = NULL;
    try {
        PHFile * vfile = shdDir->fileAtPath(vshader); 
        PHFile * ffile = shdDir->fileAtPath(fshader); 
        vShader = new PHGLShader(*header, vfile, PHGLShader::vertexShader);
        fShader = new PHGLShader(*header, ffile, PHGLShader::fragmentShader);
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
    
    identifier = PHGL::glCreateProgram();
    PHGL::glAttachShader(identifier, vShader->shaderID());
    PHGL::glAttachShader(identifier, fShader->shaderID());
    
    lua_getglobal(L,"attributeBindings");
    if (lua_istable(L, -1))
    {
        lua_pushnil(L);
        while (lua_next(L, -2) != 0)
        {
            if (lua_isstring(L, -2) && lua_isnumber(L, -1))
                PHGL::glBindAttribLocation(identifier, (GLint)lua_tonumber(L, -1), lua_tostring(L, -2));
            lua_pop(L,1);
        }
    }
    lua_pop(L, 1);
    lua_close(L);
    
    PHGL::glLinkProgram(identifier);
#ifdef PH_DEBUG
    GLint logLength;
    PHGL::glGetProgramiv(identifier, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar * log = new GLchar[logLength];
        PHGL::glGetProgramInfoLog(identifier, logLength, &logLength, log);
        PHLog("GL: Program link log: %s",log);
        delete [] log;
    }
#endif
    
    PHGL::glDetachShader(identifier, vShader->shaderID());
    PHGL::glDetachShader(identifier, fShader->shaderID());
    vShader->release();
    fShader->release();
    
    GLint status;
    PHGL::glGetProgramiv(identifier, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        PHGL::glDeleteProgram(identifier);
        throw "OpenGL Shader program linking failed: " + nm;
    }

#ifdef PH_DEBUG
    PHLog("Shader program \"%s\" compiled and linked", pname.c_str());
#endif
}

bool PHGLShaderProgram::validate()
{
    PHGL::glValidateProgram(identifier);
    
    GLint logLength;
    PHGL::glGetProgramiv(identifier, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar * log = new GLchar[logLength];
        PHGL::glGetProgramInfoLog(identifier, logLength, &logLength, log);
        PHLog("GL: Program validate log: %s",log);
        delete [] log;
    }
    
    GLint status;
    PHGL::glGetProgramiv(identifier, GL_VALIDATE_STATUS, &status);
    return (status==GL_TRUE);
}

void PHGLShaderProgram::use()
{ 
    gm->useShader(this); 
}
