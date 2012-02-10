//
//  PHDeferredView.cpp
//  Porkholt
//
//  Created by Marius Petcu on 2/8/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#include "PHDeferredView.h"
#include "PHGLUniformStates.h"
#include "PHGameManager.h"
#include "PHGLShaderProgram.h"
#include <assert.h>

#define INIT inited(false), ambient(PHColor(0.1,0.1,0.1)), diffuse(PHWhiteColor), specular(PHWhiteColor), speculars(false), normal(false)

PHDeferredView::PHDeferredView() : PHView(), INIT 
{
}

PHDeferredView::PHDeferredView(const PHRect & r) : PHView(r), INIT
{
}

void PHDeferredView::init()
{
    if (!_gameManager->useShaders())
        return;
    if (inited) return;
    inited = true;
    
    ambientShader = _gameManager->shaderProgramNamed("deferred_ambient");
    diffusePointShader = _gameManager->shaderProgramNamed("deferred_diffuse_point");
    diffuseNormalPointShader = _gameManager->shaderProgramNamed("deferred_diffuse_normal_point");
    states = new PHGLUniformStates;
    PHRect r = _gameManager->screenBounds();
    
    states->insert("modelViewProjectionMatrix", PHGameManager::modelViewSpriteUniform);
    states->insert("color", PHGameManager::colorSpriteUniform);
    states->insert("texture",PHGameManager::textureSpriteUniform) = 0;
    states->insert("normalMap",normalMapUniform) = 1;
    states->insert("diffuseColor",diffuseColorUniform);
    states->insert("specularColor",specularColorUniform);
    states->insert("lightPosition",lightPositionUniform);
    states->insert("lightIntensity",lightIntensityUniform);
    states->insert("clip",clipUniform);
    states->insert("scale",scaleUniform);
    
    glGenFramebuffers(1, &colorFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, colorFBO);
    glGenTextures(1, &colorTex);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, r.width, r.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER)==GL_FRAMEBUFFER_COMPLETE);
    
    if (normal)
    {
        glGenFramebuffers(1, &normalFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, normalFBO);
        glGenTextures(1, &normalTex);
        glBindTexture(GL_TEXTURE_2D, normalTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, r.width, r.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalTex, 0);
        assert(glCheckFramebufferStatus(GL_FRAMEBUFFER)==GL_FRAMEBUFFER_COMPLETE);
    }
        
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

PHDeferredView::~PHDeferredView()
{
    if (inited)
    {
        glDeleteTextures(1, &colorTex);
        glDeleteFramebuffers(1, &colorFBO);
        if (normal)
        {
            glDeleteTextures(1, &normalTex);
            glDeleteFramebuffers(1, &normalFBO);
        }
        
        states->release();
    }
}

void PHDeferredView::setGameManager(PHGameManager * gm)
{
    PHView::setGameManager(gm);
    if (gm)
        init();
}

void PHDeferredView::render()
{
    if (inited)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, colorFBO);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    PHView::render();
    if (inited && normal)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, normalFBO);
        glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        _gameManager->setRenderMode(normalMapRenderMode);
        PHView::render();
        _gameManager->setRenderMode(PHGameManager::defaultRenderMode);
    }
    composite();
}

void PHDeferredView::composite()
{
    if (!inited) return;
    glBindFramebuffer(GL_FRAMEBUFFER, _gameManager->defaultFBO());
    
    PHRect v = _gameManager->screenBounds();
    
	const GLfloat attributes[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f
    };

    PHGLSetStates(PHGLVertexArray | PHGLTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    if (normal)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTex);
    }
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, attributes);
    
	glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
    
    PHVector2 scale(v.width,v.height);
    
    if (ambient!=PHBlackColor)
    {
        _gameManager->useShader(ambientShader);
        (states->at(PHGameManager::modelViewSpriteUniform) = _gameManager->projectionMatrix()).apply(ambientShader);
        states->at(PHGameManager::textureSpriteUniform).apply(ambientShader);
        (states->at(PHGameManager::colorSpriteUniform) = ambient).apply(ambientShader);
        (states->at(scaleUniform) = scale).apply(ambientShader);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    if (!pointLights.empty())
    {
        PHGLShaderProgram * shader = speculars?(normal?specularNormalPointShader:specularPointShader):(normal?diffuseNormalPointShader:diffusePointShader);
        _gameManager->useShader(shader);
        (states->at(PHGameManager::modelViewSpriteUniform) = _gameManager->projectionMatrix()).apply(shader);
        states->at(PHGameManager::textureSpriteUniform).apply(shader);
        (states->at(scaleUniform) = scale).apply(shader);
        if (normal)
            states->at(normalMapUniform).apply(shader);
        
        for (set<PHGLPointLight*>::iterator i = pointLights.begin(); i!= pointLights.end(); i++)
        {
            PHGLPointLight * p =*i;
            PH3DPoint pnt = p->position;
            ph_float z = pnt.z;
            ph_float z_2 = z*z;
            if (z<=0) continue;
            ph_float it = p->intensity;
//            static const ph_float alpha = 0.3f;
//            static const ph_float beta = sqrt( 1/sqrt(alpha*alpha*alpha) - 1.0f );
//            ph_float rad2D = z * beta;
            static const ph_float L = 0.1f;
            ph_float rad2D = sqrt(pow(z*it*it/L,2.0f/3.0f)-z_2);
            
            PHRect portion((pnt.x-rad2D)/v.width,(pnt.y-rad2D)/v.height,(rad2D*2)/v.width,(rad2D*2)/v.height);
            
            if (portion.x<0) 
            { 
                portion.width+=portion.x; 
                portion.x = 0;
            }
            
            if (portion.y<0) 
            { 
                portion.height+=portion.y;
                portion.y = 0;
            }
            
            if (portion.x+portion.width>1) 
                portion.width = 1-portion.x;
            if (portion.y+portion.height>1) 
                portion.height = 1-portion.y;
            
            if (portion.width<=0 || portion.height<=0)
                continue;
            
            (states->at(clipUniform) = portion).apply(shader);
            
            
            (states->at(lightPositionUniform) = p->position).apply(shader);
            (states->at(lightIntensityUniform) = (normal?(it*it):(z*it*it))).apply(shader);
            (states->at(diffuseColorUniform) = (p->diffuse * diffuse)).apply(shader);
            if (speculars)
                (states->at(specularColorUniform) = (p->specular * specular)).apply(shader);
            
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void PHDeferredView::addLight(PHGLLight * light)
{
    PHGLPointLight * pl = dynamic_cast<PHGLPointLight*>(light);
    if (pl)
    {
        if (pointLights.insert(pl).second)
            pl->retain();
    }
}

void PHDeferredView::removeLight(PHGLLight * light)
{
    PHGLPointLight * pl = dynamic_cast<PHGLPointLight*>(light);
    if (pl)
    {
        if (pointLights.erase(pl))
            pl->release();
    }
}