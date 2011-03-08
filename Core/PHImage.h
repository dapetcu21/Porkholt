/*
 *  PHImage.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */
#ifndef PHIMAGE_H
#define PHIMAGE_H
#include "PHObject.h"

class PHImage;
#include <map>

class PHImage : public PHObject
{
public:
	static PHImage* imageFromPath(const string & path);
	static PHImage* imageNamed(const string & name);
	static void clearImages();
	
private:
	static map<string,PHImage*> images;
	PHImage(const string & path);
	
	int _height;
	int actHeight;
	int actWidth;
	int _width;
	uint8_t bit_depth;
	uint8_t color_type;
	GLint format;
	
	unsigned int texid;
	
public:
	int height() { return _height; };
	int width() { return _width; };
	void bindToTexture();
	~PHImage();
	
	void renderInFrame(const PHRect & frm);
	
	void loadToTexture(PHObject * sender, void * ud); //don't use
};

#endif