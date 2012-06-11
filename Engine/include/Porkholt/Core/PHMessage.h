/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHMESSAGE_H
#define PHMESSAGE_H

#include <Porkholt/Core/PHMain.h>

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
