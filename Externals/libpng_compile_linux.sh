#!/bin/sh
 
# build_fat.sh
#
# Created by Robert Carlsen on 15.07.2009.
# Heavily Modified by Marius Petcu
# build an arm / i386 lib of standard linux project

cd libpng

LIBFILE=libpng

LIBPATH_static=$LIBFILE.a
LIBNAME_static=`basename $LIBPATH_static`
LIBINSTALL_static=libpng15.a

make -f scripts/makefile.linux clean
make -f scripts/makefile.linux 

mkdir -p ../lib/linux
cp $LIBPATH_static ../lib/linux/$LIBINSTALL_static

cd ..
