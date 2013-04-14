/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHUTILITIES_H
#define PHUTILITIES_H

//Includes

#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <cstring>

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

#define PH_UNAMED_INITIALIZER() PH_INITIALIZER(PH_TOKENPASTE(PHUnamedInitializer_, PH_UNIQUE_TOKEN))


//Allocators

typedef void * (*PHAllocator)(void);
typedef void (*PHFunction)(void);

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

#define PH_EXPAND(...) __VA_ARGS__

#ifdef __COUNTER__
#define PH_UNIQUE_TOKEN __COUNTER__
#else
#define PH_UNIQUE_TOKEN __LINE__
#endif

#define PHTrap() __builtin_trap()
#define PHTrapCount(c) do { static int trap_count = 0; if (++trap_count==c) PHTrap() } while(0) 

#if __cplusplus >= 201103L
#define PH_DECLTYPE(x) decltype(x)
#else
#ifdef __GNUC__
#define PH_DECLTYPE(x) typeof(x)
#endif
#endif

#define PHStringFormat(x) (static_cast<ostringstream*>( &(ostringstream() << x) )->str())


//Static assertions

template< bool cond > struct _PH_STATIC_ASSERTION_FAILURE;
template <> struct _PH_STATIC_ASSERTION_FAILURE< true > {};
template <> struct _PH_STATIC_ASSERTION_FAILURE< false > {};

#define PH_STATIC_ASSERT(cond) enum { PH_TOKENPASTE(dummy,PH_UNIQUE_TOKEN) = sizeof(_PH_STATIC_ASSERTION_FAILURE< (bool)(cond) >) }


//Class registration

//map<string,PHAllocator> * list = NULL;
#define PH_REGISTERCLASS(list,name,clss) PH_INITIALIZER( PH_TOKENPASTE(PH_TOKENPASTE(PHRegister_, clss), PH_UNIQUE_TOKEN)) { if (!list) list = new map<string,PHAllocator>; list->insert(make_pair<string,void * (*)(void)>(name,PHAlloc<clss>)); }

//list<PHFunction> * list = NULL;
#define PH_REGISTERFUNCTION(list, function) PH_UNAMED_INITIALIZER() { if (!list) list = new PH_DECLTYPE(*list); list->push_back(function); }


//For each macro

#define PH_NARG(...) \
        PH_NARG_(_0, ## __VA_ARGS__, PH_RSEQ_N())
#define PH_NARG_(...) \
        PH_ARG_N(__VA_ARGS__) 
#define PH_ARG_N( \
        _0, _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
        _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
        _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
        _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
        _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
        _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
        _61,_62,_63, N,...) N 
#define PH_RSEQ_N() \
        63, 62, 61, 60,                         \
        59, 58, 57, 56, 55, 54, 53, 52, 51, 50, \
        49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
        39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
        29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
        19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
         9,  8,  7,  6,  5,  4,  3,  2,  1,  0 

#define PH_FOREACH_0(macro, carg, dummy)
#define PH_FOREACH_1(macro, carg, arg) macro(carg, 1, arg)
#define PH_FOREACH_2(macro, carg, arg, ...) macro(carg, 2 , arg) PH_FOREACH_1 (macro, carg, __VA_ARGS__)
#define PH_FOREACH_3(macro, carg, arg, ...) macro(carg, 3 , arg) PH_FOREACH_2 (macro, carg, __VA_ARGS__)
#define PH_FOREACH_4(macro, carg, arg, ...) macro(carg, 4 , arg) PH_FOREACH_3 (macro, carg, __VA_ARGS__)
#define PH_FOREACH_5(macro, carg, arg, ...) macro(carg, 5 , arg) PH_FOREACH_4 (macro, carg, __VA_ARGS__)
#define PH_FOREACH_6(macro, carg, arg, ...) macro(carg, 6 , arg) PH_FOREACH_5 (macro, carg, __VA_ARGS__)
#define PH_FOREACH_7(macro, carg, arg, ...) macro(carg, 7 , arg) PH_FOREACH_6 (macro, carg, __VA_ARGS__)
#define PH_FOREACH_8(macro, carg, arg, ...) macro(carg, 8 , arg) PH_FOREACH_7 (macro, carg, __VA_ARGS__)
#define PH_FOREACH_9(macro, carg, arg, ...) macro(carg, 9 , arg) PH_FOREACH_8 (macro, carg, __VA_ARGS__)
#define PH_FOREACH_10(macro, carg, arg, ...) macro(carg, 10, arg) PH_FOREACH_9 (macro, carg, __VA_ARGS__)
#define PH_FOREACH_11(macro, carg, arg, ...) macro(carg, 11, arg) PH_FOREACH_10(macro, carg, __VA_ARGS__)
#define PH_FOREACH_12(macro, carg, arg, ...) macro(carg, 12, arg) PH_FOREACH_11(macro, carg, __VA_ARGS__)
#define PH_FOREACH_13(macro, carg, arg, ...) macro(carg, 13, arg) PH_FOREACH_12(macro, carg, __VA_ARGS__)
#define PH_FOREACH_14(macro, carg, arg, ...) macro(carg, 14, arg) PH_FOREACH_13(macro, carg, __VA_ARGS__)
#define PH_FOREACH_15(macro, carg, arg, ...) macro(carg, 15, arg) PH_FOREACH_14(macro, carg, __VA_ARGS__)
#define PH_FOREACH_16(macro, carg, arg, ...) macro(carg, 16, arg) PH_FOREACH_15(macro, carg, __VA_ARGS__)
#define PH_FOREACH_17(macro, carg, arg, ...) macro(carg, 17, arg) PH_FOREACH_16(macro, carg, __VA_ARGS__)
#define PH_FOREACH_18(macro, carg, arg, ...) macro(carg, 18, arg) PH_FOREACH_17(macro, carg, __VA_ARGS__)
#define PH_FOREACH_19(macro, carg, arg, ...) macro(carg, 19, arg) PH_FOREACH_18(macro, carg, __VA_ARGS__)
#define PH_FOREACH_20(macro, carg, arg, ...) macro(carg, 20, arg) PH_FOREACH_19(macro, carg, __VA_ARGS__)
#define PH_FOREACH_21(macro, carg, arg, ...) macro(carg, 21, arg) PH_FOREACH_20(macro, carg, __VA_ARGS__)
#define PH_FOREACH_22(macro, carg, arg, ...) macro(carg, 22, arg) PH_FOREACH_21(macro, carg, __VA_ARGS__)
#define PH_FOREACH_23(macro, carg, arg, ...) macro(carg, 23, arg) PH_FOREACH_22(macro, carg, __VA_ARGS__)
#define PH_FOREACH_24(macro, carg, arg, ...) macro(carg, 24, arg) PH_FOREACH_23(macro, carg, __VA_ARGS__)
#define PH_FOREACH_25(macro, carg, arg, ...) macro(carg, 25, arg) PH_FOREACH_24(macro, carg, __VA_ARGS__)
#define PH_FOREACH_26(macro, carg, arg, ...) macro(carg, 26, arg) PH_FOREACH_25(macro, carg, __VA_ARGS__)
#define PH_FOREACH_27(macro, carg, arg, ...) macro(carg, 27, arg) PH_FOREACH_26(macro, carg, __VA_ARGS__)
#define PH_FOREACH_28(macro, carg, arg, ...) macro(carg, 28, arg) PH_FOREACH_27(macro, carg, __VA_ARGS__)
#define PH_FOREACH_29(macro, carg, arg, ...) macro(carg, 29, arg) PH_FOREACH_28(macro, carg, __VA_ARGS__)
#define PH_FOREACH_30(macro, carg, arg, ...) macro(carg, 30, arg) PH_FOREACH_29(macro, carg, __VA_ARGS__)
#define PH_FOREACH_31(macro, carg, arg, ...) macro(carg, 31, arg) PH_FOREACH_30(macro, carg, __VA_ARGS__)
#define PH_FOREACH_32(macro, carg, arg, ...) macro(carg, 32, arg) PH_FOREACH_31(macro, carg, __VA_ARGS__)
#define PH_FOREACH_33(macro, carg, arg, ...) macro(carg, 33, arg) PH_FOREACH_32(macro, carg, __VA_ARGS__)
#define PH_FOREACH_34(macro, carg, arg, ...) macro(carg, 34, arg) PH_FOREACH_33(macro, carg, __VA_ARGS__)
#define PH_FOREACH_35(macro, carg, arg, ...) macro(carg, 35, arg) PH_FOREACH_34(macro, carg, __VA_ARGS__)
#define PH_FOREACH_36(macro, carg, arg, ...) macro(carg, 36, arg) PH_FOREACH_35(macro, carg, __VA_ARGS__)
#define PH_FOREACH_37(macro, carg, arg, ...) macro(carg, 37, arg) PH_FOREACH_36(macro, carg, __VA_ARGS__)
#define PH_FOREACH_38(macro, carg, arg, ...) macro(carg, 38, arg) PH_FOREACH_37(macro, carg, __VA_ARGS__)
#define PH_FOREACH_39(macro, carg, arg, ...) macro(carg, 39, arg) PH_FOREACH_38(macro, carg, __VA_ARGS__)
#define PH_FOREACH_40(macro, carg, arg, ...) macro(carg, 40, arg) PH_FOREACH_39(macro, carg, __VA_ARGS__)
#define PH_FOREACH_41(macro, carg, arg, ...) macro(carg, 41, arg) PH_FOREACH_40(macro, carg, __VA_ARGS__)
#define PH_FOREACH_42(macro, carg, arg, ...) macro(carg, 42, arg) PH_FOREACH_41(macro, carg, __VA_ARGS__)
#define PH_FOREACH_43(macro, carg, arg, ...) macro(carg, 43, arg) PH_FOREACH_42(macro, carg, __VA_ARGS__)
#define PH_FOREACH_44(macro, carg, arg, ...) macro(carg, 44, arg) PH_FOREACH_43(macro, carg, __VA_ARGS__)
#define PH_FOREACH_45(macro, carg, arg, ...) macro(carg, 45, arg) PH_FOREACH_44(macro, carg, __VA_ARGS__)
#define PH_FOREACH_46(macro, carg, arg, ...) macro(carg, 46, arg) PH_FOREACH_45(macro, carg, __VA_ARGS__)
#define PH_FOREACH_47(macro, carg, arg, ...) macro(carg, 47, arg) PH_FOREACH_46(macro, carg, __VA_ARGS__)
#define PH_FOREACH_48(macro, carg, arg, ...) macro(carg, 48, arg) PH_FOREACH_47(macro, carg, __VA_ARGS__)
#define PH_FOREACH_49(macro, carg, arg, ...) macro(carg, 49, arg) PH_FOREACH_48(macro, carg, __VA_ARGS__)
#define PH_FOREACH_50(macro, carg, arg, ...) macro(carg, 50, arg) PH_FOREACH_49(macro, carg, __VA_ARGS__)
#define PH_FOREACH_51(macro, carg, arg, ...) macro(carg, 51, arg) PH_FOREACH_50(macro, carg, __VA_ARGS__)
#define PH_FOREACH_52(macro, carg, arg, ...) macro(carg, 52, arg) PH_FOREACH_51(macro, carg, __VA_ARGS__)
#define PH_FOREACH_53(macro, carg, arg, ...) macro(carg, 53, arg) PH_FOREACH_52(macro, carg, __VA_ARGS__)
#define PH_FOREACH_54(macro, carg, arg, ...) macro(carg, 54, arg) PH_FOREACH_53(macro, carg, __VA_ARGS__)
#define PH_FOREACH_55(macro, carg, arg, ...) macro(carg, 55, arg) PH_FOREACH_54(macro, carg, __VA_ARGS__)
#define PH_FOREACH_56(macro, carg, arg, ...) macro(carg, 56, arg) PH_FOREACH_55(macro, carg, __VA_ARGS__)
#define PH_FOREACH_57(macro, carg, arg, ...) macro(carg, 57, arg) PH_FOREACH_56(macro, carg, __VA_ARGS__)
#define PH_FOREACH_58(macro, carg, arg, ...) macro(carg, 58, arg) PH_FOREACH_57(macro, carg, __VA_ARGS__)
#define PH_FOREACH_59(macro, carg, arg, ...) macro(carg, 59, arg) PH_FOREACH_58(macro, carg, __VA_ARGS__)
#define PH_FOREACH_60(macro, carg, arg, ...) macro(carg, 60, arg) PH_FOREACH_59(macro, carg, __VA_ARGS__)
#define PH_FOREACH_61(macro, carg, arg, ...) macro(carg, 61, arg) PH_FOREACH_60(macro, carg, __VA_ARGS__)
#define PH_FOREACH_62(macro, carg, arg, ...) macro(carg, 62, arg) PH_FOREACH_61(macro, carg, __VA_ARGS__)
#define PH_FOREACH_63(macro, carg, arg, ...) macro(carg, 63, arg) PH_FOREACH_62(macro, carg, __VA_ARGS__)

#define PH_FOREACH_COMMA_0(macro, carg, dummy)
#define PH_FOREACH_COMMA_1(macro, carg, arg) macro(carg, 1, arg)
#define PH_FOREACH_COMMA_2(macro, carg, arg, ...) macro(carg, 2 , arg), PH_FOREACH_COMMA_1 (macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_3(macro, carg, arg, ...) macro(carg, 3 , arg), PH_FOREACH_COMMA_2 (macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_4(macro, carg, arg, ...) macro(carg, 4 , arg), PH_FOREACH_COMMA_3 (macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_5(macro, carg, arg, ...) macro(carg, 5 , arg), PH_FOREACH_COMMA_4 (macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_6(macro, carg, arg, ...) macro(carg, 6 , arg), PH_FOREACH_COMMA_5 (macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_7(macro, carg, arg, ...) macro(carg, 7 , arg), PH_FOREACH_COMMA_6 (macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_8(macro, carg, arg, ...) macro(carg, 8 , arg), PH_FOREACH_COMMA_7 (macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_9(macro, carg, arg, ...) macro(carg, 9 , arg), PH_FOREACH_COMMA_8 (macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_10(macro, carg, arg, ...) macro(carg, 10, arg), PH_FOREACH_COMMA_9 (macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_11(macro, carg, arg, ...) macro(carg, 11, arg), PH_FOREACH_COMMA_10(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_12(macro, carg, arg, ...) macro(carg, 12, arg), PH_FOREACH_COMMA_11(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_13(macro, carg, arg, ...) macro(carg, 13, arg), PH_FOREACH_COMMA_12(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_14(macro, carg, arg, ...) macro(carg, 14, arg), PH_FOREACH_COMMA_13(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_15(macro, carg, arg, ...) macro(carg, 15, arg), PH_FOREACH_COMMA_14(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_16(macro, carg, arg, ...) macro(carg, 16, arg), PH_FOREACH_COMMA_15(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_17(macro, carg, arg, ...) macro(carg, 17, arg), PH_FOREACH_COMMA_16(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_18(macro, carg, arg, ...) macro(carg, 18, arg), PH_FOREACH_COMMA_17(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_19(macro, carg, arg, ...) macro(carg, 19, arg), PH_FOREACH_COMMA_18(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_20(macro, carg, arg, ...) macro(carg, 20, arg), PH_FOREACH_COMMA_19(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_21(macro, carg, arg, ...) macro(carg, 21, arg), PH_FOREACH_COMMA_20(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_22(macro, carg, arg, ...) macro(carg, 22, arg), PH_FOREACH_COMMA_21(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_23(macro, carg, arg, ...) macro(carg, 23, arg), PH_FOREACH_COMMA_22(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_24(macro, carg, arg, ...) macro(carg, 24, arg), PH_FOREACH_COMMA_23(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_25(macro, carg, arg, ...) macro(carg, 25, arg), PH_FOREACH_COMMA_24(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_26(macro, carg, arg, ...) macro(carg, 26, arg), PH_FOREACH_COMMA_25(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_27(macro, carg, arg, ...) macro(carg, 27, arg), PH_FOREACH_COMMA_26(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_28(macro, carg, arg, ...) macro(carg, 28, arg), PH_FOREACH_COMMA_27(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_29(macro, carg, arg, ...) macro(carg, 29, arg), PH_FOREACH_COMMA_28(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_30(macro, carg, arg, ...) macro(carg, 30, arg), PH_FOREACH_COMMA_29(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_31(macro, carg, arg, ...) macro(carg, 31, arg), PH_FOREACH_COMMA_30(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_32(macro, carg, arg, ...) macro(carg, 32, arg), PH_FOREACH_COMMA_31(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_33(macro, carg, arg, ...) macro(carg, 33, arg), PH_FOREACH_COMMA_32(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_34(macro, carg, arg, ...) macro(carg, 34, arg), PH_FOREACH_COMMA_33(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_35(macro, carg, arg, ...) macro(carg, 35, arg), PH_FOREACH_COMMA_34(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_36(macro, carg, arg, ...) macro(carg, 36, arg), PH_FOREACH_COMMA_35(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_37(macro, carg, arg, ...) macro(carg, 37, arg), PH_FOREACH_COMMA_36(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_38(macro, carg, arg, ...) macro(carg, 38, arg), PH_FOREACH_COMMA_37(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_39(macro, carg, arg, ...) macro(carg, 39, arg), PH_FOREACH_COMMA_38(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_40(macro, carg, arg, ...) macro(carg, 40, arg), PH_FOREACH_COMMA_39(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_41(macro, carg, arg, ...) macro(carg, 41, arg), PH_FOREACH_COMMA_40(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_42(macro, carg, arg, ...) macro(carg, 42, arg), PH_FOREACH_COMMA_41(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_43(macro, carg, arg, ...) macro(carg, 43, arg), PH_FOREACH_COMMA_42(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_44(macro, carg, arg, ...) macro(carg, 44, arg), PH_FOREACH_COMMA_43(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_45(macro, carg, arg, ...) macro(carg, 45, arg), PH_FOREACH_COMMA_44(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_46(macro, carg, arg, ...) macro(carg, 46, arg), PH_FOREACH_COMMA_45(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_47(macro, carg, arg, ...) macro(carg, 47, arg), PH_FOREACH_COMMA_46(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_48(macro, carg, arg, ...) macro(carg, 48, arg), PH_FOREACH_COMMA_47(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_49(macro, carg, arg, ...) macro(carg, 49, arg), PH_FOREACH_COMMA_48(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_50(macro, carg, arg, ...) macro(carg, 50, arg), PH_FOREACH_COMMA_49(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_51(macro, carg, arg, ...) macro(carg, 51, arg), PH_FOREACH_COMMA_50(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_52(macro, carg, arg, ...) macro(carg, 52, arg), PH_FOREACH_COMMA_51(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_53(macro, carg, arg, ...) macro(carg, 53, arg), PH_FOREACH_COMMA_52(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_54(macro, carg, arg, ...) macro(carg, 54, arg), PH_FOREACH_COMMA_53(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_55(macro, carg, arg, ...) macro(carg, 55, arg), PH_FOREACH_COMMA_54(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_56(macro, carg, arg, ...) macro(carg, 56, arg), PH_FOREACH_COMMA_55(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_57(macro, carg, arg, ...) macro(carg, 57, arg), PH_FOREACH_COMMA_56(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_58(macro, carg, arg, ...) macro(carg, 58, arg), PH_FOREACH_COMMA_57(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_59(macro, carg, arg, ...) macro(carg, 59, arg), PH_FOREACH_COMMA_58(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_60(macro, carg, arg, ...) macro(carg, 60, arg), PH_FOREACH_COMMA_59(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_61(macro, carg, arg, ...) macro(carg, 61, arg), PH_FOREACH_COMMA_60(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_62(macro, carg, arg, ...) macro(carg, 62, arg), PH_FOREACH_COMMA_61(macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA_63(macro, carg, arg, ...) macro(carg, 63, arg), PH_FOREACH_COMMA_62(macro, carg, __VA_ARGS__)

//macro should be a macro taking 3 arguments: carg (passed along), index and arg. index is counting backwards from NARG to 1
#define PH_FOREACH(macro, carg, ...)  PH_TOKENPASTE(PH_FOREACH_, PH_NARG(__VA_ARGS__)) (macro, carg, __VA_ARGS__)
#define PH_FOREACH_COMMA(macro, carg, ...)  PH_TOKENPASTE(PH_FOREACH_COMMA_, PH_NARG(__VA_ARGS__)) (macro, carg, __VA_ARGS__)


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
