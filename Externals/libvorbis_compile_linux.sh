#!/bin/sh

cd libvorbis 

make distclean
CFLAGS="-fPIC" ./configure --with-ogg-includes=../libogg/include --with-ogg-libraries=../libogg/src/.libs
make

LIBFILE=lib/.libs/libvorbis
LIBPATH_static=$LIBFILE.a
LIBNAME_static=`basename $LIBPATH_static`
LIBINSTALL_static=$LIBNAME_static

mkdir -p ../lib/linux
cp $LIBPATH_static ../lib/linux/$LIBINSTALL_static

LIBFILE=lib/.libs/libvorbisfile
LIBPATH_static=$LIBFILE.a
LIBNAME_static=`basename $LIBPATH_static`
LIBINSTALL_static=$LIBNAME_static

cp $LIBPATH_static ../lib/linux/$LIBINSTALL_static

cd ..
