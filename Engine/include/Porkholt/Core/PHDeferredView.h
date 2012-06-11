/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHDEFERREDVIEW_H
#define PHDEFERREDVIEW_H

#include <Porkholt/Core/PHView.h>
#include <Porkholt/Core/PHGLLight.h>

class PHShaderProgram;
class PHGLUniformStates;
class PHDeferredView : public PHView
{
protected:
    GLuint colorFBO,normalFBO,colorRB,normalRB,colorTex,normalTex;
    bool inited;
    
    void init();
    void attachedToGameManager();
    
    void render();
    void composite();
    
    PHGLShaderProgram * ambientShader, * diffusePointShader, * specularPointShader, * diffuseNormalPointShader, * specularNormalPointShader;
    PHGLUniformStates * states;
    
    enum shaderUniforms
    {
        diffuseColorUniform = 10,
        specularColorUniform = 11,
        lightPositionUniform = 12,
        lightIntensityUniform = 13,
        clipUniform = 14,
        scaleUniform = 16,
        normalMapUniform = 17
    };
    
    bool speculars,normal;
    
    PHColor ambient, diffuse, specular;
    set<PHGLLight*> pointLights;
    
    ph_float cutoff;
    
    void setCutoffIntensity(ph_float c) { cutoff = c; }
    ph_float cutoffIntensity() { return cutoff; }
    
public:
    
    enum renderModes
    {
        normalMapRenderMode = 10
    };
    
    const PHColor & ambientColor() { return ambient; }
    const PHColor & diffuseColor() { return diffuse; }
    const PHColor & specularColor() { return specular; }
    void setAmbientColor(const PHColor & clr) { ambient = clr; }
    void setDiffuseColor(const PHColor & clr) { diffuse = clr; }
    void setSpecularColor(const PHColor & clr) { specular = clr; }
    
    bool hasSpecular() { return speculars; }
    void setSpecular(bool s) { speculars = s; }
    bool hasNormalMapping() { return normal; }
    void setNormalMapping(bool n) { normal = n; }
    
    void addLight(PHGLLight * light);
    void removeLight(PHGLLight * light);
    
    PHGLLight * addPointLight(const PHVector3 & pos, const PHColor & diffuse, const PHColor & spec, ph_float inten)
    {
        PHGLLight * l = new PHGLLight(PHGLLight::pointLight,pos,diffuse,spec,inten);
        addLight(l);
        l->release();
        return l;
    }
    PHGLLight * addPointLight(const PHVector3 & pos, const PHColor & diffuse, ph_float inten)
    {
        PHGLLight * l = new PHGLLight(PHGLLight::pointLight,pos,diffuse,inten);
        addLight(l);
        l->release();
        return l;
    }
    
    PHDeferredView();
    PHDeferredView(const PHRect & frm);
    ~PHDeferredView();
};

#endif
