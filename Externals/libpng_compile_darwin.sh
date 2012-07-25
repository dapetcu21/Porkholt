#!/bin/sh
 
# build_fat.sh
#
# Created by Robert Carlsen on 15.07.2009.
# Heavily Modified by Marius Petcu
# build an arm / i386 lib of standard linux project

cd libpng

MACSDK=SDKs/MacOSX10.6.sdk
IOSSDK=SDKs/iPhoneOS5.0.sdk
SIMSDK=SDKs/iPhoneSimulator5.0.sdk

LIBPATH_static=.libs/libpng15.a
LIBINSTALL_static=libpng15.a
LIBNAME_static=libpng15.a

mkdir -p lnsout

#-------------------------------

DEVROOT=/Developer/Platforms/iPhoneOS.platform/Developer
SDKROOT=$DEVROOT/$IOSSDK
CC=$DEVROOT/usr/bin/cc

export LDFLAGS="-L../lib/darwin/ios"

CFLAGS="-arch armv6 -isysroot $SDKROOT -miphoneos-version-min=2.2 -pipe -no-cpp-precomp" 
./configure CC="$CC" CFLAGS="-I../zlib $CFLAGS" --host=armv6-apple-darwin 
make clean
make 
cp $LIBPATH_static lnsout/$LIBNAME_static.armv6

CFLAGS="-arch armv7 -isysroot $SDKROOT -miphoneos-version-min=2.2 -pipe -no-cpp-precomp"
./configure CC="$CC" CFLAGS="-I../zlib $CFLAGS" --host=armv7-apple-darwin
make clean
make 
cp $LIBPATH_static lnsout/$LIBNAME_static.armv7

#-------------------------------

DEVROOT=/Developer/Platforms/iPhoneSimulator.platform/Developer
SDKROOT=$DEVROOT/$SIMSDK
CC=$DEVROOT/usr/bin/cc

CFLAGS="-arch i386 -isysroot $SDKROOT -mmacosx-version-min=10.6 -pipe -no-cpp-precomp"
./configure CC="$CC" CFLAGS="-I../zlib $CFLAGS"
make clean
make 
cp $LIBPATH_static lnsout/$LIBNAME_static.i386.sim
 
#-------------------------------

DEVROOT=/Developer
SDKROOT=$DEVROOT/$MACSDK
CC="cc"

export LDFLAGS="-L../lib/darwin/osx"

CFLAGS="-arch i386 -isysroot $SDKROOT -mmacosx-version-min=10.5 -pipe -no-cpp-precomp"
./configure CC="$CC" CFLAGS="-I../zlib $CFLAGS"
make clean
make
cp $LIBPATH_static lnsout/$LIBNAME_static.i386

CFLAGS="-arch x86_64 -isysroot $SDKROOT -mmacosx-version-min=10.5 -pipe -no-cpp-precomp"
./configure CC="$CC" CFLAGS="-I../zlib $CFLAGS"
make clean
make
cp $LIBPATH_static lnsout/$LIBNAME_static.x86_64

#-------------------------------

/usr/bin/lipo -arch armv6 lnsout/$LIBNAME_static.armv6 -arch armv7 lnsout/$LIBNAME_static.armv7 -arch i386 lnsout/$LIBNAME_static.i386.sim -create -output lnsout/$LIBNAME_static.ios
/usr/bin/lipo -arch i386 lnsout/$LIBNAME_static.i386 -arch x86_64 lnsout/$LIBNAME_static.x86_64 -create -output lnsout/$LIBNAME_static.osx

mkdir -p ../lib/darwin/ios
mkdir -p ../lib/darwin/osx
cp lnsout/$LIBNAME_static.ios ../lib/darwin/ios/$LIBINSTALL_static
cp lnsout/$LIBNAME_static.osx ../lib/darwin/osx/$LIBINSTALL_static

unset LDFLAGS

cd ..
