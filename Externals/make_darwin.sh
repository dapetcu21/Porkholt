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

if [ -d /Developer ]; then
    DEVEL=/Developer
else
    DEVEL=/Applications/XCode.app/Contents/Developer
fi

OSXSDKS="$DEVEL/SDKs"
IOSSDKS="$DEVEL/Platforms/iPhoneOS.platform/Developer/SDKs"
SIMSDKS="$DEVEL/Platforms/iPhoneSimulator.platform/Developer/SDKs"
if [ ! -d "$OSXSDKS" ]; then
    OSXSDKS="$DEVEL/Platforms/MacOSX.platform/Developer/SDKs"
fi

export OSXCC=cc
export OSXCPP=c++
export IOSCC="$DEVEL/Platforms/iPhoneOS.platform/Developer/usr/bin/cc"
export IOSCPP="$DEVEL/Platforms/iPhoneOS.platform/Developer/usr/bin/c++"
export SIMCC="$DEVEL/Platforms/iPhoneSimulator.platform/Developer/usr/bin/cc"
export SIMCPP="$DEVEL/Platforms/iPhoneSimulator.platform/Developer/usr/bin/c++"
if [ ! -d "$SIMCC" ]; then
    export SIMCC="$DEVEL/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang"
    export SIMCPP="$DEVEL/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang"
fi

latestsdk() {
    echo "$1/$(ls "$1" | sort | tail -n 1)"
}

export OSXSDK=$(latestsdk "$OSXSDKS")
export IOSSDK=$(latestsdk "$IOSSDKS")
export SIMSDK=$(latestsdk "$SIMSDKS")

compilelib libz.a zlib ./zlib_compile_darwin.sh $1
compilelib libpng15.a libpng ./libpng_compile_darwin.sh $1
compilelib liblua.a Lua ./liblua_compile_darwin.sh $1
compilelib libogg.a Ogg ./libogg_compile_darwin.sh $1
compilelib libvorbis.a Vorbis ./libvorbis_compile_darwin.sh $1
compilelibosx libopenal.dylib OpenAL ./libopenal_compile_darwin.sh $1

