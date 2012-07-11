/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHSERIALIZATION_H
#define PHSERIALIZATION_H

#include <arpa/inet.h>
#include <stdint.h>
#include <cmath>

#ifndef htonll
#ifdef __BIG_ENDIAN__
#define htonll(x) (x)
#else
#define htonll(x) PHByteSwap64(x)
#endif

#if defined(__GNUC__)
#define PHByteSwap64(x) __builtin_bswap64(x)
#define PHByteSwap32(x) __builtin_bswap32(x)
inline uint16_t PHByteSwap16(uint16_t x)
{
    return (x>>8) | (x<<8);
}
#elif defined(_MSC_VER)
#define PHByteSwap64(x) _byteswap_uint64(x)
#define PHByteSwap32(x) _byteswap_ulong(x)
#define PHByteSwap16(x) _byteswap_ushort(x)
#else
inline uint64_t PHByteSwap64(uint64_t x)
{
    return ((x<<56) | 
           ((x<<40) & 0xff000000000000ULL) |
           ((x<<24) & 0xff0000000000ULL) |
           ((x<<8)  & 0xff00000000ULL) |
           ((x>>8)  & 0xff000000ULL) |
           ((x>>24) & 0xff0000ULL) |
           ((x>>40) & 0xff00ULL) |
            (x>>56));
}

inline uint32_t PHByteSwap32(uint32_t x)
{
    return ((x<<24) |
           ((x<<8) & 0xff0000U) |
           ((x>>8) & 0xff00U) |
            (x>>24));
}

inline uint16_t PHByteSwap16(uint16_t x)
{
    return (x>>8) | (x<<8);
}
#endif

#ifdef __BIG_ENDIAN__
    #define PHBEToH16(x) (x)
    #define PHBEToH32(x) (x)
    #define PHBEToH64(x) (x)
    #define PHLEToH16(x) PHByteSwap16(x)
    #define PHLEToH32(x) PHByteSwap32(x)
    #define PHLEToH64(x) PHByteSwap64(x)
#else
    #define PHLEToH16(x) (x)
    #define PHLEToH32(x) (x)
    #define PHLEToH64(x) (x)
    #define PHBEToH16(x) PHByteSwap16(x)
    #define PHBEToH32(x) PHByteSwap32(x)
    #define PHBEToH64(x) PHByteSwap64(x)
#endif

#define ntohll htonll
#endif

inline uint16_t PHEncode(uint16_t n) 
{
    return htons(n);
}

inline uint32_t PHEncode(uint32_t n)
{
    return htonl(n);
}

inline uint64_t PHEncode(uint64_t n)
{
    return htonll(n);
}

inline uint16_t PHDecode(uint16_t n) 
{
    return ntohs(n);
}

inline uint32_t PHDecode(uint32_t n)
{
    return ntohl(n);
}

inline uint64_t PHDecode(uint64_t n)
{
    return ntohll(n);
}

template <class ftype>
inline ftype PHldexp(ftype f, int n)
{
    return ldexp(f,n);
}

template <>
inline float PHldexp<float>(float f, int n)
{
    return ldexpf(f,n);
}

template <class ftype>
inline ftype PHfrexp(ftype f, int * n)
{
    return frexp(f,n);
}

template <>
inline float PHfrexp<float>(float f, int * n)
{
    return frexpf(f,n);
}

template <class ftype,class itype,int exp_size,int mantissa_size,int exp_bias>
itype PHFloatToIEEE754(ftype f)
{
    itype sign = f<0;
    if (f==0)
        return 0;
    if (f != f)
        return ((((itype)1<<(exp_size+1))-1)<<(mantissa_size-1)); //qNaN
    if (f - f != 0)
        return (sign<<(exp_size+mantissa_size))|((((itype)1<<exp_size)-1)<<mantissa_size); //infinity
        
    int exp;
    if (sign)
        f=-f;
    f = frexp(f,&exp);
    exp--;
    f = PHldexp<ftype>(f,1);
    exp+=exp_bias;
    if (exp<=0)
    {
        f = PHldexp<ftype>(f,exp-1);
        exp = 0;
    }
    itype mantissa = (itype)(PHldexp<ftype>(f,mantissa_size)) & (((itype)1<<mantissa_size)-1);
    return (sign<<(exp_size+mantissa_size))| ((itype)exp<<mantissa_size) | mantissa;
}

template <class ftype,class itype,int exp_size,int mantissa_size,int exp_bias>
ftype PHFloatFromIEEE754(itype i)
{
    itype mantissa = i & (((itype)1<<mantissa_size)-1);
    int exp = (i>>mantissa_size) & (((itype)1<<exp_size)-1);
    if (!exp && !mantissa)
        return 0;
    if (exp == (((itype)1<<exp_size)-1))
    {
        if (mantissa)
            return NAN;
        if (i & (((itype)1)<<(exp_size+mantissa_size)))
            return -INFINITY;
        return INFINITY;
    }
    ftype f;
    if (exp == 0)
        f = PHldexp<ftype>((ftype)mantissa, -mantissa_size-exp_bias+1);
    else
        f = PHldexp<ftype>((ftype)(mantissa | ((itype)1<<mantissa_size)), -mantissa_size+exp-exp_bias);
    if (i & (((itype)1)<<(exp_size+mantissa_size)))
        return -f;
    else
        return f;
}

inline uint32_t PHEncodeF(float n)
{
    uint32_t e = PHFloatToIEEE754<float,uint32_t,8,23,127>(n);
    return htonl(e);
}

inline float PHDecodeF(uint32_t n)
{
    return PHFloatFromIEEE754<float,uint32_t,8,23,127>(ntohl(n));
}

inline uint64_t PHEncodeF(double n)
{
    uint64_t e = PHFloatToIEEE754<double,uint64_t,11,52,1023>(n);
    return htonll(e);
}

inline double PHDecodeF(uint64_t n)
{
    return PHFloatFromIEEE754<double,uint64_t,11,52,1023>(ntohll(n));
}

#endif
