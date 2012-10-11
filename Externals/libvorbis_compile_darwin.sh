#!/bin/sh

cd libvorbis

LIBFILE=lib/.libs/libvorbis
LIBPATH_static=$LIBFILE.a
LIBNAME_static=`basename $LIBPATH_static`
LIBINSTALL_static=$LIBNAME_static

LIBFILE2=lib/.libs/libvorbisfile
LIBPATH2_static=$LIBFILE2.a
LIBNAME2_static=`basename $LIBPATH2_static`
LIBINSTALL2_static=$LIBNAME2_static

rm -rf lnsout
mkdir -p lnsout

LIBOGG="$(pwd)/../libogg"

#-------------------------------

export CC="$IOSCC"
export CFLAGS="-arch armv6 -isysroot $IOSSDK -miphoneos-version-min=2.2 -pipe -no-cpp-precomp -O3" 
make distclean 2>&1 > /dev/null
./configure --with-ogg-includes="$LIBOGG/include/" --with-ogg-libraries="$LIBOGG/prebuilt/ios-armv6/" --host=armv6-apple-darwin 
make
cp $LIBPATH_static lnsout/$LIBNAME_static.armv6
cp $LIBPATH2_static lnsout/$LIBNAME2_static.armv6

export CFLAGS="-arch armv7 -isysroot $IOSSDK -miphoneos-version-min=2.2 -pipe -no-cpp-precomp -O3"
make distclean 2>&1 > /dev/null
./configure --with-ogg-includes="$LIBOGG/include" --with-ogg-libraries="$LIBOGG/prebuilt/ios-armv7" --host=armv7-apple-darwin
make
cp $LIBPATH_static lnsout/$LIBNAME_static.armv7
cp $LIBPATH2_static lnsout/$LIBNAME2_static.armv7

#-------------------------------

export CC="$SIMCC"
export CFLAGS="-arch i386 -isysroot $SIMSDK -mmacosx-version-min=10.6 -pipe -no-cpp-precomp -O3"
make distclean 2>&1 > /dev/null
./configure --with-ogg-includes="$LIBOGG/include" --with-ogg-libraries="$LIBOGG/prebuilt/ios-i386" --disable-oggtest
make
cp $LIBPATH_static lnsout/$LIBNAME_static.i386.sim
cp $LIBPATH2_static lnsout/$LIBNAME2_static.i386.sim
 
#-------------------------------

export CC="$OSXCC"
export CFLAGS="-arch i386 -isysroot $OSXSDK -mmacosx-version-min=10.5 -pipe -no-cpp-precomp -O3"
make distclean 2>&1 > /dev/null
./configure --with-ogg-includes="$LIBOGG/include" --with-ogg-libraries="$LIBOGG/prebuilt/osx-i386" --target=i386-apple-darwin --disable-oggtest
make
cp $LIBPATH_static lnsout/$LIBNAME_static.i386
cp $LIBPATH2_static lnsout/$LIBNAME2_static.i386

export CFLAGS="-arch x86_64 -isysroot $OSXSDK -mmacosx-version-min=10.5 -pipe -no-cpp-precomp -O3"
make distclean 2>&1 > /dev/null
./configure --with-ogg-includes="$LIBOGG/include" --with-ogg-libraries="$LIBOGG/prebuilt/osx-x86_64" --target=x86_64-apple-darwin --disable-oggtest
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

rm -rf lnsout
cd ..

