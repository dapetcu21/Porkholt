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
		if [ ! -f lib/darwin/osx/$LIBFILE ]
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

compilelib libz.a zlib ./zlib_compile_darwin.sh $1
compilelib libpng15.a libpng ./libpng_compile_darwin.sh $1
compilelib liblua.a Lua ./liblua_compile_darwin.sh $1