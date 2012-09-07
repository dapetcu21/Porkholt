/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHEVENTHANDLER_H
#define PHEVENTHANDLER_H

#include <Porkholt/Core/PHMain.h>

class PHEvent;
class PHDrawable;
class PHGameManager;
class PHMutex;

class PHEventHandler : public PHObject
{
private:
	map<void*,PHEvent*> events;
    set<PHDrawable*> mtlisteners;
    list<PHEvent*> evtqueue;
    PHMutex * mutex;

	PHEventHandler(PHGameManager * gameManager);
    ~PHEventHandler();

	PHEvent * eventForUD(void * ud);
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
	
    void addEvent(PHEvent * evt);
    void processQueue(); 

    enum modifiers
    {
        shiftModifier = (1<<0),
        optionModifier = (1<<1),
        controlModifier = (1<<2),
        commandModifier = (1<<3)
    };
    static int modifierMask();
    
    void registerForMultitouchEvents(PHDrawable*);
    void unregisterForMultitouchEvents(PHDrawable*);
    void removeDrawable(PHDrawable*);

};

#endif
