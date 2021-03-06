#!/bin/bash

cd luajit

LIBFILE=src/libluajit
BINFILE=src/luajit
BINNAME=`basename $BINFILE`
LIBPATH_static=$LIBFILE.a
LIBNAME_static=`basename $LIBPATH_static`
LIBINSTALL_static=$LIBNAME_static

rm -rf lnsout
mkdir -p lnsout

for ARCH in $IOSARCHS; do
    CFLAGS="-arch $ARCH -isysroot $IOSSDK -miphoneos-version-min=4.2 -pipe"
    make clean 2>&1 > /dev/null
    make HOST_CC="cc -arch i386" CROSS="xcrun -sdk iphoneos " TARGET_FLAGS="$CFLAGS" TARGET_SYS=iOS || exit 1
    cp $LIBPATH_static lnsout/$LIBNAME_static.ios.$ARCH
done

for ARCH in $SIMARCHS; do
    CFLAGS="-arch $ARCH -isysroot $SIMSDK -miphoneos-version-min=4.2 -pipe"
    make clean 2>&1 > /dev/null
    make HOST_CC="cc -arch $ARCH" CROSS="xcrun -sdk iphonesimulator " TARGET_FLAGS="$CFLAGS" TARGET_SYS=iOS || exit 1
    cp $LIBPATH_static lnsout/$LIBNAME_static.sim.$ARCH
done

for ARCH in $OSXARCHS; do
    CFLAGS="-arch $ARCH -isysroot $OSXSDK -mmacosx-version-min=10.5 -pipe"
    make clean 2>&1 > /dev/null
    make HOST_CC="$OSXCC $CFLAGS" CROSS="xcrun -sdk macosx " TARGET_FLAGS="$CFLAGS" || exit 1
    cp $LIBPATH_static lnsout/$LIBNAME_static.osx.$ARCH
    cp $BINFILE lnsout/$BINNAME.osx.$ARCH
done

$IOSLIPO lnsout/$LIBNAME_static.ios.* lnsout/$LIBNAME_static.sim.* -create -output lnsout/$LIBNAME_static.ios
$OSXLIPO lnsout/$LIBNAME_static.osx.* -create -output lnsout/$LIBNAME_static.osx
$OSXLIPO lnsout/$BINNAME.osx.* -create -output lnsout/$BINNAME
mkdir -p ../lib/darwin/ios
mkdir -p ../lib/darwin/osx
cp lnsout/$LIBNAME_static.ios ../lib/darwin/ios/$LIBINSTALL_static
cp lnsout/$LIBNAME_static.osx ../lib/darwin/osx/$LIBINSTALL_static
mkdir -p ../bin/darwin/osx
cp lnsout/$BINNAME ../bin/darwin/osx/$BINNAME

rm -rf lnsout
cd ..
