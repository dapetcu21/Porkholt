/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHMeshBody.h"
#include "PHGameManager.h"
#include "PHGLVertexArrayObject.h"
#include "PHGLLight.h"
#include "PHNormalImage.h"

void PHMeshBody::setMesh(PHMesh * m)
{
    if (m)
        m->retain();
    if (_mesh)
        _mesh->release();
    _mesh = m;
}

char front_pixel_shading_pnt[] =            "front_pixel_shading[point_light]";
char front_pixel_shading_dir[] =            "front_pixel_shading[directional_light]";
char front_pixel_shading_pnt_tex[] =        "front_pixel_shading[point_light][texture]";
char front_pixel_shading_dir_tex[] =        "front_pixel_shading[directional_light][texture]";
char front_pixel_shading_pnt_nrm[] =        "front_pixel_shading[point_light][texture][normal_map]";
char front_pixel_shading_dir_nrm[] =        "front_pixel_shading[directional_light][texture][normal_map]";
char front_pixel_shading_pnt_spec[] =       "front_pixel_shading[point_light][specular]";
char front_pixel_shading_dir_spec[] =       "front_pixel_shading[directional_light][specular]";
char front_pixel_shading_pnt_tex_spec[] =   "front_pixel_shading[point_light][texture][specular]";
char front_pixel_shading_dir_tex_spec[] =   "front_pixel_shading[directional_light][texture][specular]";
char front_pixel_shading_pnt_nrm_spec[] =   "front_pixel_shading[point_light][texture][normal_map][specular]";
char front_pixel_shading_dir_nrm_spec[] =   "front_pixel_shading[directional_light][texture][normal_map][specular]";
char front_pixel_shading[] =                "front_pixel_shading";
char front_pixel_shading_tex[] =            "front_pixel_shading[texture]";

static PHGLShaderProgram * frontShaderFor(PHGameManager * gm, bool pixelShading, int lightType, bool texture, bool normalMap, bool specular)
{
    if (pixelShading)
    {
        switch (lightType) {
            case PHGLLight::directionalLight:
            {
                if (specular)
                {
                    if (texture)
                    {
                        if (normalMap)
                            return gm->shaderProgramNamed<front_pixel_shading_dir_nrm_spec>();
                        return gm->shaderProgramNamed<front_pixel_shading_dir_tex_spec>();
                    }
                    return gm->shaderProgramNamed<front_pixel_shading_dir_spec>();
                } else {
                    if (texture)
                    {
                        if (normalMap)
                            return gm->shaderProgramNamed<front_pixel_shading_dir_nrm>();
                        return gm->shaderProgramNamed<front_pixel_shading_dir_tex>();
                    }
                    return gm->shaderProgramNamed<front_pixel_shading_dir>();
                } 
            }
            case PHGLLight::pointLight:
            {
                if (specular)
                {
                    if (texture)
                    {
                        if (normalMap)
                            return gm->shaderProgramNamed<front_pixel_shading_pnt_nrm_spec>();
                        return gm->shaderProgramNamed<front_pixel_shading_pnt_tex_spec>();
                    }
                    return gm->shaderProgramNamed<front_pixel_shading_pnt_spec>();
                } else {
                    if (texture)
                    {
                        if (normalMap)
                            return gm->shaderProgramNamed<front_pixel_shading_pnt_nrm>();
                        return gm->shaderProgramNamed<front_pixel_shading_pnt_tex>();
                    }
                    return gm->shaderProgramNamed<front_pixel_shading_pnt>();
                }
            }
            case PHGLLight::ambientLight:
                if (texture)
                    return gm->shaderProgramNamed<front_pixel_shading_tex>();
                return gm->shaderProgramNamed<front_pixel_shading>();
            default:
                return NULL;
        }
    } else {
        return NULL;
    }
}


void PHMeshBody::draw()
{
    PHGLLight * l = gm->currentLight();
    if (!_mesh || !mat) return;
    bool lvalid = l && ((l->type == PHGLLight::pointLight) || (l->type == PHGLLight::directionalLight));

    bool hasSpec = !(!lvalid || mat->specular.isBlack() || l->specular.isBlack());
    bool hasDiff = !(!lvalid || mat->diffuse.isBlack() || l->diffuse.isBlack());
    bool isAmbient = !hasSpec && !hasDiff;
    PHGLShaderProgram * shader = frontShaderFor(gm, true, isAmbient? PHGLLight::ambientLight : l->type, tex!=NULL, nmap!=NULL, hasSpec);
    gm->setGLStates(PHGLZTesting);
    gm->applyShader(shader);
    modelViewMatrixU = gm->modelViewMatrix();
    projectionMatrixU = gm->projectionMatrix();
    if (lvalid)
        normalMatrixU = gm->modelViewMatrix().inverse().transposed();
    ambientColorU = mat->emissive + (gm->ambientColor() * mat->diffuse);
    shininessU = mat->shininess;
    if ((l->type == PHGLLight::pointLight) || (l->type == PHGLLight::directionalLight))
    {
        diffuseColorU = mat->diffuse * l->diffuse * l->intensity;
        specularColorU = mat->specular * l->specular * l->intensity;
        lightPositionU = l->position;
    }
    if (tex)
    {
        tex->bind(0);
        textureRU = PHVector4(tex_r.x, tex_r.y, tex_r.width, tex_r.height);
    }
    if (nmap && lvalid)
    {
        nmap->bind(1);
        normalMapRU = PHVector4(nmap_r.x, nmap_r.y, nmap_r.width, nmap_r.height);
    }
    uniforms->apply(shader);
    _mesh->vao()->draw();
}

void PHMeshBody::setMaterial(PHMaterial *m)
{
    if (m) m->retain();
    if (mat) mat->release();
    mat = m;
}

PHMeshBody::PHMeshBody() : _mesh(NULL), uniforms(new  PHGLUniformStates), 
    modelViewMatrixU(uniforms->at("modelViewMatrix")),
    projectionMatrixU(uniforms->at("projectionMatrix")),
    diffuseColorU(uniforms->at("diffuseColor")),
    specularColorU(uniforms->at("specularColor")),
    lightPositionU(uniforms->at("lightPosition")),
    ambientColorU(uniforms->at("ambientColor")),
    shininessU(uniforms->at("shininess")),
    textureU(uniforms->at("tex")),
    normalMapU(uniforms->at("nMap")),
    textureRU(uniforms->at("texR")),
    normalMapRU(uniforms->at("nMapR")),
    normalMatrixU(uniforms->at("normalMatrix")),
    mat(NULL), tex(NULL), nmap(NULL), tex_r(PHWholeRect), nmap_r(PHWholeRect) 
{
    textureU = 0;
    normalMapU = 1;
}
        
PHMeshBody::~PHMeshBody()
{
    uniforms->release();
}

void PHMeshBody::setImage(PHNormalImage * img)
{
    if (!img) return;
    setTexture(img->texture(), img->textureCoordinates());
    PHNormalImage * nm = dynamic_cast<PHNormalImage*>(img->normalMap());
    if (nm)
        setNormalMap(nm->texture(), nm->textureCoordinates());
}

void PHMeshBody::setImage(PHImage * img)
{
    setImage(dynamic_cast<PHNormalImage*>(img));
}

void PHMeshBody::setTexture(PHGLTexture2D * tx)
{
    if (tx) tx->retain();
    if (tex) tex->release();
    tex = tx;
}


void PHMeshBody::setNormalMap(PHGLTexture2D * tx)
{
    if (tx) tx->retain();
    if (nmap) nmap->release();
    nmap = tx;
}
