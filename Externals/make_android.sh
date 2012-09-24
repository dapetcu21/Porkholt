#!/bin/bash

ANDROID_NDK="$1"
ANDROID_ABI="$2"
ANDROID_NDK_ABI_NAME="$3"

compile_lib() {
    DIR="$1"
    NAME="$2"
    LIBPATH="lib/android/libs/${ANDROID_NDK_ABI_NAME}/lib${NAME}.so"
    if [ ! -f "${LIBPATH}" ]
    then
        echo "Compiling lib${NAME} for Android (${ANDROID_ABI})"
        mkdir -p "${DIR}/build-${ANDROID_ABI}"
        mkdir -p lib/android/libs/${ANDROID_NDK_ABI_NAME}
        cd "${DIR}/build-${ANDROID_ABI}"
        ( cmake .. -DANDROID_NDK="${ANDROID_NDK}" -DCMAKE_TOOLCHAIN_FILE="${OLDPWD}/../Engine/scripts/android.toolchain.cmake" -DANDROID_ABI="${ANDROID_ABI}" && make -j2 && cp libs/${ANDROID_NDK_ABI_NAME}/lib${NAME}.so ${OLDPWD}/lib/android/libs/${ANDROID_NDK_ABI_NAME}/lib${NAME}.so ) || (echo "Could not compile lib${NAME}" && exit 1)
        cd -
    fi
}

compile_lib openal-soft openal
compile_lib lua lua
compile_lib libpng png15
