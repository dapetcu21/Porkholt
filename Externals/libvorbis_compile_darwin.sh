#!/bin/sh
 
# build_fat.sh
#
# Created by Robert Carlsen on 15.07.2009.
# Heavily Modified by Marius Petcu
# build an arm / i386 lib of standard linux project

cd zlib

LIBFILE=lib/.libs/libvorbis

MACSDK=SDKs/MacOSX10.6.sdk
IOSSDK=SDKs/iPhoneOS5.0.sdk
SIMSDK=SDKs/iPhoneSimulator5.0.sdk

LIBPATH_static=$LIBFILE.a
LIBNAME_static=`basename $LIBPATH_static`
LIBINSTALL_static=$LIBNAME_static

LIBFILE2=lib/.libs/libvorbisfile
LIBPATH2_static=$LIBFILE2.a
LIBNAME2_static=`basename $LIBPATH2_static`
LIBINSTALL2_static=$LIBNAME2_static

CONFIG= "--with-ogg-includes=../libogg/include --with-ogg-libraries=../libogg/src/.libs"

mkdir -p lnsout

#-------------------------------

DEVROOT=/Developer/Platforms/iPhoneOS.platform/Developer
SDKROOT=$DEVROOT/$IOSSDK
export CC=$DEVROOT/usr/bin/cc

export CFLAGS="-arch armv6 -isysroot $SDKROOT -miphoneos-version-min=2.2 -pipe -no-cpp-precomp" 
make distclean
./configure $CONFIG
make
cp $LIBPATH_static lnsout/$LIBNAME_static.armv6
cp $LIBPATH2_static lnsout/$LIBNAME2_static.armv6

export CFLAGS="-arch armv7 -isysroot $SDKROOT -miphoneos-version-min=2.2 -pipe -no-cpp-precomp"
make distclean
./configure $CONFIG
make
cp $LIBPATH_static lnsout/$LIBNAME_static.armv7
cp $LIBPATH2_static lnsout/$LIBNAME2_static.armv7

#-------------------------------

DEVROOT=/Developer/Platforms/iPhoneSimulator.platform/Developer
SDKROOT=$DEVROOT/$SIMSDK
export CC=$DEVROOT/usr/bin/cc

export CFLAGS="-arch i386 -isysroot $SDKROOT -mmacosx-version-min=10.6 -pipe -no-cpp-precomp"
make distclean
./configure $CONFIG
make
cp $LIBPATH_static lnsout/$LIBNAME_static.i386.sim
cp $LIBPATH2_static lnsout/$LIBNAME2_static.i386.sim
 
#-------------------------------

DEVROOT=/Developer
SDKROOT=$DEVROOT/$MACSDK
export CC="cc"

export CFLAGS="-arch i386 -isysroot $SDKROOT -mmacosx-version-min=10.5 -pipe -no-cpp-precomp"
make distclean
./configure $CONFIG
make
cp $LIBPATH_static lnsout/$LIBNAME_static.i386

export CFLAGS="-arch x86_64 -isysroot $SDKROOT -mmacosx-version-min=10.5 -pipe -no-cpp-precomp"
make distclean
./configure $CONFIG
make
cp $LIBPATH_static lnsout/$LIBNAME_static.x86_64
cp $LIBPATH2_static lnsout/$LIBNAME2_static.x86_64

#-------------------------------

/usr/bin/lipo -arch armv6 lnsout/$LIBNAME_static.armv6 -arch armv7 lnsout/$LIBNAME_static.armv7 -arch i386 lnsout/$LIBNAME_static.i386.sim -create -output lnsout/$LIBNAME_static.ios
/usr/bin/lipo -arch armv6 lnsout/$LIBNAME2_static.armv6 -arch armv7 lnsout/$LIBNAME2_static.armv7 -arch i386 lnsout/$LIBNAME2_static.i386.sim -create -output lnsout/$LIBNAME2_static.ios
/usr/bin/lipo -arch i386 lnsout/$LIBNAME_static.i386 -arch x86_64 lnsout/$LIBNAME_static.x86_64 -create -output lnsout/$LIBNAME_static.osx
/usr/bin/lipo -arch i386 lnsout/$LIBNAME2_static.i386 -arch x86_64 lnsout/$LIBNAME2_static.x86_64 -create -output lnsout/$LIBNAME2_static.osx

mkdir -p ../lib/darwin/ios
mkdir -p ../lib/darwin/osx
cp lnsout/$LIBNAME_static.ios ../lib/darwin/ios/$LIBINSTALL_static
cp lnsout/$LIBNAME2_static.ios ../lib/darwin/ios/$LIBINSTALL2_static
cp lnsout/$LIBNAME_static.osx ../lib/darwin/osx/$LIBINSTALL_static
cp lnsout/$LIBNAME2_static.osx ../lib/darwin/osx/$LIBINSTALL2_static
cd ..
