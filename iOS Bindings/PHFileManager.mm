/*
 *  PHFileManager.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#import <UIKit/UIKit.h>
#include "PHFileManager.h"
#include <fstream>
#include <sys/stat.h>

const string & PHFileManager::resourcePath()
{
	static string str = (string)([[[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"rsrc"] UTF8String]);
	return str;
}

bool PHFileManager::fileExists(const string & path)
{
	struct stat results;
    return (stat(path.c_str(), &results) == 0);
}


size_t PHFileManager::fileSize(const string & path)
{
    struct stat results;
    if (stat(path.c_str(), &results) == 0)
        return results.st_size;
	else
	{
		throw PHIOError;
	}
}

uint8_t * PHFileManager::loadFile(const string & path, size_t & size)
{
	ifstream file;
	file.open (path.c_str(), ios::in | ios::binary);
	file.seekg (0, ios::end);
	size = file.tellg();
	file.seekg (0, ios::beg);
	if (!file.good())
		throw PHIOError;
	uint8_t * buf = new uint8_t[size];
	file.read((char*)buf, size);
	if (!file.good())
	{
		delete[] buf;
		throw PHIOError;
	}
	return buf;
}

PHFileManager * PHFileManager::singleton()
{
	static PHFileManager * obj = NULL;
	if (!obj)
		obj = new PHFileManager();
	return obj;
}