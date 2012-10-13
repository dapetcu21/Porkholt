/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHAnimator.h>

class PHGameManager;
class PHDirectory;
class IGObject;
class PHView;

class IGWorld : public PHAnimator
{
    protected:
        list<IGObject*> _objects;
        PHView * _view;
        PHGameManager * gm;
        PHDirectory * dir;
    public:
        IGWorld(PHGameManager * gm, PHDirectory * dir, const PHRect & size);
        ~IGWorld();

        const list<IGObject*> & objects() { return _objects; }
        PHView * view() { return _view; }
        
        void advanceAnimation(ph_float elapsed);
};
