/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHMaterialInitPool.h>
#include <Porkholt/Core/PHLuaMaterial.h>
#include <Porkholt/Core/PHAutoreleasePool.h>
#include <Porkholt/IO/PHDirectory.h>
#include <Porkholt/IO/PHFile.h>

PHMaterial * PHMaterialInitPool::materialFromFile(PHFile * file, const string & options)
{
    PHMaterial * mat;
    PHHashedString path(file->path());
    PHHashedString opts(options);
    pair<PHHashedString, PHHashedString> key = make_pair(path, opts);
    map< pair < PHHashedString, PHHashedString >, PHMaterial*>::iterator i = materials.find(key);
    if (i==materials.end())
    {
        PHGameManager * gm = gameManager();
        mat = new PHLuaMaterial(gm, file, options);
        materials[key] = mat;
    } else {
        mat = i->second;
    }
    return mat;
}

PHMaterial * PHMaterialInitPool::materialNamed(const string & name, PHDirectory * dir)
{
    PHAutoreleasePool ap;
    string opts;
    string nm = name;
    size_t n = name.size();
    if (n && name[n - 1] == ']')
    {
        size_t f = name.find("[");
        if (f != string::npos)
        {
            opts = name.substr(f + 1, n - f - 2);
            nm = name.substr(0, f);
        }
    }
    try { return materialFromFile(dir->fileAtPath(nm + ".lua"), opts); }
    catch(const string & ex) {
        PHLog("Could not load material \"%s\": %s", name.c_str(), ex.c_str());
    }
    return NULL;
}

void PHMaterialInitPool::collectGarbageMaterials()
{
    map<pair<PHHashedString, PHHashedString>,PHMaterial*>::iterator i;
	map<pair<PHHashedString, PHHashedString>,PHMaterial*> tmp;
	for (i = materials.begin(); i!=materials.end(); i++)
	{
		if ((i->second)->referenceCount()>=2)
			tmp[i->first] = i->second;
		else
			i->second->release();
	}
	materials = tmp;
}

PHMaterialInitPool::~PHMaterialInitPool()
{
    PHLog("Deallocating materials");
    for (map<pair<PHHashedString, PHHashedString>,PHMaterial*>::iterator i = materials.begin(); i != materials.end(); i++)
        i->second->release();
}
