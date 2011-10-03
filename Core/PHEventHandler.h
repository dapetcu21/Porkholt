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

class PHTouch;
class PHView;
class PHGameManager;

class PHEventHandler : public PHObject
{
private:
	list<PHTouch*> touches;
	PHEventHandler(PHGameManager * gm) : gameManager(gm) {};
	PHTouch * touchForUserData(void * ud, list<PHTouch*>::iterator & i);
    PHGameManager * gameManager;
    
    friend class PHGameManager;
    
public:
	void touchDown(PHPoint pnt,void * ud);
	void touchUp(PHPoint pnt, void * ud);
	void touchMoved(PHPoint pnt, void * ud);
	void touchCancelled(PHPoint pnt, void *ud);
	void removeView(PHView * view);
};

#endif