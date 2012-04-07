cmake_minimum_required(VERSION 2.8)
set(PH_PLATFORM iOS CACHE STRING "The build platform")
set(PH_EXTERNALS "${CMAKE_CURRENT_SOURCE_DIR}/../Externals" CACHE STRING "Externals path")
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLib/Porkholt_${PH_PLATFORM}.cmake)
