cmake_minimum_required(VERSION 2.8)
project(VideoModes)

set(PH_ENGINE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../Engine")
set(CMAKE_MODULE_PATH "${PH_ENGINE_PATH}/scripts")
set(PH_NAME "Video Modes")
set(PH_IOS_TARGET_IPAD "YES")

include(Porkholt)
porkholt(VideoModes main.cpp)
