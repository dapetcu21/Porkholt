cmake_minimum_required(VERSION 2.8)
project(Particles)

set(PH_ENGINE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../Engine")
set(CMAKE_MODULE_PATH "${PH_ENGINE_PATH}/scripts")
include(Porkholt)
set(PH_NAME "Particles")
set(PH_IOS_TARGET_IPAD "YES")

porkholt(Particles main.cpp)
