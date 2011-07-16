//
//  PHFont.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/16/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHFont.h"
#include "PHFileManager.h"

map<string,PHFont*> PHFont::fonts;

PHFont::PHFont(const string & path)
{
    
}


PHFont* PHFont::fontFromPath(const string & path)
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
		fnt = fonts[path];
	}
	return fnt;
}

PHFont* PHFont::fontNamed(const string & name)
{
    return PHFont::fontFromPath(PHFileManager::singleton()->resourcePath()+"/fnt/"+name+".fnt");
}

void PHFont::collectGarbage()
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