/*
 *  PHImage.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHMain.h"
#include "PHImage.h"
#include "PHFileManager.h"
#include "PHNormalImage.h"
#include "PHAnimatedImage.h"

map<string,PHImage*> PHImage::images;

PHImage::PHImage(const string & path) {};

PHImage::~PHImage() {};

PHImage * PHImage::imageFromPath(const string & path)
{
	PHImage * img;
	map<string,PHImage*>::iterator i = images.find(path);
	if (i==images.end())
	{
		try {
            if (PHFileManager::singleton()->isDirectory(path))
                img = new PHAnimatedImage(path);
            else
                img = new PHNormalImage(path);
		} catch (string ex)
		{
			PHLog(ex);
			return NULL;
		}
		images[path] = img;
	} else {
		img = images[path];
	}
	img->retain();
	return img;
}

PHImage * PHImage::imageNamed(const string & string)
{
	return PHImage::imageFromPath(PHFileManager::singleton()->resourcePath()+"/img/"+string+".png");
}
void PHImage::clearImages()
{
	map<string,PHImage*>::iterator i;
	map<string,PHImage*> tmp;
	for (i = images.begin(); i!=images.end(); i++)
	{
		if ((i->second)->referenceCount()>=2)
			tmp[i->first] = i->second;
		else
			i->second->release();
	}
	images = tmp;
}