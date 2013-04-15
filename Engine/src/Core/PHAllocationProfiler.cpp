/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Core/PHAllocationProfiler.h>
#include <Porkholt/IO/PHRegularFile.h>
#include <Porkholt/IO/PHEmbeddedFile.h>
#include <typeinfo>
#include <execinfo.h>

PHAllocationProfiler & PHAllocationProfiler::singleton()
{
    static PHAllocationProfiler * s = new PHAllocationProfiler();
    return *s;
}

void PHAllocationProfiler::objectRetain(PHObject * o)
{
    int r = o->referenceCount();
    addTrace(new Object::Trace(o, r == 1 ? Object::Trace::TraceAlloc  : Object::Trace::TraceRetain, r, 2, 10));
}

void PHAllocationProfiler::objectRelease(PHObject * o)
{
    int r = o->referenceCount();
    addTrace(new Object::Trace(o, Object::Trace::TraceRelease, r, 2, 10));
}

void PHAllocationProfiler::objectDealloc(PHObject * o)
{
    int r = o->referenceCount();
    addTrace(new Object::Trace(o, Object::Trace::TraceDealloc, r, 2, 10));
}

void PHAllocationProfiler::addTrace(PHAllocationProfiler::Object::Trace * t)
{
    map<void *, Object*>::iterator i = objects.find(t->address);
    if (i == objects.end())
        i = objects.insert(make_pair(t->address, new Object(t->address))).first;
    i->second->addTrace(t);
}

void PHAllocationProfiler::dump()
{
    PHFile * f = NULL;
    PHFile * cf = NULL;
    PHLog("Saving allocation log to ./ph_alloc_log.lua");
    try {
    f = new PHRegularFile("ph_alloc_log.lua", false);
    f->open(PHFile::Write | PHFile::Create | PHFile::Trunc);
    f->write("objects = {\n");
    objectRelease(f);
    for (map<void *, Object *>::iterator i = objects.begin(); i != objects.end(); i++)
        if (i->second->refcount != 0)
            i->second->dump(f);
    objectRetain(f);
    f->write("}\n");
    cf = new PHEmbeddedFile("scripts/alloc_log.lua");
    f->write(cf->loadToString());
    } catch (string ex)
    {
        PHLog("Can't save allocation log: %s", ex.c_str());
    };
    if (f)
    {
        f->close();
        f->release();
    }
    if (cf)
    {
        cf->close();
        cf->release();
    }
}

PHAllocationProfiler::Object::Object(void * addr) : address(addr), refcount(0)
{
}

#include <cxxabi.h>

void PHAllocationProfiler::Object::addTrace(PHAllocationProfiler::Object::Trace * t)
{
    traces.push_back(t);
    if (t->type != Trace::TraceAlloc)
        if (name == "")
        {
            int status;
            const char * ss = typeid(*static_cast<PHObject*>(address)).name();
            char * s = abi::__cxa_demangle(ss, 0, 0, &status);
            name = status ? ss : s;
            free(s);
        }
    switch (t->type)
    { 
        case Trace::TraceAlloc:
            if (t->global_init)
                break;
        case Trace::TraceRetain:
            refcount++;
            break;
        case Trace::TraceDealloc:
        case Trace::TraceRelease:
            refcount--;
            break;
    }
}

void PHAllocationProfiler::Object::dump(PHFile * f)
{
    stringstream ss;
    ss << "  {\n";
    ss << "    refcount = " << refcount << ",\n";
    ss << "    class = \"" << name << "\",\n";
    ss << "    address = \"" << std::hex << address << "\",\n";
    ss << "    traces = {\n";
    f->write(ss.str());
    for (list<Trace *>::iterator i = traces.begin(); i != traces.end(); i ++)
        (*i)->dump(f);
    f->write("    },\n  },\n");
}

PHAllocationProfiler::Object::Trace::Trace(void * addr, enum PHAllocationProfiler::Object::Trace::trace_type _type, int _refcount, int strip, int count) : refcount(_refcount), address(addr), type(_type), global_init(false)
{
    strip++;
    void ** tr = (void**)malloc(sizeof(void*)*(strip + count));
    int total = ::backtrace(tr, strip + count);
    total -= strip;
    if (total > 0)
    {
        backtrace.reserve(total);
        char ** sym = backtrace_symbols(tr + strip, total);
        for (int i = 0; i < total; i++)
        {
            if (type == TraceAlloc && !global_init && strstr(sym[i], "_GLOBAL__I_a"))
                global_init = true;
            backtrace.push_back(string(sym[i]));
        }
        free(sym);
    }
    free(tr);
}

void PHAllocationProfiler::Object::Trace::dump(PHFile * f)
{
    stringstream ss;
    ss << "      {\n";
    ss << "        type = " << (int)type << ",\n";
    ss << "        refcount = " << refcount<< ",\n";
    ss << "        address = \"" << std::hex << address << "\",\n";
    ss << "        backtrace = {\n";
    for (vector<string>::iterator i = backtrace.begin(); i != backtrace.end(); i++)
    ss << "          \"" << *i << "\",\n";
    ss << "        },\n";
    ss << "      },\n";
    f->write(ss.str());
}
