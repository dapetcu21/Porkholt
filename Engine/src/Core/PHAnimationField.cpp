/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHCinematicActor.h>
#define DOT_THRESHOLD 0.9995f

enum PHAnimationFieldType
{
    //additive/non-additive? exponential/linear?
    tFloat_A_L = 0,
    tFloat_N_L = 1,
    tFloat_A_E = 2,
    tFloat_N_E = 3,

    tVec2_A_L = 4,
    tVec2_N_L = 5,
    tVec2_A_E = 6,
    tVec2_N_E = 7,

    tVec3_A_L = 8,
    tVec3_N_L = 9,
    tVec3_A_E = 10,
    tVec3_N_E = 11,

    tColor = 12,

    tQuat_A = 13,
    tQuat_N =14,
};

#define cast(type) (*((type ## _struct *)(void*)storage))

PHAnimationField::PHAnimationField(int f, ph_float value, bool additive, bool exponential) : field(f)
{
    cast(f).diff = value;
    type = tFloat_A_L | (additive == false) | (exponential != false)<<1;
}

PHAnimationField::PHAnimationField(int f, const PHVector2 & value, bool additive, bool exponential) : field(f)
{
    cast(v2).diff = value;
    type = tVec2_A_L | (additive == false) | (exponential != false)<<1;
}

PHAnimationField::PHAnimationField(int f, const PHVector3 & value, bool additive, bool exponential) : field(f)
{
    cast(v3).diff = value;
    type = tVec3_A_L | (additive == false) | (exponential != false)<<1;
}

PHAnimationField::PHAnimationField(int f, const PHColor & value) : field(f)
{
    cast(clr).end = value;
    type = tColor;
}

PHAnimationField::PHAnimationField(int f, const PHQuaternion & value, bool additive) : field(f)
{
    if (additive)
    {
        cast(quat_add).theta = value.rotationAngle();
        cast(quat_add).axis = value.rotationAxis();
        type = tQuat_A;
    } else {
        cast(quat).v2 = value;
        type = tQuat_N;
    }
}

void PHAnimationField::initState(PHCinematicActor * actor)
{
    switch (type)
    {
        case tFloat_N_L:
            cast(f).diff -= cast(f).start = actor->getAnimationFieldF(field);
            break;
        case tFloat_N_E:
            cast(f).diff /= cast(f).start = actor->getAnimationFieldF(field);
            break;
            
        case tVec2_N_L:
            cast(v2).diff -= cast(v2).start = actor->getAnimationFieldV2(field);
            break;
        case tVec2_N_E:
            cast(v2).diff /= cast(v2).start = actor->getAnimationFieldV2(field);
            break;

        case tVec3_N_L:
            cast(v3).diff -= cast(v3).start = actor->getAnimationFieldV3(field);
            break;
        case tVec3_N_E:
            cast(v3).diff /= cast(v3).start = actor->getAnimationFieldV3(field);
            break;

        case tColor:
            cast(clr).start = actor->getAnimationFieldC(field);
            break;

#define th cast(quat)
        case tQuat_N:
            th.v0 = PHVector4(actor->getAnimationFieldQ(field));
            if ((th.dot = PHVector4::dot(th.v0, th.v2)) < DOT_THRESHOLD)
            {
                th.theta = acosf(th.dot);
                th.v2 = th.v2 - th.v0 * th.dot;
                th.v2.normalize();
            } else {
                th.v2 -= th.v0;
            }
            break;
#undef th

        default:
            break;
    }
}

void PHAnimationField::animate(PHCinematicActor * a, ph_float lt, ph_float t)
{
    switch (type)
    {
        case tFloat_A_L:
            a->setAnimationFieldF(field, a->getAnimationFieldF(field) + cast(f).diff * (t - lt));
            break;
        case tFloat_N_L:
            a->setAnimationFieldF(field, cast(f).start + t * cast(f).diff);
            break;
        case tFloat_A_E:
            a->setAnimationFieldF(field, a->getAnimationFieldF(field) * powf(cast(f).diff, (t-lt)));
            break;
        case tFloat_N_E:
            a->setAnimationFieldF(field, cast(f).start * powf(cast(f).diff, t));
            break;

        case tVec2_A_L:
            a->setAnimationFieldV2(field, a->getAnimationFieldV2(field) + cast(v2).diff * (t-lt));
            break;
        case tVec2_N_L:
            a->setAnimationFieldV2(field, cast(v2).start + cast(v2).diff * t);
            break;
        case tVec2_A_E:
            t -= lt;
            a->setAnimationFieldV2(field, a->getAnimationFieldV2(field) * PHVector2(
                powf(cast(v2).diff.x, t), 
                powf(cast(v2).diff.y, t)));
            break;
        case tVec2_N_E:
            a->setAnimationFieldV2(field, cast(v2).start * PHVector2(
                powf(cast(v2).diff.x, t), 
                powf(cast(v2).diff.y, t)));
            break;

        case tVec3_A_L:
            a->setAnimationFieldV3(field, a->getAnimationFieldV3(field) + cast(v3).diff * (t-lt));
            break;
        case tVec3_N_L:
            a->setAnimationFieldV3(field, cast(v3).start + cast(v3).diff * t);
            break;
        case tVec3_A_E:
            t -= lt;
            a->setAnimationFieldV3(field, a->getAnimationFieldV3(field) * PHVector3(
                powf(cast(v3).diff.x, t), 
                powf(cast(v3).diff.y, t), 
                powf(cast(v3).diff.z, t)));
            break;
        case tVec3_N_E:
            a->setAnimationFieldV3(field, cast(v3).start * PHVector3(
                powf(cast(v3).diff.x, t), 
                powf(cast(v3).diff.y, t), 
                powf(cast(v3).diff.z, t)));
            break;

        case tColor:
            if (cast(clr).end.a <= 0)
            {
                PHColor c = cast(clr).start;
                c.a = c.a * (1-t);
                a->setAnimationFieldC(field, c);
            } else {
                a->setAnimationFieldC(field, cast(clr).start + (cast(clr).end - cast(clr).start) * t);
            }
            break;

        case tQuat_A:
            a->setAnimationFieldQ(field, a->getAnimationFieldQ(field) * PHQuaternion(cast(quat_add).theta * (t-lt), cast(quat_add).axis));
            break;
        case tQuat_N:
            if (cast(quat).dot < DOT_THRESHOLD)
                a->setAnimationFieldQ(field, PHQuaternion(cast(quat).v0 * cosf(cast(quat).theta) + cast(quat).v2 * sinf(cast(quat).theta)));
            else
                a->setAnimationFieldQ(field, PHQuaternion(cast(quat).v0 + cast(quat).v2 * t));
            break;
        default:
            break;
    }
}

#undef cast

