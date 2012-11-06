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
        string nam;
    public:
        PHProfiler() : time(0), recursing(0) {}
        PHProfiler(const string & nm) : time(0), recursing(0), nam(nm) {}
        void setName(const string & nm) { nam = nm; }
        const string & name() { return nam; }

        void profileStart()
        {
            if (!recursing++)
                tstart = PHTime::getTime();
        }

        void profileStop()
        {
            if (!--recursing)
                time += PHTime::getTime() - tstart;
        }

        double totalTime() { return time; }
        void clear() { time = 0; }
};

#endif
