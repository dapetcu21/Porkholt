//
//  PHTextView.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/16/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHTextView.h"

#define PHTEXTVIEW_INIT _font(NULL), size(1.0f), _alignment(alignCenter | justifyLeft), _text(""), color(PHWhiteColor)

PHTextView::PHTextView() : PHView(), PHTEXTVIEW_INIT
{
};

PHTextView::PHTextView(const PHRect &frame) : PHView(frame), PHTEXTVIEW_INIT 
{
};

PHTextView::~PHTextView()
{
    if (_font)
        _font->release();
}

void PHTextView::draw()
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
	glVertexPointer(2, GL_FLOAT, 0, squareVertices);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, squareColors);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
