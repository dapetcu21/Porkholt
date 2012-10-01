cmake_minimum_required(VERSION 2.6)

project(libogg C)

set(libogg_SRCS src/bitwise.c src/framing.c)
set(libogg_HDRS include/ogg/ogg.h include/ogg/os_types.h)

set(SIZE16 int16_t)
set(USIZE16 uint16_t)
set(SIZE32 int32_t)
set(USIZE32 uint32_t)
set(SIZE64 int64_t)
set(INCLUDE_INTTYPES_H 0)
set(INCLUDE_STDINT_H 1)
set(INCLUDE_SYS_TYPES_H 0)
configure_file(include/ogg/config_types.h.in ${CMAKE_CURRENT_BINARY_DIR}/inc/ogg/config_types.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/inc include)

add_library(ogg SHARED ${libogg_SRCS} ${libogg_HDRS})

