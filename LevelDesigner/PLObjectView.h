//
//  PLObjectView.h
//  LevelDesigner
//
//  Created by Marius Petcu on 10/5/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PLOBJECTVIEW_H
#define PLOBJECTVIEW_H

#ifdef __OBJC__
@class PLObject;
@class WorldController;
#else
#define PLObject void
#define WorldController void
#endif

#include "PHView.h"
class PLObjectMarker;

class PLObjectView : public PHView
{
protected:
    PLObject * model;
    bool sel;
    WorldController * worldController;
    bool moving,rotating;
    
    void touchEvent(PHEvent * evt);
    void reloadSubviews();
    
    PLObjectMarker * marker;
    
public:
    PLObject * modelObject() { return model; }
    PLObjectView(PLObject * model);
    ~PLObjectView();
    
    void modelChanged();
    
    bool selected() { return sel; }
    void setSelected(bool s) { sel = s; }
    bool readOnly();
    
    WorldController * controller() { return worldController; }
    void setController(WorldController *c) { worldController = c; }
    
    bool intersectsRect(const PHRect & rect);
    
    void flagsChanged();
    
    static bool rectsIntersect(PHView * base, const PHRect & r1, PHView * v1, const PHRect & r2, PHView * v2);
    
    void startMoving();
    void stopMoving();
    void startRotating();
    void stopRotating();
    void moveSubviews(PHPoint delta);
    void rotateSubviews(double ammount);
};

#endif
