#!/bin/sh
 
# build_fat.sh
#
# Created by Robert Carlsen on 15.07.2009.
# Updated 6.12.2009 to build i386 (simulator) on an x86_64 platform (10.6 SDK)
# build an arm / i386 lib of standard linux project
#
# adopted from:
# http://latenitesoft.blogspot.com/2008/10/iphone-programming-tips-building-unix.html
#
# initially configured for tesseract-ocr
 
# Set up the target lib file / path
# easiest to just build the package normally first and watch where the files are created.
cd lua

LIBFILE=src/liblua
 
# Select the desired iPhone SDK
DEVROOT=/Developer/Platforms/iPhoneOS.platform/Developer
SDKROOT=$DEVROOT/SDKs/iPhoneOS4.3.sdk
 
# Set up relevant environment variables
CPPFLAGS="-I$SDKROOT/usr/lib/gcc/arm-apple-darwin10/4.2.1/include/ -I$SDKROOT/usr/include/ -miphoneos-version-min=2.2"
CFLAGS="$CPPFLAGS -pipe -no-cpp-precomp -isysroot $SDKROOT"
CC="$DEVROOT/usr/bin/arm-apple-darwin10-gcc-4.2.1 $CFLAGS"
 
# Dynamic library location generated by the Unix package
LIBPATH=$LIBFILE.dylib
LIBNAME=`basename $LIBPATH`
 
# Static library that will be generated for ARM
LIBPATH_static=$LIBFILE.a
LIBNAME_static=`basename $LIBPATH_static`
 
make macosx CC="${CC}"
 
# Copy the ARM library to a temporary location
mkdir -p lnsout
cp $LIBPATH_static lnsout/$LIBNAME_static.arm

#exit 0 

# Do it all again for native cpu
make clean
make macosx CC="gcc -arch x86_64 -arch i386 -mmacosx-version-min=10.4" 
 
# Copy the native library to the temporary location
cp $LIBPATH_static lnsout/$LIBNAME_static.i386

# Create fat lib by combining the two versions
/usr/bin/lipo -arch arm lnsout/$LIBNAME_static.arm lnsout/$LIBNAME_static.i386 -create -output lnsout/$LIBNAME_static

make local
make clean 

mkdir -p ../lib/darwin
cp lnsout/$LIBNAME_static ../lib/darwin/
cd ..

