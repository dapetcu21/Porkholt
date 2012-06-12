/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHFONTINITPOOL_H
#define PHFONTINITPOOL_H

#include <Porkholt/Core/PHMain.h>

class PHFont;
class PHGameManager;
class PHFontInitPool
{
public:
    PHFont * fontFromPath(const string & path);
    PHFont * fontNamed(const string & name);
    void collectGarbageFonts();
    virtual const string fontDirectory();
    virtual PHGameManager * gameManager() = 0;
private:
    map<string,PHFont*> fonts;
};

#endif
