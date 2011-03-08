/*
 *  PHLevelController.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/5/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#ifndef PHLEVELCONTROLLER_H
#define PHLEVELCONTROLLER_H

class PHWorld;
class PHButtonView;

class PHLevelController : public PHViewController
{
protected:
	virtual PHView * loadView(const PHRect & frame);
	PHWorld * world;
	PHMutex * mutex;
	PHMutex * pauseMutex;
	PHThread * thread;
	volatile bool running;
	bool paused;
	string directory;
public:
	
	void pause();
	void resume();
	
	void auxThread(PHThread * sender, void * ud);
	
	PHLevelController(string path);
	virtual void updateScene(double timeElapsed);
	
	~PHLevelController();
	
	void test(PHButtonView * sender, void * ud);
	
	void viewDidAppear();
	void viewWillDisappear();
	
};

#endif