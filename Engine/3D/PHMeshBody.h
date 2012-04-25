//
//  PHMeshBody.h
//  3DDemo
//
//  Created by Marius Petcu on 4/14/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#ifndef PHMESHBODY_H
#define PHMESHBODY_H

#include "PHBody.h"
#include "PHMesh.h"
#include "PHGLShaderProgram.h"
#include "PHGLUniformStates.h"
#include "PHMaterial.h"

class PHImage;
class PHNormalImage;
class PHGLTexture2D;

class PHMeshBody : public PHBody
{
private:
    PHMesh * _mesh;
    PHGLUniformStates * uniforms;
    PHGLUniformStates::uniform & modelViewMatrixU, & projectionMatrixU, & diffuseColorU, & specularColorU, &lightPositionU, & ambientColorU, & shininessU, & textureU, & normalMapU, & textureRU, & normalMapRU, & normalMatrixU;
    PHMaterial * mat;
    
    PHGLTexture2D * tex, * nmap;
    PHRect tex_r, nmap_r;
public:
    PHMesh * mesh() { return _mesh; }
    void setMesh(PHMesh * m);
    
    PHMeshBody();
    ~PHMeshBody();
    
    void setMaterial(PHMaterial * m);
    PHMaterial * material() { return mat; }
    
    void setImage(PHNormalImage * img);
    void setImage(PHImage * img);
    
    void setTexture(PHGLTexture2D * tx);
    void setTextureRegion(const PHRect & r) { tex_r = r; }
    void setTexture(PHGLTexture2D * tx, const PHRect & region) { setTexture(tx); setTextureRegion(region); }
    
    void setNormalMap(PHGLTexture2D * tx);
    void setNormalMapRegion(const PHRect & r) { nmap_r = r; }
    void setNormalMap(PHGLTexture2D * tx, const PHRect & region) { setNormalMap(tx); setNormalMapRegion(region); }
    
    PHGLTexture2D * normalMap() { return nmap; }
    PHGLTexture2D * texture() { return tex; }
    PHRect normalMapRegion() { return nmap_r; }
    PHRect textureRegion() { return tex_r; }
protected:
    void draw();
};

#endif
