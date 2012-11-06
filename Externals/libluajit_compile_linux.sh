#!/bin/bash
 
cd lua

LIBFILE=src/libluajit

LIBPATH_static=$LIBFILE.a
LIBNAME_static=`basename $LIBPATH_static`
LIBINSTALL_static=$LIBNAME_static

make clean
make

mkdir -p ../lib/linux
cp $LIBPATH_static ../lib/linux/$LIBINSTALL_static

cd ..
