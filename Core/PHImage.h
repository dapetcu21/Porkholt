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

#define PHIMAGE_ASYNCHRONEOUS_LOADING
#define PHIMAGE_ORDERED_LOADING

class PHImage;
#include <map>

class PHImage : public PHObject
{
public:
	static PHImage* imageFromPath(const string & path);
	static PHImage* imageNamed(const string & name);
	static void collectGarbage();
#ifdef PHIMAGE_ORDERED_LOADING
    static PHMutex * loadingMutex;
#endif
	
protected:
	static map<string,PHImage*> images;
	PHImage(const string & path);
	
	int _height;
    int _width;
    
    bool loaded;
    void load() { if (!loaded) _load(); }
    virtual void _load() = 0;
	
public:
	int height() { load(); return _height; };
	int width() { load(); return _width; };

	virtual ~PHImage();
    
    virtual bool isNormal() { return false; };
    virtual bool isAnimated() { return false; };
};

#endif