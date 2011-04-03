/*
 *  PHImageView.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHImageView.h"

#define PHIMAGEVIEW_INIT _image(NULL), coords(PHWholeRect)

PHImageView::PHImageView() : PHView(), PHIMAGEVIEW_INIT
{
}

PHImageView::PHImageView(const PHRect &frame) : PHView(frame), PHIMAGEVIEW_INIT
{
}

PHImageView::PHImageView(PHImage * image) : PHView(), PHIMAGEVIEW_INIT
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
		_image->renderInFramePortion(_bounds,coords);
}

void PHImageView::setImage(PHImage * image) 
{ 
	if (image) image->retain(); 
	if (_image) _image->release(); 
	_image=image; 
}