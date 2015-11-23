#!/bin/bash

compilelib() {
	LIBFILE=$1
	LIBNAME=$2
	BUILDSCRIPT=$3
	if [ ! -f lib/darwin/ios/$LIBFILE ] || [ ! -f lib/darwin/osx/$LIBFILE ]
	then
		echo "Compiling $LIBNAME..."
		mkdir -p log
		if [ "$4" == "-v" ]
		then
			$BUILDSCRIPT
		else
			$BUILDSCRIPT > log/$LIBFILE.stdout.log 2> log/$LIBFILE.stderr.log
		fi
		if [ ! -f lib/darwin/ios/$LIBFILE ]
		then
			echo "Could not compile $LIBNAME for iOS" 1>&2
			exit 1
	    else
		if [ ! -f lib/darwin/osx/$LIBFILE ]
		then
			echo "Could not compile $LIBNAME for Mac OS X" 1>&2
			exit 1
		else
			if [ ! "$4" == "-d" ] && [ ! "$4" == "-v" ]
			then
				rm log/$LIBFILE.stdout.log log/$LIBFILE.stderr.log
			fi
			echo "Compiled $LIBNAME for iOS and Mac OS X"
		fi
		fi
	fi
}

compilelibosx() {
    LIBFILE=$1
	LIBNAME=$2
	BUILDSCRIPT=$3
	if [ ! -f lib/darwin/osx/$LIBFILE ]
	then
		echo "Compiling $LIBNAME..."
		mkdir -p log
		if [ "$4" == "-v" ]
		then
			$BUILDSCRIPT
		else
			$BUILDSCRIPT > log/$LIBFILE.stdout.log 2> log/$LIBFILE.stderr.log
		fi
		if [ ! -f lib/darwin/osx/$LIBFILE ]
		then
			echo "Could not compile $LIBNAME for Mac OS X" 1>&2
			exit 1
		else
			if [ ! "$4" == "-d" ] && [ ! "$4" == "-v" ]
			then
				rm log/$LIBFILE.stdout.log log/$LIBFILE.stderr.log
			fi
			echo "Compiled $LIBNAME for Mac OS X"
		fi
	fi
}

if [ -e /Developer/usr/bin/make ]; then
    DEVEL=/Developer
else
    DEVEL=/Applications/Xcode.app/Contents/Developer
    XCAPP=1
fi

OSXSDKS="$DEVEL/SDKs"
IOSSDKS="$DEVEL/Platforms/iPhoneOS.platform/Developer/SDKs"
SIMSDKS="$DEVEL/Platforms/iPhoneSimulator.platform/Developer/SDKs"
if [ ! -d "$OSXSDKS" ]; then
    OSXSDKS="$DEVEL/Platforms/MacOSX.platform/Developer/SDKs"
fi

export OSXCC=cc
export OSXCPP=c++
export OSXLIPO=lipo

if [ "$XCAPP" == "1" ]; then
    export IOSCC=$(xcrun -sdk iphoneos -find cc)
    export IOSCPP=$(xcrun -sdk iphoneos -find c++)
    export SIMCC=$(xcrun -sdk iphonesimulator -find cc)
    export SIMCPP=$(xcrun -sdk iphonesimulator -find c++)
    export IOSLIPO=$(xcrun -sdk iphoneos -find lipo)
    export SIMLIPO=$(xcrun -sdk iphoneos -find lipo)
else
    #iOS compilers

    IOSDIR="$DEVEL/Platforms/iPhoneOS.platform/Developer/usr/bin"
    export IOSCC="$IOSDIR/cc"
    if [ ! -f "$IOSCC" ] ; then
        export IOSCC="$IOSDIR/clang"
    if [ ! -f "$IOSCC" ] ; then
        export IOSCC="$IOSDIR/gcc"
    fi
    fi

    export IOSCPP="$IOSDIR/c++"
    if [ ! -f "$IOSCPP" ] ; then
        export IOSCPP="$IOSDIR/clang"
    if [ ! -f "$IOSCPP" ] ; then
        export IOSCPP="$IOSDIR/g++"
    fi
    fi

    export IOSLIPO="$IOSDIR/lipo"
    if [ ! -f "$IOSLIPO" ] ; then
        export IOSLIPO=lipo
    fi

    #Simulator compilers

    SIMDIR="$DEVEL/Platforms/iPhoneSimulator.platform/Developer/usr/bin"
    export SIMCC="$SIMDIR/cc"
    if [ ! -f "$SIMCC" ] ; then
        export SIMCC="$SIMDIR/clang"
    if [ ! -f "$SIMCC" ] ; then
        export SIMCC="$SIMDIR/gcc"
    if [ ! -f "$SIMCC" ] ; then
        export SIMCC="$DEVEL/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang"
    fi
    fi
    fi

    export SIMCPP="$SIMDIR/c++"
    if [ ! -f "$SIMCPP" ] ; then
        export SIMCPP="$SIMDIR/clang"
    if [ ! -f "$SIMCPP" ] ; then
        export SIMCPP="$SIMDIR/g++"
    if [ ! -f "$SIMCPP" ] ; then
        export SIMCPP="$DEVEL/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang"
    fi
    fi
    fi

    export SIMLIPO=lipo
fi

latestsdk() {
    echo "$1/$(ls "$1" | sort | tail -n 1)"
}

archsof() {
		if [[ -f $1/usr/lib/libSystem.tbd ]]; then
				cat $1/usr/lib/libSystem.tbd | grep archs | tail -n 1 | sed 's/^.*\[[ \t]*//' | sed 's/[ \t\]]*$//' | sed 's/,//g'
		else
		    $2 -info $1/usr/lib/libSystem.dylib | sed s/Architectures\ in\ the\ fat\ file.*\ are:\ // | sed s/Non-fat\ file:.*\ is\ architecture:\ //
		fi
}

export OSXSDK=$(latestsdk "$OSXSDKS")
export IOSSDK=$(latestsdk "$IOSSDKS")
export SIMSDK=$(latestsdk "$SIMSDKS")

export IOSARCHS=$(archsof "$IOSSDK" "$IOSLIPO" | sed 's/arm64//')
export SIMARCHS=$(archsof "$SIMSDK" "$SIMLIPO" )
export OSXARCHS=$(archsof "$OSXSDK" "$OSXLIPO" )

compilelib libz.a zlib ./zlib_compile_darwin.sh $1
compilelib libpng15.a libpng ./libpng_compile_darwin.sh $1
compilelib libluajit.a LuaJIT ./libluajit_compile_darwin.sh $1
compilelib libogg.a Ogg ./libogg_compile_darwin.sh $1
compilelib libvorbis.a Vorbis ./libvorbis_compile_darwin.sh $1
compilelibosx libopenal.dylib OpenAL ./libopenal_compile_darwin.sh $1
