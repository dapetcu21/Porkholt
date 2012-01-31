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
#include "PHImageView.h"
#include "PHImage.h"


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
        if (fr>=section->frames.size()-1)
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
    if (sect<0 || sect>=_image->sections.size())
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
    int actWidth,actHeight, _width, _height;
    ph_float xc,yc,xC,yC;
    
    int nt,p,r,c;
    
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
    xC = (ph_float)_width/actWidth;
    yC = (ph_float)_height/actHeight;

    return PHRect(xC*(port.x+c)+xc,yC*(port.y+port.height+r)-yc,xC*port.width-2*xc,-yC*port.height+2*yc);
}

PHRect PHImageAnimator::lastFrameTextureCoordinates(const PHRect & port)
{
    int actWidth,actHeight, _width, _height;
    ph_float xc,yc,xC,yC;
    
    int nt,p,r,c;
    
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
    xC = (ph_float)_width/actWidth;
    yC = (ph_float)_height/actHeight;
    
    return PHRect(xC*(port.x+c)+xc,yC*(port.y+port.height+r)-yc,xC*port.width-2*xc,-yC*port.height+2*yc);
}

void PHImageAnimator::renderInFramePortionTint(PHGameManager * _gameManager, const PHRect & frm,const PHRect & port,const PHColor & tint)
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
    ph_float xc,yc,xC,yC;
    
    int nt,p,r,c;
    PHColor tt = tint;
    if (tt==PHInvalidColor)
        tt = PHWhiteColor;
    
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
        xC = (ph_float)_width/actWidth;
        yC = (ph_float)_height/actHeight;
        
        const GLfloat squareTexCoords2[] = {
            xC*(port.x+c)+xc				, yC*(port.y+port.height+r)-yc,
            xC*(port.x+port.width+c)-xc     , yC*(port.y+port.height+r)-yc,
            xC*(port.x+c)+xc				, yC*(port.y+r)+yc,
            xC*(port.x+port.width+c)-xc     , yC*(port.y+r)+yc
        };
        
        glBindTexture(GL_TEXTURE_2D, _image->textures[nt].texid);
        
        int states = PHGLVertexArray | PHGLTextureCoordArray | PHGLTexture;
        PHGLSetStates(states);
        glVertexPointer(2, GL_FLOAT, 0, squareVertices);
        glTexCoordPointer(2, GL_FLOAT, 0, squareTexCoords2);
        PHGLSetColor(tt*(remaining/time));
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
    xC = (ph_float)_width/actWidth;
	yC = (ph_float)_height/actHeight;
	
	const GLfloat squareTexCoords[] = {
        xC*(port.x+c)+xc				, yC*(port.y+port.height+r)-yc,
		xC*(port.x+port.width+c)-xc     , yC*(port.y+port.height+r)-yc,
		xC*(port.x+c)+xc				, yC*(port.y+r)+yc,
		xC*(port.x+port.width+c)-xc     , yC*(port.y+r)+yc
    };
	
    glBindTexture(GL_TEXTURE_2D, _image->textures[nt].texid);
    
    int states = PHGLVertexArray | PHGLTextureCoordArray | PHGLTexture ;
    PHGLSetStates(states);
	glVertexPointer(2, GL_FLOAT, 0, squareVertices);
	glTexCoordPointer(2, GL_FLOAT, 0, squareTexCoords);
    if (fade)
    {
        tt.a *= 1.0f-(remaining/time);
    }
    PHGLSetColor(tt);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void PHImageAnimator::bindCurrentFrameToTexture()
{
    int nt = realframe/_image->ipt;
    glBindTexture(GL_TEXTURE_2D, _image->textures[nt].texid);
}
void PHImageAnimator::bindLastFrameToTexture()
{
    int nt = lastframe/_image->ipt;
    glBindTexture(GL_TEXTURE_2D, _image->textures[nt].texid);
}

void PHImageAnimator::rebuildVBOs(PHImageView * imageView, GLuint & vbo1, PHImage::VBOParams & params1, GLuint & vbo2, PHImage::VBOParams & params2)
{
    if (!vbo1)
        glGenBuffers(1, &vbo1);
    if (fade&&!vbo2)
        glGenBuffers(1, &vbo2);
    if (!fade&&vbo2)
    {
        glDeleteBuffers(1, &vbo2);
        vbo2 = 0;
    }
    PHPoint repeat = PHPoint(imageView->repeatX(),imageView->repeatY());
    PHRect port = imageView->textureCoordinates();
    
    int actWidth,actHeight, _width, _height;
    ph_float xc,yc,xC,yC;
    
    int nt,p,r,c;
    
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
    xC = (ph_float)_width/actWidth;
    yC = (ph_float)_height/actHeight;

    
    PHImage::buildImageVBO(vbo1, params1, repeat, port, PHRect(xC*c,yC*r,xC,yC), PHPoint(xc,yc));
    
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
        xC = (ph_float)_width/actWidth;
        yC = (ph_float)_height/actHeight;

        PHImage::buildImageVBO(vbo2, params2, repeat, port, PHRect(xC*c,yC*r,xC,yC), PHPoint(xc,yc));
    }
}