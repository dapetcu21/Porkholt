/*
 *  PHTestView.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/12/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHTestView.h"

void PHTestView::draw()
{
	GLfloat squareVertices[] = {
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
	
    PHGLSetStates(PHGLVertexArray | PHGLColorArray);
    _gameManager->pushSpriteShader(_gameManager->coloredNoTexSpriteShader());
    _gameManager->applySpriteShader();
    _gameManager->popSpriteShader();
	PHGLVertexPointer(2, GL_FLOAT, 0, squareVertices);
	PHGLColorPointer(4, GL_UNSIGNED_BYTE, 0, squareColors);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void PHTestView::touchEvent(PHEvent * event)
{
	if (event->type() == PHEvent::touchMoved)
	{
		PHPoint p[2];
		p[0] = event->location();
		p[1] = event->lastLocation();
		if (superView)
		{
			superView->toMyCoordinates(p, 2);
		}
		PHRect frame;
		frame = this->frame();
		frame.x+=p[0].x-p[1].x;
		frame.y+=p[0].y-p[1].y;
		setFrame(frame);
        event->setHandled(true);
	}
}