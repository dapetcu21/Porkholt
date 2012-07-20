/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHDRAWABLE_H
#define PHDRAWABLE_H

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Core/PHGameManager.h>

class PHMutex;
class PHAuxLayerView;
class PHView;
class PHDrawable : public PHObject
{
protected:
    list<PHDrawable*> _children;
    list<PHDrawable*>::iterator currPos;
    
    PHGameManager * gm;
    
	PHDrawable * _parent;
    size_t _tag;
    PHMutex * mtx;
    
	virtual void draw() {}
public:
    virtual void render();
    
    virtual bool supportsRenderMode(int rm) { return rm == PHGameManager::defaultRenderMode; }
    
    const list<PHDrawable*> & children() { return _children; }
	
	PHDrawable * parent() { return _parent; };
	
	void addChild(PHDrawable * view) { addChildBefore(view, NULL); }
    void addChildBefore(PHDrawable * view, PHDrawable * before);
	void removeFromParent();
    void removeAllChildren();
	void bringToFront();
    void sendToBack();
    
    void setTag(size_t tag) { _tag = tag; };
    size_t tag() { return _tag; }

    PHDrawable * childWithTag(size_t tag);
    PHDrawable * childWithTagAfter(size_t tag, PHDrawable * after);
    list<PHDrawable*> * childrenWithTag(size_t tag); //this returns a new-allocated list
	
    PHDrawable();
	virtual ~PHDrawable();
	friend class PHGameManager;
	
protected:
    set<lua_State*> luaStates;
    const string * luaClass;
    static const string _luaClass;
public:
    const string & getLuaClass() { return *luaClass; }
    void getLuaHandle(lua_State * L);
    static void registerLuaInterface(lua_State * L);
public:
    PHMutex * mutex() { if (!mtx) mtx = new PHMutex(true); return mtx; }
    
public:
    PHGameManager * gameManager() { return gm; }
    void setGameManager(PHGameManager * gameManager);
    
protected:
    virtual void attachedToGameManager() {};
    
    bool _isView;
public:
    bool isView() { return _isView; }
    PHView * toView() { if (_isView) return (PHView*)this; return NULL; }
};

#endif
