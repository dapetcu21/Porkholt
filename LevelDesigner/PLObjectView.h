//
//  PLObjectView.h
//  LevelDesigner
//
//  Created by Marius Petcu on 10/5/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PLOBJECTVIEW_H
#define PLOBJECTVIEW_H

@class PLObject;
@class WorldController;
#include "PHView.h"

class PLObjectView : public PHView
{
protected:
    PLObject * model;
    void draw();
    bool sel;
    WorldController * worldController;
    bool moving,rotating;
    
    
    void touchEvent(PHEvent * evt);
    
public:
    PLObject * modelObject() { return model; }
    PLObjectView(PLObject * model);
    ~PLObjectView();
    
    void modelChanged();
    
    bool selected() { return sel; }
    void setSelected(bool s) { sel = s; }
    
    WorldController * controller() { return worldController; }
    void setController(WorldController *c) { worldController = c; }
    
    bool intersectsRect(const PHRect & rect);
};

#endif
