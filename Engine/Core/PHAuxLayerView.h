//
//  PHAuxLayerView.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/18/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHAUXLAYERVIEW_H
#define PHAUXLAYERVIEW_H

#include "PHView.h"

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