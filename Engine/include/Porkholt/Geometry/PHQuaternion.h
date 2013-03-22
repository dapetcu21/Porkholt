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
        ph_float r,w,a,q0;
    };
    union
    {
        ph_float x,b,q1;
    };
    union
    {
        ph_float y,c,q2;
    };
    union
    {
        ph_float z,d,q3;
    };

    bool operator == (const PHQuaternion & o) const {
        return (r==o.r)&&(x==o.x)&&(y==o.y)&&(z==o.z);
    }
    bool operator != (const PHQuaternion & o) const {
        return (r!=o.r)||(x!=o.x)||(y!=o.y)||(z!=o.z);
    }
    const PHQuaternion & operator *= (ph_float re)
    {
        a*=re;
		b*=re;
		c*=re;
		d*=re;
        return * this;
    }
	const PHQuaternion & operator /= (ph_float re)
    {
        a/=re;
		b/=re;
		c/=re;
		d/=re;
        return * this;
    }
    PHQuaternion operator * (ph_float re) const
    {
        return PHQuaternion(a*re, b*re, c*re, d*re);
    }
	PHQuaternion operator / (ph_float re) const
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

    PHQuaternion(ph_float _a, ph_float _b, ph_float _c, ph_float _d) : a(_a), b(_b), c(_c), d(_d) {};
	PHQuaternion(ph_float alpha, PHVector3 axis) : r(cos(alpha/2)) 
	{ 
        axis.normalize();
		ph_float s = sin(alpha/2); 
		x=axis.x*s; 
		y=axis.y*s; 
		z=axis.z*s;
	};
    PHQuaternion(const PHVector4 & v) : r(v.x), x(v.y), y(v.z), z(v.t) {};
    PHQuaternion() {};
	PHVector3 vector() const { return PHVector3(x,y,z); }
	PHVector3 rotationAxis() const { ph_float scale = sqrt(x*x + y*y + z*z); return PHVector3(x/scale, y/scale, z/scale); }
	ph_float length() const { return sqrt(r*r + x*x + y*y + z*z); }
	ph_float squaredLength() const { return r*r + x*x + y*y + z*z; }
	ph_float rotationAngle() const { return 2 * acos(r / length()); }
	void normalize() { (*this) /= length(); }
	PHQuaternion normalized() const { return (*this) / length(); }
    static PHQuaternion fromLua(lua_State * L, int index);
    void saveToLua(lua_State * L) const;
	static PHQuaternion rotation(ph_float alpha, ph_float x, ph_float y, ph_float z)
	{
		ph_float s = sin(alpha/2);
		return PHQuaternion(cos(alpha/2), x*s, y*s, z*s);
	}
	PHMatrix rotationMatrix(); //use normalize() first

    void toEuler(ph_float & heading, ph_float & attitude, ph_float & bank);
    static PHQuaternion fromEuler(ph_float heading, ph_float attitude, ph_float bank);
    static PHQuaternion fromPointsOnSphere(const PHVector3 & from, const PHVector3 & to);
} PH_PACKED_STRUCT;

#ifdef PH_PACKED_STRUCT_PRAGMA
#pragma pack pop
#endif

extern const PHQuaternion PHIdentityQuaternion;

namespace PHQuaternion_conditions {
    PH_STATIC_ASSERT(sizeof(PHQuaternion) == sizeof(ph_float)*4);
}

#endif
