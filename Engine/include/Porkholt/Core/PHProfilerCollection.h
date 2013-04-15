/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHPROFILERCOLLECTION_H
#define PHPROFILERCOLLECTION_H

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Core/PHProfiler.h>

class PHProfilerCollection : public PHObject
{
    protected:
        vector<PHProfiler*> profs;
        vector<vector<double> > d;
        int autoDump;
        int dcount;
        bool overt;
        double lastTime;
        double lastCPUTime;
    public:
        PHProfilerCollection();
        ~PHProfilerCollection();

        const vector<PHProfiler*> & profilers() { return profs; }
        const vector<vector<double> > & data() { return d; }
        void addProfiler(PHProfiler* p);
        void removeProfiler(PHProfiler* p);

        void dump(); 
        void clear();

        int autoClearAndDump() { return autoDump; }
        void setAutoClearAndDump(int s) { autoDump = dcount = s; }
        bool onlyDumpOvertime() { return overt; }
        void setOnlyDumpOvertime(bool s) { overt = s; }
        
        void collect();
        void start();
};

#ifdef PH_PROFILING
#define PHProfilingStart(p) p->profileStart()
#define PHProfilingStop(p) p->profileStop()
extern PHProfiler * PHLuaProfiler;
extern PHProfiler * PHRenderProfiler;
extern PHProfilerCollection * PHMainProfilers;
#else
#define PHProfilingStart(p)
#define PHProfilingStop(p)
#define PHLuaProfiler NULL
#define PHMainProfilers NULL
#endif

#endif
