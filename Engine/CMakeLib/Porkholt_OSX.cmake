if(NOT PH_SAME_PROJECT)
  project(Porkholt_OSX)
endif(NOT PH_SAME_PROJECT)

set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_OSX_SYSROOT macosx)
set(CMAKE_OSX_ARCHITECTURES "i386 x86_64")

include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLib/Porkholt_Common.cmake)

add_library(Porkholt_OSX ${PH_ENGINE_SRCS})
