/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHMessage.h>

PHMessagePool PHMessage::pool;

PHMessage * PHMessagePool::messageWithName(const string & name)
{
	map<string,PHMessage*>::iterator i = messages.find(name);
	if (i!=messages.end())
		return (i->second);
	PHMessage * msg = new PHMessage(name);
	messages[name] = msg;
	return msg;
}

PHMessagePool::~PHMessagePool()
{
    for (map<string,PHMessage*>::iterator i = messages.begin(); i != messages.end(); i++)
        i->second->release();
}

PHMessage::PHMessage(string _name)
{
	name = name;
}

void PHMessage::addListener(const PHInvocation & inv)
{
	objects.push_back(inv);
}
					  
void PHMessage::removeListener(PHObject * obj)
{
	list<PHInvocation>::iterator nx,i;
	for ((i = nx = objects.begin()),nx++; i!=objects.end(); i=nx,nx++)
	{
		if (i->target == obj)
			objects.erase(i);
	}
}	
void PHMessage::removeListener(PHObject * obj, PHCallback cb)
{
	list<PHInvocation>::iterator nx,i;
	for ((i = nx = objects.begin()),nx++; i!=objects.end(); i=nx,nx++)
	{
		if ((i->target == obj)&&(i->callback == cb))
			objects.erase(i);
	}
}

void PHMessage::removeListener(const PHInvocation & inv)
{
    removeListener(inv.target, inv.callback);
}

void PHMessage::broadcast(PHObject * sender, void * ud)
{
    list<PHInvocation> obj = objects;
	for (list<PHInvocation>::iterator i = obj.begin(); i!=obj.end(); i++)
        i->call(sender, ud);
}

void PHMessage::broadcast(PHObject * sender)
{
    list<PHInvocation> obj = objects;
    for (list<PHInvocation>::iterator i = obj.begin(); i!=obj.end(); i++)
        i->call(sender);
}

