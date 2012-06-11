/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHView.h>
#include <Porkholt/Core/PHAuxLayerView.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHEventHandler.h>
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHGLVertexArrayObject.h>

const string PHView::_luaClass("PHView");

#define PHVIEW_INITLIST _bounds(PHRect(0, 0, -1, -1)), fhoriz(false), fvert(false), _rotation(0), _scaleX(1), _scaleY(1), _alpha(1.0f), _userInput(false), _optimize(false), _backColor(PHClearColor), effOrder(EffectOrderScaleRotateFlip), effectCached(false), matrixCached(false), autoresize(false), resizeMask(ResizeStatic), auxLayer(NULL), auxSuperview(NULL), drawingOnAuxLayer(false), dontDrawOnMain(true)

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

void PHView::resizeSubviews(const PHSize & delta)
{
    for (list<PHDrawable*>::iterator i = _children.begin(); i!= _children.end(); i++)
        if ((*i)->isView())
            ((PHView*)(*i))->autoresizeMyself(delta);
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

void PHView::autoresizeMyself(const PHSize & delta)
{
    PHDrawable * p = parent();
    if (!p || !p->isView()) return;
    PHRect pb = ((PHView*)p)->bounds();
    PHPoint dx = doTheMaths(_frame.x, _frame.width, pb.width - _frame.x - _frame.width,
                        !(resizeMask & ResizeFixedLeft), (resizeMask & ResizeFlexibleWidth)!=0, !(resizeMask & ResizeFixedRight), delta.x);
    PHPoint dy = doTheMaths(_frame.y, _frame.height, pb.height - _frame.y - _frame.height,
                             !(resizeMask & ResizeFixedDown), (resizeMask & ResizeFlexibleHeight)!=0, !(resizeMask & ResizeFixedUp), delta.y);
    if (dx.x || dx.y || dy.x || dy.y)
        setFrame(PHRect(_frame.x + dx.x, _frame.y + dy.x, _frame.width + dx.y, _frame.height + dy.y));
}

void PHView::setBounds(const PHRect &bnd) 
{ 
    if (autoresize)
    {
        PHSize delta = bnd.size() - _bounds.size();
        resizeSubviews(delta);
    }
    _bounds = bnd; 
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
		pnt = m.transformPoint(PHPoint(_bounds.x,_bounds.y));
		test;
		pnt = m.transformPoint(PHPoint(_bounds.x+_bounds.width,_bounds.y));
		test;
		pnt = m.transformPoint(PHPoint(_bounds.x+_bounds.width,_bounds.y+_bounds.height));
		test;
		pnt = m.transformPoint(PHPoint(_bounds.x,_bounds.y+_bounds.height));
		test;
		
		PHRect bounds = gm->screenBounds();
		
		optimizeOut = (minX<bounds.x && maxX<bounds.x) || (minY<bounds.y && maxY<bounds.y) ||
						(minX>bounds.x+bounds.width && maxX>bounds.x+bounds.width) || (minY>bounds.y+bounds.height && maxY>bounds.y+bounds.height);
	}
	
	if (!optimizeOut)
	{
        if ( (!auxLayer || !dontDrawOnMain) && supportsRenderMode(gm->renderMode()) )
        {
            drawBackground();
            draw();
        }
        for (list<PHDrawable*>::iterator i = _children.begin(); i!=_children.end(); i++)
            (*i)->render();
	}
    gm->setModelViewMatrix(om);
    if (mtx) mtx->unlock();
}

PHView::~PHView()
{
    if (auxLayer)
        auxLayer->removeView(this);
    if (gm)
        gm->eventHandler()->removeView(this);
}

void PHView::drawBackground()
{
	if (_backColor.a <= 0) return;
    if (gm->renderMode() != PHGameManager::defaultRenderMode)
        return;
	
    gm->setGLStates(PHGLBlending | PHGLVertexArray);
    gm->setColor(_backColor);
    PHMatrix old = gm->modelViewMatrix();
    gm->setModelViewMatrix(old * PHMatrix::scaling(PHSize(_bounds.width,_bounds.height)));
    gm->applyShader(gm->solidColorShader());
    gm->solidSquareVAO()->draw();
    gm->setModelViewMatrix(old);
    
}

//stuff
PHView * PHView::pointerDeepFirst(const PHMatrix & om, PHEvent * touch)
{
	if (!_userInput) return NULL;
    PHMatrix m = om * applyMatrices();
	PHView * view = NULL;
    for (list<PHDrawable*>::reverse_iterator i = _children.rbegin(); i!= _children.rend(); i++)
        if ((*i)->isView())
        {
            PHView * v = ((PHView*)(*i))->pointerDeepFirst(m, touch);
            if (v)
            {
                view = v;
                break;
            }
        }
	if (!view)
	{
		PHPoint pnt = m.untransformPoint(touch->location());
		if (PHPointInRect(pnt,_bounds))
        {
            touchEvent(touch);
			view = touch->handled()?this:NULL;
        }
	}
	return view;
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

//geometry
PHMatrix PHView::loadMatrixTree(PHView * until)
{
	if (_parent && (_parent!=until) && (_parent->isView()))
        return (((PHView*)_parent)->loadMatrixTree(until)) * applyMatrices();
    return applyMatrices();
}

PHPoint PHView::toMyCoordinates(const PHPoint & pnt, PHView * until)
{
    if (until==this) return pnt;
    return loadMatrixTree(until).untransformPoint(pnt);
}

void PHView::toMyCoordinates(PHPoint * pnt, int n, PHView * until)
{
    if (until==this) return;
    PHMatrix m = loadMatrixTree(until).inverse();
	for (int i=0; i<n; i++)
		pnt[i] = m.transformPoint(pnt[i]);
}

PHPoint PHView::fromMyCoordinates(const PHPoint & pnt, PHView * until)
{
    if (until==this) return pnt;
    return loadMatrixTree(until).transformPoint(pnt);
}

void PHView::fromMyCoordinates(PHPoint * pnt, int n, PHView * until)
{
    if (until==this) return;
    PHMatrix m = loadMatrixTree(until);
    for (int i=0; i<n; i++)
		pnt[i] = m.transformPoint(pnt[i]);
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
