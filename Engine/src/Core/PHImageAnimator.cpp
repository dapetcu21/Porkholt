/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHImageAnimator.h>
#include <Porkholt/Core/PHAnimatedImage.h>
#include <Porkholt/Core/PHAnimatorPool.h>
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/Core/PHImage.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHGLVertexArrayObject.h>
#include <Porkholt/Core/PHGLVertexBufferObject.h>
#include <Porkholt/Core/PHTextureAtlas.h>
#include <Porkholt/Core/PHGLTexture.h>
#include <Porkholt/Core/PHMutex.h>

#define INIT _image(img), running(true)

PHImageAnimator::PHImageAnimator(PHAnimatedImage * img) : PHAnimator(), INIT
{
    reset();
}

PHImageAnimator::PHImageAnimator(PHAnimatedImage * img, PHAnimatorPool * p) : PHAnimator(p), INIT
{
    reset();
}

ph_float PHImageAnimator::timeForFrameInSection(int fr, int sec)
{
    try {
        PHAnimatedImage::section * section = _image->sections.at(sec);
        PHAnimatedImage::frame & frame = section->frames.at(fr);
        if (fr>=int(section->frames.size())-1)
        {
            invocation.call(this);
            invocation.clear();
            return INFINITY;
        }
        if (frame.type == 1)
        {
            ph_float res = 0;
            PHAnimatedImage::luaMutex->lock();
            
            lua_State * L = PHAnimatedImage::L;
            PHLuaGetHardRef(L, _image);
            lua_pushnumber(L, frame.fnumber);
            lua_gettable(L, -2);
            if (lua_isfunction(L,-1))
                PHLuaCall(L, 0, 1);
            if (lua_isnumber(L, -1))
                res = lua_tonumber(L, -1);
            lua_pop(PHAnimatedImage::L, 2);
            
            PHAnimatedImage::luaMutex->unlock();
            return res;
        } else
            return frame.duration;
    }
    catch (...)
    {
        return 0;
    }
}

int PHImageAnimator::realFrame(int fr, int sec)
{
    try {
        return _image->sections.at(sec)->frames.at(fr).frameno;
    } catch (...)
    {
        return  -1;
    }
}

void PHImageAnimator::animateSection(const string & name, const PHInvocation & inv)
{
    animateSection(_image->sectionNo(name),inv);
}


void PHImageAnimator::reset(const PHInvocation & inv)
{
    animateSection(_image->defaultSection,inv);
}

void PHImageAnimator::animateSection(int sect, const PHInvocation & inv)
{
    if (sect<0 || sect>=(int)(_image->sections.size()))
    {
        section = -1;
        frame = -1;
        realframe = -1;
        return;
    }
    
    invocation = inv;
    
    section = sect;
    frame = 0;
    PHAnimatedImage::section * sec = _image->sections.at(section);
    PHAnimatedImage::frame * frm = &(sec->frames.at(frame));
    while (frm->type == 2)
    {
        frame = frm->frameno;
        section = frm->section;
        sec = _image->sections.at(section);
        frm = &(sec->frames.at(frame));
    }
    remaining = time = timeForFrameInSection(frame, section);
    realframe = realFrame(frame,section);
    lastframe = realframe;
    fade = frm->fade;
}

void PHImageAnimator::advanceAnimation(ph_float elapsedTime)
{
    if (section==-1 || frame==-1) return;
    if (!running) return;
    
    remaining-=elapsedTime;
    PHAnimatedImage::section * sec = _image->sections.at(section);
    PHAnimatedImage::frame * frm;
    while (remaining<=0)
    {
        int nframes = (int)sec->frames.size();
        if (!nframes) return;
        frame++;
        frm = &(sec->frames.at(frame));
        while (frm->type == 2)
        {
            frame = frm->frameno;
            section = frm->section;
            sec = _image->sections.at(section);
            frm = &(sec->frames.at(frame));
        }
        time = timeForFrameInSection(frame, section);
        remaining += time;
        lastframe = realframe;
        realframe = realFrame(frame,section);
        fade = frm->fade;
    }
}

PHRect PHImageAnimator::currentFrameTextureCoordinates(const PHRect & port)
{
    return _image->atlas()->textureCoordinates(realframe);
}

PHRect PHImageAnimator::lastFrameTextureCoordinates(const PHRect & port)
{
    return _image->atlas()->textureCoordinates(lastframe);
}

void PHImageAnimator::renderInFramePortionTint(PHGameManager * gm, const PHRect & frm,const PHRect & port,const PHColor & tint)
{
    if (realframe<0) return;
    
    _image->load();
    
    const GLfloat squareVertices[] = {
        frm.x,			frm.y,
        frm.x+frm.width,frm.y,
        frm.x,			frm.y+frm.height,
        frm.x+frm.width,frm.y+frm.height,
    };

    PHColor tt = tint;
    if (tt==PHInvalidColor)
        tt = PHWhiteColor;
    
    if (fade)
    {
        PHRect r = port.portionOf(_image->atlas()->textureCoordinates(lastframe));
        const GLfloat squareTexCoords2[] = {
            r.x             , r.y,
            r.x + r.width   , r.y,
            r.x             , r.y + r.height,
            r.x + r.width   , r.y + r.height,
        };
        
        _image->atlas()->texture(lastframe)->bind(0);
        
        int states = PHGLBlending | PHGLVertexArray | PHGLTextureCoordArray | PHGLTexture0;
        gm->setGLStates(states);
        if (gm->useShaders())
        {
            glVertexAttribPointer(PHIMAGEATTRIBUTE_POS, 2, GL_FLOAT, GL_FALSE, 0, squareVertices);
            glVertexAttribPointer(PHIMAGEATTRIBUTE_TXC, 2, GL_FLOAT, GL_FALSE, 0, squareTexCoords2);
        } else {
            glVertexPointer(2, GL_FLOAT, 0, squareVertices);
            glTexCoordPointer(2, GL_FLOAT, 0, squareTexCoords2);
        }
        gm->setColor(tt.multipliedAlpha(remaining/time));
        gm->applySpriteShader();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
	
    PHRect r = port.portionOf(_image->atlas()->textureCoordinates(realframe));
	const GLfloat squareTexCoords[] = {
            r.x             , r.y,
            r.x + r.width   , r.y,
            r.x             , r.y + r.height,
            r.x + r.width   , r.y + r.height
        };
        

	
    _image->atlas()->texture(realframe)->bind(0);
    
    int states = PHGLBlending | PHGLVertexArray | PHGLTextureCoordArray | PHGLTexture0;
    gm->setGLStates(states);
    if (gm->useShaders())
    {
        glVertexAttribPointer(PHIMAGEATTRIBUTE_POS, 2, GL_FLOAT, GL_FALSE, 0, squareVertices);
        glVertexAttribPointer(PHIMAGEATTRIBUTE_TXC, 2, GL_FLOAT, GL_FALSE, 0, squareTexCoords);
    } else {
        glVertexPointer(2, GL_FLOAT, 0, squareVertices);
        glTexCoordPointer(2, GL_FLOAT, 0, squareTexCoords);
    }
    if (fade)
    {
        tt.a *= 1.0f-(remaining/time);
    }
    gm->setColor(tt);
    gm->applySpriteShader();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void PHImageAnimator::bindCurrentFrameToTexture(int tx)
{
    _image->atlas()->texture(realframe)->bind(tx);
}
void PHImageAnimator::bindLastFrameToTexture(int tx)
{
    _image->atlas()->texture(lastframe)->bind(tx);
}

void PHImageAnimator::rebuildVAOs(PHImageView * imageView, PHGLVertexArrayObject * & vao1, PHGLVertexBufferObject * & vbo1, PHGLVertexArrayObject * & vao2, PHGLVertexBufferObject * & vbo2)
{
    if (!vao1)
        vao1 = new PHGLVertexArrayObject(imageView->gameManager());
    if (!vbo1)
        vbo1 = new PHGLVertexBufferObject(imageView->gameManager());
    if (fade&&!vao2)
        vao2 = new PHGLVertexArrayObject(imageView->gameManager());
    if (fade&&!vbo2)
        vbo2 = new PHGLVertexBufferObject(imageView->gameManager());
    if (!fade&&vbo2)
    {
        vbo2->release();
        vbo2 = NULL;
    }
    if (!fade&&vao2)
    {
        vao2->release();
        vao2 = NULL;
    }

    PHPoint repeat(imageView->repeatX(), imageView->repeatY());
    PHRect port(imageView->textureCoordinates());

    PHImage::buildImageVAO(vao1, vbo1, repeat, port, _image->atlas()->textureCoordinates(realframe));
    
    if (fade)
    {
        PHImage::buildImageVAO(vao2, vbo2, repeat, port, _image->atlas()->textureCoordinates(lastframe));
    }
}
