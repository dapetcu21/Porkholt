/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHWORLD_H
#define PHWORLD_H

#include <Porkholt/Core/PHMain.h>
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
class PHAnimatorPool;
class PHLNPC;
class PHDialog;
class PHScripting;
class PHTextView;
class PHHeartView;
class PHGameManager;
class PHImage;
class PHSound;
class PHSoundPool;
class PHDirectory;

class PHWorld : public PHObject 
{
private:
	PHCaptureView * view;
	PHView * worldView;
	PHView * layerView;
    PHAnimatorPool * modelQueue; 
    PHAnimatorPool * viewQueue;
    PHAnimatorPool * realQueue;
    PHSoundPool * sndPool;
	
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
    
	PHGaugeView * jumpGaugeView;
    PHHeartView * heartView;
	
	
	struct layer
	{
		PHView * container;
		float scale;
	};
	list<layer> layers;
    
    PHScripting * scripting;
    
    PHGameManager * gm;
	
public:
	PHWorld::layer * addLayer(float scale);
	void addToLayer(layer * lyr, PHImageView * img);
	
    void setScripting(PHScripting * s) { scripting = s; } 
    PHScripting * getScripting() { return scripting; }
    
	PHWorld(PHGameManager * mgr, const PHRect & size,PHLevelController * cnt);
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
    void updatePhysics();
	void updateScene(float elapsed, float interpolate);
    void updateTimers(float frameInterval);
    
    b2World * getPhysicsWorld() { return physicsWorld; }
        
    PHLevelController * levelController() { return controller; }
    PHAnimatorPool * viewEventQueue() { return viewQueue; }
    PHAnimatorPool * modelEventQueue() { return modelQueue; }
    PHAnimatorPool * realTimeEventQueue() { return realQueue; }
    PHSoundPool * soundPool() { return sndPool; }
    
    PHLCamera * getCamera() { return camera; }
    PHLPlayer * getPlayer() { return player; }
    PHView * getWorldView() { return worldView; }
    
    PHGameManager * gameManager() { return gm; }
    
public:
    
    void fadeToColor(const PHColor & color) { fadeToColor(color,NULL); }
    void fadeToColor(const PHColor & color, void * ud);
    void dismissFading() { dismissFading(NULL); }
    void dismissFading(void * ud);
    
    void overlayText(const string & s, float duration);
    
    void advanceDialog();
    void updateDialogs();
    void addDialog(PHDialog* d);
    
    void boom(const PHPoint & location, float magnitude, float damage, float radius);
    
    PHDirectory * resourceDirectory();
    string resourcePath();
    //void printObjects();
    
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
    
    set<PHImage*> preloadedImages;
    set<PHSound*> preloadedSounds;
public:
    void preloadAssets();
    void unloadAssets();
  
};

#endif
