cmake_minimum_required(VERSION 2.8)
project(Template)

set(PH_ENGINE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../Engine")
set(PH_NAME "Template")
set(PH_SOURCES
  main.cpp
  )

include(${PH_ENGINE_PATH}/CMakeLib/Porkholt_All.cmake)