/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHMain.h>

struct lua_State;
class PHDirectory;
class PHGameManager;
class IGWorld;

class IGScripting : public PHObject
{
    protected:
        lua_State * L;
        PHDirectory * dir;
        PHGameManager * gm;
        IGWorld * world;

        void loadCInterface();
    public:
        IGScripting(PHGameManager * gm, PHDirectory * dir, IGWorld * world);
        ~IGScripting();

        void frame(ph_float elapsed);
};
