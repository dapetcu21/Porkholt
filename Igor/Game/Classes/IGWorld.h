/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGWORLD_H
#define IGWORLD_H

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
        void insertObject(IGObject * obj, bool before = true, IGObject * ref = NULL);
        void removeObject(IGObject * obj);
        PHView * view() { return _view; }

        PHGameManager * gameManager() { return gm; }
        
        void advanceAnimation(ph_float elapsed);
};

#endif
