/*
 *  PHRegularFile.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/13/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHRegularFile.h"
#include <sys/errno.h>
#include <sys/stat.h>
#include <unistd.h>

PHRegularFile::PHRegularFile(const string & dpath, int dummy) : path(dpath), fd(-1)
{
    init();
}

PHRegularFile::PHRegularFile(const string & dpath) : path(dpath), fd(-1)
{
    struct stat result;
    if (stat(path.c_str(), &result) != 0)
        throw string(strerror(errno));
    if (!(result.st_mode & S_IFREG))
        throw string("not a file");
    init();
}

void PHRegularFile::init()
{
}

void PHRegularFile::open(int flags)
{
    int oflags = (flags & PHStream::Read) ? ((flags & PHStream::Write) ? O_RDWR : O_RDONLY) : ((flags & PHStream::Write) ? O_WRONLY : 0);
    if (flags & PHStream::Create)
        oflags |= O_CREAT;
    if (flags & PHStream::Append)
        oflags |= O_APPEND;
    if ((fd = ::open(path.c_str(), oflags)) != 0)
        throw string(strerror(errno));
}

void PHRegularFile::close()
{
    if (fd >= 0)
        ::close(fd);
}

size_t PHRegularFile::read(uint8_t * buf, size_t size)
{
    ssize_t ret;
    do {
        ret = ::read(fd, buf, size);
        if (ret == -1)
        {
            if (errno == EINTR)
                continue;
            else
                throw string(strerror(errno));
        } else
            return size_t(ret);
    } while (true);
}

size_t PHRegularFile::write(const uint8_t * buf, size_t size)
{
    ssize_t ret;
    do {
        ret = ::write(fd, buf, size);
        if (ret == -1)
        {
            if (errno == EINTR)
                continue;
            else
                throw string(strerror(errno));
        } else
            return size_t(ret);
    } while (true);
}


size_t PHRegularFile::pointer()
{
    off_t r;
    if ((r = lseek(fd, 0, SEEK_CUR)) < 0)
        throw string(strerror(errno));
    return size_t(r);
}

void PHRegularFile::setPointer(size_t p)
{
    if (lseek(fd, p, SEEK_SET) < 0)
        throw string(strerror(errno));
}

size_t PHRegularFile::size()
{
    struct stat r;
    if (fstat(fd, &r) != 0)
        throw string(strerror(errno));
    return size_t(r.st_size);
}

void PHRegularFile::seek(ssize_t p)
{
    if (lseek(fd, p, SEEK_CUR) < 0)
        throw string(strerror(errno));
}

PHRegularFile::~PHRegularFile()
{
    close();
}

