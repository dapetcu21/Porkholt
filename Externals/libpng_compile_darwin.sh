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
cd libpng

LIBFILE=libpng
 
export DEVROOT=/Developer/Platforms/iPhoneOS.platform/Developer
export SDKROOT=$DEVROOT/SDKs/iPhoneOS4.3.sdk
 
LIBPATH=$LIBFILE.dylib
LIBNAME=`basename $LIBPATH`
 
export LDFLAGS="-L$SDKROOT/usr/lib/ -Wl,-dylib_install_name,@executable_path/$LIBNAME"
 
LIBPATH_static=$LIBFILE.a
LIBNAME_static=`basename $LIBPATH_static`

make -f scripts/makefile.darwin clean 
make -f scripts/makefile.darwin CFLAGS="-I$SDKROOT/usr/lib/gcc/arm-apple-darwin10/4.2.1/include/ -I$SDKROOT/usr/include/ -miphoneos-version-min=2.2 -pipe -no-cpp-precomp" CC=$DEVROOT/usr/bin/arm-apple-darwin10-gcc-4.2.1 ARCH="-arch arm -isysroot $SDKROOT"
mkdir -p lnsout
cp $LIBPATH_static lnsout/$LIBNAME_static.arm
make -f scripts/makefile.darwin clean
unset CPPFLAGS CFLAGS CPP LDFLAGS CXXFLAGS DEVROOT SDKROOT
 
export DEVROOT=/Developer
export SDKROOT=$DEVROOT/SDKs/MacOSX10.6.sdk
make -f scripts/makefile.darwin CFLAGS="-pipe -no-cpp-precomp -isysroot $SDKROOT -arch i386" ARCH="-arch i386 -isysroot $SDKROOT"
cp $LIBPATH_static lnsout/$LIBNAME_static.i386
make -f scripts/makefile.darwin clean
unset CPPFLAGS CFLAGS CPP LDFLAGS CPP CXXFLAGS DEVROOT SDKROOT

export DEVROOT=/Developer
export SDKROOT=$DEVROOT/SDKs/MacOSX10.6.sdk
make -f scripts/makefile.darwin CFLAGS="$CPPFLAGS -pipe -no-cpp-precomp -isysroot $SDKROOT -arch x86_64" ARCH="-arch x86_64 -isysroot $SDKROOT"
cp $LIBPATH_static lnsout/$LIBNAME_static.x86_64
unset CPPFLAGS CFLAGS CPP LDFLAGS CPP CXXFLAGS DEVROOT SDKROOT

/usr/bin/lipo -arch arm lnsout/$LIBNAME_static.arm -arch i386 lnsout/$LIBNAME_static.i386 -arch x86_64 lnsout/$LIBNAME_static.x86_64 -create -output lnsout/$LIBNAME_static

mkdir -p ../lib/darwin
cp lnsout/$LIBNAME_static ../lib/darwin/libpng15.a
cd ..

