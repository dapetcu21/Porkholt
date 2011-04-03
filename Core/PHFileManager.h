/*
 *  PHFileManager.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#ifndef PHFILEMANAGER_H
#define PHFILEMANAGER_H
#include "PHMain.h"

class PHFileManager: public PHObject
{
private:
	PHFileManager() {};
public:
	static PHFileManager * singleton();
	const string & resourcePath();
	size_t fileSize(const string & path);
	bool fileExists(const string & path);
	uint8_t * loadFile(const string & path, size_t & size);
};

#endif
