/*
 *  PHMessage.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/30/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHMessage.h"

map<string,PHMessage*> PHMessage::messages;

PHMessage * PHMessage::messageWithName(string name)
{
	map<string,PHMessage*>::iterator i = messages.find(name);
	if (i!=messages.end())
		return (i->second);
	PHMessage * msg = new PHMessage(name);
	messages[name] = msg;
	return msg;
}

PHMessage::PHMessage(string _name)
{
	name = name;
}

PHMessage::~PHMessage()
{
	map<string,PHMessage*>::iterator i = messages.find(name);
	if (i==messages.end())
		return;
	messages.erase(i);
}

void PHMessage::addListener(PHObject * obj, PHCallback cb)
{
	objects.push_back(make_pair<PHObject*,PHCallback>(obj,cb));
}
					  
void PHMessage::removeListener(PHObject * obj)
{
	list< pair<PHObject*,PHCallback> >::iterator nx,i;
	for ((i = nx = objects.begin()),nx++; i!=objects.end(); i=nx,nx++)
	{
		if (i->first==obj)
			objects.erase(i);
	}
}	
void PHMessage::removeListener(PHObject * obj, PHCallback cb)
{
	
	list< pair<PHObject*,PHCallback> >::iterator nx,i;
	for ((i = nx = objects.begin()),nx++; i!=objects.end(); i=nx,nx++)
	{
		if ((i->first==obj)&&(i->second==cb))
			objects.erase(i);
	}
}
void PHMessage::broadcast(PHObject * sender, void * ud)
{
    list< pair<PHObject*,PHCallback> > obj = objects;
	for (list< pair<PHObject*,PHCallback> >::iterator i = obj.begin(); i!=obj.end(); i++)
	{
		((i->first)->*(i->second))(sender,ud);
	}
}