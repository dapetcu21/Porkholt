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

PLObjectView::PLObjectView(PLObject * _model) : PHView(PHRect(-0.13,-0.13,0.26,0.26)), model(_model), sel(false)
{
    [model setActor:this];
    modelChanged();
    setUserInput(true);
}

PLObjectView::~PLObjectView()
{
    [model setActor:NULL];
}

void PLObjectView::modelChanged()
{
    NSPoint pos = [model propertyAtKeyPath:@"pos"].pointValue;
    setPosition(PHPoint(pos.x,pos.y)-boundsCenter());
    setRotation(-toRad([model propertyAtKeyPath:@"rotation"].numberValue));
    setHorizontallyFlipped([model propertyAtKeyPath:@"flipped"].numberValue);
}

void PLObjectView::draw()
{
    GLfloat squareVertices[] = {
        0, 0,
        _bounds.width, 0,
        0,  _bounds.height,
        _bounds.width,  _bounds.height,
        
        _bounds.width,  _bounds.height,
        0,_bounds.height/2,
        
        0,_bounds.height/2,
        _bounds.width/2, _bounds.height,
        _bounds.width/2, _bounds.height/2,
        
        _bounds.width/2, _bounds.height/2,
        _bounds.width/2, _bounds.height,
        _bounds.width, _bounds.height/2
        
    };
	
    GLubyte by = model.readOnly?127:255;
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
    return PHPointInRect(center(), rect);
}

void PLObjectView::touchEvent(PHEvent * evt)
{
    if (evt->type() == PHEvent::touchDown)
    {
        if (PHEventHandler::modifierMask() & PHEventHandler::shiftModifier)
        {
            eventHandled = false;
            return;
        }
        bool cmd = PHEventHandler::modifierMask() & PHEventHandler::commandModifier;
        bool alt = PHEventHandler::modifierMask() & PHEventHandler::optionModifier;
        if (!cmd && ! alt)
            [[worldController model] clearSelection];
        if (alt)
            [[worldController model] removeEntity:model inSelectionForArray:0];
        else
            [[worldController model] insertEntity:model inSelectionForArray:0];
            
    } else 
        eventHandled = false;
}