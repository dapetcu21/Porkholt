/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGLEVELCONTROLLER_H
#define IGLEVELCONTROLLER_H

#include <Porkholt/Core/PHViewController.h>

class IGScripting;
class IGWorld;
class IGBackground;
class PHDirectory;
class PHAnimatorPool;

class IGLevelController : public PHViewController
{
    protected:
        IGScripting * scripting;
        IGWorld * world;
        IGBackground * bg;
        PHDirectory * dir;
        PHAnimatorPool * animpool;
    public:
        IGLevelController(PHGameManager * gm, PHDirectory * lvlDir);
        ~IGLevelController();

        void updateScene(ph_float elapsed);
        PHView * loadView(const PHRect & f);
};

#endif
