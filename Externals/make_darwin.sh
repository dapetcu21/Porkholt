#!/bin/bash

if [ ! -f lib/darwin/libpng15.a ] 
then
    ./libpng_compile_darwin.sh
fi

