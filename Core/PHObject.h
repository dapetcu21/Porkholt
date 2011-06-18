/*
 *  PHObject.h
 *  Porkholt
 *
 *  Created by Marius Petcu on 12/15/10.
 *  Copyright 2010 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef PHOBJECT_H
#define PHOBJECT_H

class PHObject
{
private:
	volatile int _refcount;
public:
	PHObject(): _refcount(1) {};
	PHObject * retain() { _refcount++; return this;};
	PHObject * release() { _refcount--; if (!_refcount) { delete this; return NULL; }; return this; };
	virtual ~PHObject() {}
	int referenceCount() { return _refcount; };
};

typedef void (PHObject::*PHCallback) (PHObject * sender, void * ud);

#endif