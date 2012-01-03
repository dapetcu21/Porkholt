/*
 *  PHView.h
 *  Porkholt
 *
 *  Created by Marius Petcu on 12/17/10.
 *  Copyright 2010 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef PHVIEW_H
#define PHVIEW_H

#include "PHMain.h"
#include "PHCinematicAnimator.h"
#include "PHEvent.h"

class PHMutex;
class PHAuxLayerView;
class PHGameManager;
class PHView : public PHObject, public PHCinematicActor
{
protected:
    list<PHView*> views;
    list<PHView*>::iterator currPos;
    
    PHGameManager * _gameManager;
    
	PHView * superView;
	PHRect _frame,_bounds;
	PHPoint _rotationalCenter,_scalingCenter, _flipCenter;
    bool fhoriz,fvert;
	double _rotation,_scaleX,_scaleY;
	double _alpha;
	bool _userInput;
	bool _optimize;
	PHColor _backColor;
	int effOrder;
    int _tag;
    PHMutex * mtx;
    
    virtual void auxRender();
	virtual void render();
	void drawBackground();
	virtual void draw();
	
	void apply_rotation();
	void apply_scaling();
	void applyMatrices();
	
public:
    virtual void touchEvent(PHEvent * touch);
    void loadMatrixTree(PHView * until);
	
    enum Effects
    {
        EffectScale = 1,
        EffectRotate = 2,
        EffectFlip = 3
    };
    
	enum EffectOrder
	{
		EffectOrderScaleRotateFlip = EffectScale | EffectRotate<<2 | EffectFlip<<4,
		EffectOrderRotateScaleFlip = EffectRotate | EffectScale<<2 | EffectFlip<<4,
        EffectOrderScaleFlipRotate = EffectScale | EffectFlip<<2 | EffectRotate<<4,
        EffectOrderRotateFlipScale = EffectRotate | EffectFlip<<2 | EffectScale<<4,
        EffectOrderFlipScaleRotate = EffectFlip | EffectScale<<2 | EffectRotate<<4,
		EffectOrderFlipRotateScale = EffectFlip | EffectRotate<<2 | EffectScale<<4
	};
	
	PHView();
	PHView(const PHRect &frame);
    
    const list<PHView*> & subViews() { return views; }
	
	virtual void setFrame(const PHRect &frame);
	void setPosition(const PHPoint &pos);
    PHPoint position() { return PHPoint(_frame.x,_frame.y); }
	PHRect frame() const { return _frame; }
	PHRect bounds() const { return _bounds; }
	virtual void setBounds(const PHRect &bnd);
	PHPoint center() const;
	void setCenter(const PHPoint &center);
	PHPoint boundsCenter() const;
	PHPoint rotationalCenter() const {return _rotationalCenter;}
	PHPoint scalingCenter() const {return _scalingCenter; }
    PHPoint flipCenter() const { return _flipCenter; };
    void setFlipCenter(const PHPoint &center) { _flipCenter = center; }
    bool horizontallyFlipped() const { return fhoriz; }
    bool verticallyFlipped() const { return fvert; }
    void setHorizontallyFlipped(bool b) { fhoriz = b; }
    void setVerticallyFlipped(bool b) { fvert = b; }  
	void setRotationalCenter(const PHPoint &center) { _rotationalCenter = center;}
	void setScalingCenter(const PHPoint &center) { _scalingCenter = center; }
	double rotation() { return _rotation; };
	virtual void setRotation(double rot) { _rotation = rot; };
	double scaleX() { return _scaleX; };
	void setScaleX(double scale) { _scaleX = scale; };
	double scaleY() { return _scaleY; };
	void setScaleY(double scale) { _scaleY = scale; };
	int effectOrder() { return effOrder; };
	void setEffectOrder(int eff) { effOrder = eff; };
	void rotate(double rot) { _rotation+= rot; };
	void setAlpha(double alpha) { _alpha = alpha; if (alpha<0) alpha = 0; if (alpha>1) alpha = 1; };
	double alpha() { return _alpha; }
	void setBackgroundColor(const PHColor &color) { _backColor = color; };
	PHColor backgroundColor() { return _backColor; };
	void setUserInput(bool ui) { _userInput = ui; };
	bool userInput() { return _userInput; };
	void setOptimizations(bool ui) { _optimize = ui; };
	bool optimizations() { return _optimize; };
	PHView * superview() { return superView; };
	
	void addSubview(PHView * view) { addSubviewBefore(view, NULL); }
    void addSubviewBefore(PHView * view, PHView * before);
	void removeFromSuperview();
    void removeAllSubviews();
	void bringToFront();
    void sendToBack();
    

	PHPoint toMyCoordinates(const PHPoint & pnt, PHView * until);
	void toMyCoordinates(PHPoint * pnt, int n, PHView * until);
	PHPoint fromMyCoordinates(const PHPoint & pnt, PHView * until);
	void fromMyCoordinates(PHPoint * pnt, int n, PHView * until);
    
	PHPoint toMyCoordinates(const PHPoint & pnt) { return toMyCoordinates(pnt, NULL); }
	void toMyCoordinates(PHPoint * pnt, int n) { toMyCoordinates(pnt, n, NULL); }
	PHPoint fromMyCoordinates(const PHPoint & pnt) { return fromMyCoordinates(pnt, NULL); }
	void fromMyCoordinates(PHPoint * pnt, int n) { fromMyCoordinates(pnt, n, NULL); }
	
    void setTag(int tag) { _tag = tag; };
    int tag() { return _tag; }
    PHView * viewWithTag(int tag);
    PHView * viewWithTagAfter(int tag, PHView * v);
    list<PHView*> * viewsWithTag(int tag); //this returns a new-allocated list
	
	virtual ~PHView();
	friend class PHGameManager;
	friend class PHEventHandler;
	
private:
    PHView * pointerDeepFirst(PHEvent * touch);
    
//animation system
protected:
    void setCinematicPosition(const PHPoint & p) { setPosition(p); }
    PHPoint cinematicPosition() { return position(); }
    void setCinematicRotation(double r) { setRotation(r); }
    double cinematicRotation() { return rotation(); }
    void setCinematicScale(const PHSize & s) { setScaleX(s.x); setScaleY(s.y); }
    PHSize cinematicScale() { return PHSize(scaleX(),scaleY()); }
    void setCinematicBgColor(const PHColor & c) { setBackgroundColor(c); }
    PHColor cinematicBgColor() { return backgroundColor(); }

//auxiliar view binding
protected:
    PHAuxLayerView * auxLayer;
    PHView * auxSuperview;
    bool drawingOnAuxLayer;
    bool dontDrawOnMain;
    
    friend class PHAuxLayerView;
public:
    void bindToAuxLayer(PHAuxLayerView * layer, PHView * from);
    void setDontDrawToMain( bool val) { dontDrawOnMain = val; }
//scripting
protected:
    set<lua_State*> luaStates;
    string luaClass;
public:
    void getLuaHandle(lua_State * L);
    static void registerLuaInterface(lua_State * L);
public:
    PHMutex * mutex() { if (!mtx) mtx = new PHMutex(true); return mtx; }
    
public:
    PHGameManager * gameManager() { return _gameManager; }
    void setGameManager(PHGameManager * gm);
};

#endif