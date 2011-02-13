/*
 *  PHImageView.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#ifndef PHIMAGEVIEW_H
#define PHIMAGEVIEW_H

#include "PHMain.h"

class PHImage;

class PHImageView : public PHView
{
private:
	PHImage * _image;
public:
	PHImage * image() { return _image; };
	void setImage(PHImage * image);
	PHImageView();
	PHImageView(const PHRect &frame);
	PHImageView(PHImage * image);
	virtual ~PHImageView();
	virtual void draw();
};

#endif