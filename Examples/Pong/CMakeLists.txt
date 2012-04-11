cmake_minimum_required(VERSION 2.8)
project(Pong)

set(PH_ENGINE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../Engine")
set(PH_NAME "Pong")
set(PH_SOURCES
  main.cpp
  )
set(PH_IOS_TARGET_IPHONE "NO")
set(PH_IOS_TARGET_IPAD "YES")

include(${PH_ENGINE_PATH}/CMakeLib/Porkholt_All.cmake)
