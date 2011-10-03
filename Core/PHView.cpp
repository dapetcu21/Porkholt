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

std::list<PHAnimationDescriptor*> PHView::animations;

#ifdef PHVIEW_STD_LIST
#define PREFIX
#else
#define PREFIX viewsSt(NULL), viewsEn(NULL),
#endif
#define PHVIEW_INITLIST PREFIX superView(NULL), _bounds(PHRect(0, 0, -1, -1)),\
						_rotation(0), _scaleX(1), _scaleY(1), effOrder(EffectOrderScaleRotateFlip),\
						_backColor(PHClearColor), _alpha(1.0f), _userInput(false), _optimize(false), _tag(0), auxLayer(NULL), auxSuperview(NULL), drawingOnAuxLayer(false), dontDrawOnMain(true), fhoriz(false), fvert(false), luaClass("PHView"), mtx(NULL), _gameManager(NULL)

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
#ifdef PHVIEW_STD_LIST
    for (list<PHView*>::iterator i = views.begin(); i!=views.end(); i++)
        (*i)->setGameManager(gm);
#else
    PHView::ViewEl * p = viewsSt;
    while (p)
    {
        p->el->setGameManager(gm);
        p=p->next;
    }
#endif
}

void PHView::setFrame(const PHRect &frame)
{

	if (_bounds.width != frame.width || _bounds.height != frame.height)
	{
		_bounds.x = _bounds.y = 0;
		_bounds.width = frame.width;
		_bounds.height = frame.height;
		_scalingCenter = _rotationalCenter = _flipCenter = boundsCenter();
	}
	_frame = frame;
}

void PHView::setBounds(const PHRect &bnd) 
{ 
	_bounds = bnd; 
	_scalingCenter = _rotationalCenter = _flipCenter = boundsCenter();
}

void PHView::setPosition(const PHPoint &pos)
{
	_frame.x = pos.x;
	_frame.y = pos.y;
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

void PHView::apply_rotation()
{
	if (!_rotation) return;
	glTranslatef(_rotationalCenter.x, _rotationalCenter.y, 0);
    PHGLRotate(_rotation);
	glTranslatef(-_rotationalCenter.x, -_rotationalCenter.y, 0);
}

void PHView::apply_scaling()
{
	if (_scaleX==1 && _scaleY==1) return;
	glTranslatef(_scalingCenter.x, _scalingCenter.y, 0);
	glScalef(_scaleX, _scaleY, 1);
	glTranslatef(-_scalingCenter.x, -_scalingCenter.y, 0);
}

void PHView::applyMatrices()
{
	glTranslatef(_frame.x, _frame.y, 0);
	glScalef(_bounds.width?_frame.width/_bounds.width:1, _bounds.height?_frame.height/_bounds.height:1, 1);
//    glTranslatef(-_bounds.x, -_bounds.y, 0);
    int eo = effOrder;
    while (eo)
    {
        switch (eo&3)
        {
            case EffectRotate:
                apply_rotation();
                break;
            case EffectScale:
                apply_scaling();
                break;
            case EffectFlip:
                PHGLFlip(_flipCenter,fhoriz,fvert);
                break;
        }
        eo>>=2;
    }
}
extern PHView * playerView;
void PHView::render()
{
    if (mtx) mtx->lock();
	glPushMatrix();
	applyMatrices();
	
	bool optimizeOut = false;
	if (_optimize)
	{
		GLfloat m[16];
		glGetFloatv(GL_MODELVIEW, m);
		PHPoint pnt;
		double minX,minY,maxX,maxY;
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
		pnt = PHTransformPointMatrix(m, PHPoint(_bounds.x,_bounds.y));
		test;
		pnt = PHTransformPointMatrix(m, PHPoint(_bounds.x+_bounds.width,_bounds.y));
		test;
		pnt = PHTransformPointMatrix(m, PHPoint(_bounds.x+_bounds.width,_bounds.y+_bounds.height));
		test;
		pnt = PHTransformPointMatrix(m, PHPoint(_bounds.x,_bounds.y+_bounds.height));
		test;
		
		PHRect bounds = _gameManager->screenBounds();
		
		optimizeOut =	(minX<bounds.x && maxX<bounds.x) || (minY<bounds.y && maxY<bounds.y) ||
						(minX>bounds.x+bounds.width && maxX>bounds.x+bounds.width) || (minY>bounds.y+bounds.height && maxY>bounds.y+bounds.height);
	}
	
	if (!optimizeOut)
	{
        if ( !auxLayer || !dontDrawOnMain )
        {
            drawBackground();
            draw();
        }
#ifdef PHVIEW_STD_LIST
        for (list<PHView*>::iterator i = views.begin(); i!=views.end(); i++)
            (*i)->render();
#else
		PHView::ViewEl * p = viewsSt;
		while (p)
		{
			p->el->render();
			p=p->next;
		}
#endif
	}
	glPopMatrix();
    if (mtx) mtx->unlock();
}

void PHView::addSubview(PHView * view)
{
	if (!view)
		return;
    if (mtx) mtx->lock();
	view->retain();
    view->setGameManager(_gameManager);
	view->removeFromSuperview();
#ifdef PHVIEW_STD_LIST
    views.push_back(view);
    list<PHView*>::iterator i = views.end();
    i--;
    view->currPos = i;
#else
	PHView::ViewEl * tmp = new PHView::ViewEl;
	tmp->el = view;
	tmp->prev = viewsEn;
	tmp->next = NULL;
	viewsEn = tmp;
	if (!tmp->prev)
		viewsSt = tmp;
	else
		tmp->prev->next = tmp;
	view->currPos = tmp;
#endif
	view->superView = this;
    if (mtx) mtx->unlock();
}

void PHView::removeFromSuperview()
{
	if (!superView) return;
    PHMutex * m = superView->mtx;
    if (m) m->lock();
    if (mtx) mtx->lock();
#ifdef PHVIEW_STD_LIST
    superView->views.erase(currPos);
#else
	if (currPos->prev)
		currPos->prev->next = currPos->next;
	else
		superView->viewsSt = currPos->next;
	if (currPos->next)
		currPos->next->prev = currPos->prev;
	else
		superView->viewsEn = currPos->prev;
	delete currPos;
    currPos = NULL;
#endif
	superView = NULL;
    if (mtx) mtx->unlock();
    if (m) m->unlock();
	release();
}

void PHView::cancelAnimationsWithTag(int tag)
{
	for (std::list<PHAnimationDescriptor*>::iterator i = animations.begin(); i!=animations.end(); i++)
	{
		PHAnimationDescriptor * p  = *i;
		while (p)
		{
			if ((p->view == this) && ((tag==0) || (p->tag == tag)))
				p->view = NULL;
			p=p->nextAnimation();
		}
	}
}

void PHView::cancelAllAnimationsWithTag(int tag)
{
	for (std::list<PHAnimationDescriptor*>::iterator i = animations.begin(); i!=animations.end(); i++)
	{
		PHAnimationDescriptor * p  = *i;
		while (p)
		{
			if ((tag==0) || (p->tag == tag))
				p->view = NULL;
			p=p->nextAnimation();
		}
	}
}

PHView::~PHView()
{
    cancelAnimations();
	for (set<lua_State*>::iterator i = luaStates.begin(); i!=luaStates.end(); i++)
        PHLuaDeleteWeakRef(*i, this);
    if (auxLayer)
        auxLayer->removeView(this);
    if (_gameManager)
        _gameManager->eventHandler()->removeView(this);	
    removeFromSuperview();
#ifdef PHVIEW_STD_LIST
    list<PHView*>::iterator i, it;
    for (i = views.begin(); (it = i),(it++),(i!=views.end()); i=it)
        (*i)->removeFromSuperview();
#else
	PHView::ViewEl * p = viewsSt;
	while (p)
	{
		PHView * view = p->el;
		p=p->next;
		view->removeFromSuperview();
	}
#endif
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

void PHView::touchEvent(PHTouch * touch)
{
    eventHandled = false;
}

//animation system
void PHView::addAnimation(PHAnimationDescriptor * anim)
{
	anim->retain();
	anim->totalTime = anim->time;
	animations.push_back(anim);
}


inline double circleEq(double x,double mx,double my,double r)
{
	return sqrt(r*r-(x-mx)*(x-mx))+my;
}

#define bounceL 0.5f

double PHView::animFunction(double time,int ftype)
{
	if (ftype==PHAnimationDescriptor::LinearFunction)
		return time;
	if (ftype==PHAnimationDescriptor::BounceFunction)
	{
		if (time<1-bounceL)
		{
			return time/(1-bounceL);
		} else {
			double m = (bounceL-1);
			double n = 1-m*(1-bounceL);
			double yy = m*(1-bounceL/2)+n;
			double r = sqrt(bounceL*bounceL/4+(1-yy)*(1-yy));
			return circleEq(time, 1-bounceL/2, yy, r);
		}
	}
	if (ftype==PHAnimationDescriptor::FadeInFunction)
	{
		return time*time;
	}
	if (ftype==PHAnimationDescriptor::FadeOutFunction)
	{
		return -time*time+2*time;
	}
	if (ftype==PHAnimationDescriptor::FadeInOutFunction)
	{
		return sin(M_PI*(time-0.5f))/2+0.5;
	}
	return time; //revert to linear
}

void PHView::updateAnimation(double time)
{
	std::list<PHAnimationDescriptor*>::iterator nx;
	for (std::list<PHAnimationDescriptor*>::iterator i = animations.begin(); i!=animations.end(); i=nx)
	{
		double tm = time;
		nx = i;
		nx++;
		while (tm)
		{
			double lastRatio,ratio,diff;
			PHAnimationDescriptor * anim = *i;
			if (tm<anim->time)
			{
				lastRatio = animFunction((anim->totalTime-anim->time)/anim->totalTime,anim->timeFunction);
				anim->time -= tm;
				ratio = animFunction((anim->totalTime-anim->time)/anim->totalTime,anim->timeFunction);
				tm = 0;
			} else {
				lastRatio = animFunction((anim->totalTime-anim->time)/anim->totalTime,anim->timeFunction);
				tm -= anim->time;
				anim->time = 0;
				ratio = 1;
			}
			diff = ratio-lastRatio;
			if (anim->view)
			{
				if ((anim->moveX) || (anim->moveY))
				{
					PHRect fr = ((PHView*)anim->view)->frame();
					fr.x += diff * anim->moveX;
					fr.y += diff * anim->moveY;
					((PHView*)anim->view)->setFrame(fr);
				}
				if (anim->rotate)
				{
					((PHView*)anim->view)->rotate(diff * anim->rotate);
				}
                if (anim->customValue)
                {
                    ((PHView*)anim->view)->incrementAnimatedValue(diff * anim->customValue);
                }
				if (anim->scaleX!=1)
				{
					double lastM,m;
					lastM = 1 + (anim->scaleX-1)*lastRatio;
					m = 1 + (anim->scaleX-1)*ratio;
					((PHView*)anim->view)->setScaleX(((PHView*)anim->view)->scaleX()/lastM*m);
				}
				if (anim->scaleY!=1)
				{
					double lastM,m;
					lastM = 1 + (anim->scaleY-1)*lastRatio;
					m = 1 + (anim->scaleY-1)*ratio;
					((PHView*)anim->view)->setScaleY(((PHView*)anim->view)->scaleY()/lastM*m);
				}
				if (anim->bgColor.a>=0)
				{
					PHColor clr,trg,crr;
					crr = ((PHView*)anim->view)->backgroundColor();
					trg = anim->bgColor;
                    if (trg.a>0)
                    {
                        if (crr.a>0)
                        {
                            clr.r = ((lastRatio==1)?0:((crr.r-trg.r)/(1-lastRatio)))*(1-ratio)+trg.r;
                            clr.g = ((lastRatio==1)?0:((crr.g-trg.g)/(1-lastRatio)))*(1-ratio)+trg.g;
                            clr.b = ((lastRatio==1)?0:((crr.b-trg.b)/(1-lastRatio)))*(1-ratio)+trg.b;
                        } else {
                            clr.r = trg.r;
                            clr.g = trg.g;
                            clr.b = trg.b;
                        }
                    } else {
                        clr.r = crr.r;
                        clr.g = crr.g;
                        clr.b = crr.b;
                    }
					clr.a = ((lastRatio==1)?0:((crr.a-trg.a)/(1-lastRatio)))*(1-ratio)+trg.a;
					((PHView*)anim->view)->setBackgroundColor(clr);
				}
                if (anim->customColor.a>=0)
				{
					PHColor clr,trg,crr;
					crr = ((PHView*)anim->view)->animatedColor();
					trg = anim->customColor;
                    if (trg.a>0)
                    {
                        if (crr.a>0)
                        {
                            clr.r = ((lastRatio==1)?0:((crr.r-trg.r)/(1-lastRatio)))*(1-ratio)+trg.r;
                            clr.g = ((lastRatio==1)?0:((crr.g-trg.g)/(1-lastRatio)))*(1-ratio)+trg.g;
                            clr.b = ((lastRatio==1)?0:((crr.b-trg.b)/(1-lastRatio)))*(1-ratio)+trg.b;
                        } else {
                            clr.r = trg.r;
                            clr.g = trg.g;
                            clr.b = trg.b;
                        }
                    } else {
                        clr.r = crr.r;
                        clr.g = crr.g;
                        clr.b = crr.b;
                    }
					clr.a = ((lastRatio==1)?0:((crr.a-trg.a)/(1-lastRatio)))*(1-ratio)+trg.a;
					((PHView*)anim->view)->setAnimatedColor(clr);
				}
			}
			if (tm)
			{
				PHAnimationDescriptor * next = anim->nextAnimation();
				if (next)
					next->retain();
				*i = next;
				if (anim -> view && anim->target && anim->callback)
					(anim->target->*(anim->callback))(anim->target,anim->userdata);
				anim->release();
				if (!*i)
				{
					animations.erase(i);
					tm = 0;
				}
			}
		}
	}
}

//stuff
PHView * PHView::pointerDeepFirst(PHTouch * touch)
{
	if (!_userInput) return NULL;
	glPushMatrix();
	applyMatrices();
	PHView * view = NULL;
#ifdef PHVIEW_STD_LIST
    for (list<PHView*>::reverse_iterator i = views.rbegin(); i!= views.rend(); i++)
    {
        PHView * v = (*i)->pointerDeepFirst(touch);
        if (v)
        {
            view = v;
            break;
        }
    }
#else
	PHView::ViewEl * p = viewsEn;
	while (p)
	{
		PHView * v = p->el->pointerDeepFirst(touch);
		if (v)
		{
			view = v;
			break;
		}
		p=p->prev;
	}
#endif
	if (!view)
	{
		PHPoint pnt = PHUnTransformedPoint(touch->location());
		if (pnt.x>=0 && pnt.y>=0 && pnt.x<=_bounds.width && pnt.y<=_bounds.height)
        {
            eventHandled = true;
            touchEvent(touch);
			view = eventHandled?this:false;
        }
	}
	glPopMatrix();
	return view;
}

//geometry
void PHView::loadMatrixTree(PHView * until)
{
	if (superView&&(superView!=until))
	{
		superView->loadMatrixTree(until);
	}
	applyMatrices();
}

PHPoint PHView::toMyCoordinates(const PHPoint & pnt, PHView * until)
{
	GLfloat m[16];
	glPushMatrix();
	glLoadIdentity();
	loadMatrixTree(until);
	glGetFloatv(GL_MODELVIEW, m);
	glPopMatrix();
	return PHUnTransformPointMatrix(m, pnt);
}

void PHView::toMyCoordinates(PHPoint * pnt, int n, PHView * until)
{
	GLfloat m[16],inverse[16];
	glPushMatrix();
	glLoadIdentity();
	loadMatrixTree(until);
	glGetFloatv(GL_MODELVIEW, m);
	glPopMatrix();
	PHInvertMatrix(m, inverse);
	for (int i=0; i<n; i++)
		pnt[i] = PHTransformPointMatrix(inverse, pnt[i]);
}

PHPoint PHView::fromMyCoordinates(const PHPoint & pnt, PHView * until)
{
	GLfloat m[16];
	glPushMatrix();
	glLoadIdentity();
	loadMatrixTree(until);
	glGetFloatv(GL_MODELVIEW, m);
	glPopMatrix();
    PHLog("%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n",
          m[0],m[1],m[2],m[3],m[4],m[5],m[6],m[7],m[8],m[9],m[10],m[11],m[12],m[13],m[14],m[15]);
	return PHTransformPointMatrix(m, pnt);
}

void PHView::fromMyCoordinates(PHPoint * pnt, int n, PHView * until)
{
	GLfloat m[16];
	glPushMatrix();
	glLoadIdentity();
	loadMatrixTree(until);
	glGetFloatv(GL_MODELVIEW, m);
	glPopMatrix();
	for (int i=0; i<n; i++)
		pnt[i] = PHTransformPointMatrix(m, pnt[i]);
}

PHView * PHView::viewWithTag(int tag)
{
#ifdef PHVIEW_STD_LIST
    for (list<PHView*>::iterator i = views.begin(); i!=views.end(); i++)
        if ((*i)->tag() == tag)
            return *i;
#else
    for (ViewEl * ve = viewsSt; ve; ve=ve->next)
        if (ve->el->tag()==tag)
            return ve->el;
#endif
    return NULL;
}

PHView * PHView::viewWithTagAfter(int tag, PHView * v)
{
#ifdef PHVIEW_STD_LIST
    for (list<PHView*>::iterator i = views.begin(); i!=views.end(); i++)
    {
        if (*i == v) { v = NULL; continue; }
        if (v) continue;
        if ((*i)->tag() == tag)
            return *i;
    }
#else
    for (ViewEl * ve = viewsSt; ve; ve=ve->next)
    {
        if (ve->el==v) { v = NULL; continue; }
        if (v) continue;
        if (ve->el->tag()==tag)
            return ve->el;
    }
#endif
    return NULL;
}

list<PHView*> * PHView::viewsWithTag(int tag)
{
    list<PHView*> * l = new list<PHView*>;
#ifdef PHVIEW_STD_LIST
    for (list<PHView*>::iterator i = views.begin(); i!=views.end(); i++)
        if ((*i)->tag() == tag)
            l->push_back(*i);
#else
    for (ViewEl * ve = viewsSt; ve; ve=ve->next)
        if (ve->el->tag()==tag)
            l->push_back(ve->el);
#endif
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
