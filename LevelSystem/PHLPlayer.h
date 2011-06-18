/*
 *  PHLPlayer.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/21/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef INSIDE_PHLOBJECT_H
#ifndef PHLPLAYER_H
#define PHLPLAYER_H

#include "PHMain.h"

class PHTrailImageView;
class PHLPlayer : public PHLObject
{
private:
    PHTrailImageView * bodyView;
    PHImageView * faceView;
    PHView * worldView;
    PHPoint normal;
    double touchesSomething;
    int forceGap;
public:
	PHLPlayer();
	virtual ~PHLPlayer();
	
    bool isUsingForce() { return forceGap; }
    
	void updateControls(list<PHPoint> * queue);
	
	virtual void loadFromLua(void * L, const string & root,b2World * world);
	virtual void loadView();
    
    virtual void contactPostSolve(bool b,b2Contact* contact, const b2ContactImpulse* impulse);
};

#endif
#endif