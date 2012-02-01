#!/bin/sh
 
# build_fat.sh
#
# Created by Robert Carlsen on 15.07.2009.
# Modified by Marius Petcu
# build an arm / i386 lib of standard linux project
#
# adopted from:
# http://latenitesoft.blogspot.com/2008/10/iphone-programming-tips-building-unix.html
#
# initially configured for tesseract-ocr
 
# Set up the target lib file / path
# easiest to just build the package normally first and watch where the files are created.
cd zlib

LIBFILE=libz

export MACSDK=SDKs/MacOSX10.6.sdk
export IOSSDK=SDKs/iPhoneOS5.0.sdk

# Dynamic library location generated by the Unix package
LIBPATH=$LIBFILE.dylib
LIBNAME=`basename $LIBPATH`
 
# Static library that will be generated for ARM
LIBPATH_static=$LIBFILE.a
LIBNAME_static=`basename $LIBPATH_static`

mkdir -p lnsout

#-------------------------------
 
# Select the desired iPhone SDK
export DEVROOT=/Developer/Platforms/iPhoneOS.platform/Developer
export SDKROOT=$DEVROOT/$IOSSDK
 
# Set up relevant environment variables
export LDFLAGS="-L$SDKROOT/usr/lib/ -Wl,-dylib_install_name,@executable_path/$LIBNAME"
export CXX=$DEVROOT/usr/bin/c++ 
export CPP="$DEVROOT/usr/bin/c++"
export CC=$DEVROOT/usr/bin/cc
export LD=$DEVROOT/usr/bin/ld 

export CFLAGS="-arch armv6 -isysroot $SDKROOT -miphoneos-version-min=2.2 -pipe -no-cpp-precomp"
export CPPFLAGS="$CFLAGS"
export CXXFLAGS="$CFLAGS"

make distclean
 
./configure
make -j4

cp $LIBPATH_static lnsout/$LIBNAME_static.armv6

make distclean

export CFLAGS="-arch armv7 -isysroot $SDKROOT -miphoneos-version-min=2.2 -pipe -no-cpp-precomp"
export CPPFLAGS="$CFLAGS"
export CXXFLAGS="$CFLAGS"
 
./configure
make -j4

cp $LIBPATH_static lnsout/$LIBNAME_static.armv7


make distclean
unset CXX CC LD CPPFLAGS CFLAGS CPP LDFLAGS CXXFLAGS DEVROOT SDKROOT
 
export DEVROOT=/Developer
export SDKROOT=$DEVROOT/$MACSDK

export CFLAGS="-arch i386 -isysroot $SDKROOT -mmacosx-version-min=10.5 -pipe -no-cpp-precomp"
export CPPFLAGS="$CFLAGS"
export CXXFLAGS="$CFLAGS"

./configure
make -j4
 
cp $LIBPATH_static lnsout/$LIBNAME_static.i386

make distclean

export CFLAGS="-arch x86_64 -isysroot $SDKROOT -mmacosx-version-min=10.5 -pipe -no-cpp-precomp"
export CPPFLAGS="$CFLAGS"
export CXXFLAGS="$CFLAGS"

./configure
make -j4

cp $LIBPATH_static lnsout/$LIBNAME_static.x86_64

# Create fat lib by combining the four versions
/usr/bin/lipo -arch armv6 lnsout/$LIBNAME_static.armv6 -arch armv7 lnsout/$LIBNAME_static.armv7 -arch i386 lnsout/$LIBNAME_static.i386 -arch x86_64 lnsout/$LIBNAME_static.x86_64 -create -output lnsout/$LIBNAME_static

mkdir -p ../lib/darwin
cp lnsout/$LIBNAME_static ../lib/darwin/
cd ..

