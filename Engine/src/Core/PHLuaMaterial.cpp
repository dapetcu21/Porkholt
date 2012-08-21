/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#include <Porkholt/Core/PHLuaMaterial.h>
#include <Porkholt/Core/PHMutex.h>
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHGLShaderProgram.h>
#include <Porkholt/Core/PHGLVertexArrayObject.h>
#include <Porkholt/Core/PHGLLight.h>
#include <Porkholt/Core/PHNormalImage.h>
#include <limits.h>

lua_State * PHLuaMaterial::sL = NULL;
PHMutex * PHLuaMaterial::luaMutex = new PHMutex;

static const char * luaEnvironmentInit = "";

void PHLuaMaterial::loadFromLua(lua_State * L)
{
    lua_getglobal(L, "material");
    if (lua_istable(L, -1))
    {
        lua_pushnil(L);
        while(lua_next(L, -2))
        {
            int renderMode = INT_MAX;
            int lightType = INT_MAX;
            if (lua_istable(L, -2))
            {
                lua_getfield(L, -2, "renderMode");
                if (lua_isnumber(L, -1))
                    renderMode = lua_tonumber(L, -1);
                lua_pop(L, 1);
                lua_getfield(L, -2, "renderMode");
                if (lua_isnumber(L, -1))
                    lightType = lua_tonumber(L, -1);
                lua_pop(L, 1);

                
            } else if (lua_isnumber(L, -2))
                renderMode = lua_tonumber(L, -2);

            pair<int, int> p(renderMode, lightType);
            if (lua_istable(L, -1) && (renderers.find(p) == renderers.end()))
            {
                renderer r;

                string sh;
                PHLuaGetStringField(sh, "shader");
                r.shader = gm->shaderProgramNamed(sh);
                r.shader->retain();
                r.uniforms = new PHGLUniformStates;
                r.nvars = 0;
                r.vars = NULL;

                lua_getfield(L, -1, "uniforms");
                if (lua_istable(L, 1))
                {
                    size_t mem = 0;
                    size_t st_size = 0;
                    size_t mat_size = 0;

                    lua_pushnil(L);
                    while (lua_next(L, -2))
                    {
                        PHGLUniformStates::uniform * u;
                        if (lua_isstring(L, -2))
                        {
                            u = &r.uniforms->at(lua_tostring(L, -2));
                            bool b = true;
                            if (lua_istable(L, -1))
                            {
                                lua_getfield(L, -1, "stack");
                                if (lua_istable(L, -1))
                                {
                                    r.nvars++;
                                    mem += calculateMem(L, st_size, mat_size);
                                    b = false;
                                }
                                lua_pop(L, 1);
                            }
                            if (b)
                            {
                                renderer::solver::value v;
                                v.readFromLua(L);
                                if(!v.applyUniform(u) && lua_isstring(L, -1))
                                {
                                    PHNormalImage * img = dynamic_cast<PHNormalImage*>(gm->imageNamed(lua_tostring(L, -1)));
                                    if (img)
                                        *u = img;
                                }
                            }
                        }
                        lua_pop(L, 1);
                    }

                    mem += sizeof(renderer::solver) * r.nvars;
                    mem += sizeof(PHMatrix) * mat_size;
                    mem += sizeof(renderer::solver::value) * st_size;
                    uint8_t * m = (uint8_t*)malloc(mem);
                    PHLog("malloc(%d)", int(mem));
                    r.vars = (renderer::solver*)m;
                    m += sizeof(renderer::solver) * r.nvars;
                    PHMatrix * mat_stack = (PHMatrix*)m;
                    m += sizeof(PHMatrix) * mat_size;
                    renderer::solver::value * stack = (renderer::solver::value*) m;
                    m += sizeof(renderer::solver::value) * st_size;

                    int i = 0;
                    lua_pushnil(L);
                    while (lua_next(L, -2))
                    {
                        PHGLUniformStates::uniform * u;
                        if (lua_isstring(L, -2))
                        {
                            u = &r.uniforms->at(lua_tostring(L, -2));
                            if (lua_istable(L, -1))
                            {
                                lua_getfield(L, -1, "stack");
                                if (lua_istable(L, -1))
                                {
                                    renderer::solver & s = r.vars[i];
                                    s.stack = stack;
                                    s.matrixStack = mat_stack;
                                    s.uniform = u;
                                    PHLuaGetNumberField(s.eq_n, "n");
                                    PHLog("giving it from(%d)", int(m - (uint8_t*)r.vars));
                                    s.equation = (renderer::solver::value*)m;
                                    m+= sizeof(renderer::solver::value) * s.eq_n;
                                    PHLog("giving it   to(%d)", int(m - (uint8_t*)r.vars));
                                    for (int j = 1; j<=s.eq_n; j++)
                                    {
                                        lua_pushnumber(L, j);
                                        lua_gettable(L, -2);
                                        s.equation[j-1].readFromLua(L);
                                        lua_pop(L, 1);
                                    }
                                    i++;
                                }
                                lua_pop(L, 1);
                            }
                        }
                        lua_pop(L, 1);
                    }

                }
                lua_pop(L, 1);

                renderers[p] = r;
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
}

PHLuaMaterial::~PHLuaMaterial()
{
    for (map<pair<int,int>, renderer>::iterator i = renderers.begin(); i!= renderers.end(); i++)
    {
        renderer & r = i->second;
        r.shader->release();
        r.uniforms->release();
        free(r.vars);
    }
}

PHLuaMaterial::PHLuaMaterial(PHGameManager * _gm, PHFile * file) : gm(_gm)
{
    luaMutex->lock();
    if (!sL)
    {
        sL = lua_open();
        luaL_openlibs(sL);
        PHLuaSeedRandom(sL);
        initEnvironment(sL);
    }
    PHLuaLoadFile(sL, file);
    loadFromLua(sL);
    luaMutex->unlock();
}

size_t PHLuaMaterial::calculateMem(lua_State * L, size_t & ss, size_t & ms)
{
    int n;
    PHLuaGetNumberField(n, "n");

    renderer::solver::value v;
    bool mat[n];
    int mms = 0, mss = 0;

    for (int j = 1; j<=n; j++)
    {
        lua_pushnumber(L, j);
        lua_gettable(L, -2);
        v.readFromLua(L);
        v.adjustType();
        if (v.type == renderer::solver::value::vOp)
        {
            int pop = v.argCount();
            int ret = v.returnType();
            for (;pop;pop--)
            {
                mss--;
                if (mat[mss])
                    mms--;
            }
            mat[mss] = (ret == renderer::solver::value::vMatrix);
        } else {
            mat[mss] = (v.type == renderer::solver::value::vMatrix);
        }
        mms+= mat[mss++];
        if (mss > ss)
            ss = mss;
        if (mms > ms)
            ms = mms;
        lua_pop(L, 1);
    }
    return sizeof(renderer::solver::value) * n;
}

enum e_vars
{
    modelView,
    projection,
    color,
    lIntensity,
    lPos,
    lSpec,
    lDiff,
    ambientColor
};

const char * e_names[] = {
    "modelViewMatrix",
    "projectionMatrix",
    "currentColor",
    "lightIntensity",
    "lightPosition",
    "lightSpecular",
    "lightDiffuse",
    "ambientColor"
};

const int e_type[] = {
    PHLuaMaterial::renderer::solver::value::vMatrix,
    PHLuaMaterial::renderer::solver::value::vMatrix,
    PHLuaMaterial::renderer::solver::value::vVec4,
    PHLuaMaterial::renderer::solver::value::vFloat,
    PHLuaMaterial::renderer::solver::value::vVec3,
    PHLuaMaterial::renderer::solver::value::vVec4,
    PHLuaMaterial::renderer::solver::value::vVec4,
    PHLuaMaterial::renderer::solver::value::vVec4,
};

//to be used when the switch becomes not enough
typedef void (*getter)(GLfloat *, PHGameManager *);
typedef void (*matrix_getter)(PHMatrix & m, PHGameManager *);

void PHLuaMaterial::renderer::solver::value::solveVariable(PHGameManager * gm, PHMatrix * m, int & ms)
{
    if (type != vVar) return;
    int vari = i;
    switch (vari)
    {
        case modelView:
            (*m) = gm->modelViewMatrix();
            break;
        case projection:
            (*m) = gm->projectionMatrix();
            break;
        case color:
        {
            PHColor c = gm->color();
            fv[0]=c.r; fv[1]=c.g; fv[2]=c.b; fv[3]=c.a;
            break;
        }
        case ambientColor:
        {
            PHColor c = gm->ambientColor();
            fv[0]=c.r; fv[1]=c.g; fv[2]=c.b; fv[3]=c.a;
            break;
        }
        case lPos:
        {
            PHVector3 c = gm->currentLight()->position;
            fv[0]=c.x; fv[1]=c.y; fv[2]=c.z;
            break;
        }
        case lDiff:
        {
            PHColor c = gm->currentLight()->diffuse;
            fv[0]=c.r; fv[1]=c.g; fv[2]=c.b; fv[3]=c.a;
            break;
        }
        case lSpec:
        {
            PHColor c = gm->currentLight()->specular;
            fv[0]=c.r; fv[1]=c.g; fv[2]=c.b; fv[3]=c.a;
            break;
        }
        case lIntensity:
            f = gm->currentLight()->intensity;
            break;
    };
    type = e_type[vari];
    if (type == vMatrix)
    {
        this->m = m;
        ms++;
    }
}

//Operations
typedef void (*operation)(PHLuaMaterial::renderer::solver::value *);
typedef void (*matrix_operation)(PHLuaMaterial::renderer::solver::value *, PHMatrix * r);

static void fplus_f_f(PHLuaMaterial::renderer::solver::value * s){
    s[0].f += s[1].f;
}
static void fplus_v2_v2(PHLuaMaterial::renderer::solver::value * s){
    s[0].fv[0] += s[1].fv[0];
    s[0].fv[1] += s[1].fv[1];
}
static void fplus_v3_v3(PHLuaMaterial::renderer::solver::value * s){
    s[0].fv[0] += s[1].fv[0];
    s[0].fv[1] += s[1].fv[1];
    s[0].fv[2] += s[1].fv[2];
}
static void fplus_v4_v4(PHLuaMaterial::renderer::solver::value * s){
    s[0].fv[0] += s[1].fv[0];
    s[0].fv[1] += s[1].fv[1];
    s[0].fv[2] += s[1].fv[2];
    s[0].fv[3] += s[1].fv[3];
}
static void fminus_f_f(PHLuaMaterial::renderer::solver::value * s){
    s[0].f += s[1].f;
}
static void fminus_v2_v2(PHLuaMaterial::renderer::solver::value * s){
    s[0].fv[0] -= s[1].fv[0];
    s[0].fv[1] -= s[1].fv[1];
}
static void fminus_v3_v3(PHLuaMaterial::renderer::solver::value * s){
    s[0].fv[0] -= s[1].fv[0];
    s[0].fv[1] -= s[1].fv[1];
    s[0].fv[2] -= s[1].fv[2];
}
static void fminus_v4_v4(PHLuaMaterial::renderer::solver::value * s){
    s[0].fv[0] -= s[1].fv[0];
    s[0].fv[1] -= s[1].fv[1];
    s[0].fv[2] -= s[1].fv[2];
    s[0].fv[3] -= s[1].fv[3];
}
static void fmultiply_f_f(PHLuaMaterial::renderer::solver::value * s){
    s[0].f *= s[1].f;
}
static void fmultiply_v2_f(PHLuaMaterial::renderer::solver::value * s){
    s[0].fv[0] *= s[1].f;
    s[0].fv[1] *= s[1].f;
}
static void fmultiply_v2_v2(PHLuaMaterial::renderer::solver::value * s){
    s[0].fv[0] *= s[1].fv[0];
    s[0].fv[1] *= s[1].fv[1];
}
static void fmultiply_v3_f(PHLuaMaterial::renderer::solver::value * s){
    s[0].fv[0] *= s[1].f;
    s[0].fv[1] *= s[1].f;
    s[0].fv[2] *= s[1].f;
}
static void fmultiply_v3_v3(PHLuaMaterial::renderer::solver::value * s){
    s[0].fv[0] *= s[1].fv[0];
    s[0].fv[1] *= s[1].fv[1];
    s[0].fv[2] *= s[1].fv[2];
}
static void fmultiply_v4_f(PHLuaMaterial::renderer::solver::value * s){
    s[0].fv[0] *= s[1].f;
    s[0].fv[1] *= s[1].f;
    s[0].fv[2] *= s[1].f;
    s[0].fv[3] *= s[1].f;
}
static void fmultiply_v4_v4(PHLuaMaterial::renderer::solver::value * s){
    s[0].fv[0] *= s[1].fv[0];
    s[0].fv[1] *= s[1].fv[1];
    s[0].fv[2] *= s[1].fv[2];
    s[0].fv[3] *= s[1].fv[3];
}
static void fdivide_f_f(PHLuaMaterial::renderer::solver::value * s){
    s[0].f /= s[1].f;
}
static void fdivide_v2_f(PHLuaMaterial::renderer::solver::value * s){
    s[0].fv[0] /= s[1].f;
    s[0].fv[1] /= s[1].f;
}
static void fdivide_v2_v2(PHLuaMaterial::renderer::solver::value * s){
    s[0].fv[0] /= s[1].fv[0];
    s[0].fv[1] /= s[1].fv[1];
}
static void fdivide_v3_f(PHLuaMaterial::renderer::solver::value * s){
    s[0].fv[0] /= s[1].f;
    s[0].fv[1] /= s[1].f;
    s[0].fv[2] /= s[1].f;
}
static void fdivide_v3_v3(PHLuaMaterial::renderer::solver::value * s){
    s[0].fv[0] /= s[1].fv[0];
    s[0].fv[1] /= s[1].fv[1];
    s[0].fv[2] /= s[1].fv[2];
}
static void fdivide_v4_f(PHLuaMaterial::renderer::solver::value * s){
    s[0].fv[0] /= s[1].f;
    s[0].fv[1] /= s[1].f;
    s[0].fv[2] /= s[1].f;
    s[0].fv[3] /= s[1].f;
}
static void fdivide_v4_v4(PHLuaMaterial::renderer::solver::value * s){
    s[0].fv[0] /= s[1].fv[0];
    s[0].fv[1] /= s[1].fv[1];
    s[0].fv[2] /= s[1].fv[2];
    s[0].fv[3] /= s[1].fv[3];
}
static void fmultmat(PHLuaMaterial::renderer::solver::value * s, PHMatrix * m){
    (*m) = (*(s[0].m))*(*(s[1].m));
}
static void ftransposed(PHLuaMaterial::renderer::solver::value * s, PHMatrix * m){
    (*m) = s[0].m->transposed();
}
static void finverse(PHLuaMaterial::renderer::solver::value * s, PHMatrix * m){
    (*m) = s[0].m->inverse();
}

static const operation op_f[] = {
    fplus_f_f,
    fplus_v2_v2,
    fplus_v3_v3,
    fplus_v4_v4,
    fminus_f_f,
    fminus_v2_v2,
    fminus_v3_v3,
    fminus_v4_v4,
    fmultiply_f_f,
    fmultiply_v2_f,
    fmultiply_v2_v2,
    fmultiply_v3_f,
    fmultiply_v3_v3,
    fmultiply_v4_f,
    fmultiply_v4_v4,
    fdivide_f_f,
    fdivide_v2_f,
    fdivide_v2_v2,
    fdivide_v3_f,
    fdivide_v3_v3,
    fdivide_v4_f,
    fdivide_v4_v4,
    (operation)fmultmat,
    (operation)ftransposed,
    (operation)finverse
};

const char * op_names[] = {
    "plus_f_f",
    "plus_v2_v2",
    "plus_v3_v3",
    "plus_v4_v4",
    "minus_f_f",
    "minus_v2_v2",
    "minus_v3_v3",
    "minus_v4_v4",
    "multiply_f_f",
    "multiply_v2_f",
    "multiply_v2_v2",
    "multiply_v3_f",
    "multiply_v3_v3",
    "multiply_v4_f",
    "multiply_v4_v4",
    "divide_f_f",
    "divide_v2_f",
    "divide_v2_v2",
    "divide_v3_f",
    "divide_v3_v3",
    "divide_v4_f",
    "divide_v4_v4",
    "multiply_mat_mat",
    "transposed_mat",
    "inverse_mat"
};

const int op_type[] = {
    //plus
    PHLuaMaterial::renderer::solver::value::vFloat,
    PHLuaMaterial::renderer::solver::value::vVec2,
    PHLuaMaterial::renderer::solver::value::vVec3,
    PHLuaMaterial::renderer::solver::value::vVec4,
    //minus
    PHLuaMaterial::renderer::solver::value::vFloat,
    PHLuaMaterial::renderer::solver::value::vVec2,
    PHLuaMaterial::renderer::solver::value::vVec3,
    PHLuaMaterial::renderer::solver::value::vVec4,
    //multiply
    PHLuaMaterial::renderer::solver::value::vFloat,
    PHLuaMaterial::renderer::solver::value::vVec2,
    PHLuaMaterial::renderer::solver::value::vVec2,
    PHLuaMaterial::renderer::solver::value::vVec3,
    PHLuaMaterial::renderer::solver::value::vVec3,
    PHLuaMaterial::renderer::solver::value::vVec4,
    PHLuaMaterial::renderer::solver::value::vVec4,
    //divide
    PHLuaMaterial::renderer::solver::value::vFloat,
    PHLuaMaterial::renderer::solver::value::vVec2,
    PHLuaMaterial::renderer::solver::value::vVec2,
    PHLuaMaterial::renderer::solver::value::vVec3,
    PHLuaMaterial::renderer::solver::value::vVec3,
    PHLuaMaterial::renderer::solver::value::vVec4,
    PHLuaMaterial::renderer::solver::value::vVec4,
    //multmat
    PHLuaMaterial::renderer::solver::value::vMatrix,
    //transposed
    PHLuaMaterial::renderer::solver::value::vMatrix,
    //inverse
    PHLuaMaterial::renderer::solver::value::vMatrix,
};

static const int op_argcount[] = {
    //plus
    2, 2, 2, 2,
    //minus
    2, 2, 2, 2,
    //multiply
    2, 2,2, 2,2, 2,2,
    //divide
    2, 2,2, 2,2, 2,2,
    //multmat
    2,
    //transposed
    1,
    //inverse
    1
};

static const char * type_names[] = {
   "inv",
   "f",
   "v2",
   "v3",
   "v4",
   "mat",
   "var",
   "op",
   "i"
};


void PHLuaMaterial::renderer::solver::value::adjustType()
{
    if (type == vVar)
        type = e_type[i];
}

int PHLuaMaterial::renderer::solver::value::argCount()
{
   return op_argcount[i];
}

int PHLuaMaterial::renderer::solver::value::returnType()
{
    return op_type[i];
}

void PHLuaMaterial::initEnvironment(lua_State * L)
{
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setglobal(L, "ops_c");
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setglobal(L, "ops_r");
    for (int i = 0; i < sizeof(op_names)/sizeof(const char*); i++)
    {
        lua_pushnumber(L, i);
        lua_setfield(L, -3, op_names[i]);
        lua_pushstring(L, type_names[op_type[i]]);
        lua_setfield(L, -2, op_names[i]);
    }
    lua_pop(L, 2);
    PHLuaLoadFile(L, gm->resourceDirectory(), "materials/common.lua");
    
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setglobal(L, "vars");
    lua_getglobal(L, "metastack");
    lua_getfield(L, -1, "variable");
    for (int i = 0; i < sizeof(e_names)/sizeof(const char*); i++)
    {
        lua_pushvalue(L, -1);
        lua_pushnumber(L, i);
        lua_pushstring(L, type_names[e_type[i]]);
        PHLuaCall(L, 2, 1);
        lua_setfield(L, -4, e_names[i]);
    }
    lua_pop(L, 3);
}


void PHLuaMaterial::renderer::solver::solve(PHGameManager * gm)
{
    int s = 0;
    int ms = 0;
    for (int i = 0; i<eq_n; i++)
    {
        value & v = equation[i];
        if (v.type == value::vOp)
        {
            int ac = v.argCount();
            for (;ac; ac--)
            {
                s--;
                if (stack[s].type == value::vMatrix)
                    ms--;
            }
            if (op_type[v.i] == value::vMatrix)
                ((matrix_operation)op_f[v.i])(stack + s, matrixStack+(ms++));
            else
                (op_f[v.i])(stack + s);
        } else {
            stack[s] = v;
            stack[s].solveVariable(gm, matrixStack+ms, ms);
        }
        s++;
    }
    stack[0].applyUniform(uniform);
}

bool PHLuaMaterial::renderer::solver::value::applyUniform(PHGLUniformStates::uniform * u)
{
    switch(type)
    {
        case vInt:
            u->setValue(i);
            return true;
        case vFloat:
            u->setValue(f);
            return true;
        case vVec2:
            u->setValue(fv[0], fv[1]);
            return true;
        case vVec3:
            u->setValue(fv[0], fv[1], fv[2]);
            return true;
        case vVec4:
            u->setValue(fv[0], fv[1], fv[2], fv[3]);
            return true;
        case vMatrix:
            u->setValue(*m);
            return true;
    };
    return false;
}

void PHLuaMaterial::renderer::solver::value::readFromLua(lua_State * L)
{
    if (lua_isnumber(L, -1))
    {
        type = vFloat;
        f = lua_tonumber(L, -1);
        return;
    }
    if (lua_istable(L, -1))
    {
        type = vInvalid;
        char c[2] = {'x','\0'};
        for (int i = 0; i<4; i++, c[0]++)
        {
            lua_pushnumber(L, i+1);
            lua_gettable(L, -2);
            if (!lua_isnumber(L, -1))
            {
                lua_pop(L, 1);
                lua_getfield(L, -1, c);
            }
            if (lua_isnumber(L, -1))
            {
                type++;
                fv[i] = lua_tonumber(L, -1);
                lua_pop(L, 1);
            } else {
                lua_pop(L, 1);
                break;
            }
        }
        if (type == vInvalid)
        {
            lua_getfield(L, -1, "op");
            if (lua_isnumber(L, -1))
            {
                i = lua_tonumber(L, -1);
                type = vOp;
            }
            lua_pop(L, 1);
            if (type == vInvalid)
            {
                lua_getfield(L, -1, "var");
                if (lua_isnumber(L, -1))
                {
                    i = lua_tonumber(L, -1);
                    type = vVar;
                }
                lua_pop(L, 1);
            }
        }
        return;
    }
    type = vInvalid;
}

void PHLuaMaterial::renderVAO(PHGLVertexArrayObject * vao, PHGLUniformStates * us)
{
    if (!vao) return;
    PHGLLight * l = gm->currentLight();
    pair<int,int> key(gm->renderMode(), l?l->type:0);
    map<pair<int,int>, renderer>::iterator i, e = renderers.end();

    i = renderers.find(key);
    if (i == e)
    {
        i = renderers.find(make_pair(key.first, INT_MAX));
        if (i == e)
        {
            i = renderers.find(make_pair(INT_MAX, key.second));
            if (i == e)
            {
                i = renderers.find(make_pair(INT_MAX, INT_MAX));
                if (i == e)
                    return;
            }
        }
    }
    renderer & r = i->second;

    for (int i = 0; i < r.nvars; i++)
        r.vars[i].solve(gm);

    r.shader->use();
    int c = 0;
    r.uniforms->apply(r.shader, &c);
    if (us)
        us->apply(r.shader, &c);
    vao->draw();
}
