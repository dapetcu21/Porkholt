#!/bin/bash

LINKER=$1

for ARG in ${@:2:$#} ; do
    if [ -z "$NEXT_ARCH" ]; then 
        if [ "$ARG" == "-arch" ]; then
            NEXT_ARCH="Yes"
        else
            if [ -z "$NEXT_OUT" ]; then
                if [ "$ARG" == "-o" ]; then
                    NEXT_OUT="Yes"
                else
                    ARGS="$ARGS $ARG"
                fi
            else
                OUT="$ARG"
                NEXT_OUT=""
            fi
        fi
    else
        ARCHS="$ARCHS $ARG"
        NEXT_ARCH=""
    fi
done

for ARCH in $ARCHS ; do
    $LINKER -arch $ARCH $ARGS -o ${OUT}.$ARCH
    OUTS=$OUTS
done

