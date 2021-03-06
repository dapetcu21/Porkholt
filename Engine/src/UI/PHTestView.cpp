/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/UI/PHTestView.h>
#include <Porkholt/Core/PHDrawableCoordinates.h>
#include <Porkholt/Core/PHGLUniformStates.h>

char color_notex_sprites [] = "color_notex_sprites";

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
	
    gm->setGLStates(PHGLBlending | PHGLVertexArray | PHGLColorArray);
    gm->updateSpriteUniforms();
    gm->spriteUniformStates()->apply(gm->shaderProgramNamed<color_notex_sprites>());
    gm->vertexPointer(2, GL_FLOAT, 0, squareVertices);
    gm->colorPointer(4, GL_UNSIGNED_BYTE, 0, squareColors);
    
    PHGL::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void PHTestView::touchEvent(PHEvent * event)
{
    if (event->type() == PHEvent::touchDown)
        event->setHandled(true);
	if (event->type() == PHEvent::touchMoved)
	{
		PHPoint p[2];
        if (_parent && _parent->isView())
		{
            p[0] = event->location();
            p[1] = event->lastLocation();
            //PHLog("%f %f  %f %f", p[0].x, p[0].y, p[1].x, p[1].y);
            p[0] = event->drawableLocation()->pointInView(parent());
            p[1] = event->lastDrawableLocation()->pointInView(parent());
            //PHLog("%f %f  %f %f", p[0].x, p[0].y, p[1].x, p[1].y);
		}
		PHRect frame;
		frame = this->frame();
		frame.x+=p[0].x-p[1].x;
		frame.y+=p[0].y-p[1].y;
		setFrame(frame);
        event->setHandled(true);
	}
}
