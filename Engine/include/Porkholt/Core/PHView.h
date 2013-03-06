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
    ph_float _ZPosition;
    bool _blendingEnabled, _zTestingEnabled;
	PHPoint _rotationalCenter,_scalingCenter, _flipCenter;
    bool fhoriz,fvert;
	ph_float _rotation,_scaleX,_scaleY;
	ph_float _alpha;
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
    PHMatrix loadMatrixTree(PHView * until);
	
    void handleEvent(PHEvent * evt);
    PHPositionalVector positionInMyCoordinates(PHDrawableCoordinates * d);
    PHPositionalVector positionInParent(PHDrawableCoordinates * d, PHPositionalVector & p);
public:
	
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
    void setPosition3D(const PH3DPoint &pos) { setPosition(pos.xy()); _ZPosition = pos.z; matrixCached = false; }
    PHPoint position() { return PHPoint(_frame.x,_frame.y); }
    PH3DPoint position3D() { return PH3DPoint(_frame.x, _frame.y, _ZPosition); }
    ph_float zPosition() { return _ZPosition; }
    void setZPosition(ph_float p) { _ZPosition = p; effectCached = false; }
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

	void setOptimizations(bool ui) { _optimize = ui; };
	bool optimizations() { return _optimize; };

    void setZTestingEnabled(bool z) { _zTestingEnabled = z; }
    bool zTestingEnabled() { return _zTestingEnabled; }
    void setBlendingEnabled(bool b) { _blendingEnabled = b; }
    bool blendingEnabled() { return _blendingEnabled; }
    
    void setAutoresizesSubviews(bool b) { autoresize = b; }
    bool autoresizesSubviews() { return autoresize; }
    int autoresizeMask() { return resizeMask; }
    void setAutoresizeMask(int m) { resizeMask = m; }
	
	virtual ~PHView();
	friend class PHGameManager;
	friend class PHEventHandler;
	
    virtual void layoutSubviews(const PHRect & oldBounds);
    void autoresizeMyself(const PHRect & parentBounds, const PHSize & delta);
//animation system
protected:
    virtual void setAnimationFieldF(int field, ph_float v);
    virtual void setAnimationFieldV2(int field, const PHVector2 & v);
    virtual void setAnimationFieldC(int field, const PHColor & v);
    
    virtual ph_float getAnimationFieldF(int field);
    virtual PHVector2 getAnimationFieldV2(int field);
    virtual PHColor getAnimationFieldC(int field);
 
public:
    void animateBgColor(const PHColor & clr) { animateField(PHAnimationField(PHCinematicActor::fieldBgColor, clr)); }

protected:
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
