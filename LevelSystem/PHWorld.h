/*
 *  PHWorld.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/5/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#ifndef PHWORLD_H
#define PHWORLD_H

class PHWorld : public PHObject 
{
private:
	PHView * view;
	PHView * worldView;
public:
	PHWorld(const PHRect & size);
	PHView * getView() { return view; }
	virtual ~PHWorld();
	
	void updateScene(double time);
};

#endif