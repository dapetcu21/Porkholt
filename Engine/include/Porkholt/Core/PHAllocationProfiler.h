/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHALLOCATIONPROFILER_H
#define PHALLOCATIONPROFILER_H

#include <map>
#include <vector>
#include <string>
#include <list>

class PHFile;
class PHObject;

class PHAllocationProfiler
{
    protected:
        class Object
        {
            public:
                Object(void * addr);

                class Trace
                {
                    public:
                        enum trace_type
                        {
                            TraceAlloc,
                            TraceDealloc,
                            TraceRetain,
                            TraceRelease,
                        };
                        Trace(void * address, enum trace_type type, int refcount, int strip, int count);
                        enum trace_type type;
                        int refcount;
                        void * address;
                        std::vector<std::string> backtrace;

                        void dump(PHFile * f);
                };
                void * address;
                int refcount;
                std::string name;
                std::list<Trace*> traces;

                void addTrace(Trace * t);
                void dump(PHFile * f);
        };
        std::map<void *, Object *> objects;

    public:
        void objectRetain(PHObject * o);
        void objectRelease(PHObject * o);
        void addTrace(Object::Trace * t);

        void dump();

        static PHAllocationProfiler & singleton();
};

class PHAllocationDumper
{
    public:
        ~PHAllocationDumper()
        {
            PHAllocationProfiler::singleton().dump();
        }
};

#endif
