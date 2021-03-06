/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHAUXLAYERVIEW_H
#define PHAUXLAYERVIEW_H

#include <Porkholt/Core/PHView.h>

class PHAuxLayerView : public PHView
{
private:
    int _name;
    list<PHView*> views;
    
    void addView(PHView * view);
    void removeView(PHView * view);
    
    static map<int,PHAuxLayerView*> ilist;
    
    friend class PHView;
    
protected:
    virtual void draw();
    
public:
    PHAuxLayerView(int name);
    virtual ~PHAuxLayerView();
    
    static PHAuxLayerView * auxLayerViewWithName(int n);
};

#endif
