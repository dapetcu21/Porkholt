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

class PHLCamera;
class PHLObject;

class PHWorld : public PHObject 
{
private:
	PHView * view;
	PHView * worldView;
	
	PHLCamera * camera;
	PHLObject * player;
	
	PHRect worldSize;
public:
	PHWorld(const PHRect & size);
	PHView * getView() { return view; }
	virtual ~PHWorld();
	
	list<PHLObject*> objects;
	void addObject(PHLObject * obj);
	void removeObject(PHLObject * obj);
	
	void updateScene(double time);
};

#endif