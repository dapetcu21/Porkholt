/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHCHAPTERCONTROLLER_H

#include "PHViewController.h"

class PHImage;
class PHChapterController : public PHViewController
{
private:
    string path;
    PHImage * bg;
protected:
    virtual PHView * loadView(const PHRect & frame);
    void mouseUp(PHObject * sender, void * ud);
    void levelEnded(PHObject * sender, void *ud);
    void backPressed(PHObject * sender, void *ud);
    void loadLevel(int nr,bool replace);
public:
    PHChapterController(const string & path);
    virtual ~PHChapterController();
};

#endif
