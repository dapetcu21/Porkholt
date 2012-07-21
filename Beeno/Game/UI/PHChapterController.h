/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHCHAPTERCONTROLLER_H

#include <Porkholt/Core/PHViewController.h>

class PHImage;
class PHDirectory;

class PHChapterController : public PHViewController
{
private:
    PHDirectory * path;
    PHImage * bg;
protected:
    virtual PHView * loadView(const PHRect & frame);
    void mouseUp(PHObject * sender, void * ud);
    void levelEnded(PHObject * sender, void *ud);
    void backPressed(PHObject * sender, void *ud);
    void loadLevel(int nr,bool replace);
public:
    PHChapterController(PHDirectory * path);
    virtual ~PHChapterController();
};

#endif
