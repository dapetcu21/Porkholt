/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/3D/PHMeshBody.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHGLVertexArrayObject.h>
#include <Porkholt/Core/PHGLLight.h>
#include <Porkholt/Core/PHNormalImage.h>

void PHMeshBody::setMesh(PHMesh * m)
{
    if (m)
        m->retain();
    if (_mesh)
        _mesh->release();
    _mesh = m;
}
void PHMeshBody::draw()
{
    if (!_mesh || !mat) return;
    gm->setGLStates(_openGLStates);
    mat->renderVAO(_mesh->vao(), _additionalUniforms);
}

void PHMeshBody::setMaterial(PHMaterial *m)
{
    if (m) m->retain();
    if (mat) mat->release();
    mat = m;
}

PHMeshBody::PHMeshBody() : _mesh(NULL), mat(NULL), _additionalUniforms(NULL)
{
}

PHGLUniformStates * PHMeshBody::additionalUniforms()
{
    if (!_additionalUniforms)
        _additionalUniforms = new PHGLUniformStates();
    return _additionalUniforms;
}
        
PHMeshBody::~PHMeshBody()
{
    if (mat)
        mat->release();
    if (_mesh)
        _mesh->release();
    if (_additionalUniforms)
        _additionalUniforms->release();
}

