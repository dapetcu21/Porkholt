#!/bin/sh

cd libpng

LIBPATH_static=.libs/libpng15.a
LIBINSTALL_static=libpng15.a
LIBNAME_static=libpng15.a

mkdir -p lnsout

export LDFLAGS="-L../lib/darwin/ios"
for ARCH in $IOSARCHS; do
    CFLAGS="-arch $ARCH -isysroot $IOSSDK -miphoneos-version-min=4.2 -pipe -no-cpp-precomp"
    CFLAGS="-I../zlib $CFLAGS"
    make distclean 2>&1 > /dev/null
    ./configure CC="$IOSCC" CFLAGS="$CFLAGS" CPPFLAGS="$CFLAGS" --host=arm-apple-darwin && make || exit 1
    cp $LIBPATH_static lnsout/$LIBNAME_static.ios.$ARCH
done

for ARCH in $SIMARCHS; do
    CFLAGS="-arch $ARCH -isysroot $SIMSDK -miphoneos-version-min=4.2 -pipe -no-cpp-precomp"
    CFLAGS="-I../zlib $CFLAGS"
    make distclean 2>&1 > /dev/null
    ./configure CC="$SIMCC" CFLAGS="$CFLAGS" CPPFLAGS="$CFLAGS" && make || exit 1
    cp $LIBPATH_static lnsout/$LIBNAME_static.sim.$ARCH
done

export LDFLAGS="-L../lib/darwin/osx"
for ARCH in $OSXARCHS; do
    CFLAGS="-arch $ARCH -isysroot $OSXSDK -mmacosx-version-min=10.5 -pipe -no-cpp-precomp"
    CFLAGS="-I../zlib $CFLAGS"
    make distclean 2>&1 > /dev/null
    ./configure CC="$OSXCC" CFLAGS="$CFLAGS" CPPFLAGS="$CFLAGS" && make || exit 1
    cp $LIBPATH_static lnsout/$LIBNAME_static.osx.$ARCH
done

$IOSLIPO lnsout/$LIBNAME_static.ios.* lnsout/$LIBNAME_static.sim.* -create -output lnsout/$LIBNAME_static.ios
$OSXLIPO lnsout/$LIBNAME_static.osx.* -create -output lnsout/$LIBNAME_static.osx
mkdir -p ../lib/darwin/ios
mkdir -p ../lib/darwin/osx
cp lnsout/$LIBNAME_static.ios ../lib/darwin/ios/$LIBINSTALL_static
cp lnsout/$LIBNAME_static.osx ../lib/darwin/osx/$LIBINSTALL_static

unset LDFLAGS
rm -rf lnsout
cd ..
