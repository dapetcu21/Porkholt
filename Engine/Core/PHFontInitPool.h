//
//  PHFontInitPool.h
//  Porkholt
//
//  Created by Marius Petcu on 1/30/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#ifndef PHFONTINITPOOL_H
#define PHFONTINITPOOL_H

#include "PHMain.h"

class PHFont;
class PHFontInitPool
{
public:
    PHFont * fontFromPath(const string & path);
    PHFont * fontNamed(const string & name);
    void collectGarbageFonts();
    virtual const string fontDirectory();
private:
    map<string,PHFont*> fonts;
};

#endif
