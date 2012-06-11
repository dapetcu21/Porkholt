/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHFILEMANAGER_H
#define PHFILEMANAGER_H
#include <Porkholt/Core/PHMain.h>

class PHFileManager: public PHObject
{
public:
	static size_t fileSize(const string & path);
	static bool fileExists(const string & path);
    static bool isDirectory(const string & path);
    static uint8_t * loadFile(const string & path, size_t & size);
};

#endif
