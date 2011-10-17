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

//#define VIRTUAL_PHOBJECT

#ifdef VIRTUAL_PHOBJECT
#define PHOBJECT_PREFIX virtual
#else
#define PHOBJECT_PREFIX
#endif

class PHObject
{
private:
	volatile int _refcount;
public:
	PHObject(): _refcount(1) {};
	PHOBJECT_PREFIX PHObject * retain() { _refcount++; return this;};
	PHOBJECT_PREFIX PHObject * release() { _refcount--; if (!_refcount) { delete this; return NULL; }; return this; };
	virtual ~PHObject() {}
	int referenceCount() { return _refcount; };
};

#endif