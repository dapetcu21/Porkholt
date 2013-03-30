/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHQUATERNION_H
#define PHQUATERNION_H

#include <Porkholt/Geometry/PHPoint.h>

#ifdef PH_PACKED_STRUCT_PRAGMA
#pragma pack(1)
#endif

struct PHQuaternion
{
    union
    {
        float r,w,a,q0;
    };
    union
    {
        float x,b,q1;
    };
    union
    {
        float y,c,q2;
    };
    union
    {
        float z,d,q3;
    };

    bool operator == (const PHQuaternion & o) const {
        return (r==o.r)&&(x==o.x)&&(y==o.y)&&(z==o.z);
    }
    bool operator != (const PHQuaternion & o) const {
        return (r!=o.r)||(x!=o.x)||(y!=o.y)||(z!=o.z);
    }
    const PHQuaternion & operator *= (float re)
    {
        a*=re;
		b*=re;
		c*=re;
		d*=re;
        return * this;
    }
	const PHQuaternion & operator /= (float re)
    {
        a/=re;
		b/=re;
		c/=re;
		d/=re;
        return * this;
    }
    PHQuaternion operator * (float re) const
    {
        return PHQuaternion(a*re, b*re, c*re, d*re);
    }
	PHQuaternion operator / (float re) const
    {
        return PHQuaternion(a/re, b/re, c/re, d/re);
    }
    const PHQuaternion & operator *= (const PHQuaternion & o)
    {
		(*this) = (*this) * o;
        return * this;
    }
    PHQuaternion operator * (const PHQuaternion & o) const
    {
        return PHQuaternion(q0*o.q0 - q1*o.q1 - q2*o.q2 - q3*o.q3,
							q0*o.q1 + q1*o.q0 + q2*o.q3 - q3*o.q2,
							q0*o.q2 + q2*o.q0 + q3*o.q1 - q1*o.q3,
							q0*o.q3 + q3*o.q0 + q1*o.q2 - q2*o.q1);
    } 

    PHVector3 operator * (const PHVector3 & o) const;

    PHQuaternion(float _a, float _b, float _c, float _d) : a(_a), b(_b), c(_c), d(_d) {};
	PHQuaternion(float alpha, PHVector3 axis) : r(cos(alpha/2)) 
	{ 
        axis.normalize();
		float s = sin(alpha/2); 
		x=axis.x*s; 
		y=axis.y*s; 
		z=axis.z*s;
	};
    PHQuaternion(const PHVector4 & v) : r(v.x), x(v.y), y(v.z), z(v.t) {};
    PHQuaternion() {};
	PHVector3 vector() const { return PHVector3(x,y,z); }
	PHVector3 rotationAxis() const { float scale = sqrt(x*x + y*y + z*z); return PHVector3(x/scale, y/scale, z/scale); }
	float length() const { return sqrt(r*r + x*x + y*y + z*z); }
	float squaredLength() const { return r*r + x*x + y*y + z*z; }
	float rotationAngle() const { return 2 * acos(r / length()); }
	void normalize() { (*this) /= length(); }
	PHQuaternion normalized() const { return (*this) / length(); }
    static PHQuaternion fromLua(lua_State * L, int index);
    void saveToLua(lua_State * L) const;
	static PHQuaternion rotation(float alpha, float x, float y, float z)
	{
		float s = sin(alpha/2);
		return PHQuaternion(cos(alpha/2), x*s, y*s, z*s);
	}
	PHMatrix rotationMatrix(); //use normalize() first

    void toEuler(float & heading, float & attitude, float & bank);
    static PHQuaternion fromEuler(float heading, float attitude, float bank);
    static PHQuaternion fromPointsOnSphere(const PHVector3 & from, const PHVector3 & to);
} PH_PACKED_STRUCT;

#ifdef PH_PACKED_STRUCT_PRAGMA
#pragma pack pop
#endif

extern const PHQuaternion PHIdentityQuaternion;

namespace PHQuaternion_conditions {
    PH_STATIC_ASSERT(sizeof(PHQuaternion) == sizeof(float)*4);
}

#endif
