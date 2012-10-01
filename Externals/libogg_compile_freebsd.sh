#!/bin/sh

cd libogg

LIBFILE=src/.libs/freebsd

LIBPATH_static=$LIBFILE.a
LIBNAME_static=`basename $LIBPATH_static`
LIBINSTALL_static=$LIBNAME_static

make distclean
CFLAGS="-fPIC" ./configure
make

mkdir -p ../lib/freebsd
cp $LIBPATH_static ../lib/freebsd/$LIBINSTALL_static

cd ..
