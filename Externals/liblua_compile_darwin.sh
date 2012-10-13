#!/bin/bash

cd lua

LIBFILE=src/liblua
LIBPATH_static=$LIBFILE.a
LIBNAME_static=`basename $LIBPATH_static`
LIBINSTALL_static=$LIBNAME_static

rm -rf lnsout
mkdir -p lnsout

for ARCH in $IOSARCHS; do
    CFLAGS="-arch $ARCH -isysroot $IOSSDK -miphoneos-version-min=2.2 -pipe -no-cpp-precomp" 
    make clean 2>&1 > /dev/null
    cd src && make liblua.a CC="$IOSCC $CFLAGS" MYCFLAGS=-DLUA_USE_LINUX && cd - || exit 1
    cp $LIBPATH_static lnsout/$LIBNAME_static.ios.$ARCH
done

for ARCH in $SIMARCHS; do
    CFLAGS="-arch $ARCH -isysroot $SIMSDK -mmacosx-version-min=10.6 -pipe -no-cpp-precomp"
    make clean 2>&1 > /dev/null
    cd src && make liblua.a CC="$SIMCC $CFLAGS" MYCFLAGS=-DLUA_USE_LINUX && cd - || exit 1
    cp $LIBPATH_static lnsout/$LIBNAME_static.sim.$ARCH
done
 
for ARCH in $OSXARCHS; do
    CFLAGS="-arch $ARCH -isysroot $OSXSDK -mmacosx-version-min=10.5 -pipe -no-cpp-precomp"
    make clean 2>&1 > /dev/null
    make macosx CC="$OSXCC $CFLAGS" || exit 1
    cp $LIBPATH_static lnsout/$LIBNAME_static.osx.$ARCH
done

$IOSLIPO lnsout/$LIBNAME_static.ios.* lnsout/$LIBNAME_static.sim.* -create -output lnsout/$LIBNAME_static.ios
$OSXLIPO lnsout/$LIBNAME_static.osx.* -create -output lnsout/$LIBNAME_static.osx
mkdir -p ../lib/darwin/ios
mkdir -p ../lib/darwin/osx
cp lnsout/$LIBNAME_static.ios ../lib/darwin/ios/$LIBINSTALL_static
cp lnsout/$LIBNAME_static.osx ../lib/darwin/osx/$LIBINSTALL_static

rm -rf lnsout
cd ..
