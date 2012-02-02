/*
 *  PHView.cpp
 *  Porkholt
 *
 *  Created by Marius Petcu on 12/17/10.
 *  Copyright 2010 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHView.h"
#include "PHAuxLayerView.h"
#include "PHGameManager.h"
#include "PHEventHandler.h"
#include "PHLua.h"

#define PHVIEW_INITLIST superView(NULL), _bounds(PHRect(0, 0, -1, -1)),\
						_rotation(0), _scaleX(1), _scaleY(1), effOrder(EffectOrderScaleRotateFlip),\
						_backColor(PHClearColor), _alpha(1.0f), _userInput(false), _optimize(false), _tag(0), auxLayer(NULL), auxSuperview(NULL), drawingOnAuxLayer(false), dontDrawOnMain(true), fhoriz(false), fvert(false), luaClass("PHView"), mtx(NULL), _gameManager(NULL), effectCached(false), matrixCached(false)

PHView::PHView() :  PHVIEW_INITLIST
{
	setFrame(PHRect(0, 0, 0, 0));
}

PHView::PHView(const PHRect &frame) : PHVIEW_INITLIST
{
	setFrame(frame);
}

void PHView::setGameManager(PHGameManager * gm)
{
    if (_gameManager == gm)
        return;
    _gameManager = gm;
    for (list<PHView*>::iterator i = views.begin(); i!=views.end(); i++)
        (*i)->setGameManager(gm);
}

void PHView::setFrame(const PHRect &frame)
{

	if (_bounds.width != frame.width || _bounds.height != frame.height)
        setBounds(PHRect(0,0,frame.width,frame.height));
	_frame = frame;
    matrixCached = false;
}

void PHView::setBounds(const PHRect &bnd) 
{ 
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
                    effectCache *= PHMatrix::translation(_scalingCenter) * PHMatrix::scaling(PHSize(_scaleX,_scaleY)) * PHMatrix::translation(-_scalingCenter);                    
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
        PHMatrix::translation(PHPoint(_frame.x,_frame.y)) * 
        PHMatrix::scaling(PHSize(_bounds.width?_frame.width/_bounds.width:1, _bounds.height?_frame.height/_bounds.height:1)) *
        PHMatrix::translation(PHPoint(-_bounds.x, -_bounds.y)) *
        effectCache;
    return matrixCache;
}
extern PHView * playerView;
void PHView::render()
{
    if (mtx) mtx->lock();
    PHMatrix om = _gameManager->modelViewMatrix();
    PHMatrix m = om * applyMatrices();
    _gameManager->setModelViewMatrix(m);
	
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
		
		PHRect bounds = _gameManager->screenBounds();
		
		optimizeOut = (minX<bounds.x && maxX<bounds.x) || (minY<bounds.y && maxY<bounds.y) ||
						(minX>bounds.x+bounds.width && maxX>bounds.x+bounds.width) || (minY>bounds.y+bounds.height && maxY>bounds.y+bounds.height);
	}
	
	if (!optimizeOut)
	{
        if ( !auxLayer || !dontDrawOnMain )
        {
            drawBackground();
            draw();
        }
        for (list<PHView*>::iterator i = views.begin(); i!=views.end(); i++)
            (*i)->render();
	}
    _gameManager->setModelViewMatrix(om);
    if (mtx) mtx->unlock();
}

void PHView::addSubviewBefore(PHView * view, PHView * before)
{
	if (!view)
		return;
    if (mtx) mtx->lock();
	view->retain();
    view->setGameManager(_gameManager);
	view->removeFromSuperview();
    list<PHView*>::iterator i = views.end();
    if (before && before->superview() == this )
        i = before->currPos;
    view->currPos = views.insert(i, view);
	view->superView = this;
    if (mtx) mtx->unlock();
}

void PHView::removeFromSuperview()
{
	if (!superView) return;
    PHMutex * m = superView->mtx;
    if (m) m->lock();
    if (mtx) mtx->lock();
    superView->views.erase(currPos);
	superView = NULL;
    if (mtx) mtx->unlock();
    if (m) m->unlock();
	release();
}

void PHView::removeAllSubviews()
{
    list<PHView*>::iterator i, it;
    for (i = views.begin(); (it = i),(it++),(i!=views.end()); i=it)
        (*i)->removeFromSuperview();
}

PHView::~PHView()
{
	for (set<lua_State*>::iterator i = luaStates.begin(); i!=luaStates.end(); i++)
        PHLuaDeleteWeakRef(*i, this);
    if (auxLayer)
        auxLayer->removeView(this);
    if (_gameManager)
        _gameManager->eventHandler()->removeView(this);	
    removeFromSuperview();
    removeAllSubviews();
    if (mtx) mtx->release();
}

void PHView::bringToFront()
{
	retain();
	PHView * su = superView;
	removeFromSuperview();
	su->addSubview(this);
	release();
}

void PHView::sendToBack()
{
	retain();
	PHView * su = superView;
	removeFromSuperview();
    list<PHView*>::iterator i = views.begin();
	su->addSubviewBefore(this, (i==views.end())?NULL:*i);
	release();
}

void PHView::drawBackground()
{
	if (_backColor.a <= 0) return;
	const GLfloat squareVertices[] = {
        0, 0,
        _bounds.width, 0,
        0,  _bounds.height,
        _bounds.width,  _bounds.height,
    };
	
    PHGLSetStates(PHGLVertexArray);
    PHGLSetColor(_backColor);
	glVertexPointer(2, GL_FLOAT, 0, squareVertices);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
		
void PHView::draw()
{	
}

void PHView::touchEvent(PHEvent * touch)
{
}

//stuff
PHView * PHView::pointerDeepFirst(PHMatrix m, PHEvent * touch)
{
	if (!_userInput) return NULL;
    m *= applyMatrices();
	PHView * view = NULL;
    for (list<PHView*>::reverse_iterator i = views.rbegin(); i!= views.rend(); i++)
    {
        PHView * v = (*i)->pointerDeepFirst(m, touch);
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

//geometry
PHMatrix PHView::loadMatrixTree(PHView * until)
{
	if (superView&&(superView!=until))
        return (superView->loadMatrixTree(until)) * applyMatrices();
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

PHView * PHView::viewWithTag(int tag)
{
    for (list<PHView*>::iterator i = views.begin(); i!=views.end(); i++)
        if ((*i)->tag() == tag)
            return *i;
    return NULL;
}

PHView * PHView::viewWithTagAfter(int tag, PHView * v)
{
    for (list<PHView*>::iterator i = views.begin(); i!=views.end(); i++)
    {
        if (*i == v) { v = NULL; continue; }
        if (v) continue;
        if ((*i)->tag() == tag)
            return *i;
    }
    return NULL;
}

list<PHView*> * PHView::viewsWithTag(int tag)
{
    list<PHView*> * l = new list<PHView*>;
    for (list<PHView*>::iterator i = views.begin(); i!=views.end(); i++)
        if ((*i)->tag() == tag)
            l->push_back(*i);
    return l;
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

void PHView::getLuaHandle(lua_State * L)
{
    if (!L) return;
    PHLuaGetWeakRef(L, this);
    if (!lua_istable(L, -1))
    {
        lua_pop(L,1);
        lua_getglobal(L, luaClass.c_str());
        lua_getfield(L, -1, "new");
        lua_pushvalue(L, -2);
        PHLuaCall(L, 1, 1);
        lua_pushlightuserdata(L, this);
        lua_setfield(L, -2, "ud");
        lua_pushvalue(L, -1);
        PHLuaSetWeakRef(L, this);
        lua_remove(L, -2);
        luaStates.insert(L);
    }
}

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
