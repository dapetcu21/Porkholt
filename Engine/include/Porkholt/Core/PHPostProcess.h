/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHPOSTPROCESS_H
#define PHPOSTPROCESS_H

#include <Porkholt/Core/PHTextureCanvas.h>

class PHMaterial;
class PHGLUniformStates;
class PHGameManager;

class PHPostProcess : public PHTextureCanvas
{
    protected:
        PHMaterial * mat;
        PHGLUniformStates * us;
        bool resMat, efen;

        void render();
        void draw();
    public:
        PHMaterial * material() { return mat; }
        void setMaterial(PHMaterial * mat);
        bool resetsMatrices() { return resMat; }
        void setResetsMatrices(bool b) { resMat = b; }
        bool effectEnabled() { return efen; }
        void setEffectEnabled(bool ef) { efen = ef; }
        PHGLUniformStates * additionalUniforms() { return us; }

        PHPostProcess(PHGameManager * gm);
        ~PHPostProcess();

};

#endif
