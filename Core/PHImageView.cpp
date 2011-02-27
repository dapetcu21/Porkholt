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
	if (_image)
		_image->renderInFrame(_bounds);
}

void PHImageView::setImage(PHImage * image) 
{ 
	if (image) image->retain(); 
	if (_image) _image->release(); 
	_image=image; 
}