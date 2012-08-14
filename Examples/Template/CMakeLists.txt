cmake_minimum_required(VERSION 2.8)
project("Template")

set(PH_ENGINE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../Engine")
set(CMAKE_MODULE_PATH "${PH_ENGINE_PATH}/CMakeLib")
set(PH_NAME "Template")
set(PH_SOURCES
  main.cpp
  )
set(PH_IOS_TARGET_IPAD "YES")

include(Porkholt)
