/*
 *  PHFileManager.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef PHFILEMANAGER_H
#define PHFILEMANAGER_H
#include "PHMain.h"

class PHFileManager: public PHObject
{
public:
    static const string & resourcePath();
	static size_t fileSize(const string & path);
	static bool fileExists(const string & path);
    static bool isDirectory(const string & path);
    static uint8_t * loadFile(const string & path, size_t & size);
};

#endif
