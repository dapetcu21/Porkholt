/*
 *  PHLPlayer.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/21/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#ifndef PHLPLAYER_H
#define PHLPLAYER_H

#include "PHMain.h"

class PHTrailImageView;
class PHLPlayer : public PHLObject
{
private:
    PHTrailImageView * bodyView;
    PHView * worldView;
public:
	PHLPlayer();
	virtual ~PHLPlayer();
	
	void updateControls(list<PHPoint> * queue);
	
	virtual void loadFromLua(void * L, const string & root,b2World * world);
	virtual void loadView();
};

#endif