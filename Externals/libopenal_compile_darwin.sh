#!/bin/bash

mkdir -p openal-soft/build && cd openal-soft/build && cmake .. -DCMAKE_OSX_ARCHITECTURES=i386\;x86_64 && make && cp libopenal.dylib ../../lib/darwin/osx/ && install_name_tool -id "@executable_path/../lib/libopenal.dylib" ../../lib/darwin/osx/libopenal.dylib

