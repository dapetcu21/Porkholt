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

export CC="$IOSCC"
for ARCH in $IOSARCHS; do
    export CFLAGS="-arch $ARCH -isysroot $IOSSDK -miphoneos-version-min=2.2 -pipe -no-cpp-precomp -O3" 
    make distclean 2>&1 > /dev/null
    echo ./configure --with-ogg-includes="$LIBOGG/include/" --with-ogg-libraries="$LIBOGG/prebuilt/ios-$ARCH/" --host=$ARCH-apple-darwin
    ./configure --with-ogg-includes="$LIBOGG/include/" --with-ogg-libraries="$LIBOGG/prebuilt/ios-$ARCH/" --host=$ARCH-apple-darwin && make || exit 1
    cp $LIBPATH_static lnsout/$LIBNAME_static.ios.$ARCH
    cp $LIBPATH2_static lnsout/$LIBNAME2_static.ios.$ARCH
done

export CC="$SIMCC"
for ARCH in $SIMARCHS; do
    export CFLAGS="-arch $ARCH -isysroot $SIMSDK -mmacosx-version-min=10.6 -pipe -no-cpp-precomp -O3"
    make distclean 2>&1 > /dev/null
    ./configure --with-ogg-includes="$LIBOGG/include" --with-ogg-libraries="$LIBOGG/prebuilt/ios-$ARCH" --disable-oggtest && make || exit 1
    cp $LIBPATH_static lnsout/$LIBNAME_static.sim.$ARCH
    cp $LIBPATH2_static lnsout/$LIBNAME2_static.sim.$ARCH
done

export CC="$OSXCC"
for ARCH in $OSXARCHS; do
    export CFLAGS="-arch $ARCH -isysroot $OSXSDK -mmacosx-version-min=10.5 -pipe -no-cpp-precomp -O3"
    make distclean 2>&1 > /dev/null
    ./configure --with-ogg-includes="$LIBOGG/include" --with-ogg-libraries="$LIBOGG/prebuilt/osx-$ARCH" --target=$ARCH-apple-darwin --disable-oggtest && make || exit 1
    cp $LIBPATH_static lnsout/$LIBNAME_static.osx.$ARCH
    cp $LIBPATH2_static lnsout/$LIBNAME2_static.osx.$ARCH
done

$IOSLIPO  lnsout/$LIBNAME_static.ios.*  lnsout/$LIBNAME_static.sim.* -create -output  lnsout/$LIBNAME_static.ios
$IOSLIPO lnsout/$LIBNAME2_static.ios.* lnsout/$LIBNAME2_static.sim.* -create -output lnsout/$LIBNAME2_static.ios
$OSXLIPO  lnsout/$LIBNAME_static.osx.* -create -output  lnsout/$LIBNAME_static.osx
$OSXLIPO lnsout/$LIBNAME2_static.osx.* -create -output lnsout/$LIBNAME2_static.osx
mkdir -p ../lib/darwin/ios
mkdir -p ../lib/darwin/osx
cp  lnsout/$LIBNAME_static.ios  ../lib/darwin/ios/$LIBINSTALL_static
cp lnsout/$LIBNAME2_static.ios ../lib/darwin/ios/$LIBINSTALL2_static
cp  lnsout/$LIBNAME_static.osx  ../lib/darwin/osx/$LIBINSTALL_static
cp lnsout/$LIBNAME2_static.osx ../lib/darwin/osx/$LIBINSTALL2_static

rm -rf lnsout
cd ..

