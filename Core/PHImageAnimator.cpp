//
//  PHImageAnimator.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/13/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHImageAnimator.h"
#include "PHAnimatedImage.h"
#include "PHAnimatorPool.h"
#include "PHLua.h"


#define INIT _image(img), advanceManually(false), target(NULL), callback(NULL), userdata(NULL), running(true)

PHImageAnimator::PHImageAnimator(PHAnimatedImage * img) : pool(PHAnimatorPool::mainAnimatorPool()), INIT
{
    reset();
    if (pool)
        pool->insertAnimator(this);
}

PHImageAnimator::PHImageAnimator(PHAnimatedImage * img, PHAnimatorPool * p) : pool(p), INIT
{
    reset();
    if (pool)
        pool->insertAnimator(this);
}

void PHImageAnimator::setAnimatorPool(PHAnimatorPool * p)
{
    if (pool)
        pool->removeAnimator(this);
    pool = p;
    if (pool)
        pool->insertAnimator(this);
}

PHImageAnimator::~PHImageAnimator()
{
    pool->removeAnimator(this);
}

double PHImageAnimator::timeForFrameInSection(int fr, int sec)
{
    try {
        PHAnimatedImage::section * section = _image->sections.at(sec);
        PHAnimatedImage::frame & frame = section->frames.at(fr);
        if (fr>=section->frames.size()-1)
        {
            if (target && callback)
            {
                (target->*callback)(this,userdata);
                target = NULL;
                callback = NULL;
                userdata = NULL;
            }
            return INFINITY;
        }
        if (frame.type == 1)
        {
            double res = 0;
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

void PHImageAnimator::animateSection(const string & name, PHObject * target, PHCallback callback, void * userdata)
{
    animateSection(_image->sectionNo(name),target,callback,userdata);
}


void PHImageAnimator::reset(PHObject * target, PHCallback callback, void * userdata)
{
    animateSection(_image->defaultSection,target,callback,userdata);
}

void PHImageAnimator::animateSection(int sect, PHObject * trg, PHCallback cb, void * ud)
{
    if (sect<0 || sect>=_image->sections.size())
    {
        section = -1;
        frame = -1;
        realframe = -1;
        return;
    }
    
    target = trg;
    callback = cb;
    userdata = ud;
    
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

void PHImageAnimator::advanceAnimation(double elapsedTime)
{
    if (section==-1 || frame==-1) return;
    if (!running) return;
    
    remaining-=elapsedTime;
    PHAnimatedImage::section * sec = _image->sections.at(section);
    PHAnimatedImage::frame * frm;
    while (remaining<=0)
    {
        int nframes = sec->frames.size();
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

void PHImageAnimator::renderInFramePortionTint(const PHRect & frm,const PHRect & port,const PHColor & tint)
{
    if (realframe<0) return;
    
    _image->load();
    
    const GLfloat squareVertices[] = {
        frm.x,			frm.y,
        frm.x+frm.width,frm.y,
        frm.x,			frm.y+frm.height,
        frm.x+frm.width,frm.y+frm.height,
    };

    int actWidth,actHeight, _width, _height;
    double xc,yc,xC,yC;
    
    int nt,p,r,c;
    
    if (fade)
    {
        nt = lastframe/_image->ipt;
        p = lastframe-nt*_image->ipt;
        r = p/_image->cols;
        c = p%_image->cols;
        
        actWidth = _image->textures[nt].awidth;
        actHeight = _image->textures[nt].aheight;
        _width = _image->_width;
        _height = _image->_height;
        xc = 0.5f/actWidth;
        yc = 0.5f/actHeight;
        xC = (double)_width/actWidth;
        yC = (double)_height/actHeight;
        
        const GLfloat squareTexCoords2[] = {
            xC*(port.x+c)+xc				, yC*(port.y+port.height+r)-yc,
            xC*(port.x+port.width+c)-xc     , yC*(port.y+port.height+r)-yc,
            xC*(port.x+c)+xc				, yC*(port.y+r)+yc,
            xC*(port.x+port.width+c)-xc     , yC*(port.y+r)+yc
        };
        
        glBindTexture(GL_TEXTURE_2D, _image->textures[nt].texid);
        
        int states = PHGLVertexArray | PHGLTextureCoordArray | PHGLTexture  | PHGLColorArray;
        PHGLSetStates(states);
        glVertexPointer(2, GL_FLOAT, 0, squareVertices);
        glTexCoordPointer(2, GL_FLOAT, 0, squareTexCoords2);
        PHColor tt = tint;
        if (tt==PHInvalidColor)
            tt = PHWhiteColor;
        if (fade)
            tt.a *= (remaining/time);
        PH24BitColor t(tt);
        const GLubyte colors2[] = { 
            t.r, t.g, t.b, t.a,
            t.r, t.g, t.b, t.a,
            t.r, t.g, t.b, t.a,
            t.r, t.g, t.b, t.a
        };
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors2);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);	
    }
    
    nt = realframe/_image->ipt;
    p = realframe-nt*_image->ipt;
    r = p/_image->cols;
    c = p%_image->cols;
    
    actWidth = _image->textures[nt].awidth;
    actHeight = _image->textures[nt].aheight;
    _width = _image->_width;
    _height = _image->_height;
    xc = 0.5f/actWidth;
    yc = 0.5f/actHeight;
    xC = (double)_width/actWidth;
	yC = (double)_height/actHeight;
	
	const GLfloat squareTexCoords[] = {
        xC*(port.x+c)+xc				, yC*(port.y+port.height+r)-yc,
		xC*(port.x+port.width+c)-xc     , yC*(port.y+port.height+r)-yc,
		xC*(port.x+c)+xc				, yC*(port.y+r)+yc,
		xC*(port.x+port.width+c)-xc     , yC*(port.y+r)+yc
    };
	
    glBindTexture(GL_TEXTURE_2D, _image->textures[nt].texid);
    
    int states = PHGLVertexArray | PHGLTextureCoordArray | PHGLTexture  | ((tint!=PHInvalidColor || fade)?PHGLColorArray:0);
    PHGLSetStates(states);
	glVertexPointer(2, GL_FLOAT, 0, squareVertices);
	glTexCoordPointer(2, GL_FLOAT, 0, squareTexCoords);
    if (states & PHGLColorArray)
    {
        PHColor tt = tint;
        if (tt==PHInvalidColor)
            tt = PHWhiteColor;
        if (fade)
            tt.a *= 1.0f-(remaining/time);
        PH24BitColor t(tt);
        const GLubyte colors[] = { 
            t.r, t.g, t.b, t.a,
            t.r, t.g, t.b, t.a,
            t.r, t.g, t.b, t.a,
            t.r, t.g, t.b, t.a
        };
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
    }
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);	

}