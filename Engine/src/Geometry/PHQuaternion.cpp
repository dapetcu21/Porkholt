/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Geometry/PHQuaternion.h>
#include <Porkholt/Core/PHLua.h>

const PHQuaternion PHIdentityQuaternion(1,0,0,0);

PHMatrix PHQuaternion::rotationMatrix()
{
	return PHMatrix(q0*q0 + q1*q1 - q2*q2 - q3*q3, 2*(q2*q1 + q0*q3), 2*(q3*q1 - q0*q2), 0,
	 				2*(q1*q2 - q0*q3), q0*q0 - q1*q1 + q2*q2 - q3*q3, 2*(q2*q3 + q0*q1), 0,
					2*(q1*q3 + q0*q2), 2*(q2*q3 - q0*q1), q0*q0 - q1*q1 - q2*q2 + q3*q3, 0,
					0, 0, 0, q0*q0 + q1*q1 + q2*q2 + q3*q3);
}

PHQuaternion PHQuaternion::fromLua(lua_State * L, int index)
{
    if (!lua_istable(L, index)) return PHIdentityQuaternion;
    PHQuaternion q = PHIdentityQuaternion;
    
    lua_getfield(L, index, "r");
    if (lua_isnumber(L, -1))
        q.r = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, index, "x");
    if (lua_isnumber(L, -1))
        q.x = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, index, "y");
    if (lua_isnumber(L, -1))
        q.y = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, index, "z");
    if (lua_isnumber(L, -1))
        q.z = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return q;
}

void PHQuaternion::saveToLua(lua_State * L) const
{
    lua_newtable(L);
    lua_pushnumber(L, r);
    lua_setfield(L, -2, "r");
    lua_pushnumber(L, x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, y);
    lua_setfield(L, -2, "y");
    lua_pushnumber(L, z);
    lua_setfield(L, -2, "z");
}


PHQuaternion PHQuaternion::fromPointsOnSphere(const PHVector3 & from, const PHVector3 & to)
{
    if (from == to) return PHIdentityQuaternion;
    PHVector3 normal = PHVector3::cross(from, to);
    float cosa = PHVector3::dot(from, to);
    float cosa2 = sqrt((cosa + 1) / 2);
    normal /= 2*cosa2;
    return PHQuaternion(cosa2, normal.x, normal.y, normal.z);
}

void PHQuaternion::toEuler(float & heading, float & attitude, float & bank)
{
    float sqw = w*w;
    float sqx = x*x;
    float sqy = y*y;
    float sqz = z*z;
    float unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
    float test = x*y + z*w;
    if (test > 0.499*unit) { // singularity at north pole
        heading = 2 * atan2(x,w);
        attitude = M_PI_2;
        bank = 0;
        return;
    }
    if (test < -0.499*unit) { // singularity at south pole
        heading = -2 * atan2(x,w);
        attitude = -M_PI_2;
        bank = 0;
        return;
    }
    heading = atan2(2*y*w - 2*x*z, sqx - sqy - sqz + sqw);
    attitude = asin(2*test/unit);
    bank = atan2(2*x*w - 2*y*z, -sqx + sqy - sqz + sqw);
}

PHQuaternion PHQuaternion::fromEuler(float heading, float attitude, float bank)
{
    float c1 = cosf(heading/2);
    float s1 = sinf(heading/2);
    float c2 = cosf(attitude/2);
    float s2 = sinf(attitude/2);
    float c3 = cosf(bank/2);
    float s3 = sinf(bank/2);
    float c1c2 = c1*c2;
    float s1s2 = s1*s2;
    return PHQuaternion(c1c2*c3 - s1s2*s3,
                        c1c2*s3 + s1s2*c3,
                        s1*c2*c3 + c1*s2*s3,
                        c1*s2*c3 - s1*c2*s3);
}

PHVector3 PHQuaternion::operator* (const PHVector3 & v) const
{
    PHVector3 uv, uuv;
    PHVector3 qvec(x, y, z);
    uv = qvec.cross(v);
    uuv = qvec.cross(uv);
    uv *= (2.0f * w);
    uuv *= 2.0f;

    return v + uv + uuv;
}
