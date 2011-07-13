/*
 *  PHImage.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */
#ifndef PHIMAGE_H
#define PHIMAGE_H
#include "PHMain.h"

class PHImage;
#include <map>

class PHImage : public PHObject
{
public:
	static PHImage* imageFromPath(const string & path);
	static PHImage* imageNamed(const string & name);
	static void clearImages();
	
protected:
	static map<string,PHImage*> images;
	PHImage(const string & path);
	
	int _height;
    int _width;
	
public:
	int height() { return _height; };
	int width() { return _width; };

	~PHImage();
    
    virtual bool isNormal() { return false; };
    virtual bool isAnimated() { return false; };
};

#define PHIMAGE_SIMULTANEOUS_LOADING

#endif