#!/bin/sh

cd libvorbis 

make distclean
CFLAGS="-fPIC" ./configure --with-ogg-includes=../libogg/include --with-ogg-libraries=../libogg/src/.libs
make

LIBFILE=lib/.libs/libvorbis
LIBPATH_static=$LIBFILE.a
LIBNAME_static=`basename $LIBPATH_static`
LIBINSTALL_static=$LIBNAME_static

mkdir -p ../lib/freebsd
cp $LIBPATH_static ../lib/freebsd/$LIBINSTALL_static

LIBFILE=lib/.libs/libvorbisfile
LIBPATH_static=$LIBFILE.a
LIBNAME_static=`basename $LIBPATH_static`
LIBINSTALL_static=$LIBNAME_static

cp $LIBPATH_static ../lib/freebsd/$LIBINSTALL_static

cd ..
