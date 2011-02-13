/*
 *  PHImageView.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHImageView.h"

PHImageView::PHImageView() : PHView(), _image(NULL)
{
}

PHImageView::PHImageView(const PHRect &frame) : PHView(frame), _image(NULL)
{
}

PHImageView::PHImageView(PHImage * image) : PHView(), _image(NULL)
{
	setImage(image);
}

PHImageView::~PHImageView()
{
	if (_image)
		_image->release();
}

void PHImageView::draw()
{
	const GLfloat squareVertices[] = {
        0, 0,
        _bounds.width, 0,
        0,  _bounds.height,
        _bounds.width,  _bounds.height,
    };
	
	const GLfloat squareTexCoords[] = {
        0.0f,1.0f,
		1.0f,1.0f,
		0.0f,0.0f,
		1.0f,0.0f,
    };
	
	_image->bindToTexture();
	glVertexPointer(2, GL_FLOAT, 0, squareVertices);
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, squareTexCoords);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
	
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);	
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
}

void PHImageView::setImage(PHImage * image) 
{ 
	if (image) image->retain(); 
	if (_image) _image->release(); 
	_image=image; 
}