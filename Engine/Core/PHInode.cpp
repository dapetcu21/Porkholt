/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHInode.h"
#include "PHRegularFile.h"
#include "PHRegularDirectory.h"
#include <sys/stat.h>
#include <sys/errno.h>

PHInode * PHInode::itemAtFSPath(const string & p)
{
    struct stat results;
    if (stat(p.c_str(), &results) != 0)
        throw string(strerror(errno));
    if (results.st_mode & S_IFDIR)
        return new PHRegularDirectory(p, false);
    if (results.st_mode & S_IFREG)
        return new PHRegularFile(p, false);
}
