 //
//  PHScrollerView.cpp
//  LevelDesigner
//
//  Created by Marius Petcu on 10/4/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHScrollerView.h"
#include "PHEventHandler.h"

#define SCROLLER_INIT content(NULL), scl(1), inscroll(PHOriginPoint), inertial(true)

PHScrollerView::PHScrollerView() : SCROLLER_INIT
{
    setBackgroundColor(PHBlackColor);
    setUserInput(true);
}

PHScrollerView::PHScrollerView(const PHRect &frame) : SCROLLER_INIT
{
    setBackgroundColor(PHBlackColor);
    setUserInput(true);
}

void PHScrollerView::setScale(double s, const PHPoint & center)
{
    if (s>1000)
        s = 1000;
    if (s<20)
        s = 20;
    if (s==scl) return;
    setBounds(bounds()*scl/s);
    if (content)
        content->setPosition(content->position()+center*(scl/s-1));
    scl = s;
}

void PHScrollerView::resetToOrigin()
{
    if (content)
    {
        content->setPosition(PHPoint(_bounds.width/2,_bounds.height/2));
    }
    lastin = inscroll = PHOriginPoint;
}

void PHScrollerView::move(const PHPoint & p)
{
    if (content)
        content->setPosition(content->position()+p);
}

void PHScrollerView::setContentView(PHView * cv)
{
    if (content)
    {
        content->removeFromSuperview();
        content->release();
    }
    content = cv;
    if (content)
    {
        content->retain();
        addSubview(content);
        resetToOrigin();
    }
}

void PHScrollerView::setFrame(const PHRect &frame)
{
    PHPoint b(_bounds.width, _bounds.height);
    PHView::setFrame(frame);
    setBounds(bounds()/scl);
    if (content)
    {
        PHPoint c(_bounds.width, _bounds.height);
        content->setPosition((c-b)/2+content->position());
    }
}

void PHScrollerView::drawGrid(double gap, double width, const PHColor & color)
{
    if (width*scl < 1.0f) return;
    PHPoint p = content?content->position():PHOriginPoint;
    PHRect b = _bounds;
    p.x = PHWarp(p.x, gap);
    p.y = PHWarp(p.y, gap);
    int nx = ((int)((_bounds.width-p.x)/gap))+1;
    int ny = ((int)((_bounds.height-p.y)/gap))+1;
    int nv = ((ny&&nx)?2:0)+(ny?(ny-1)*6+4:0)+(nx?(nx-1)*6+4:0);
    
    if (!nv) return;
    
    PHGLSetStates(PHGLVertexArray);
    PHGLSetColor(color);
    
    GLfloat * vertices = new GLfloat[nv*2];
    GLfloat * v = vertices;
    
    for (int i=0; i<nx; i++)
    {
        if (i)
        {
            v[0] = p.x+gap*(i-1)+width/2;
            v[1] = b.height;
            v+=2; 
            
            v[0] = p.x+gap*i-width/2;
            v[1] = 0;
            v+=2;
        }
        
        v[0] = p.x+gap*i-width/2;
        v[1] = 0;
        v+=2;

        v[0] = p.x+gap*i+width/2;
        v[1] = 0;
        v+=2;
        
        v[0] = p.x+gap*i-width/2;
        v[1] = b.height;
        v+=2;
        
        v[0] = p.x+gap*i+width/2;
        v[1] = b.height;
        v+=2;
    }
    
    if (nx&&ny)
    {
        v[0] = p.x+gap*(nx-1)+width/2;
        v[1] = b.height;
        v+=2;
        
        v[1] = p.y-width/2;
        v[0] = 0;
        v+=2;
    }
    
    for (int i=0; i<ny; i++)
    {
        if (i)
        {
            v[1] = p.y+gap*(i-1)+width/2;
            v[0] = b.width;
            v+=2;
            
            v[1] = p.y+gap*i-width/2;
            v[0] = 0;
            v+=2;
        }
        
        v[1] = p.y+gap*i-width/2;
        v[0] = 0;
        v+=2;
        
        v[1] = p.y+gap*i+width/2;
        v[0] = 0;
        v+=2;
        
        v[1] = p.y+gap*i-width/2;
        v[0] = b.width;
        v+=2;
        
        v[1] = p.y+gap*i+width/2;
        v[0] = b.width;
        v+=2;
    }
    
    PHGLVertexPointer(2, GL_FLOAT, 0, vertices);
    _gameManager->applyShader(_gameManager->solidColorShader());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, nv);
    
    delete [] vertices;
}

void PHScrollerView::draw()
{    
    if (inertial && (inscroll.x || inscroll.y))
    {
        move(inscroll);
        PHLowPassFilter(inscroll.x, 0, 1/60.0f, 4.0f);
        PHLowPassFilter(inscroll.y, 0, 1/60.0f, 4.0f);
        if (fabs(inscroll.x)<0.001)
            inscroll.x=0;
        if (fabs(inscroll.y)<0.001)
            inscroll.y=0;
    }
    drawGrid(1.0,0.05,PHColor(0,1,0,0.6));
    drawGrid(0.5,0.025,PHColor(0,1,0,0.3));
    drawGrid(0.1,0.015,PHColor(0,1,0,0.2));
    
    if (content)
    {
        PHPoint p = content->position();
        GLfloat vertices[] = {
            p.x, p.y+0.15,
            p.x-0.15, p.y,
            p.x+0.15, p.y,
            p.x, p.y-0.15
        };
        
        PHGLSetStates(PHGLVertexArray);
        PHGLSetColor(PHColor(1,0,0,1));
        PHGLVertexPointer(2, GL_FLOAT, 0, vertices);
        _gameManager->applyShader(_gameManager->solidColorShader());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}


void PHScrollerView::touchEvent(PHEvent * event)
{
    if (content && content->userInput())
    {
        content->touchEvent(event);
        if (event->handled())
            return;
    }
    event->setHandled(true);
    if (event->type() == PHEvent::touchDown)
    {
        lastin = PHOriginPoint;
        inscroll = PHOriginPoint;
    } else
    if (event->type() == PHEvent::touchMoved)
    {
        move(lastin = (toMyCoordinates(event->location())-toMyCoordinates(event->lastLocation())));
    } else
    if (event->type() == PHEvent::touchUp)
    {
        if (inertial)
            inscroll = lastin;
    } else
    if (event->type() == PHEvent::scrollWheel)
    {
        if (PHEventHandler::modifierMask() == PHEventHandler::optionModifier)
        {
            double sc = 1-(event->delta().y/150.0f);
            if (sc<0.1)
                sc = 0.1;
            setScale(scale()*sc, toMyCoordinates(event->location()));
        } else
            move(event->delta()/scl);
    } else
    if (event->type() == PHEvent::pinchZoom)
    {
        double sc = 1+(event->zoom()/1.5f);
        if (sc<0.1)
            sc = 0.1;
        setScale(scale()*sc, toMyCoordinates(event->location()));
    } else
    {
        event->setHandled(false);
    }
}
