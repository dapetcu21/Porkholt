/*
 *  PHTestView.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/12/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHTestView.h"

void PHTestView::draw()
{
	const GLfloat squareVertices[] = {
        0, 0,
        _bounds.width, 0,
        0,  _bounds.height,
        _bounds.width,  _bounds.height,
    };
	
    static const GLubyte squareColors[] = {
        255, 255,   0, 255,
        0,   255, 255, 255,
        0,     0,   0,   0,
        255,   0, 255, 255,
    };
	
	glVertexPointer(2, GL_FLOAT, 0, squareVertices);
	glEnableClientState(GL_VERTEX_ARRAY);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, squareColors);
	glEnableClientState(GL_COLOR_ARRAY);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

void PHTestView::touchEvent(PHTouch * touch)
{
	if (touch->phase() == PHTouch::touchMovedState)
	{
		PHPoint p[2];
		p[0] = touch->location();
		p[1] = touch->lastLocation();
		if (superView)
		{
			superView->toMyCoordinates(p, 2);
		}
		PHRect frame;
		frame = this->frame();
		frame.x+=p[0].x-p[1].x;
		frame.y+=p[0].y-p[1].y;
		setFrame(frame);
		
	}
}