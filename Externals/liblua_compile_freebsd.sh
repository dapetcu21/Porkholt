#!/bin/bash
 
# build_fat.sh
#
# Created by Robert Carlsen on 15.07.2009.
# Heavily Modified by Marius Petcu
# build an arm / i386 lib of standard linux project

cd lua

LIBFILE=src/liblua

LIBPATH_static=$LIBFILE.a
LIBNAME_static=`basename $LIBPATH_static`
LIBINSTALL_static=$LIBNAME_static

make clean
make freebsd

mkdir -p ../lib/freebsd
cp $LIBPATH_static ../lib/freebsd/$LIBINSTALL_static

cd ..
