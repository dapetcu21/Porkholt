/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHMATERIALINITPOOL_H
#define PHMATERIALINITPOOL_H

#include <Porkholt/Core/PHMain.h>

class PHMaterial;
class PHGameManager;
class PHDirectory;
class PHFile;

class PHMaterialInitPool 
{
    protected:
        map<PHHashedString, PHMaterial*> materials; 
    public:
        virtual PHDirectory * materialDirectory() = 0;
        virtual PHGameManager * gameManager() = 0;
        PHMaterial * materialFromFile(PHFile * file);
        PHMaterial * materialNamed(const string & name, PHDirectory * dir);
        PHMaterial * materialNamed(const string & name) { return materialNamed(name, materialDirectory()); }
        void collectGarbageMaterials();
};

#endif
