cmake_minimum_required(VERSION 2.8)

add_custom_target(luajit-clean make clean WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} VERBATIM)
add_custom_target(luajit make "HOST_CC=cc -m32" "CROSS=${ANDROID_TOOLCHAIN_ROOT}/bin/${ANDROID_TOOLCHAIN_MACHINE_NAME}-" "TARGET_FLAGS=${CMAKE_C_FLAGS} -Wl,-soname,libluajit.so " "TARGET_SYS=Android" WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} VERBATIM)
add_dependencies(luajit luajit-clean)
add_custom_target(luajit-install ALL mkdir -p "${CMAKE_BINARY_DIR}/libs/${ANDROID_NDK_ABI_NAME}" COMMAND cp "${CMAKE_SOURCE_DIR}/src/libluajit.so" "${CMAKE_BINARY_DIR}/libs/${ANDROID_NDK_ABI_NAME}/libluajit.so" VERBATIM)
add_dependencies(luajit-install luajit)
