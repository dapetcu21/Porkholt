#!/bin/sh

cd zlib

LIBFILE=libz
LIBPATH_static=$LIBFILE.a
LIBNAME_static=`basename $LIBPATH_static`
LIBINSTALL_static=$LIBNAME_static

rm -rf lnsout
mkdir -p lnsout

#-------------------------------

export CC="$IOSCC"
export CFLAGS="-arch armv6 -isysroot $IOSSDK -miphoneos-version-min=2.2 -pipe -no-cpp-precomp" 
make distclean 2>&1 > /dev/null
./configure
make
cp $LIBPATH_static lnsout/$LIBNAME_static.armv6

export CFLAGS="-arch armv7 -isysroot $IOSSDK -miphoneos-version-min=2.2 -pipe -no-cpp-precomp"
make distclean 2>&1 > /dev/null
./configure
make
cp $LIBPATH_static lnsout/$LIBNAME_static.armv7

#-------------------------------

export CC="$SIMCC"
export CFLAGS="-arch i386 -isysroot $SIMSDK -mmacosx-version-min=10.6 -pipe -no-cpp-precomp"
make distclean 2>&1 > /dev/null
./configure
make
cp $LIBPATH_static lnsout/$LIBNAME_static.i386.sim
 
#-------------------------------

export CC="$OSXCC"
export CFLAGS="-arch i386 -isysroot $OSXSDK -mmacosx-version-min=10.5 -pipe -no-cpp-precomp"
make distclean 2>&1 > /dev/null
./configure
make
cp $LIBPATH_static lnsout/$LIBNAME_static.i386

export CFLAGS="-arch x86_64 -isysroot $OSXSDK -mmacosx-version-min=10.5 -pipe -no-cpp-precomp"
make distclean 2>&1 > /dev/null
./configure
make
cp $LIBPATH_static lnsout/$LIBNAME_static.x86_64

#-------------------------------

/usr/bin/lipo -arch armv6 lnsout/$LIBNAME_static.armv6 -arch armv7 lnsout/$LIBNAME_static.armv7 -arch i386 lnsout/$LIBNAME_static.i386.sim -create -output lnsout/$LIBNAME_static.ios
/usr/bin/lipo -arch i386 lnsout/$LIBNAME_static.i386 -arch x86_64 lnsout/$LIBNAME_static.x86_64 -create -output lnsout/$LIBNAME_static.osx

mkdir -p ../lib/darwin/ios
mkdir -p ../lib/darwin/osx
cp lnsout/$LIBNAME_static.ios ../lib/darwin/ios/$LIBINSTALL_static
cp lnsout/$LIBNAME_static.osx ../lib/darwin/osx/$LIBINSTALL_static

rm -rf lnsout
cd ..
