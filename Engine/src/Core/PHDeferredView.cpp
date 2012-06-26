/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHDeferredView.h>
#include <Porkholt/Core/PHGLUniformStates.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHGLShaderProgram.h>
#include <Porkholt/Core/PHGLVertexArrayObject.h>
#include <Porkholt/Core/PHGLTexture.h>
#include <Porkholt/Core/PHGLFramebuffer.h>
#include <assert.h>

#define INIT1 inited(false), speculars(false), normal(false), ambient(PHColor(0.1,0.1,0.1)), diffuse(PHWhiteColor), specular(PHWhiteColor)
#ifdef PH_MOBILE
#define INIT INIT1, cutoff(0.1)
#else
#define INIT INIT1, cutoff(0)
#endif

PHDeferredView::PHDeferredView() : PHView(), INIT 
{
}

PHDeferredView::PHDeferredView(const PHRect & r) : PHView(r), INIT
{
}

void PHDeferredView::attachedToGameManager()
{
    PHView::attachedToGameManager();
    if (!gm->useShaders())
        return;
    if (inited) return;
    inited = true;
    
    ambientShader = gm->shaderProgramNamed("deferred_ambient");
    diffusePointShader = gm->shaderProgramNamed("deferred_diffuse_point");
    diffuseNormalPointShader = gm->shaderProgramNamed("deferred_diffuse_normal_point");
    states = new PHGLUniformStates;
    PHRect r = gm->screenBounds();
    
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
    
    colorFBO = new PHGLFramebuffer(gm);
    PHGLTexture2D * tex = new PHGLTexture2D(gm);
    tex->setWrapS(PHGLTexture::clampToEdge);
    tex->setWrapT(PHGLTexture::clampToEdge);
    tex->setMinFilter(PHGLTexture::nearest);
    tex->setMagFilter(PHGLTexture::nearest);
    tex->setData(NULL, r.width, r.height, PHGLTexture::RGBA8);
    colorFBO->attachColor(tex, 0);
    tex->release();

    if (normal)
    {
        normalFBO = new PHGLFramebuffer(gm);
        tex = new PHGLTexture2D(gm);
        tex->setWrapS(PHGLTexture::clampToEdge);
        tex->setWrapT(PHGLTexture::clampToEdge);
        tex->setMinFilter(PHGLTexture::nearest);
        tex->setMagFilter(PHGLTexture::nearest);
        tex->setData(NULL, r.width, r.height, PHGLTexture::RGBA8);
        normalFBO->attachColor(tex, 0);
        tex->release();
    }
}

PHDeferredView::~PHDeferredView()
{
    if (inited)
    {
        colorFBO->release();
        if (normal)
            normalFBO->release();
        
        states->release();
    }
}

void PHDeferredView::render()
{
    if (inited)
    {
        colorFBO->bind();
        gm->setClearColor(PHClearColor);
        gm->clearBuffers(PHGameManager::colorBuffers);
    }
    PHView::render();
    if (inited && normal)
    {
        normalFBO->bind();
        gm->setClearColor(PHColor(0.5f, 0.5f, 1.0f, 1.0f));
        gm->clearBuffers(PHGameManager::colorBuffers);
        gm->setRenderMode(normalMapRenderMode);
        PHView::render();
        gm->setRenderMode(PHGameManager::defaultRenderMode);
    }
    composite();
}

void PHDeferredView::composite()
{
    if (!inited) return;
    gm->bindFramebuffer(NULL);
    
    PHRect v = gm->screenBounds();
    
    gm->setGLStates(PHGLBlending | PHGLTexture0);
    static_cast<PHGLTexture2D*>(colorFBO->colorAttachment(0))->bind(0);
    if (normal)
        static_cast<PHGLTexture2D*>(normalFBO->colorAttachment(0))->bind(1);
    
	glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
    PHGLVertexArrayObject * vao = gm->solidSquareVAO();
    vao->bind();
    
    PHVector2 scale(v.width,v.height);
    
    if (ambient!=PHBlackColor)
    {
        gm->useShader(ambientShader);
        (states->at(PHGameManager::modelViewSpriteUniform) = gm->projectionMatrix()).apply(ambientShader);
        states->at(PHGameManager::textureSpriteUniform).apply(ambientShader);
        (states->at(PHGameManager::colorSpriteUniform) = ambient).apply(ambientShader);
        (states->at(scaleUniform) = scale).apply(ambientShader);
        vao->draw();
    }
    if (!pointLights.empty())
    {
        PHGLShaderProgram * shader = speculars?(normal?specularNormalPointShader:specularPointShader):(normal?diffuseNormalPointShader:diffusePointShader);
        gm->useShader(shader);
        (states->at(PHGameManager::modelViewSpriteUniform) = gm->projectionMatrix()).apply(shader);
        states->at(PHGameManager::textureSpriteUniform).apply(shader);
        (states->at(scaleUniform) = scale).apply(shader);
        if (normal)
            states->at(normalMapUniform).apply(shader);
        
        for (set<PHGLLight*>::iterator i = pointLights.begin(); i!= pointLights.end(); i++)
        {
            PHGLLight * p =*i;
            PH3DPoint pnt = p->position;
            ph_float z = pnt.z;
            ph_float z_2 = z*z;
            if (z<=0) continue;
            ph_float it = p->intensity;
            if (cutoff)
            {
                ph_float rad2D = sqrt(pow(z*it*it/cutoff,2.0f/3.0f)-z_2);
                
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
            } else 
                (states->at(clipUniform) = PHWholeRect).apply(shader);
            
            
            (states->at(lightPositionUniform) = p->position).apply(shader);
            (states->at(lightIntensityUniform) = (normal?(it*it):(z*it*it))).apply(shader);
            (states->at(diffuseColorUniform) = (p->diffuse * diffuse)).apply(shader);
            if (speculars)
                (states->at(specularColorUniform) = (p->specular * specular)).apply(shader);
            
            vao->draw();
        }
    }
    
    vao->unbind();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void PHDeferredView::addLight(PHGLLight * l)
{
    if (l->type == PHGLLight::pointLight)
    {
        if (pointLights.insert(l).second)
            l->retain();
    }
}

void PHDeferredView::removeLight(PHGLLight * l)
{
    if (l->type == PHGLLight::pointLight)
    {
        if (pointLights.erase(l))
            l->release();
    }
}
