/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/IO/PHRegularFile.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <unistd.h>

PHRegularFile::PHRegularFile(const string & dpath, bool check_exists) : PHFile(dpath), fd(-1)
{
    if (check_exists)
    {
        struct stat result;
        if (stat(_path.c_str(), &result) != 0)
            throw string(strerror(errno));
        if (!(result.st_mode & S_IFREG))
            throw string("not a file");
    }
}

void PHRegularFile::open(int flags)
{
    if (fd >= 0)
    {
        ::close(fd);
        fd = -1;
    }
    int oflags = (flags & PHStream::Read) ? ((flags & PHStream::Write) ? O_RDWR : O_RDONLY) : ((flags & PHStream::Write) ? O_WRONLY : 0);
    if (flags & PHStream::Create)
        oflags |= O_CREAT;
    if (flags & PHStream::Append)
        oflags |= O_APPEND;
    if (flags & PHStream::Trunc)
        oflags |= O_TRUNC;
    if ((fd = ::open(_path.c_str(), oflags, 0644)) < 0)
        throw string(strerror(errno));
    PHFile::open(flags);
}

void PHRegularFile::close()
{
    if (fd >= 0)
    {
        ::close(fd);
        fd = -1;
    }
    PHStream::close();
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
    if (((fd>=0)?fstat(fd, &r):stat(_path.c_str(), &r)) != 0)
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

