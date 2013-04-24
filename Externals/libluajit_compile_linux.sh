#!/bin/bash
 
cd luajit

LIBFILE=src/libluajit
BINFILE=src/luajit

LIBPATH_static=$LIBFILE.a
LIBNAME_static=`basename $LIBPATH_static`
LIBINSTALL_static=$LIBNAME_static
BINNAME=`basename $BINFILE`

make clean
make

mkdir -p ../lib/linux
mkdir -p ../bin/linux
cp $LIBPATH_static ../lib/linux/$LIBINSTALL_static
cp $BINFILE ../bin/linux/$BINNAME

cd ..
