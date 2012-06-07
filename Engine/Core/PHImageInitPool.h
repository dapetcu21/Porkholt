/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHIMAGEINITPOOL_H
#define PHIMAGEINITPOOL_H

#include "PHMain.h"

class PHImage;
class PHGameManager;
class PHImageInitPool
{
public:
    PHImage * imageFromPath(const string & path);
    bool imageExists(const string & name);
	PHImage * imageNamed(const string & name);
    void loadAllImages();
	void collectGarbageImages();
    virtual const string imageDirectory();
    virtual PHGameManager * gameManager() = 0;
private:
    map<string,PHImage*> images;
};

#endif
