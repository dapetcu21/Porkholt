/*
 *  PHView.cpp
 *  Porkholt
 *
 *  Created by Marius Petcu on 12/17/10.
 *  Copyright 2010 Home. All rights reserved.
 *
 */

#include "PHView.h"

std::list<PHAnimationDescriptor*> PHView::animations;

struct PHView::ViewEl
{
	PHView * el;
	PHView::ViewEl * next, * prev;
};

#define PHVIEW_INITLIST viewsSt(NULL), viewsEn(NULL), superView(NULL), _bounds(PHMakeRect(0, 0, -1, -1)),\
						_rotation(0), _scaleX(1), _scaleY(1), effOrder(EffectOrderScaleRotate),\
						_backColor(PHClearColor), _alpha(1.0f), _userInput(false), _optimize(false), _inputRouting(false)

PHView::PHView() :  PHVIEW_INITLIST
{
	setFrame(PHMakeRect(0, 0, 0, 0));
}

PHView::PHView(const PHRect &frame) : PHVIEW_INITLIST
{
	setFrame(frame);
}

void PHView::setFrame(const PHRect &frame)
{

	if (_bounds.width != frame.width || _bounds.height != frame.height)
	{
		_bounds.x = _bounds.y = 0;
		_bounds.width = frame.width;
		_bounds.height = frame.height;
		_scalingCenter = _rotationalCenter = boundsCenter();
	}
	_frame = frame;
}

void PHView::setBounds(const PHRect &bnd) 
{ 
	_bounds = bnd; 
	_scalingCenter = _rotationalCenter = boundsCenter();
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
	setFrame(PHMakeRect(_center.x-_frame.width/2, _center.y-_frame.height/2, _frame.width, _frame.height));
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
	glRotatef(_rotation, 0, 0, -1);
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
	if (effOrder == EffectOrderScaleRotate)
	{
		apply_rotation();
		apply_scaling();
	} else {
		apply_scaling();
		apply_rotation();
	}
}
extern PHView * playerView;
void PHView::render()
{
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
		pnt = PHTransformPointMatrix(m, PHMakePoint(_bounds.x,_bounds.y));
		test;
		pnt = PHTransformPointMatrix(m, PHMakePoint(_bounds.x+_bounds.width,_bounds.y));
		test;
		pnt = PHTransformPointMatrix(m, PHMakePoint(_bounds.x+_bounds.width,_bounds.y+_bounds.height));
		test;
		pnt = PHTransformPointMatrix(m, PHMakePoint(_bounds.x,_bounds.y+_bounds.height));
		test;
		
		PHRect bounds = PHMainEvents::sharedInstance()->screenBounds();
		
		optimizeOut =	(minX<bounds.x && maxX<bounds.x) || (minY<bounds.y && maxY<bounds.y) ||
						(minX>bounds.x+bounds.width && maxX>bounds.x+bounds.width) || (minY>bounds.y+bounds.height && maxY>bounds.y+bounds.height);
	}
	
	if (!optimizeOut)
	{
		drawBackground();
		draw();
		PHView::ViewEl * p = viewsSt;
		while (p)
		{
			p->el->render();
			p=p->next;
		}
	}
	glPopMatrix();
}

void PHView::addSubview(PHView * view)
{
	if (!view)
		return;
	view->retain();
	view->removeFromSuperview();
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
	view->superView = this;
}

void PHView::removeFromSuperview()
{
	if (!superView) return;
	if (currPos->prev)
		currPos->prev->next = currPos->next;
	else
		superView->viewsSt = currPos->next;
	if (currPos->next)
		currPos->next->prev = currPos->prev;
	else
		superView->viewsEn = currPos->prev;
	delete currPos;
	superView = NULL;
	currPos = NULL;
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
	
	PHEventHandler::sharedInstance()->removeView(this);
	
	PHView::ViewEl * p = viewsSt;
	while (p)
	{
		PHView * view = p->el;
		p=p->next;
		view->removeFromSuperview();
	}
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
	if (_backColor.a == 0) return;
	const GLfloat squareVertices[] = {
        0, 0,
        _bounds.width, 0,
        0,  _bounds.height,
        _bounds.width,  _bounds.height,
    };
	
	const GLfloat squareColors[] = {
        _backColor.r, _backColor.g,  _backColor.b, _backColor.a,
        _backColor.r, _backColor.g,  _backColor.b, _backColor.a,
        _backColor.r, _backColor.g,  _backColor.b, _backColor.a,
        _backColor.r, _backColor.g,  _backColor.b, _backColor.a,
    };
	
	glVertexPointer(2, GL_FLOAT, 0, squareVertices);
	glEnableClientState(GL_VERTEX_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, squareColors);
	glEnableClientState(GL_COLOR_ARRAY);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}
		
void PHView::draw()
{	
}

void PHView::touchEvent(PHTouch * touch)
{
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
		return sin(M_PI*(time-0.5f))/2;
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
					clr.r = ((lastRatio==1)?0:((crr.r-trg.r)/(1-lastRatio)))*(1-ratio)+trg.r;
					clr.g = ((lastRatio==1)?0:((crr.g-trg.g)/(1-lastRatio)))*(1-ratio)+trg.g;
					clr.b = ((lastRatio==1)?0:((crr.b-trg.b)/(1-lastRatio)))*(1-ratio)+trg.b;
					clr.a = ((lastRatio==1)?0:((crr.a-trg.a)/(1-lastRatio)))*(1-ratio)+trg.a;
					((PHView*)anim->view)->setBackgroundColor(clr);
				}
			}
			if (tm)
			{
				PHAnimationDescriptor * next = anim->nextAnimation();
				if (next)
					next->retain();
				*i = next;
				if (anim -> view && anim->target && anim->callback)
					(anim->target->*(anim->callback))(anim->userdata);
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
	if ((!view)&&(!_inputRouting))
	{
		PHPoint pnt = PHUnTransformedPoint(touch->location());
		if (pnt.x>=0 && pnt.y>=0 && pnt.x<=_bounds.width && pnt.y<=_bounds.height)
			view = this;
	}
	glPopMatrix();
	return view;
}

//geometry
void PHView::loadMatrixTree()
{
	if (superView)
	{
		superView->loadMatrixTree();
	}
	applyMatrices();
}

PHPoint PHView::toMyCoordinates(PHPoint pnt)
{
	GLfloat m[16];
	glPushMatrix();
	glLoadIdentity();
	loadMatrixTree();
	glGetFloatv(GL_MODELVIEW, m);
	glPopMatrix();
	return PHUnTransformPointMatrix(m, pnt);
}

void PHView::toMyCoordinates(PHPoint * pnt, int n)
{
	GLfloat m[16],inverse[16];
	glPushMatrix();
	glLoadIdentity();
	loadMatrixTree();
	glGetFloatv(GL_MODELVIEW, m);
	glPopMatrix();
	PHInvertMatrix(m, inverse);
	for (int i=0; i<n; i++)
		pnt[i] = PHTransformPointMatrix(inverse, pnt[i]);
}

PHPoint PHView::fromMyCoordinates(PHPoint pnt)
{
	GLfloat m[16];
	glPushMatrix();
	glLoadIdentity();
	loadMatrixTree();
	glGetFloatv(GL_MODELVIEW, m);
	glPopMatrix();
	return PHTransformPointMatrix(m, pnt);
}

void PHView::fromMyCoordinates(PHPoint * pnt, int n)
{
	GLfloat m[16];
	glPushMatrix();
	glLoadIdentity();
	loadMatrixTree();
	glGetFloatv(GL_MODELVIEW, m);
	glPopMatrix();
	for (int i=0; i<n; i++)
		pnt[i] = PHTransformPointMatrix(m, pnt[i]);
}
