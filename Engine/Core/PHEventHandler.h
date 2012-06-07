/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHEVENTHANDLER_H
#define PHEVENTHANDLER_H

#include "PHMain.h"

class PHEvent;
class PHView;
class PHGameManager;

class PHEventHandler : public PHObject
{
private:
	map<void*,PHEvent*> events;
    set<PHView*> mtviews;
	PHEventHandler(PHGameManager * gameManager) : gm(gameManager) {};
	PHEvent * touchForUserData(void * ud, map<void*,PHEvent*>::iterator & i);
    PHGameManager * gm;
    
    friend class PHGameManager;
    
public:
	void touchDown(PHPoint pnt,void * ud);
	void touchUp(PHPoint pnt, void * ud);
	void touchMoved(PHPoint pnt, void * ud);
	void touchCancelled(PHPoint pnt, void *ud);
    void scrollWheel(PHPoint pnt, PHPoint delta, void *ud);
    void pinchZoom(PHPoint pnt, ph_float zoom, void *ud);
    void pinchRotate(PHPoint pnt, ph_float rotation, void *ud);
    void multitouchBegin(void *ud);
    void multitouchEnd(void *ud);
	
    enum modifiers
    {
        shiftModifier = (1<<0),
        optionModifier = (1<<1),
        controlModifier = (1<<2),
        commandModifier = (1<<3)
    };
    static int modifierMask();
    
    void registerViewForMultitouchEvents(PHView*);
    void unregisterViewForMultitouchEvents(PHView*);
    void removeView(PHView * view);
};

#endif
