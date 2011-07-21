//
//  PHImageAnimator.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/13/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHImageAnimator.h"
#include "PHAnimatedImage.h"
#include "PHLua.h"

set<PHImageAnimator*> PHImageAnimator::animators;

PHImageAnimator::PHImageAnimator(PHAnimatedImage * img) : _image(img), advanceManually(false)
{
    reset();
    animators.insert(this);
}

PHImageAnimator::~PHImageAnimator()
{
    animators.erase(this);
}

double PHImageAnimator::timeForFrameInSection(int fr, int sec)
{
    try {
        PHAnimatedImage::section * section = _image->sections.at(sec);
        PHAnimatedImage::frame & frame = section->frames.at(fr);
        if (fr>=section->frames.size()-1)
            return INFINITY;
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

void PHImageAnimator::animateSection(const string & name)
{
    animateSection(_image->sectionNo(name));
}


void PHImageAnimator::reset()
{
    animateSection(_image->defaultSection);
}

void PHImageAnimator::animateSection(int sect)
{
    if (sect<0 || sect>=_image->sections.size())
    {
        section = -1;
        frame = -1;
        realframe = -1;
        return;
    }
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
    remaining = timeForFrameInSection(frame, section);
    realframe = realFrame(frame,section);
}

void PHImageAnimator::advanceAnimations(double elapsedTime)
{
    for (set<PHImageAnimator*>::iterator i = animators.begin(); i!=animators.end(); i++)
    {
        if (!((*i)->advanceManually))
            (*i)->advanceAnimation(elapsedTime);
    }
}

void PHImageAnimator::advanceAnimation(double elapsedTime)
{
    if (section==-1 || frame==-1) return;
    
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
        remaining += timeForFrameInSection(frame, section);
        realframe = realFrame(frame,section);
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
	
    int nt = realframe/_image->ipt;
    int p = realframe-nt*_image->ipt;
    int r = p/_image->cols;
    int c = p%_image->cols;
    
    int actWidth = _image->textures[nt].awidth;
    int actHeight = _image->textures[nt].awidth;
    int _width = _image->_width;
    int _height = _image->_height;
    double xc = 0.5f/actWidth;
    double yc = 0.5f/actHeight;
    double xC = (double)_width/actWidth;
	double yC = (double)_height/actHeight;
    
	
	const GLfloat squareTexCoords[] = {
        xC*(port.x+c)+xc				, yC*(port.y+port.height+r)-yc,
		xC*(port.x+port.width+c)-xc     , yC*(port.y+port.height+r)-yc,
		xC*(port.x+c)+xc				, yC*(port.y+r)+yc,
		xC*(port.x+port.width+c)-xc     , yC*(port.y+r)+yc
    };
	
    glBindTexture(GL_TEXTURE_2D, _image->textures[nt].texid);
    
    int states = PHGLVertexArray | PHGLTextureCoordArray | PHGLTexture  | ((tint==PHInvalidColor)?0:PHGLColorArray);
    PHGLSetStates(states);
	glVertexPointer(2, GL_FLOAT, 0, squareVertices);
	glTexCoordPointer(2, GL_FLOAT, 0, squareTexCoords);
    if (states & PHGLColorArray)
    {
        PH24BitColor t(tint);
        const GLubyte colors[] = { 
            t.r, t.g, t.b, t.a,
            t.r, t.g, t.b, t.a,
            t.r, t.g, t.b, t.a,
            t.r, t.g, t.b, t.a
        };
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
    }
	PHGLSetStates(states);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);	

}