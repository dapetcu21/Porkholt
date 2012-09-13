/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHView.h>
#include <Porkholt/Core/PHAuxLayerView.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHEventHandler.h>
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHGLVertexArrayObject.h>
#include <Porkholt/Core/PHMutex.h>
#include <Porkholt/Core/PHDrawableCoordinates.h>

const string PHView::_luaClass("PHView");

#define PHVIEW_INITLIST _bounds(PHRect(0, 0, -1, -1)), fhoriz(false), fvert(false), _rotation(0), _scaleX(1), _scaleY(1), _alpha(1.0f), _optimize(false), _backColor(PHClearColor), effOrder(EffectOrderScaleRotateFlip), effectCached(false), matrixCached(false), autoresize(false), resizeMask(ResizeStatic), auxLayer(NULL), auxSuperview(NULL), drawingOnAuxLayer(false), dontDrawOnMain(true)

PHView::PHView() :  PHVIEW_INITLIST
{
    luaClass = &_luaClass;
    _isView = true;
	setFrame(PHRect(0, 0, 0, 0));
}

PHView::PHView(const PHRect &frame) : PHVIEW_INITLIST
{
	luaClass = &_luaClass;
    _isView = true;
    setFrame(frame);
}

void PHView::setFrame(const PHRect &frame)
{
	_frame = frame;
	if (_bounds.width != frame.width || _bounds.height != frame.height)
        setBounds(PHRect(0,0,frame.width,frame.height));
    matrixCached = false;
}

void PHView::layoutSubviews(const PHRect & oldBounds)
{
    if (!autoresize) return;
    PHRect b = bounds();
    PHRect delta = b - oldBounds;
    PHSize d = delta.size();
    for (list<PHDrawable*>::iterator i = _children.begin(); i!= _children.end(); i++)
        if ((*i)->isView())
            ((PHView*)(*i))->autoresizeMyself(b, d);
}

inline static PHPoint doTheMaths(ph_float lw, ph_float mw, ph_float rw, bool l, bool m, bool r, ph_float delta)
{
    if ((!l && !m) || !delta)
        return PHOriginPoint;
    if (!l)
    {
        if (m && !r)
        {
            return PHPoint(0, delta);
        }
        if (m && r)
        {
            if (!(mw+rw))
                return PHOriginPoint;
            return PHPoint(0, delta * (mw/(mw+rw)));
        }
    }
    // else 
    {
        if (!m && !r)
            return PHPoint(delta, 0);
        if (m && !r)
        {
            if (!(lw+mw))
                return PHOriginPoint;
            ph_float w = lw / (lw+mw);
            return PHPoint(delta * w, delta * (1 - w));
        }
        if (r && !m)
        {
            if (!(lw+rw))
                return PHOriginPoint;
            ph_float w = lw / (lw+rw);
            return PHPoint(delta * w, 0);
        }
        //if (r && m)
        {
            ph_float sum = lw + mw + rw;
            if (!sum)
                return PHOriginPoint;
            return PHPoint(delta * (lw / sum), delta * (mw / sum));
        }
    }
}

void PHView::autoresizeMyself(const PHRect & pb, const PHSize & delta)
{
    PHPoint dx = doTheMaths(_frame.x, _frame.width, pb.width - _frame.x - _frame.width,
                        !(resizeMask & ResizeFixedLeft), (resizeMask & ResizeFlexibleWidth)!=0, !(resizeMask & ResizeFixedRight), delta.x);
    PHPoint dy = doTheMaths(_frame.y, _frame.height, pb.height - _frame.y - _frame.height,
                             !(resizeMask & ResizeFixedDown), (resizeMask & ResizeFlexibleHeight)!=0, !(resizeMask & ResizeFixedUp), delta.y);
    if (dx.x || dx.y || dy.x || dy.y)
        setFrame(PHRect(_frame.x + dx.x, _frame.y + dy.x, _frame.width + dx.y, _frame.height + dy.y));
}

void PHView::setBounds(const PHRect &bnd) 
{ 
    PHRect old = _bounds;
    _bounds = bnd; 
    layoutSubviews(old);
	_scalingCenter = _rotationalCenter = _flipCenter = boundsCenter();
    matrixCached = false;
}

void PHView::setPosition(const PHPoint &pos)
{
	_frame.x = pos.x;
	_frame.y = pos.y;
    effectCached = false;
}

PHPoint PHView::center() const
{
	PHPoint tmp;
	tmp.x = _frame.x+_frame.width/2;
	tmp.y = _frame.y+_frame.height/2;
	return tmp;
}

void PHView::setCenter(const PHPoint &_center)
{
	setFrame(PHRect(_center.x-_frame.width/2, _center.y-_frame.height/2, _frame.width, _frame.height));
}

PHPoint PHView::boundsCenter() const
{
	PHPoint tmp;
	tmp.x = _bounds.x+_bounds.width/2;
	tmp.y = _bounds.y+_bounds.height/2;
	return tmp;
}

PHMatrix PHView::applyMatrices()
{
    if (matrixCached && effectCached)
        return matrixCache;
    if (!effectCached)
    {
        effectCached = true;
        effectCache = PHIdentityMatrix;
        int eo = effOrder;
        while (eo)
        {
            switch (eo&3)
            {
                case EffectRotate:
                {
                    if (!_rotation) break;
                    effectCache *= PHMatrix::translation(_rotationalCenter) * PHMatrix::rotation(_rotation) * PHMatrix::translation(-_rotationalCenter);
                    break;
                }
                case EffectScale:
                {
                    if (_scaleX==1 && _scaleY==1) break;
                    effectCache *= PHMatrix::translation(_scalingCenter) * PHMatrix::scaling(_scaleX,_scaleY) * PHMatrix::translation(-_scalingCenter);                    
                    break;
                }
                case EffectFlip:
                {
                    if (!fhoriz && !fvert) break;
                    effectCache *= PHMatrix::flipping(_flipCenter, fhoriz, fvert, false);
                    break;
                }
            }
            eo>>=2;
        }
    }
    matrixCached = true;
    matrixCache = 
        PHMatrix::translation(_frame.x,_frame.y) * 
        PHMatrix::scaling(_bounds.width?_frame.width/_bounds.width:1, _bounds.height?_frame.height/_bounds.height:1) *
        PHMatrix::translation(-_bounds.x, -_bounds.y) *
        effectCache;
    return matrixCache;
}

void PHView::render()
{
    if (mtx) mtx->lock();
    PHMatrix om = gm->modelViewMatrix();
    PHMatrix m = om * applyMatrices();
    gm->setModelViewMatrix(m);
	
	bool optimizeOut = false;
	if (_optimize)
	{
		PHPoint pnt;
		ph_float minX,minY,maxX,maxY;
		minX=minY=0x3f3f3f3f;
		maxX=maxY=-0x3f3f3f3f;
#define test\
		if (pnt.x<minX)\
			minX = pnt.x;\
		if (pnt.x>maxX)\
			maxX = pnt.x;\
		if (pnt.y<minY)\
			minY = pnt.y;\
		if (pnt.y>maxY)\
			maxY = pnt.y;
        PHMatrix pm = gm->projectionMatrix() * m;
		pnt = pm.transformPoint(PHPoint(_bounds.x,_bounds.y));
		test;
		pnt = pm.transformPoint(PHPoint(_bounds.x+_bounds.width,_bounds.y));
		test;
		pnt = pm.transformPoint(PHPoint(_bounds.x+_bounds.width,_bounds.y+_bounds.height));
		test;
		pnt = pm.transformPoint(PHPoint(_bounds.x,_bounds.y+_bounds.height));
		test;
		
		optimizeOut = !PHRectIntersectsRect(PHRect(minX, minY, maxX-minX, maxY-maxY), PHRect(-1, -1, 2, 2));
	}
	
	if (!optimizeOut)
	{
        if ( (!auxLayer || !dontDrawOnMain) && supportsRenderMode(gm->renderMode()) )
        {
            drawBackground();
            draw();
        }
        renderChildren();
	}
    gm->setModelViewMatrix(om);
    if (mtx) mtx->unlock();
}

PHView::~PHView()
{
    if (auxLayer)
        auxLayer->removeView(this);
}

char notex_sprites [] = "notex_sprites";

void PHView::drawBackground()
{
	if (_backColor.a <= 0) return;
    if (gm->renderMode() != PHGameManager::defaultRenderMode)
        return;
	
    gm->setGLStates(PHGLBlending | PHGLVertexArray);
    gm->setColor(_backColor);
    PHMatrix old = gm->modelViewMatrix();
    gm->setModelViewMatrix(old * PHMatrix::scaling(PHSize(_bounds.width,_bounds.height)));
    gm->applyShader(gm->shaderProgramNamed<notex_sprites>());
    gm->solidSquareVAO()->draw();
    gm->setModelViewMatrix(old);
    
}

PHView * PHView::viewWithTag(int tag)
{
    for (list<PHDrawable*>::iterator i = _children.begin(); i!=_children.end(); i++)
        if (((*i)->tag() == tag) && (*i)->isView())
            return (PHView*)*i;
    return NULL;
}

PHView * PHView::viewWithTagAfter(int tag, PHDrawable * v)
{
    do {
        v = childWithTagAfter(tag, v);
    } while (v && !(v->isView()));
    return (PHView*)v;
}

list<PHView*> * PHView::viewsWithTag(int tag)
{
    list<PHView*> * l = new list<PHView*>;
    for (list<PHDrawable*>::iterator i = _children.begin(); i!=_children.end(); i++)
        if (((*i)->tag() == tag) && (*i)->isView())
            l->push_back((PHView*)(*i));
    return l;
}

PHMatrix PHView::loadMatrixTree(PHView * until)
{
	if (_parent && (_parent!=until) && (_parent->isView()))
        return (((PHView*)_parent)->loadMatrixTree(until)) * applyMatrices();
    return applyMatrices();
}

void PHView::bindToAuxLayer(PHAuxLayerView * layer, PHView * from)
{
    if (auxLayer)
        auxLayer->removeView(this);
    auxLayer = layer;
    if (auxLayer)
        auxLayer->addView(this);
    auxSuperview = from;
}
void PHView::auxRender()
{
    render();
}

void PHView::attachedToGameManager()
{
    PHDrawable::attachedToGameManager();
    actorAttachedToGameManager(gm);
}

void PHView::handleEvent(PHEvent * evt)
{
    if (evt->owner() == this)
    {
        touchEvent(evt);
        return;
    }
    if (!userInput()) return;

    for (list<PHDrawable*>::iterator i = _children.begin(); i!=_children.end() && !evt->handled(); i++)
        (*i)->handleEvent(evt);

    if (!evt->handled())
    {
        if (!evt->owner() && !PHPointInRect(evt->drawableLocation()->pointInView(this), bounds()))
            return;
        touchEvent(evt);
        if (evt->handled())
            evt->setOwner(this);
    }
}

PHPositionalVector PHView::positionInMyCoordinates(PHDrawableCoordinates * d)
{
    return applyMatrices().inverse() * d->positionInDrawable(parent());
}

#pragma mark -
#pragma Scripting

PHLuaAngleGetter(PHView, rotation);
PHLuaAngleSetter(PHView, setRotation);
PHLuaBoolGetter(PHView, horizontallyFlipped);
PHLuaBoolSetter(PHView, setHorizontallyFlipped);
PHLuaBoolGetter(PHView, verticallyFlipped);
PHLuaBoolSetter(PHView, setVerticallyFlipped);
PHLuaRectGetter(PHView, frame);
PHLuaRectSetter(PHView, setFrame);

void PHView::registerLuaInterface(lua_State * L)
{
    lua_getglobal(L, "PHView");
    
    PHLuaAddMethod(PHView, rotation);
    PHLuaAddMethod(PHView, setRotation);
    PHLuaAddMethod(PHView, horizontallyFlipped);
    PHLuaAddMethod(PHView, setHorizontallyFlipped);
    PHLuaAddMethod(PHView, verticallyFlipped);
    PHLuaAddMethod(PHView, setVerticallyFlipped);
    PHLuaAddMethod(PHView, frame);
    PHLuaAddMethod(PHView, setFrame);
    
    lua_pop(L, 1);
}
