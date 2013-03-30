/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHUTILITIES_H
#define PHUTILITIES_H

//Includes

#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>
#include <sstream>
#include <climits>
#include <cfloat>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <algorithm>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <queue>
#include <utility>

using namespace std;

//Determining the environment

#ifdef __APPLE__
    #ifdef __IPHONE_OS_VERSION_MIN_REQUIRED
        #define PH_IPHONE_OS
        #define PH_MOBILE
        #define PH_GLES
        #ifdef __i386__
            #define PH_SIMULATOR
        #endif
    #else
        #define PH_MAC_OS
        #define PH_DESKTOP
    #endif
    #define PH_DARWIN
#endif

#ifdef __linux__
    #define PH_LINUX
    #ifdef __ANDROID__
        #define PH_ANDROID
        #define PH_MOBILE
        #define PH_GLES
    #else
        #define PH_DESKTOP
    #endif
#endif

#if !defined(PH_MOBILE) && !defined(PH_DESKTOP)
    #define PH_DESKTOP
#endif

#if defined(DEBUG) || defined(_DEBUG)
    #define PH_DEBUG
#endif


//Restrict

#if (__STDC_VERSION__ >= 199901L)
    #define PH_RESTRICT restrict
#else
    #define PH_RESTRICT __restrict
#endif


//Packed

#define PH_NO_PACKED_STRUCT

#ifdef PH_NO_PACKED_STRUCT
#define PH_PACKED_STRUCT
#else
#ifdef __GNUC__
#define PH_PACKED_STRUCT __attribute__((packed))
#elif _MSC_VER
#define PH_PACKED_STRUCT_PRAGMA
#define PH_PACKED_STRUCT
#endif
#endif


//Initializers

#ifdef _MSC_VER
    #pragma section(".CRT$XCU",read)
    #define PH_INITIALIZER(f) \
    static void __cdecl f(void); \
    __declspec(allocate(".CRT$XCU")) void (__cdecl*f##_)(void) = f; \
    static void __cdecl f(void)

#elif defined(__GNUC__)
    #define PH_INITIALIZER(f) \
    static void f(void) __attribute__((constructor, used)); \
    static void f(void)
#endif


//Allocators

typedef void * (*PHAllocator)(void);

template <class T>
void * PHAlloc(void)
{
    return (void*) new T;
}


//Miscelaneous

struct lua_State; 

#define PH_TOKENPASTE_(x, y) x ## y
#define PH_TOKENPASTE(x, y) PH_TOKENPASTE_(x,y)
#define PH_TOKENPASTE3(x, y) PH_TOKENPASTE(x, y)

#define PH_STRINGIFY_(x) #x
#define PH_STRINGIFY(x) PH_STRINGIFY_(x)
#define PH_STRINGIFY3(x) PH_STRINGIFY(x)

#ifdef __COUNTER__
#define PH_UNIQUE_TOKEN __COUNTER__
#else
#define PH_UNIQUE_TOKEN __LINE__
#endif

#define PHTrap() __builtin_trap()
#define PHTrapCount(c) do { static int trap_count = 0; if (++trap_count==c) PHTrap() } while(0) 


//Static assertions

template< bool cond > struct _PH_STATIC_ASSERTION_FAILURE;
template <> struct _PH_STATIC_ASSERTION_FAILURE< true > {};
template <> struct _PH_STATIC_ASSERTION_FAILURE< false > {};

#define PH_STATIC_ASSERT(cond) enum { PH_TOKENPASTE(dummy,PH_UNIQUE_TOKEN) = sizeof(_PH_STATIC_ASSERTION_FAILURE< (bool)(cond) >) }


//Class registration

//map<string,PHAllocator> * list;
#define PH_REGISTERCLASS(list,name,clss) PH_INITIALIZER( PH_TOKENPASTE(PH_TOKENPASTE(PHRegister_, clss), PH_UNIQUE_TOKEN)) { if (!list) list = new map<string,PHAllocator>; list->insert(make_pair<string,void * (*)(void)>(name,PHAlloc<clss>)); }


//Hashing

template <class T>
class PHHashedStr
{
public:
    T str;
private:
    uint32_t h;
public:
    PHHashedStr<T>() : str(), h(0) {}
    PHHashedStr<T>(const PHHashedStr<T> & o) : str(o.str), h(o.h) {}
    PHHashedStr<T>(const T & s) : str(s) { recomputeHash(); }
    
    void recomputeHash();

    void setString(const T & s) 
    {
        str = s;
        recomputeHash();
    }

    PHHashedStr<T> & operator = (const PHHashedStr<T> & o)
    {
        h = o.h;
        str = o.str;
        return (*this);
    }

    const string & getString() { return str; }
    int compare(const PHHashedStr<T> & o) const
    {
        if (h == o.h)
            return str-o.str;
        else
        return (int32_t)h-(int32_t)o.h;
    }
    
    bool operator == (const PHHashedStr<T> & o) const
    {
        return compare(o) == 0;
    }

    bool operator != (const PHHashedStr<T> & o) const
    {
        return compare(o) != 0;
    }

    bool operator < (const PHHashedStr<T> & o) const
    {
        return compare(o) < 0;
    }
    
    bool operator > (const PHHashedStr<T> & o) const
    {
        return compare(o) > 0;
    }
    
    bool operator <= (const PHHashedStr<T> & o) const
    {
        return compare(o) <= 0;
    }
    
    bool operator >= (const PHHashedStr<T> & o) const
    {
        return compare(o) >= 0;
    }
};

template <>
inline int PHHashedStr<string>::compare(const PHHashedStr<string> & o) const
{
    if (h == o.h)
        return str.compare(o.str);
    else
    return (int32_t)h-(int32_t)o.h;
}

template <>
inline int PHHashedStr<const char *>::compare(const PHHashedStr<const char*> & o) const
{
    if (h == o.h)
        return strcmp(str, o.str);
    else
    return (int32_t)h-(int32_t)o.h;
}

typedef PHHashedStr<string> PHHashedString;
typedef PHHashedStr<const char *> PHHashedCString;

uint32_t PHCRC32(uint32_t crc, const uint8_t * buf, size_t size);
uint32_t PHCRC32Until(uint32_t crc, const uint8_t * p, uint8_t until = 0);

#endif
