//
//  PLObjectView.mm
//  LevelDesigner
//
//  Created by Marius Petcu on 10/5/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLObjectView.h"
#import "PLProperty.h"
#import "PLObject.h"
#import "PHEventHandler.h"
#import "ObjectController.h"
#import "WorldController.h"
#import "PLImage.h"
#import "PLImageView.h"
#import "SubentityController.h"
#import "PLFixture.h"
#import "PLFixtureView.h"

PLObjectView::PLObjectView(PLObject * _model) : PHView(PHRect(-0.7,-0.7,0.14,0.14)), model(_model), sel(false), moving(false), rotating(false)
{
    setBounds(bounds()-boundsCenter());
    setRotationalCenter(PHOriginPoint);
    setScalingCenter(PHOriginPoint);
    [model retain];
    [model setActor:this];
    modelChanged();
    setUserInput(true);
}

PLObjectView::~PLObjectView()
{
    [model setActor:NULL];
    [model release];
}

void PLObjectView::reloadSubviews()
{
    for (PLImage * obj in [[model subentityModel] images])
        if (obj.actor)
            (obj.actor)->retain();
    for (PLFixture * obj in [[model subentityModel] fixtures])
        if (obj.actor)
            (obj.actor)->retain();
    
    removeAllSubviews();
    if (((ObjectController*)[model owner]).showImages)
        for (PLImage * obj in [[model subentityModel] images])
        {
            if (!obj.actor)
            {
                obj.actor = new PLImageView(obj);
                obj.actor->setController(this);
            }
            addSubview(obj.actor);
        }
    if (((ObjectController*)[model owner]).showFixtures)
        for (PLFixture * obj in [[model subentityModel] fixtures])
        {
            if (!obj.actor)
            {
                obj.actor = new PLFixtureView(obj);
                obj.actor->setController(this);
            }
            addSubview(obj.actor);
        }

    
    for (PLImage * obj in [[model subentityModel] images])
        if (obj.actor)
            (obj.actor)->release();
    for (PLFixture * obj in [[model subentityModel] fixtures])
        if (obj.actor)
            (obj.actor)->release();
}

void PLObjectView::modelChanged()
{
    NSPoint pos = [model propertyAtKeyPath:@"pos"].pointValue;
    setPosition(PHPoint(pos.x,pos.y)+bounds().origin());
    setRotation(-toRad([model propertyAtKeyPath:@"rotation"].numberValue));
    setHorizontallyFlipped([model propertyAtKeyPath:@"flipped"].numberValue);
    
    reloadSubviews();
}

void PLObjectView::flagsChanged()
{
    modelChanged();
}

void PLObjectView::draw()
{
    if (!((ObjectController*)[model owner]).showMarkers) return;
    GLfloat squareVertices[] = {
        _bounds.x+0,               _bounds.y+0,
        _bounds.x+_bounds.width,   _bounds.y+0,
        _bounds.x+0,               _bounds.y+_bounds.height,
        _bounds.x+_bounds.width,   _bounds.y+_bounds.height,
        
        _bounds.x+_bounds.width,   _bounds.y+_bounds.height,
        _bounds.x+0,               _bounds.y+_bounds.height/2,
        
        _bounds.x+0,               _bounds.y+_bounds.height/2,
        _bounds.x+_bounds.width/2, _bounds.y+_bounds.height,
        _bounds.x+_bounds.width/2, _bounds.y+_bounds.height/2,
        
        _bounds.x+_bounds.width/2, _bounds.y+_bounds.height/2,
        _bounds.x+_bounds.width/2, _bounds.y+_bounds.height,
        _bounds.x+_bounds.width,   _bounds.y+_bounds.height/2
        
    };
	
    GLubyte by = model.readOnly?170:255;
    GLubyte r=sel?127:by,g=sel?127:by,b=sel?255:by,a=255;
    
    const GLubyte squareColors[] = {
        r,g,b,a,
        r,g,b,a,
        r,g,b,a,
        r,g,b,a,
        
        r,g,b,a,
        255,0,0,255,
        
        255,0,0,255,
        255,0,0,255,
        255,0,0,255,
        0,0,255,255,
        0,0,255,255,
        0,0,255,255
    };
	
    PHGLSetStates(PHGLVertexArray | PHGLColorArray);
	glVertexPointer(2, GL_FLOAT, 0, squareVertices);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, squareColors);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 12);
}

bool PLObjectView::intersectsRect(const PHRect &rect)
{
    return ((ObjectController*)[model owner]).showMarkers?PHPointInRect(center(), rect):false;
}

void PLObjectView::touchEvent(PHEvent * evt)
{
    if (((evt->sender() == this) || (evt->sender() == NULL))  && (!((ObjectController*)[model owner]).showMarkers))
    {
        eventHandled = false;
        return;
    }
    if (evt->type() == PHEvent::touchDown)
    {
        if (PHEventHandler::modifierMask() & PHEventHandler::shiftModifier)
        {
            eventHandled = false;
            return;
        }
        bool cmd = PHEventHandler::modifierMask() & PHEventHandler::commandModifier;
        bool alt = PHEventHandler::modifierMask() & PHEventHandler::optionModifier;
        if (!cmd && ! alt && !sel)
            [[worldController model] clearSelection];
        if (alt)
            [[worldController model] removeEntity:model inSelectionForArray:0];
        else
            [[worldController model] insertEntity:model inSelectionForArray:0];
    } 
    else
    if (evt->type() == PHEvent::touchMoved)
    {
        if (evt->userData() == (void*)1)
        {
            if (!moving)
            {
                [worldController startMoving];
                moving = true;
            }
            PHPoint delta = superView->toMyCoordinates(evt->location()) - superView->toMyCoordinates(evt->lastLocation());
            [worldController move:delta];
        }
        else
        if (evt->userData() == (void*)2)
        {
            if (!rotating)
            {
                [worldController startRotating];
                rotating = true;
            }
            [worldController rotate:-(evt->location().y - evt->lastLocation().y)/2];
        }
    }
    else
    if (evt->type() == PHEvent::touchUp)
    {
        if ((evt->userData() == (void*)1) && moving)
        {
            [worldController stopMoving];
            moving = false;
        }
        if ((evt->userData() == (void*)2) && rotating)
        {
            [worldController stopRotating];
            rotating = false;
        }
    }
    else 
        eventHandled = false;
}