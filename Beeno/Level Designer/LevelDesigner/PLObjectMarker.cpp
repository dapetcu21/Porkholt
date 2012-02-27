//
//  PLObjectMarker.mm
//  LevelDesigner
//
//  Created by Marius Petcu on 10/11/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PLObjectMarker.h"
#include "PLObjectView.h"

void PLObjectMarker::draw()
{
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
	
    GLubyte by = ((PLObjectView*)superview())->readOnly()?170:255;
    bool sel = ((PLObjectView*)superview())->selected();
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
	
    gm->setGLStates(PHGLVertexArray | PHGLColorArray);
	gm->vertexPointer(2, GL_FLOAT, 0, squareVertices);
	gm->colorPointer(4, GL_UNSIGNED_BYTE, 0, squareColors);
    gm->applyShader(gm->coloredNoTexSpriteShader());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 12);
}

void PLObjectMarker::touchEvent(PHEvent * event)
{
    superview()->touchEvent(event);
}
