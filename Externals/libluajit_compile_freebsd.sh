#!/bin/bash
 
cd luajit

LIBFILE=src/libluajit
BINFILE=src/luajit

LIBPATH_static=$LIBFILE.a
LIBNAME_static=`basename $LIBPATH_static`
LIBINSTALL_static=$LIBNAME_static
BINNAME=`basename $BINFILE`

make clean
make freebsd

mkdir -p ../lib/freebsd
mkdir -p ../bin/freebsd
cp $LIBPATH_static ../lib/freebsd/$LIBINSTALL_static
cp $BINFILE ../bin/freebsd/$BINNAME

cd ..
