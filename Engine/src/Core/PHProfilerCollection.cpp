/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#include <Porkholt/Core/PHProfilerCollection.h>
#include <iomanip>

PHProfilerCollection::PHProfilerCollection() : autoDump(0), overt(false)
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
        const double tm[2] = {i->at(n), i->at(n+1)};
        if (overt)
        {
            printit = false;
            for (size_t j = 0; j < n; j++)
                if (i->at(j) >= tm[profs[j]->useCpuTime()] * profs[j]->overtimeRatio())
                {
                    printit = true;
                    break;
                }
        }
        if (printit)
        {
            stringstream oss;
#define mani setprecision(2) << setw(6) << right << fixed
            oss << "total: " << mani << (int)(tm[1]*1000000) << "us cpu, " << mani << (int)(tm[0]*1000000) << "us real | ";
            for (size_t j = 0; j < n; j++)
                oss << profs[j]->name() << ": " << mani << (int)(i->at(j)*1000000) << "us " << mani << i->at(j)/tm[profs[j]->useCpuTime()] * 100 << "%% " << (profs[j]->useCpuTime() ? "cpu" : "real") << " | ";
#undef mani
            PHLog(oss.str());
        }
    }
}

void PHProfilerCollection::start()
{
    lastTime = PHTime::getTime();
    lastCPUTime = PHTime::getCPUTime();
}

void PHProfilerCollection::collect()
{
    double ntime = PHTime::getTime();
    double nctime = PHTime::getCPUTime();
    double time = ntime - lastTime;
    double cpuTime = nctime - lastCPUTime;
    lastTime = ntime;
    lastCPUTime = nctime;

    vector<vector<double> >::iterator i = d.insert(d.end(), vector<double>());
    for (vector<PHProfiler*>::iterator j = profs.begin(); j != profs.end(); j++)
    {
        PHProfiler * p = *j;
        i->push_back(p->totalTime()); 
        p->clear();
    }
    i->push_back(time);
    i->push_back(cpuTime);
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
PHProfiler * PHRenderProfiler;
PHProfilerCollection * PHMainProfilers;

PH_INITIALIZER(PHProfilerCollection_init)
{
    PHLuaProfiler = new PHProfiler("lua");
    PHLuaProfiler->setOvertimeRatio(0.2);

    PHRenderProfiler = new PHProfiler("render");
    PHRenderProfiler->setOvertimeRatio(0.5);
    PHRenderProfiler->setUseCpuTime(false);

    PHMainProfilers = new PHProfilerCollection();
    PHMainProfilers->addProfiler(PHLuaProfiler);
    PHMainProfilers->addProfiler(PHRenderProfiler);
    PHMainProfilers->setAutoClearAndDump(60);
//    PHMainProfilers->setOnlyDumpOvertime(true);
    PHMainProfilers->start();
}
#endif
