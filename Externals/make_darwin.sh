#!/bin/bash

if [ ! -f lib/darwin/libz.a ]
then
    ./zlib_compile_darwin.sh
fi

if [ ! -f lib/darwin/libpng15.a ] 
then
    ./libpng_compile_darwin.sh
fi

if [ ! -f lib/darwin/liblua.a ]
then
    ./liblua_compile_darwin.sh
fi
