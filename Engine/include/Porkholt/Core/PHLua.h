/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHLUA_H
#define PHLUA_H

#include <Porkholt/Core/PHMain.h>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

class PHFile;
class PHDirectory;
class PHGameManager;

bool PHLuaLoadFile(lua_State * L, PHFile * file);
bool PHLuaLoadFile(lua_State * L, PHDirectory * dir, const string & fname);
bool PHLuaLoadString(lua_State * L, string s);
bool PHLuaLoadString(lua_State * L, const char * s);
bool PHLuaCall(lua_State * L,int inargs, int outargs);

void PHLuaSetIncludePath(lua_State * L, string path);
void PHLuaAddIncludeDir(lua_State * L, PHDirectory * dir);
void PHLuaGetWeakRef(lua_State * L, void * ref);
void PHLuaSetWeakRef(lua_State * L, void * ref);
void PHLuaDeleteWeakRef(lua_State * L, void * ref);
void PHLuaGetHardRef(lua_State * L, void * ref);
void PHLuaSetHardRef(lua_State * L, void * ref);
void PHLuaDeleteHardRef(lua_State * L, void * ref);
void * PHLuaThisPointer(lua_State * L, int index = 1);
void * PHLuaThisPointerNoNil(lua_State * L, int index = 1);
void PHLuaSeedRandom(lua_State * L);

typedef int (*PHLuaFunction)(lua_State * L);
typedef void (*PHLuaInitFunction)(lua_State * L);

extern list<PHLuaInitFunction> * PHLuaContextInitList;

void PHLuaSetUpContext(lua_State * L);
lua_State * PHLuaCreateContext();

void PHPureLuaEntryPoint(PHGameManager * gm);

#define PHLuaContextInit__(name) \
static void PH_TOKENPASTE(PHLuaContextInit_, name)(lua_State * L); \
PH_REGISTERFUNCTION(PHLuaContextInitList, PH_TOKENPASTE(PHLuaContextInit_, name)); \
static void PH_TOKENPASTE(PHLuaContextInit_, name)(lua_State * L)

#define PHLuaContextInit() PHLuaContextInit__(PH_UNIQUE_TOKEN)

#define PHLuaDefineClass(clss, subclss) \
list<PHLuaInitFunction> * PH_TOKENPASTE(PHLuaClassInitList_, clss) = NULL; \
static void PH_TOKENPASTE(PHLuaDefineClass_, clss)(lua_State * L) { \
    lua_getglobal(L, PH_STRINGIFY(clss)); \
    if (!lua_istable(L, -1)) \
    { \
        lua_pop(L, 1); \
        lua_newtable(L); \
        lua_pushvalue(L, -1); \
        lua_setglobal(L, PH_STRINGIFY(clss)); \
    } \
    lua_getglobal(L, PH_STRINGIFY(subclss)); \
    if (!lua_istable(L, -1)) \
    { \
        lua_pop(L, 1); \
        lua_newtable(L); \
        lua_pushvalue(L, -1); \
        lua_setglobal(L, PH_STRINGIFY(subclss)); \
    } \
    lua_setfield(L, -2, "__index"); \
    lua_pushvalue(L, -1); \
    lua_setmetatable(L, -2); \
 \
    if (PH_TOKENPASTE(PHLuaClassInitList_, clss)) \
        for (list<PHLuaInitFunction>::iterator i = PH_TOKENPASTE(PHLuaClassInitList_, clss)->begin(); i != PH_TOKENPASTE(PHLuaClassInitList_, clss)->end(); i++) \
            (*i)(L); \
 \
    lua_pop(L, 1); \
} \
PH_REGISTERFUNCTION(PHLuaContextInitList, PH_TOKENPASTE(PHLuaDefineClass_, clss));

#define PHLuaDefineMethod(classn, method) \
extern list<PHLuaInitFunction> * PH_TOKENPASTE(PHLuaClassInitList_, classn); \
int PH_TOKENPASTE(PHLuaMethod_, PH_TOKENPASTE(classn, PH_TOKENPASTE(_, method)))(lua_State * L); \
static void PH_TOKENPASTE(PHLuaAdd_, PH_TOKENPASTE(classn, PH_TOKENPASTE(_, method)))(lua_State * L) \
{ \
    lua_pushcfunction(L, PHLuaSafeFunction<PH_TOKENPASTE(PHLuaMethod_, PH_TOKENPASTE(classn, PH_TOKENPASTE(_, method)))>); \
    lua_setfield(L, -2, PH_STRINGIFY(method)); \
} \
PH_REGISTERFUNCTION(PH_TOKENPASTE(PHLuaClassInitList_, classn), PH_TOKENPASTE(PHLuaAdd_, PH_TOKENPASTE(classn, PH_TOKENPASTE(_, method)))); \
int PH_TOKENPASTE(PHLuaMethod_, PH_TOKENPASTE(classn, PH_TOKENPASTE(_, method)))(lua_State * L)

#define PHLuaDefineFunction(function) \
int PH_TOKENPASTE(PHLuaFunction_, function) (lua_State * L); \
static void PH_TOKENPASTE(PHLuaAdd_, function) (lua_State * L) \
{ \
    lua_pushcfunction(L, PHLuaSafeFunction<PH_TOKENPASTE(PHLuaFunction_, function)>); \
    lua_setglobal(L, PH_STRINGIFY(function)); \
} \
PH_REGISTERFUNCTION(PHLuaContextInitList, PH_TOKENPASTE(PHLuaAdd_, function)); \
int PH_TOKENPASTE(PHLuaFunction_, function)(lua_State * L)


#define PHLuaMethod_FOREACH_1(narg, i, arg) PHLuaConvert<arg>::from(L, narg - i + 2)
#define PHLuaMethod_FOREACH_2(narg, i, arg) PHLuaMethod_FOREACH_2_(narg, i, arg) 
#define PHLuaMethod_FOREACH_2_(narg, i, arg, def) PHLuaConvert<arg>::fromDefault(L, narg - i + 2, def)
#define PHLuaMethod_FOREACH(narg, i, arg) PH_TOKENPASTE(PHLuaMethod_FOREACH_, PH_NARG(PH_EXPAND arg)) (narg, i, PH_EXPAND arg)

#define PHLuaMethod(classn, method, rtype, ...) PHLuaDefineMethod(classn, method) { \
    PHLuaConvert<rtype>::to(L, ((classn*)PHLuaThisPointerNoNil(L, 1))->method(PH_FOREACH_COMMA(PHLuaMethod_FOREACH, PH_NARG(__VA_ARGS__), __VA_ARGS__))); \
    return 1; \
}

#define PHLuaMethodV(classn, method, ...) PHLuaDefineMethod(classn, method) { \
    ((classn*)PHLuaThisPointerNoNil(L, 1))->method(PH_FOREACH_COMMA(PHLuaMethod_FOREACH, PH_NARG(__VA_ARGS__), __VA_ARGS__)); \
    return 0; \
}

#define PHLuaMethodS(classn, method, rtype, ...) PHLuaDefineMethod(classn, method) { \
    PHLuaConvert<rtype>::to(L, classn::method(PH_FOREACH_COMMA(PHLuaMethod_FOREACH, PH_NARG(__VA_ARGS__), __VA_ARGS__))); \
    return 1; \
}

#define PHLuaMethodSV(classn, method, ...) PHLuaDefineMethod(classn, method) { \
    classn::method(PH_FOREACH_COMMA(PHLuaMethod_FOREACH, PH_NARG(__VA_ARGS__), __VA_ARGS__)); \
    return 0; \
}

#define PHLuaConstructor(classn, name, ...) PHLuaDefineMethod(classn, name) { \
    classn * i = new classn(PH_FOREACH_COMMA(PHLuaMethod_FOREACH, PH_NARG(__VA_ARGS__), __VA_ARGS__)); \
    i->pushLuaInstance(L); \
    i->release(); \
    return 1; \
}

template<lua_CFunction func>
int PHLuaSafeFunction(lua_State * L){
    try { return func(L); }
    catch (string ex) { lua_pushstring(L, ex.c_str()); }
    return lua_error(L);
}

template<typename T> inline int PHLuaArgCount() { return 1; }
template<> inline int PHLuaArgCount<void>() { return 0; }

template<typename T> class PHLuaConvert {
    public:
        static void to(lua_State * L, const T & a) { a.saveToLua(L); }
        static T from(lua_State * L, int index) { return T::fromLua(L, index); }
        static T fromDefault(lua_State * L, int index, const T & def) { return T::fromLuaDefault(L, index, def); }
};

template<> class PHLuaConvert<void> {
    public:
        static void to(lua_State * L, ...) {}
        static void from(lua_State * L, int index) {}
        static void fromDefault(lua_State * L, int index) {}
};

template<typename T> class PHLuaConvert<T *> {
    public:
        static void to(lua_State * L, T * a)
        {
            if (a)
                ((PHObject*)a)->pushLuaInstance(L);
            else
                lua_pushnil(L);
        }
        static T * fromUD(lua_State * L, int index, bool & found)
        {
            found = true;
            if (lua_istable(L, index))
            {
                lua_getfield(L, index, "ud");
                if (lua_isuserdata(L, -1))
                {
                    T * r = (T*)lua_touserdata(L, -1); 
                    lua_pop(L, 1);
                    return r;
                }
                lua_pop(L, 1);
            }
            if (lua_isuserdata(L, index))
                return (T*)lua_touserdata(L, index);
            found = false;
            return NULL;
        }
        static T * from(lua_State * L, int index)
        {
            bool found;
            T * r = fromUD(L, index, found);
            if (found)
                return r;
            else
                throw PHStringFormat("Argument" << index << "must be an object");
        }
        static T * fromDefault(lua_State * L, int index, T * def)
        {
            bool found;
            T * r = fromUD(L, index, found);
            if (found)
                return r;
            else
                return def;
        }
};

#define spec(T) \
template<> class PHLuaConvert<T> { \
    public: \
        static void to(lua_State * L, T a) \
        { \
            lua_pushnumber(L, a); \
        } \
        static T from(lua_State * L, int index) \
        { \
            if (lua_isnumber(L, index)) \
                return lua_tonumber(L, index); \
            else \
                throw PHStringFormat("Argument" << index << "must be an integer"); \
        } \
        static T fromDefault(lua_State * L, int index, T def) \
        { \
            if (lua_isnumber(L, index)) \
                return lua_tonumber(L, index); \
            else \
                return def; \
        } \
}
spec(int);
spec(unsigned int);
spec(long long);
spec(unsigned long long);
spec(short);
spec(unsigned short);
spec(char);
spec(unsigned char);
#undef spec

template<> class PHLuaConvert<float> {
    public:
        static void to(lua_State * L, float a)
        {
            lua_pushnumber(L, a);
        }
        static float from(lua_State * L, int index)
        {
            if (lua_isnumber(L, index))
                return lua_tonumber(L, index);
            else
                throw PHStringFormat("Argument" << index << "must be a float");
        }
        static float fromDefault(lua_State * L, int index, float def)
        {
            if (lua_isnumber(L, index))
                return lua_tonumber(L, index);
            else
                return def;
        }
};

template<> class PHLuaConvert<double> {
    public:
        static void to(lua_State * L, double a)
        {
            lua_pushnumber(L, a);
        }
        static double from(lua_State * L, int index)
        {
            if (lua_isnumber(L, index))
                return lua_tonumber(L, index);
            else
                throw PHStringFormat("Argument" << index << "must be a double");
        }
        static double fromDefault(lua_State * L, int index, double def)
        {
            if (lua_isnumber(L, index))
                return lua_tonumber(L, index);
            else
                return def;
        }
};

template<> class PHLuaConvert<bool> {
    public:
        static void to(lua_State * L, bool a)
        {
            lua_pushboolean(L, a);
        }
        static bool from(lua_State * L, int index)
        {
            if (lua_isboolean(L, index))
                return lua_toboolean(L, index);
            else
                throw PHStringFormat("Argument" << index << "must be a bool");
        }
        static bool fromDefault(lua_State * L, int index, bool def)
        {
            if (lua_isboolean(L, index))
                return lua_toboolean(L, index);
            else
                return def;
        }
};

template<> class PHLuaConvert<const char *> {
    public:
        static void to(lua_State * L, const char * a)
        {
            lua_pushstring(L, a);
        }
        static const char * from(lua_State * L, int index)
        {
            if (lua_isstring(L, index))
                return lua_tostring(L, index);
            else
                throw PHStringFormat("Argument" << index << "must be a string");
        }
        static const char * fromDefault(lua_State * L, int index, const char * def)
        {
            if (lua_isstring(L, index))
                return lua_tostring(L, index);
            else
                return def;
        }
};

template<> class PHLuaConvert<string>{
    public:
        static void to(lua_State * L, const string & a)
        {
            lua_pushstring(L, a.c_str());
        }
        static string from(lua_State * L, int index)
        {
            if (lua_isstring(L, index))
                return string(lua_tostring(L, index));
            else
                throw PHStringFormat("Argument" << index << "must be a string");
        }
        static string fromDefault(lua_State * L, int index, const string & def)
        {
            if (lua_isstring(L, index))
                return string(lua_tostring(L, index));
            else
                return def;
        }
};

template<> class PHLuaConvert<PHMatrix> {
    public:
        static void to(lua_State * L, const PHMatrix & a) { a.saveToLua(L); }
        static const PHMatrix & from(lua_State * L, int index) { return PHMatrix::fromLua(L, index); }
        static const PHMatrix & fromDefault(lua_State * L, int index, const PHMatrix & def) { return PHMatrix::fromLuaDefault(L, index, def); }
};

//deprecated begin

#define PHLuaAddMethod(classn,method) lua_pushcfunction(L, classn ## _ ## method); lua_setfield(L, -2, #method);
#define PHLuaAddMethod_(classn,method) lua_pushcfunction(L, classn ## _ ## method); lua_setfield(L, -2, "_" #method);

#define PHLuaDefineCall(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    o->method(); \
    return 0; \
}

#define PHLuaBoolSetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    luaL_checktype(L, 2, LUA_TBOOLEAN); \
    o->method(lua_toboolean(L, 2)); \
    return 0; \
}

#define PHLuaBoolGetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    lua_pushboolean(L, o->method()); \
    return 1; \
}

#define PHLuaNumberSetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    luaL_checknumber(L, 2); \
    o->method((float)lua_tonumber(L, 2)); \
    return 0; \
}

#define PHLuaNumberGetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    lua_pushnumber(L, o->method()); \
    return 1; \
}

#define PHLuaAngleGetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    lua_pushnumber(L, PHWarp(-toDeg(o->method()),360)); \
    return 1; \
}

#define PHLuaAngleSetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    luaL_checknumber(L, 2); \
    o->method(-toRad((float)lua_tonumber(L, 2))); \
    return 0; \
}

#define PHLuaStringSetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    luaL_checkstring(L, 2); \
    o->method(lua_tostring(L, 2)); \
    return 0; \
}

#define PHLuaStringGetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    lua_pushstring(L, (o->method()).c_str()); \
    return 1; \
}

#define PHLuaPointSetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    luaL_checktype(L, 2, LUA_TTABLE); \
    o->method(PHPoint::fromLua(L, 2)); \
    return 0; \
}

#define PHLuaPointGetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    (o->method()).saveToLua(L); \
    return 1; \
}

#define PHLuaColorSetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    luaL_checktype(L, 2, LUA_TTABLE); \
    o->method(PHColor::fromLua(L, 2)); \
    return 0; \
}

#define PHLuaColorGetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    (o->method()).saveToLua(L); \
    return 1; \
}

#define PHLuaRectSetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    luaL_checktype(L, 2, LUA_TTABLE); \
    o->method(PHRect::fromLua(L, 2)); \
    return 0; \
}

#define PHLuaRectGetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    (o->method()).saveToLua(L); \
    return 1; \
}

#define PHLuaGetNumberField(var,name) \
lua_getfield(L, -1, name); \
if (lua_isnumber(L, -1)) \
    var = (float)lua_tonumber(L, -1); \
lua_pop(L,1)

#define PHLuaGetBoolField(var,name) \
lua_getfield(L, -1, name); \
if (lua_isboolean(L, -1)) \
    var = lua_toboolean(L, -1); \
lua_pop(L,1)

#define PHLuaGetStringField(var,name) \
lua_getfield(L, -1, name); \
if (lua_isstring(L, -1)) \
    var = lua_tostring(L, -1); \
lua_pop(L,1)

#define PHLuaGetAngleField(var,name) \
lua_getfield(L, -1, name); \
if (lua_isnumber(L, -1)) \
    var = -toRad((float)lua_tonumber(L, -1)); \
lua_pop(L,1)

#define PHLuaGetStructField(var,name,structname) \
lua_getfield(L, -1, name); \
if (lua_istable(L, -1)) \
    var = structname::fromLua(L,-1); \
lua_pop(L,1);

#define PHLuaGetPointField(var,name) PHLuaGetStructField(var,name,PHPoint)
#define PHLuaGetRectField(var,name) PHLuaGetStructField(var,name,PHRect)
#define PHLuaGetColorField(var,name) PHLuaGetStructField(var,name,PHColor)

//end deprecated

class PHLuaCallback : public PHObject
{
private:
    lua_State * L;
    PHInvocation inv;
public:
    PHLuaCallback(lua_State * l, int index);
    virtual ~PHLuaCallback();
    void call();
    
    void setConsequentCallback(const PHInvocation & invocation) { inv = invocation; };
};

#define PHLuaForEach(index) for(int i=0; ; i++) { lua_pushnumber(L,i); lua_gettable(L,(index)-1); if (lua_isnil(L,-1)) { lua_pop(L,1); if (i) break; else continue; } {
#define _PHLuaForEach(index) for(int i=0; ; i++) { lua_pushnumber(L,i); lua_gettable(L,(index)); if (lua_isnil(L,-1)) { lua_pop(L,1); if (i) break; else continue; } {
#define PHLuaForEach_  } lua_pop(L,1); }

#endif
