//
//  PHGLShader.cpp
//  Porkholt
//
//  Created by Marius Petcu on 2/5/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#include "PHGLShader.h"
#include "PHFileManager.h"

PHGLShader::PHGLShader(const string & header, const string & path, int type)
{
    size_t size;
    const GLchar * data = (const GLchar*)PHFileManager::loadFile(path, size);
    GLint t;
    switch (type) {
        case vertexShader:
            t = GL_VERTEX_SHADER;
            break;
        case fragmentShader:
            t = GL_FRAGMENT_SHADER;
            break;
        default:
            t = -1;
            break;
    }
    identifier = glCreateShader(t);
    const GLchar * strings[2] = { header.c_str(), data };
    GLint sizes[2] = { header.length(), size };
    glShaderSource(identifier, 2, strings, sizes);
    glCompileShader(identifier);
    delete [] data;
    
#ifdef PH_DEBUG
    GLint logLength;
    glGetShaderiv(identifier, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar * log = new GLchar[logLength];
        glGetShaderInfoLog(identifier, logLength, &logLength, log);
        PHLog("GL: Shader compile log: %s",log);
        delete [] log;
    }
#endif
    
    GLint status;
    glGetShaderiv(identifier, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glDeleteShader(identifier);
        throw "OpenGL Shader compilation failed: " + path;
    }
}

PHGLShader::~PHGLShader()
{
    glDeleteShader(identifier);
}

