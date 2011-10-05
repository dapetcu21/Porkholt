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

PLObjectView::PLObjectView(PLObject * _model) : PHView(PHRect(-0.13,-0.13,0.26,0.26)), model(_model), sel(false)
{
    [model setActor:this];
    modelChanged();
}

PLObjectView::~PLObjectView()
{
    [model setActor:NULL];
}

void PLObjectView::modelChanged()
{
    NSPoint pos = [model propertyAtKeyPath:@"pos"].pointValue;
    setPosition(PHPoint(pos.x,pos.y)-boundsCenter());
    double rot = [model propertyAtKeyPath:@"rotation"].numberValue;
    setRotation(-toRad(rot));
    
}

void PLObjectView::draw()
{
    GLfloat squareVertices[] = {
        0, 0,
        _bounds.width, 0,
        0,  _bounds.height,
        _bounds.width,  _bounds.height,
    };
	
    GLubyte s = sel?127:0;
    
    const GLubyte squareColors[] = {
        255, 255,   s, 255,
        s,   255, 255, 255,
        s,     s,   s,   s,
        255,   s, 255, 255,
    };
	
    PHGLSetStates(PHGLVertexArray | PHGLColorArray);
	glVertexPointer(2, GL_FLOAT, 0, squareVertices);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, squareColors);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}