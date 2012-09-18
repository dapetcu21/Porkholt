/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHGLShader.h>
#include <Porkholt/IO/PHFile.h>

PHGLShader::PHGLShader(const string & header, const string & data, int type)
{
    loadWithData(header, (const uint8_t *)&(data[0]), data.size(), type);
}

PHGLShader::PHGLShader(const string & header, PHFile * file, int type)
{
    size_t s = 0;
    uint8_t * d = file->loadToBuffer(s);
    try {
    loadWithData(header, d, s, type);
    } catch (string ex) {
        delete[] d;
        throw ex;
    }
    delete[] d;
}

PHGLShader::PHGLShader(const string & header, const uint8_t * d, size_t size, int type)
{
    loadWithData(header, d, size, type);
}
    
void PHGLShader::loadWithData(const string & header, const uint8_t * d, size_t size, int type)
{
    const GLchar * data = (const GLchar*)d;
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
    identifier = PHGL::glCreateShader(t);
    const GLchar * strings[2] = { header.c_str(), data };
    GLint sizes[2] = { header.length(), size };
    PHGL::glShaderSource(identifier, 2, strings, sizes);
    PHGL::glCompileShader(identifier);
    
#ifdef PH_DEBUG
    GLint logLength;
    PHGL::glGetShaderiv(identifier, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar * log = new GLchar[logLength];
        PHGL::glGetShaderInfoLog(identifier, logLength, &logLength, log);
        PHLog("GL: Shader compile log: %s",log);
        delete [] log;
    }
#endif
    
    GLint status;
    PHGL::glGetShaderiv(identifier, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        PHGL::glDeleteShader(identifier);
        throw "OpenGL Shader compilation failed";
    }
}

PHGLShader::~PHGLShader()
{
    PHGL::glDeleteShader(identifier);
}

