/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHPROFILER_H
#define PHPROFILER_H

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Core/PHTime.h>

class PHProfiler : public PHObject
{
    protected:
        double time, tstart;
        int recursing;
        bool cpu;
        double oratio; 
        string nam;
    public:
#define PHPROFILER_INIT time(0), recursing(0), cpu(true), oratio(0.0)
        PHProfiler() : PHPROFILER_INIT {}
        PHProfiler(const string & nm) : PHPROFILER_INIT, nam(nm) {}
#undef PHPROFILER_INIT

        void setName(const string & nm) { nam = nm; }
        const string & name() { return nam; }
        double overtimeRatio() { return oratio; }
        void setOvertimeRatio(double d) { oratio = d; }
        bool useCpuTime() { return cpu; }
        void setUseCpuTime(bool c) { cpu = c; }

        double getTime()
        {
            if (cpu)
                return PHTime::getCPUTime();
            return PHTime::getTime();
        }

        void profileStart()
        {
            if (!recursing++)
                tstart = getTime();
        }

        void profileStop()
        {
            if (!--recursing)
                time += getTime() - tstart;
        }

        double totalTime() 
        { 
            if (recursing)
            {
                double t = getTime();
                time += t - tstart;
                tstart = t;
            }
            return time; 
        }
        void clear() 
        { 
            time = 0; 
            if (recursing) 
                tstart = getTime();
        }
};

#endif
