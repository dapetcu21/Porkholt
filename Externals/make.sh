#!/bin/bash

PLATFORM=$(uname)

if [ ${PLATFORM} == "Darwin" ]; then
    ./make_darwin.sh $@
elif [ ${PLATFORM} == "FreeBSD" ]; then
    ./make_freebsd.sh $@
else
    ./make_linux.sh $@
fi
