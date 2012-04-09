cmake_minimum_required(VERSION 2.8)
project(Template)

set(PH_ENGINE_PATH "../../Engine")
set(PH_NAME "Template")
set(PH_IOS_CODE_SIGN_IDENTITY "iPhone Developer: Marius Petcu")
set(PH_SOURCES
  Bindings/main.cpp
  )

set(PH_SAME_PROJECT, "YES")
include(${PH_ENGINE_PATH}/CMakeLib/Porkholt_All.cmake)
