/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHMESHBODY_H
#define PHMESHBODY_H

#include <Porkholt/3D/PHBody.h>
#include <Porkholt/3D/PHMesh.h>
#include <Porkholt/Core/PHGLShaderProgram.h>
#include <Porkholt/Core/PHGLUniformStates.h>
#include <Porkholt/Core/PHMaterial.h>

class PHImage;
class PHNormalImage;
class PHGLTexture2D;

class PHMeshBody : public PHBody
{
private:
    PHMesh * _mesh;
    PHMaterial * mat;
    
public:
    PHMeshBody();
    ~PHMeshBody();
 
    PHMesh * mesh() { return _mesh; }
    void setMesh(PHMesh * m);
   
    void setMaterial(PHMaterial * m);
    PHMaterial * material() { return mat; }
protected:
    void draw();
};

#endif
