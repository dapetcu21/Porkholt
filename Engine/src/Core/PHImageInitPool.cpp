/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHImageInitPool.h>
#include <Porkholt/Core/PHImage.h>
#include <Porkholt/IO/PHFileManager.h>
#include <Porkholt/Core/PHAnimatedImage.h>
#include <Porkholt/Core/PHNormalImage.h>
#include <Porkholt/IO/PHDirectory.h>
#include <Porkholt/IO/PHFile.h>

PHImage * PHImageInitPool::imageNamed(const string & name, PHDirectory * dir, bool nmap)
{
    PHImage * img = NULL;
    PHInode * file = NULL;
    bool aa = dir->itemExists(name + ".png.aa");
    try {
        file = dir->itemAtPath(name + hdsuf + ".png");
        img = PHImageInitPool::imageFromFile(file, aa);
        file->release();
    } catch (...)
    {
        if (file)
            file ->release();
        try {
            file = dir->itemAtPath(name + ".png");
            img = PHImageInitPool::imageFromFile(file, aa);
            file->release();
        } catch (string ex)
        {
            if (file)
                file->release();
            PHLog("Could not load image \"%s\": %s", name.c_str(), ex.c_str());
            img = PHImageInitPool::imageNamed("placeholder");
        }
    }
    if (nmap)
    {
        string nm = name + ".nmap";
        if (imageExists(nm, dir))
            img->setNormalMap(imageNamed(nm, dir, false));
    }
    return img;
}

PHImage * PHImageInitPool::imageFromFile(PHInode * file, bool aa)
{
	PHImage * img;
    PHHashedString path(file->path());
	map<PHHashedString, PHImage*>::iterator i = images.find(path);
	if (i==images.end())
	{
        PHGameManager * gm = gameManager();
        if (file->isDirectory())    
            img = new PHAnimatedImage(gm, (PHDirectory*)file);
        else if (file->isFile())
            img = new PHNormalImage(gm, (PHFile*)file, aa);
		if (img) 
            images[path] = img;
        else 
            throw string("Not a file or directory");
	} else {
		img = i->second;
	}
	return img;
}

bool PHImageInitPool::imageExists(const string & name, PHDirectory * dir)
{
    return  dir->itemExists(name + hdsuf + ".png") || 
            dir->itemExists(name + ".png");
}

void PHImageInitPool::collectGarbageImages()
{
	map<PHHashedString,PHImage*>::iterator i;
	map<PHHashedString,PHImage*> tmp;
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
    map<PHHashedString, PHImage*>::iterator i;
    for (i = images.begin(); i!=images.end(); i++)
        i->second->load();
}
