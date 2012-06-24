/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHGLSHADER_H
#define PHGLSHADER_H

#include <Porkholt/Core/PHMain.h>

class PHFile;
class PHGLShader : public PHObject
{
private:
    GLint identifier;
    void loadWithData(const string & header, const uint8_t * data, size_t size, int type);
public:
    enum types
    {
        vertexShader = 0,
        fragmentShader = 1
    };
    
    PHGLShader(const string & header, const string & data, int type);
    PHGLShader(const string & header, PHFile * file, int type);
    PHGLShader(const string & header, const uint8_t * d, size_t size, int type);
    ~PHGLShader();
    GLint shaderID() { return identifier; }
};

#endif
