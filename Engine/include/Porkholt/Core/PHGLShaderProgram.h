/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHGLSHADERPROGRAM_H
#define PHGLSHADERPROGRAM_H

#include <Porkholt/Core/PHMain.h>

class PHGLShader;
class PHGameManager;
class PHFile;
class PHDirectory;

class PHGLShaderProgram : public PHObject
{
protected:
    GLint identifier;
    PHGLShader * vShader, * fShader;
    PHGameManager * gm;
    
    void init(PHGameManager * gm, PHDirectory * shdDir, PHFile * file, const vector<string> * ops);

    void _use() { PHGL::glUseProgram(identifier); }

    friend class PHGameManager;
public:
    PHGLShaderProgram(PHGameManager * gameManager, PHDirectory * shdDir, PHFile * file);
    PHGLShaderProgram(PHGameManager * gameManager, PHDirectory * shdDir, PHFile * file, const vector<string> & ops);
    PHGLShader * vertexShader() { return vShader; }
    PHGLShader * fragmentShader() { return fShader; }
    bool validate();
    GLint programID() { return identifier; }
    
    void use();
    PHGameManager * gameManager() { return gm; }
};

#endif
