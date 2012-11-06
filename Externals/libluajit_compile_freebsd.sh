#!/bin/bash
 
cd lua

LIBFILE=src/libluajit

LIBPATH_static=$LIBFILE.a
LIBNAME_static=`basename $LIBPATH_static`
LIBINSTALL_static=$LIBNAME_static

make clean
make freebsd

mkdir -p ../lib/freebsd
cp $LIBPATH_static ../lib/freebsd/$LIBINSTALL_static

cd ..
