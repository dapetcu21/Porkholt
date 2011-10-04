/*
 *  PHEventHandler.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/18/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef PHEVENTHANDLER_H
#define PHEVENTHANDLER_H

#include "PHMain.h"

class PHEvent;
class PHView;
class PHGameManager;

class PHEventHandler : public PHObject
{
private:
	list<PHEvent*> touches;
    set<PHView*> mtviews;
	PHEventHandler(PHGameManager * gm) : gameManager(gm) {};
	PHEvent * touchForUserData(void * ud, list<PHEvent*>::iterator & i);
    PHGameManager * gameManager;
    
    friend class PHGameManager;
    
public:
	void touchDown(PHPoint pnt,void * ud);
	void touchUp(PHPoint pnt, void * ud);
	void touchMoved(PHPoint pnt, void * ud);
	void touchCancelled(PHPoint pnt, void *ud);
    void scrollWheel(PHPoint pnt, PHPoint delta, void *ud);
    void pinchZoom(PHPoint pnt, double zoom, void *ud);
    void pinchRotate(PHPoint pnt, double rotation, void *ud);
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