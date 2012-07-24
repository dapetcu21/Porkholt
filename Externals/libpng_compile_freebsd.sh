#!/bin/sh
 
# build_fat.sh
#
# Created by Robert Carlsen on 15.07.2009.
# Heavily Modified by Marius Petcu
# build an arm / i386 lib of standard linux project

cd libpng

LIBPATH_static=.libs/libpng15.a
LIBINSTALL_static=libpng15.a

LDFLAGS="-L../lib/freebsd" CFLAGS="-I../zlib" ./configure
make clean
make

mkdir -p ../lib/freebsd
cp $LIBPATH_static ../lib/freebsd/$LIBINSTALL_static

cd ..
