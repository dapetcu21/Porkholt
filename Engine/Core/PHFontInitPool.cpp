//
//  PHFontInitPool.cpp
//  Porkholt
//
//  Created by Marius Petcu on 1/30/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#include "PHFontInitPool.h"
#include "PHFont.h"

const string PHFontInitPool::fontDirectory()
{
    return "./rsrc/fnt/";
}

PHFont* PHFontInitPool::fontFromPath(const string & path)
{
    PHFont * fnt;
	map<string,PHFont*>::iterator i = fonts.find(path);
	if (i==fonts.end())
	{
		try {
            fnt = new PHFont(path);
		} catch (string ex)
		{
			PHLog(ex);
			return NULL;
		}
		fonts[path] = fnt;
	} else {
		fnt = i->second;
	}
	return fnt;
}

PHFont* PHFontInitPool::fontNamed(const string & name)
{
    return PHFontInitPool::fontFromPath(fontDirectory()+name+".phf");
}

void PHFontInitPool::collectGarbageFonts()
{
    map<string,PHFont*>::iterator i;
	map<string,PHFont*> tmp;
	for (i = fonts.begin(); i!=fonts.end(); i++)
	{
		if ((i->second)->referenceCount()>=2)
			tmp[i->first] = i->second;
		else
			i->second->release();
	}
	fonts = tmp;
}