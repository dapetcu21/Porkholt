#!/bin/sh
 
# build_fat.sh
#
# Created by Robert Carlsen on 15.07.2009.
# Heavily Modified by Marius Petcu
# build an arm / i386 lib of standard linux project

cd libpng

LIBPATH_static=.libs/libpng15.a
LIBINSTALL_static=libpng15.a

LDFLAGS="-L../lib/linux" CFLAGS="-I../zlib" ./configure
make clean
make

mkdir -p ../lib/linux
cp $LIBPATH_static ../lib/linux/$LIBINSTALL_static

cd ..
