/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHMESSAGE_H
#define PHMESSAGE_H

#include <Porkholt/Core/PHMain.h>

class PHMessage;

class PHMessagePool 
{
    private:
        map<string,PHMessage*> messages;
    public:
        PHMessage * messageWithName(const string & name);
        ~PHMessagePool();
};

class PHMessage : public PHObject
{
    private:
        string name;
        PHMessage(string name);
        list<PHInvocation> objects;
        static PHMessagePool pool;
        friend class PHMessagePool;
        
    public:
        
        void addListener(const PHInvocation & inv);
        void removeListener(PHObject * obj);
        void removeListener(PHObject * obj, PHCallback cb);
        void removeListener(const PHInvocation & inv);
        void broadcast(PHObject * sender, void * ud);
        void broadcast(PHObject * sender);
        
        static PHMessage * messageWithName(const string & name)
        {
            return pool.messageWithName(name);
        }
};

#endif
