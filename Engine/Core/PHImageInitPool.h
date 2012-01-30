//
//  PHImageInitPool.h
//  Porkholt
//
//  Created by Marius Petcu on 1/30/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#ifndef PHIMAGEINITPOOL_H
#define PHIMAGEINITPOOL_H

#include "PHMain.h"

class PHImage;
class PHImageInitPool
{
public:
    PHImage * imageFromPath(const string & path);
    bool imageExists(const string & name);
	PHImage * imageNamed(const string & name);
    void loadAllImages();
	void collectGarbageImages();
    virtual const string imageDirectory();
    
private:
    map<string,PHImage*> images;
};

#endif
