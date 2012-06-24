/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHFontInitPool.h>
#include <Porkholt/Core/PHFont.h>
#include <Porkholt/IO/PHDirectory.h>
#include <Porkholt/IO/PHFile.h>

PHFont* PHFontInitPool::fontFromFile(PHFile * file)
{
    return new PHFont(gameManager(), file);
}

PHFont* PHFontInitPool::fontNamed(const string & name)
{
    PHFont * fnt;
    map<string,PHFont*>::iterator i = fonts.find(name);
    if (i==fonts.end())
    {
        PHFile * file = NULL;
        try {
            file = fontDirectory()->fileAtPath(name+".phf");
            fnt = PHFontInitPool::fontFromFile(file);
            file->release();
        } catch (string ex)
        {
            if (file)
                file->release();
            PHLog(ex);
            return NULL;
        }
        fonts[name] = fnt;
    } else
        fnt = i->second;
    return fnt;
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
