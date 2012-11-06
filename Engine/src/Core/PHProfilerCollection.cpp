/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#include <Porkholt/Core/PHProfilerCollection.h>
#include <iomanip>

PHProfilerCollection::PHProfilerCollection() : autoDump(0), overt(false), oratio(1)
{
}

PHProfilerCollection::~PHProfilerCollection()
{
    for (vector<PHProfiler*>::iterator i = profs.begin(); i != profs.end(); i++)
        (*i)->release();
}

void PHProfilerCollection::addProfiler(PHProfiler * p)
{
    p->retain();
    profs.push_back(p);
    clear();
}

void PHProfilerCollection::removeProfiler(PHProfiler * p)
{
    p->release();
    for (vector<PHProfiler*>::iterator i = profs.begin(); i != profs.end(); i++)
        if (*i == p)
        {
            profs.erase(i);
            break;
        }
    clear();
}

void PHProfilerCollection::clear()
{
    d.clear();
}

void PHProfilerCollection::dump()
{
    for (vector<vector<double> >::iterator i = d.begin(); i != d.end(); i++)
    {
        bool printit = true;
        size_t n = profs.size();
        double tm = i->at(n);
        if (overt)
        {
            printit = false;
            for (size_t j = 0; j < n; j++)
                if (i->at(j) >= tm*oratio)
                {
                    printit = true;
                    break;
                }
        }
        if (printit)
        {
            stringstream oss;
            for (size_t j = 0; j < n; j++)
                oss << profs[j]->name() << ": " << setprecision(3) << i->at(j)*1000 << "ms (" << i->at(j)/tm * 100 << "%%) ";
            PHLog(oss.str());
        }
    }
}

void PHProfilerCollection::collect(double frameTime)
{
    vector<vector<double> >::iterator i = d.insert(d.end(), vector<double>());
    for (vector<PHProfiler*>::iterator j = profs.begin(); j != profs.end(); j++)
    {
        PHProfiler * p = *j;
        i->push_back(p->totalTime()); 
        p->clear();
    }
    i->push_back(frameTime);
    if (autoDump)
    {
        if (dcount)
            dcount--;
        else
        {
            dcount = autoDump;
            dump();
            clear();
        }
    }
}

#ifdef PH_PROFILING
PHProfiler * PHLuaProfiler;
PHProfilerCollection * PHMainProfilers;

PH_INITIALIZER(PHProfilerCollection_init)
{
    PHLuaProfiler = new PHProfiler("lua");
    PHMainProfilers = new PHProfilerCollection();
    PHMainProfilers->addProfiler(PHLuaProfiler);
    PHMainProfilers->setAutoClearAndDump(60);
    PHMainProfilers->setOvertimeRatio(0.2);
    PHMainProfilers->setOnlyDumpOvertime(true);
}
#endif
