#!/bin/bash

compilelib() {
	LIBFILE=$1
	LIBNAME=$2
	BUILDSCRIPT=$3
	if [ ! -f lib/freebsd/$LIBFILE ]
	then
		echo "Compiling $LIBNAME..."
		mkdir -p log
		if [ "$4" == "-v" ]
		then
			$BUILDSCRIPT
		else
			$BUILDSCRIPT > log/$LIBFILE.stdout.log 2> log/$LIBFILE.stderr.log
		fi
		if [ ! -f lib/freebsd/$LIBFILE ]
		then
			echo "Could not compile $LIBNAME for FreeBSD" 1>&2
			exit 1
		else	
			if [ ! "$4" == "-d" ] && [ ! "$4" == "-v" ]
			then
				rm log/$LIBFILE.stdout.log log/$LIBFILE.stderr.log
			fi
			echo "Compiled $LIBNAME for FreeBSD"
		fi
	fi
}

compilelib libz.a zlib ./zlib_compile_freebsd.sh $1
compilelib libpng15.a libpng ./libpng_compile_freebsd.sh $1
compilelib libluajit.a LuaJIT ./libluajit_compile_freebsd.sh $1
compilelib libogg.a Ogg ./libogg_compile_freebsd.sh $1
compilelib libvorbis.a Vorbis ./libvorbis_compile_freebsd.sh $1
