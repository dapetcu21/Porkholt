/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHMaterialInitPool.h>
#include <Porkholt/Core/PHLuaMaterial.h>
#include <Porkholt/Core/PHAutoreleasePool.h>
#include <Porkholt/IO/PHDirectory.h>
#include <Porkholt/IO/PHFile.h>

PHMaterial * PHMaterialInitPool::materialFromFile(PHFile * file)
{
    PHMaterial * mat;
    PHHashedString path(file->path());
    map<PHHashedString, PHMaterial*>::iterator i = materials.find(path);
    if (i==materials.end())
    {
        PHGameManager * gm = gameManager();
        mat = new PHLuaMaterial(gm, file);
        materials[path] = mat;
    } else {
        mat = i->second;
    }
    return mat;
}

PHMaterial * PHMaterialInitPool::materialNamed(const string & name, PHDirectory * dir)
{
    PHAutoreleasePool ap;
    try { return materialFromFile(dir->fileAtPath(name + ".lua")); }
    catch(const string & ex) {
        PHLog("Could not load material \"%s\": %s", name.c_str(), ex.c_str());
    }
    return NULL;
}

void PHMaterialInitPool::collectGarbageMaterials()
{
    map<PHHashedString,PHMaterial*>::iterator i;
	map<PHHashedString,PHMaterial*> tmp;
	for (i = materials.begin(); i!=materials.end(); i++)
	{
		if ((i->second)->referenceCount()>=2)
			tmp[i->first] = i->second;
		else
			i->second->release();
	}
	materials = tmp;
}
