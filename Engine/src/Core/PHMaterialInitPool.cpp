/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHMaterialInitPool.h>
#include <Porkholt/Core/PHLuaMaterial.h>
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
    PHMaterial * mat = NULL;
    PHFile * file = NULL;
    try {
        file = dir->fileAtPath(name + ".lua");
        mat = new PHLuaMaterial(gameManager(), file);
        file->release();
    } catch(const string & ex)
    {
        if (file)
            file->release();
        PHLog("Could not load material \"%s\": %s", name.c_str(), ex.c_str());
        mat = NULL;
    }
    return mat;
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
