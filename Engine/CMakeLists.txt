cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED PH_PLATFORM)
  if (APPLE)
    set(PH_PLATFORM OSX)
  else(APPLE)
    set(PH_PLATFORM Linux)
  endif(APPLE)
  message("PH_PLATFORM is not defined. Defaulting to \"${PH_PLATFORM}\"")
endif(NOT DEFINED PH_PLATFORM)

if(NOT DEFINED PH_SAME_PROJECT)
  set(PH_SAME_PROJECT "NO")
endif(NOT DEFINED PH_SAME_PROJECT)

if(NOT DEFINED PH_ENGINE_PATH)
  set(PH_ENGINE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
  message("PH_ENGINE_PATH is not defined. Defaulting to \"${PH_ENGINE_PATH}\"")
endif()

IF(NOT DEFINED PH_EXTERNALS)
  set(PH_EXTERNALS "${PH_ENGINE_PATH}/../Externals")
endif(NOT DEFINED PH_EXTERNALS)

include(${PH_ENGINE_PATH}/CMakeLib/Porkholt_${PH_PLATFORM}.cmake)
