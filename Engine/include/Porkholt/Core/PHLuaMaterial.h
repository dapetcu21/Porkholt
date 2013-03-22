/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHLUAMATERIAL_H
#define PHLUAMATERIAL_H

#include <Porkholt/Core/PHGLUniformStates.h>
#include <Porkholt/Core/PHMaterial.h>

class PHMutex;
class PHFile;
class PHGameManager;
class PHGLShaderProgram;

class PHLuaMaterial : public PHMaterial
{
    private:
        static lua_State * sL;
        static PHMutex * luaMutex;
    public: //actually private. don't use these... please :o3 
        struct renderer
        {
            PHGLUniformStates * uniforms;
            struct solver
            {
                struct value
                {
                    enum 
                    {
                        vInvalid,
                        vFloat,
                        vVec2,
                        vVec3,
                        vVec4,
                        vMatrix,
                        vVar,
                        vOp,
                        vInt
                    };
                    int type;
                    union {
                        int i;
                        GLfloat f;
                        GLfloat fv[4];
                        PHMatrix * m;
                    };
                    void readFromLua(lua_State * L);
                    bool applyUniform(PHGLUniformStates::uniform * u);

                    void adjustType();
                    int argCount();
                    int returnType();

                    void solveVariable(PHGameManager * gm, PHMatrix * m, int & ms);
                };
                value * stack;
                PHMatrix * matrixStack;
                value * equation;
                size_t eq_n;
                PHGLUniformStates::uniform * uniform;
                void solve(PHGameManager * gm);
            };

            int nvars;
            solver * vars;

            PHGLShaderProgram * shader;
            void render(PHGameManager * gm);
        };
    private:

        map<pair<int, int>, struct renderer > renderers;
        void loadFromLua(lua_State * L);
        size_t calculateMem(lua_State * L, size_t & ss, size_t & ms);
        PHGameManager * gm;

    public:
        PHLuaMaterial(PHGameManager * gm, PHFile * file, const string & options = "");
        ~PHLuaMaterial();

        void initEnvironment(lua_State * L);
        bool materialSupportsRenderMode(int renderMode);
        void renderVAO(PHGLVertexArrayObject * vao, PHGLUniformStates * us);
};

#endif
