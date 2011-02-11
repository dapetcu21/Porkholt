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

PHView::PHView() :  viewsSt(NULL), viewsEn(NULL), superView(NULL), _bounds(PHMakeRect(0, 0, -1, -1)),
					_rotation(0), _scaleX(1), _scaleY(1), effOrder(EffectOrderScaleRotate)
{
	setFrame(PHMakeRect(0, 0, 0, 0));
}

PHView::PHView(const PHRect &frame) : viewsSt(NULL), viewsEn(NULL), superView(NULL), _bounds(PHMakeRect(0, 0, -1, -1)),
									  _rotation(0), _scaleX(1), _scaleY(1), effOrder(EffectOrderScaleRotate)
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

PHPoint PHView::center() const
{
	PHPoint tmp;
	tmp.x = _frame.x+_frame.width/2;
	tmp.y = _frame.y+_frame.height/2;
	return tmp;
}

void PHView::setCenter(const PHRect &_center)
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

void PHView::setup_matrix()
{
	glTranslatef(_frame.x, _frame.y, 0);
	if (effOrder == EffectOrderScaleRotate)
	{
		apply_rotation();
		apply_scaling();
	} else {
		apply_scaling();
		apply_rotation();
	}
}

void PHView::render()
{
	glPushMatrix();
	setup_matrix();
	draw();
	PHView::ViewEl * p = viewsSt;
	while (p)
	{
		p->el->render();
		p=p->next;
	}
	glPopMatrix();
}

void PHView::addSubview(PHView * view)
{
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
	view->retain();
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

PHView::~PHView()
{
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
		
void PHView::draw()
{
	
	const GLfloat squareVertices[] = {
        0, 0,
        _bounds.width, 0,
        0,  _bounds.height,
        _bounds.width,  _bounds.height,
    };
	
    static const GLubyte squareColors[] = {
        255, 255,   0, 255,
        0,   255, 255, 255,
        0,     0,   0,   0,
        255,   0, 255, 255,
    };
    
    static float transY = 0.0f;
	
    //glTranslatef(0.0f, (GLfloat)(sinf(transY)*150), 0.0f);
	transY += 0.075f;
	
	glVertexPointer(2, GL_FLOAT, 0, squareVertices);
	glEnableClientState(GL_VERTEX_ARRAY);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, squareColors);
	glEnableClientState(GL_COLOR_ARRAY);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
}

//animation system
void PHView::addAnimation(PHAnimationDescriptor * anim)
{
	if (anim->time == 0) return;
	if (anim->view == NULL) return;
	anim->retain();
	animations.push_back(anim);
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
			double amount;
			PHAnimationDescriptor * anim = *i;
			if (tm<anim->time)
			{
				amount = tm/anim->time;
				anim->time -= tm;
				tm = 0;
			} else {
				amount = 1;
				tm -= anim->time;
				anim->time = 0;
			}
			if ((anim->moveX) || (anim->moveY))
			{
				PHRect fr = ((PHView*)anim->view)->frame();
				fr.x += amount * anim->moveX;
				fr.y += amount * anim->moveY;
				((PHView*)anim->view)->setFrame(fr);
				anim->moveX *= 1-amount;
				anim->moveY *= 1-amount;
			}
			if (anim->rotate)
			{
				((PHView*)anim->view)->rotate(amount * anim->rotate);
				anim->rotate *= 1-amount;
			}
			if (anim->scaleX!=1)
			{
				double ratio = 1/(anim->_scaleX);
				anim->_scaleX+= (anim->scaleX - anim->_scaleX)*amount;
				ratio*=anim->_scaleX;
				((PHView*)anim->view)->setScaleX(((PHView*)anim->view)->scaleX()*ratio);
			}
			if (anim->scaleY!=1)
			{
				double ratio = 1/(anim->_scaleY);
				anim->_scaleY+= (anim->scaleY - anim->_scaleY)*amount;
				ratio*=anim->_scaleY;
				((PHView*)anim->view)->setScaleY(((PHView*)anim->view)->scaleY()*ratio);
			}
			if (tm)
			{
				PHAnimationDescriptor * next = anim->nextAnimation();
				if (next)
					next->retain();
				*i = next;
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