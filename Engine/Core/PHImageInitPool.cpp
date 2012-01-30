//
//  PHImageInitPool.cpp
//  Porkholt
//
//  Created by Marius Petcu on 1/30/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#include "PHImageInitPool.h"
#include "PHImage.h"
#include "PHFileManager.h"
#include "PHAnimatedImage.h"
#include "PHNormalImage.h"

PHImage * PHImageInitPool::imageFromPath(const string & path)
{
	PHImage * img;
	map<string,PHImage*>::iterator i = images.find(path);
	if (i==images.end())
	{
		try {
            if (PHFileManager::isDirectory(path))
                img = new PHAnimatedImage(path);
            else
                img = new PHNormalImage(path);
		} catch (string ex)
		{
			PHLog(ex);
            if (imageExists("placeholder"))
                return imageNamed("placeholder");
            else
                return NULL;
		}
		images[path] = img;
	} else {
		img = images[path];
	}
	return img;
}

const string PHImageInitPool::imageDirectory()
{
    return "./rsrc/img/";
}

bool PHImageInitPool::imageExists(const string & name)
{
    string path = imageDirectory()+name+".png";
    return PHFileManager::fileExists(path);
}

PHImage * PHImageInitPool::imageNamed(const string & string)
{
	return PHImageInitPool::imageFromPath(imageDirectory()+string+".png");
}

void PHImageInitPool::collectGarbageImages()
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

void PHImageInitPool::loadAllImages()
{
    map<string,PHImage*>::iterator i;
    for (i = images.begin(); i!=images.end(); i++)
        i->second->load();
}
