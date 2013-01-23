/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHFONTINITPOOL_H
#define PHFONTINITPOOL_H

#include <Porkholt/Core/PHMain.h>

class PHFont;
class PHDirectory;
class PHFile;
class PHGameManager;
class PHFontInitPool
{
private:
    PHFont * _defaultFont;
    map<string,PHFont*> fonts;
public:
    PHFontInitPool() : _defaultFont((PHFont*)(size_t)-1) {}
    PHFont * fontFromFile(PHFile * file);
    PHFont * fontNamed(const string & name);
    PHFont * defaultFont();
    void collectGarbageFonts();
    virtual PHDirectory * fontDirectory() = 0;
    virtual PHGameManager * gameManager() = 0;
};

#endif

