/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHImageInitPool.h>
#include <Porkholt/Core/PHImage.h>
#include <Porkholt/IO/PHFileManager.h>
#include <Porkholt/Core/PHAnimatedImage.h>
#include <Porkholt/Core/PHNormalImage.h>

PHImage * PHImageInitPool::imageFromPath(const string & path)
{
	PHImage * img;
	map<string,PHImage*>::iterator i = images.find(path);
	if (i==images.end())
	{
        PHGameManager * gm = gameManager();
		try {
            string nmpath = path + ".nmap";
            PHImage * nmap = NULL;
            try {
                if (PHFileManager::isDirectory(nmpath))
                    nmap = new PHAnimatedImage(nmpath,gm);
                else
                    nmap = new PHNormalImage(nmpath,gm);
            } catch (...) {};
            
            if (PHFileManager::isDirectory(path))
                img = new PHAnimatedImage(path,gm);
            else
                img = new PHNormalImage(path,gm);
            
            img->_normalMap = nmap;
            
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
		img = i->second;
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