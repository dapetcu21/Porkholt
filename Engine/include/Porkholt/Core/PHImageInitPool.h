/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHIMAGEINITPOOL_H
#define PHIMAGEINITPOOL_H

#include <Porkholt/Core/PHMain.h>

class PHImage;
class PHGameManager;
class PHInode;
class PHDirectory;

class PHImageInitPool
{
private:
    string hdsuf;
    map<PHHashedString, PHImage*> images;
public:
    virtual PHDirectory * imageDirectory() = 0;
    PHImage * imageFromFile(PHInode * file, bool antialiasing);
    PHImage * imageNamed(const string & name, PHDirectory * dir, bool normalMap);
	PHImage * imageNamed(const string & name, bool normalMap) { return imageNamed(name, imageDirectory(), normalMap); }
    PHImage * imageNamed(const string & name, PHDirectory * dir) { return imageNamed(name, dir, false); }
    PHImage * imageNamed(const string & name) { return imageNamed(name, imageDirectory(), false); }
    
    bool imageExists(const string & name, PHDirectory * dir);
    bool imageExists(const string & name) { return imageExists(name, imageDirectory()); }
    void loadAllImages();
	void collectGarbageImages();
    virtual PHGameManager * gameManager() = 0;

    const string & platformSuffix() { return hdsuf; }
    void setPlatformSuffix(const string & s) { hdsuf = s; }
};

#endif
