/*
 *  PHEventHandler.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/18/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#ifndef PHEVENTHANDLER_H
#define PHEVENTHANDLER_H

#include "PHMain.h"

class PHTouch;
class PHView;

class PHEventHandler : public PHObject
{
public:
	static PHEventHandler * sharedInstance()
	{
		static PHEventHandler * obj = new PHEventHandler();
		return obj;
	}
private:
	list<PHTouch*> touches;
	PHEventHandler() {};
	PHTouch * touchForUserData(void * ud, list<PHTouch*>::iterator & i);
	
public:
	void touchDown(PHPoint pnt,void * ud);
	void touchUp(PHPoint pnt, void * ud);
	void touchMoved(PHPoint pnt, void * ud);
	void touchCancelled(PHPoint pnt, void *ud);
	void removeView(PHView * view);
};

#endif