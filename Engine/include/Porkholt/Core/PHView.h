/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHVIEW_H
#define PHVIEW_H

#include <Porkholt/Core/PHDrawable.h>
#include <Porkholt/Core/PHCinematicActor.h>
#include <Porkholt/Core/PHCinematicAnimator.h>
#include <Porkholt/Core/PHEvent.h>

class PHMutex;
class PHAuxLayerView;
class PHView : public PHDrawable, public PHCinematicActor
{
protected:
	PHRect _frame,_bounds;
	PHPoint _rotationalCenter,_scalingCenter, _flipCenter;
    bool fhoriz,fvert;
	ph_float _rotation,_scaleX,_scaleY;
	ph_float _alpha;
	bool _userInput;
	bool _optimize;
	PHColor _backColor;
	int effOrder;
    
	void drawBackground();
public:
    virtual void auxRender();
	virtual void render();
    
private:
    PHMatrix effectCache;
    PHMatrix matrixCache;
    bool effectCached;
    bool matrixCached;
    
    bool autoresize;
    int resizeMask;
public:
	PHMatrix applyMatrices();
	
public:
    virtual void touchEvent(PHEvent * touch) {};
    PHMatrix loadMatrixTree(PHView * until);
	
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
    
    enum ResizeMask
    {
        ResizeFixedLeft = 1<<0,
        ResizeFixedRight = 1<<1,
        ResizeFixedDown = 1<<2,
        ResizeFixedUp = 1<<3,
        ResizeFlexibleWidth = 1<<4,
        ResizeFlexibleHeight = 1<<5,
        ResizeStatic = ResizeFixedLeft | ResizeFixedDown,
        ResizeFlexibleAll = ResizeFlexibleWidth | ResizeFlexibleHeight,
        ResizeFlexibleBorders = 0,
        ResizeAll = ResizeFixedLeft | ResizeFixedRight | ResizeFixedDown | ResizeFixedUp | ResizeFlexibleWidth | ResizeFlexibleHeight
    };
	
	PHView();
	PHView(const PHRect &frame);
    
    PHView * viewWithTag(int tag);
    PHView * viewWithTagAfter(int tag, PHDrawable * after);
    list<PHView*> * viewsWithTag(int tag); //this returns a new-allocated list
    
    PHView * superview() { if (_parent->isView()) return (PHView*)_parent; return NULL; };
	
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
    void setFlipCenter(const PHPoint &center) { _flipCenter = center; effectCached = false; }
    bool horizontallyFlipped() const { return fhoriz; }
    bool verticallyFlipped() const { return fvert; }
    void setHorizontallyFlipped(bool b) { fhoriz = b; effectCached = false; }
    void setVerticallyFlipped(bool b) { fvert = b; effectCached = false; }  
	void setRotationalCenter(const PHPoint &center) { _rotationalCenter = center; effectCached = false; }
	void setScalingCenter(const PHPoint &center) { _scalingCenter = center; effectCached = false; }
	ph_float rotation() { return _rotation; };
	virtual void setRotation(ph_float rot) { _rotation = rot; effectCached = false; };
	ph_float scaleX() { return _scaleX; };
	void setScaleX(ph_float scale) { _scaleX = scale; effectCached = false; };
	ph_float scaleY() { return _scaleY; };
	void setScaleY(ph_float scale) { _scaleY = scale; effectCached = false; };
    void setScale(const PHSize & s) { _scaleX = s.x, _scaleY = s.y; effectCached = false; }
	int effectOrder() { return effOrder; };
	void setEffectOrder(int eff) { effOrder = eff; effectCached = false; };
	void rotate(ph_float rot) { _rotation+= rot; effectCached = false; };
	void setAlpha(ph_float alpha) { _alpha = alpha; if (alpha<0) alpha = 0; if (alpha>1) alpha = 1; };
	ph_float alpha() { return _alpha; }
	void setBackgroundColor(const PHColor &color) { _backColor = color; };
	PHColor backgroundColor() { return _backColor; };
	void setUserInput(bool ui) { _userInput = ui; };
	bool userInput() { return _userInput; };
	void setOptimizations(bool ui) { _optimize = ui; };
	bool optimizations() { return _optimize; };
    
    void setAutoresizesSubviews(bool b) { autoresize = b; }
    bool autoresizesSubviews() { return autoresize; }
    int autoresizeMask() { return resizeMask; }
    void setAutoresizeMask(int m) { resizeMask = m; }
	
	PHPoint toMyCoordinates(const PHPoint & pnt, PHView * until);
	void toMyCoordinates(PHPoint * pnt, int n, PHView * until);
	PHPoint fromMyCoordinates(const PHPoint & pnt, PHView * until);
	void fromMyCoordinates(PHPoint * pnt, int n, PHView * until);
    
	PHPoint toMyCoordinates(const PHPoint & pnt) { return toMyCoordinates(pnt, NULL); }
	void toMyCoordinates(PHPoint * pnt, int n) { toMyCoordinates(pnt, n, NULL); }
	PHPoint fromMyCoordinates(const PHPoint & pnt) { return fromMyCoordinates(pnt, NULL); }
	void fromMyCoordinates(PHPoint * pnt, int n) { fromMyCoordinates(pnt, n, NULL); }
	
	virtual ~PHView();
	friend class PHGameManager;
	friend class PHEventHandler;
	
private:
    PHView * pointerDeepFirst(const PHMatrix & m, PHEvent * touch);
    
    virtual void layoutSubviews(const PHRect & oldBounds);
    void autoresizeMyself(const PHSize & delta);
//animation system
protected:
    void setCinematicPosition(const PHPoint & p) { setPosition(p); }
    PHPoint cinematicPosition() { return position(); }
    void setCinematicRotation(ph_float r) { setRotation(r); }
    ph_float cinematicRotation() { return rotation(); }
    void setCinematicScale(const PHSize & s) { setScaleX(s.x); setScaleY(s.y); }
    PHSize cinematicScale() { return PHSize(scaleX(),scaleY()); }
    void setCinematicBgColor(const PHColor & c) { setBackgroundColor(c); }
    PHColor cinematicBgColor() { return backgroundColor(); }

    virtual void attachedToGameManager();

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
    
    static void registerLuaInterface(lua_State * L);
    
private:
    static const string _luaClass;
};

#endif
