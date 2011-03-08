/*
 *  PHLCamera.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/6/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#ifndef PHLCAMERA_H
#define PHLCAMERA_H

class PHLCamera : public PHLObject
{
protected:
	PHRect sz;
public:
	PHLCamera();
	virtual ~PHLCamera();
	
	PHRect size() { return sz; };
	
	virtual void loadFromLUA(void * L, const string & root);
	virtual void loadView();
};

#endif
