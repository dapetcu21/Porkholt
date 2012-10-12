/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHViewController.h>

class IGScripting;
class IGWorld;
class PHDirectory;

class IGLevelController : public PHViewController
{
    protected:
        IGScripting * scripting;
        IGWorld * world;
        PHDirectory * dir;
    public:
        IGLevelController(PHGameManager * gm, PHDirectory * lvlDir);
        ~IGLevelController();

        void updateScene(ph_float elapsed);
        PHView * loadView(const PHRect & f);
};
