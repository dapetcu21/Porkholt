#!/bin/sh

cd libpng

LIBPATH_static=.libs/libpng15.a
LIBINSTALL_static=libpng15.a
LIBNAME_static=libpng15.a

mkdir -p lnsout

#-------------------------------

export LDFLAGS="-L../lib/darwin/ios"

CFLAGS="-arch armv6 -isysroot $IOSSDK -miphoneos-version-min=2.2 -pipe -no-cpp-precomp" 
make distclean 2>&1 > /dev/null
./configure CC="$IOSCC" CFLAGS="-I../zlib $CFLAGS" --host=armv6-apple-darwin 
make 
cp $LIBPATH_static lnsout/$LIBNAME_static.armv6

CFLAGS="-arch armv7 -isysroot $IOSSDK -miphoneos-version-min=2.2 -pipe -no-cpp-precomp"
make distclean 2>&1 > /dev/null
./configure CC="$IOSCC" CFLAGS="-I../zlib $CFLAGS" --host=armv7-apple-darwin
make 
cp $LIBPATH_static lnsout/$LIBNAME_static.armv7

#-------------------------------

CFLAGS="-arch i386 -isysroot $SIMSDK -mmacosx-version-min=10.6 -pipe -no-cpp-precomp"
make distclean 2>&1 > /dev/null
./configure CC="$SIMCC" CFLAGS="-I../zlib $CFLAGS"
make 
cp $LIBPATH_static lnsout/$LIBNAME_static.i386.sim
 
#-------------------------------

export LDFLAGS="-L../lib/darwin/osx"

CFLAGS="-arch i386 -isysroot $OSXSDK -mmacosx-version-min=10.5 -pipe -no-cpp-precomp"
make distclean 2>&1 > /dev/null
./configure CC="$OSXCC" CFLAGS="-I../zlib $CFLAGS"
make
cp $LIBPATH_static lnsout/$LIBNAME_static.i386

CFLAGS="-arch x86_64 -isysroot $OSXSDK -mmacosx-version-min=10.5 -pipe -no-cpp-precomp"
make distclean 2>&1 > /dev/null
./configure CC="$OSXCC" CFLAGS="-I../zlib $CFLAGS"
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
rm -rf lnsout
cd ..
