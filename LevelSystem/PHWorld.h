/*
 *  PHWorld.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/5/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef PHWORLD_H
#define PHWORLD_H

#include "PHMain.h"
#include "PHLObject.h"
#include "PHJoint.h"

class PHLCamera;
class PHLPlayer;
class PHCaptureView;
class PHGaugeView;
class b2World;
class b2ContactFilter;
class b2ContactListener;
class PHTimer;
class PHLevelController;
class PHEventQueue;
class PHLNPC;
class PHDialog;
class PHScripting;
class PHTextView;
class PHHeartView;

class PHWorld : public PHObject 
{
private:
	PHCaptureView * view;
	PHView * worldView;
	PHView * layerView;
    PHEventQueue * modelQueue; 
    PHEventQueue * viewQueue;
	
	PHLevelController * controller;
	
	PHLCamera * camera;
	PHLPlayer * player;
	
	PHRect worldSize;
	
	b2World * physicsWorld;
    b2ContactFilter * contactFilter;
    b2ContactListener * contactListener;
	
	vector<PHLObject*> objects;
    vector<PHJoint*>joints;
	list<PHPoint> eventQueue;
	
	
	friend class PHLevelController;
	friend class PHJoint;
    friend class PHScripting;
    
	double _jumpGauge,maxJump,jumpGrowth;
	PHGaugeView * jumpGaugeView;
    PHHeartView * heartView;
	
	
	struct layer
	{
		PHView * container;
		double scale;
	};
	list<layer> layers;
    
    list<PHTimer*>timers;
    
    PHScripting * scripting;
	
public:
	PHWorld::layer * addLayer(double scale);
	void addToLayer(layer * lyr, PHImageView * img);
	
    void setScripting(PHScripting * s) { scripting = s; } 
    
	PHWorld(const PHRect & size,PHLevelController * cnt);
	PHView * getView() { return (PHView *)view; }
	virtual ~PHWorld();
	
	void addObject(PHLObject * obj);
	void removeObject(PHLObject * obj);
	void removeAllObjects();
    void insertObjectAtPosition(PHLObject * obj, int insPos, PHLObject * insObj);
    
    void addJoint(PHJoint * obj);
	void removeJoint(PHJoint * obj);
	void removeAllJoints();
    
    void updatePositions();
	void updateScene();
	
	double jumpGauge() { return _jumpGauge; }
	void setJumpGauge(double j) { _jumpGauge = j; }
	double maxJumpGauge() { return maxJump; }
	void setMaxJumpGauge(double j) { maxJump = j; }
	double jumpGaugeGrowth() { return jumpGrowth; }
	void setJumpGaugeGrowth(double g) { jumpGrowth = g; }
    
    b2World * getPhysicsWorld() { return physicsWorld; }
    
    void scheduleTimer(PHTimer * timer);
    void updateTimers(double timeElapsed);
    
    PHLevelController * levelController() { return controller; }
    PHEventQueue * viewEventQueue() { return viewQueue; }
    PHEventQueue * modelEventQueue() { return modelQueue; }
    
    PHLCamera * getCamera() { return camera; }
    PHLPlayer * getPlayer() { return player; }
    PHView * getWorldView() { return worldView; }
    
public:
    
    void fadeToColor(const PHColor & color) { fadeToColor(color,NULL); }
    void fadeToColor(const PHColor & color, void * ud);
    void dismissFading() { dismissFading(NULL); }
    void dismissFading(void * ud);
    
    void overlayText(const string & s, double duration);
    
    void advanceDialog();
    void updateDialogs();
    void addDialog(PHDialog* d);
    
private:
    void fadedToColor(PHObject * obj, void * ud);
    void _fadedToColor(PHObject * obj, void * ud);
    void _fadeToColor(PHObject * sender, void * ud);
    void dismissOverlayText();
    void _overlayDismissed(PHObject * obj, void * ud);
    void _dismissFading(PHObject * sender, void * ud);
    PHColor dimColor;
    PHView * dimView;
    PHTextView * overlayView;
    
    list<PHDialog*> dialogs;
    PHDialog * currentDialog;
    PHLNPC * dialogInitiator;
  
};

#endif