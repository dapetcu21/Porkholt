/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHANIMATIONFIELD_H
#define PHANIMATIONFIELD_H

#include <Porkholt/Core/PHMain.h>

class PHCinematicActor;

class PHAnimationField
{
    protected:
        int type;
        int field;
        struct f_struct {
            ph_float diff, start;
        };
        struct v2_struct {
            PHVector2 diff, start;
        };
        struct v3_struct {
            PHVector3 diff, start;
        };
        struct clr_struct {
            PHColor start, end;
        };
        struct quat_struct {
            ph_float dot;
            ph_float theta;
            PHVector4 v0, v2;   
        };
        struct quat_add_struct {
            ph_float theta;
            PHVector3 axis;
        };

#define PHAFSMax(a,b) (((a) > (b)) ? (a) : (b))
        uint8_t storage[PHAFSMax(PHAFSMax(PHAFSMax(PHAFSMax(PHAFSMax(
            sizeof(f_struct), 
            sizeof(v2_struct)), 
            sizeof(v3_struct)), 
            sizeof(clr_struct)), 
            sizeof(quat_struct)), 
            sizeof(quat_add_struct))
        ];
        //fuck you C++. Why don't you let me do unions on classes?
#undef PHAFSMax

    public:
        PHAnimationField(int field, ph_float value, bool additive = false, bool exponential = false);
        PHAnimationField(int field, const PHVector2 & value, bool additive = false, bool exponential = false);
        PHAnimationField(int field, const PHVector3 & value, bool additive = false, bool exponential = false);
        PHAnimationField(int field, const PHColor & value);
        PHAnimationField(int field, const PHQuaternion & value, bool additive = false);

        void initState(PHCinematicActor * actor);
        void animate(PHCinematicActor * actor, ph_float lt, ph_float t);
};

#endif
