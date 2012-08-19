/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHDEFERREDVIEW_H
#define PHDEFERREDVIEW_H

#include <Porkholt/Core/PHView.h>
#include <Porkholt/Core/PHGLLight.h>

class PHShaderProgram;
class PHGLUniformStates;
class PHGLFramebuffer;

#undef uniformtrick
#ifdef PHGLUNIFORMSTATES_H
#define uniformtrick PHGLUniformStates::uniform
#else
#define uniformtrick void
#endif

class PHDeferredView : public PHView
{
protected:
    PHGLFramebuffer * colorFBO, * normalFBO;
    bool inited;
    
    void init();
    void attachedToGameManager();
    
    void render();
    void composite();
    
    PHGLShaderProgram * ambientShader, * diffusePointShader, * specularPointShader, * diffuseNormalPointShader, * specularNormalPointShader;
    PHGLUniformStates * states;
    
    uniformtrick * diffuseColorUniform;
    uniformtrick * specularColorUniform;
    uniformtrick * lightPositionUniform;
    uniformtrick * lightIntensityUniform;
    uniformtrick * clipUniform;
    uniformtrick * scaleUniform;
    uniformtrick * normalMapUniform;
    uniformtrick * matrixUniform;
    uniformtrick * colorUniform;
    uniformtrick * textureUniform;
    
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
