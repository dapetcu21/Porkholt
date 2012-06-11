/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHDrawable.h>
#include <Porkholt/Core/PHLua.h>

const string PHDrawable::_luaClass("PHDrawable");

#define PH_DRAWABLE_INIT_LIST gm(NULL), _parent(NULL), _tag(0), mtx(NULL), luaClass(&_luaClass), _isView(false)

PHDrawable::PHDrawable() : PH_DRAWABLE_INIT_LIST
{
    
}

void PHDrawable::setGameManager(PHGameManager * gameManager)
{
    if (gm == gameManager)
        return;
    gm = gameManager;
    for (list<PHDrawable*>::iterator i = _children.begin(); i!=_children.end(); i++)
        (*i)->setGameManager(gameManager);
    if (gm)
        attachedToGameManager();
}

void PHDrawable::render()
{
    draw();
    for (list<PHDrawable*>::iterator i = _children.begin(); i!=_children.end(); i++)
        (*i)->render();
}


void PHDrawable::addChildBefore(PHDrawable * drawable, PHDrawable * before)
{
	if (!drawable)
		return;
    if (mtx) mtx->lock();
	drawable->retain();
    drawable->setGameManager(gm);
	drawable->removeFromParent();
    list<PHDrawable*>::iterator i = _children.end();
    if (before && before->parent() == this )
        i = before->currPos;
    drawable->currPos = _children.insert(i, drawable);
	drawable->_parent = this;
    if (mtx) mtx->unlock();
}

void PHDrawable::removeFromParent()
{
    if (mtx) mtx->lock();
	if (!_parent) 
    {
        if (mtx) mtx->unlock();
        return;
    }
    PHMutex * m = _parent->mtx;
    if (m) m->lock();
    _parent->_children.erase(currPos);
	_parent = NULL;
    if (mtx) mtx->unlock();
    if (m) m->unlock();
	release();
}

void PHDrawable::removeAllChildren()
{
    list<PHDrawable*>::iterator i, it;
    for (i = _children.begin(); (it = i),(it++),(i!=_children.end()); i=it)
        (*i)->removeFromParent();
}

PHDrawable::~PHDrawable()
{
    for (set<lua_State*>::iterator i = luaStates.begin(); i!=luaStates.end(); i++)
        PHLuaDeleteWeakRef(*i, this);
    removeFromParent();
    removeAllChildren();
    if (mtx) mtx->release();
}

void PHDrawable::bringToFront()
{
	retain();
	PHDrawable * su = _parent;
	removeFromParent();
	su->addChild(this);
	release();
}

void PHDrawable::sendToBack()
{
	retain();
	PHDrawable * su = _parent;
	removeFromParent();
    list<PHDrawable*>::iterator i = _children.begin();
	su->addChildBefore(this, (i==_children.end())?NULL:*i);
	release();
}

PHDrawable * PHDrawable::childWithTag(int tag)
{
    for (list<PHDrawable*>::iterator i = _children.begin(); i!=_children.end(); i++)
        if ((*i)->tag() == tag)
            return *i;
    return NULL;
}

PHDrawable * PHDrawable::childWithTagAfter(int tag, PHDrawable * v)
{
    if (v && (v->_parent != this)) v = NULL;
    list<PHDrawable*>::iterator i = v?(v->currPos):_children.begin();
    if (v) i++;
    for (; i!=_children.end(); i++)
        if ((*i)->tag() == tag)
            return *i;
    return NULL;
}

list<PHDrawable*> * PHDrawable::childrenWithTag(int tag)
{
    list<PHDrawable*> * l = new list<PHDrawable*>;
    for (list<PHDrawable*>::iterator i = _children.begin(); i!=_children.end(); i++)
        if ((*i)->tag() == tag)
            l->push_back(*i);
    return l;
}

void PHDrawable::getLuaHandle(lua_State * L)
{
    if (!L) return;
    PHLuaGetWeakRef(L, this);
    if (!lua_istable(L, -1))
    {
        lua_pop(L,1);
        lua_getglobal(L, luaClass->c_str());
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

void PHDrawable::registerLuaInterface(lua_State * L)
{
    lua_getglobal(L, "PHDrawable");
    lua_pop(L, 1);
}


