
/*
 *  PHGeometry.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/18/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHGeometry.h"
#include "PHLua.h"


PHColor PHGLCurrentColor(-1,-1,-1,-1);

void PHLowPassFilter(ph_float & var, ph_float newval, ph_float period, ph_float cutoff)
{
	ph_float RC=1.0/cutoff;
	ph_float alpha=period/(period+RC);
	//alpha = 0.000005;
	var = newval * alpha + var * (1.0 - alpha);
}

bool PHPointInRect(const PHPoint & pnt, const PHRect & rect)
{
    if (pnt.x<rect.x || pnt.x>rect.x+rect.width) return false;
    if (pnt.y<rect.y || pnt.y>rect.y+rect.height) return false;
    return true;
}

bool PHPointInCircle(const PHPoint & pnt, const PHPoint & origin, ph_float radius)
{
    return (pnt-origin).length()<=radius;
}

bool PHRectIntersectsRect(const PHRect & r1, const PHRect & r2)
{
    return (PHPointInRect(r1.corner(0), r2)||
            PHPointInRect(r1.corner(1), r2)||
            PHPointInRect(r1.corner(2), r2)||
            PHPointInRect(r1.corner(3), r2)||
            PHPointInRect(r2.corner(0), r1)||
            PHPointInRect(r2.corner(1), r1)||
            PHPointInRect(r2.corner(2), r1)||
            PHPointInRect(r2.corner(3), r1));
}

ph_float PHAngleFromNormalizedVector(const PHPoint & vec)
{
    ph_float ang;
    if (abs(vec.x)<0.5)
    {
        ang = acos(vec.x);
        if (vec.y<0)
            ang = M_PI*2-ang;
    } else {
        ang = asin(vec.y);
        if (vec.x<0)
            ang = M_PI-ang;
    }
    if (ang>=M_PI*2)
        ang-=M_PI*2;
    if (ang<0)
        ang+=M_PI*2;
    return ang;
}

bool PHGLHasCapability(int cap)
{
    
    static bool cached[PHGLNumberCapabilities] = {};
    static bool values[PHGLNumberCapabilities] = {};
    static set<string> extensions;
    static bool parsedExtensions = false;
    static int openGLVersionMajor=0,openGLVersionMinor=0;
    
    if (cached[cap])
        return values[cap];
    bool val;

    if (!parsedExtensions)
    {
        const char * ver = (char*) glGetString(GL_VERSION);
        bool es = strstr(ver, "ES");
        cached[PHGLCapabilityOpenGLES] = true;
        values[PHGLCapabilityOpenGLES] = es;
        
        int number = 0;
        bool major = true;
        while (*ver)
        {
            if ((*ver)=='.')
            {
                if (major)
                {
                    openGLVersionMajor = number;
                    major = false;
                    number = 0;
                }
                else 
                    break;
            } else
            if (('0'<=(*ver)) && ((*ver)<='9'))
                number=number*10+*ver-'0';
            else
                if (number || !major)
                    break;
            ver++;
        }
        openGLVersionMinor = number;

#ifdef GL_NUM_EXTENSIONS
        if (!es && openGLVersionMajor>=3)
        {
            int n = (int)glGetInteger(GL_NUM_EXTENSIONS);
            for (int i=0; i<n; i++)
                extensions.insert(string((const char*)glGetStringi(GL_EXTENSIONS, i)));
        } else
#endif
        {
            const char * ext = (char*) glGetString(GL_EXTENSIONS);
            while (ext && ext[0])
            {
                const char * pos = strpbrk(ext, " ");
                if (!pos)
                    pos = ext+strlen(ext);
                extensions.insert(string(ext,pos-ext));
                if (!pos[0])
                    ext = pos;
                else
                    ext = pos+1;
            }
        }
        parsedExtensions = true;
        
        string s;
        for (set<string>::iterator i = extensions.begin(); i!=extensions.end(); i++)
        {
            s+=*i;
            s+=" ";
        }
        PHLog("OpenGL Version: %s %d.%d",es?"ES":"",openGLVersionMajor,openGLVersionMinor);
        PHLog("OpenGL Extensions: %s",s.c_str());
    }
    
    switch (cap)
    {
        case PHGLCapabilityNPOT:
        {
            val = (openGLVersionMajor>=2 || extensions.count("OES_texture_npot") || extensions.count("GL_ARB_texture_non_power_of_two") || extensions.count("GL_IMG_texture_npot"));
            break; 
        }
        case PHGLCapabilityAppleLimitedNPOT:
        {
            val = (extensions.count("GL_APPLE_texture_2D_limited_npot") && !extensions.count("OES_texture_npot"));
            break;
        }
        default:
            return false;
    }
    
    cached[cap] = true;
    values[cap] = val;
    return val;
}