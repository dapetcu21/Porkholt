/*
 *  PHMessage.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/30/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#ifndef PHMESSAGE_H
#define PHMESSAGE_H

#include <map>
#include <list>
#include <utility>

class PHObject;

class PHMessage : public PHObject
{
private:
	string name;
	static map<string,PHMessage*> messages;
	PHMessage(string name);
	
	list< pair<PHObject*,PHCallback> > objects;
	
public:
	~PHMessage();
	
	static PHMessage * messageWithName(string name);
	void addListener(PHObject * obj, PHCallback cb);
	void removeListener(PHObject * obj);
	void removeListener(PHObject * obj, PHCallback cb);
	void broadcast(PHObject * sender, void * ud);
};

#endif