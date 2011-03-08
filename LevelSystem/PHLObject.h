/*
 *  PHLObject.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/6/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#ifndef PHLOBJECT_H
#define PHLOBJECT_H

#define INSIDE_PHLOBJECT_H

#include "PHMain.h"

class PHImageView;

class PHLObject : public PHObject
{
protected:
	string _class;
	PHPoint pos;
	PHView * view;
	PHWorld * wrld;
	
	struct Image
	{
		PHImageView * img;
		PHRect bounds;
	};
	list<Image> images;
	PHRect viewSize;
	void loadImages();
	
	friend class PHWorld;
	
public:
	PHLObject();
	virtual ~PHLObject();
	const string & getClass() {return _class;};
	PHView * getView() { return view;};
	PHPoint position() { return pos; };
	void setPosition(PHPoint p);
	
	virtual void loadFromLUA(void * L, const string & root);
	virtual void loadView();
	
	static PHLObject * objectWithClass(const string & str);
};

#endif