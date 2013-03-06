cmake_minimum_required(VERSION 2.8)
project(Electric)

set(PH_ENGINE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../Engine")
set(CMAKE_MODULE_PATH "${PH_ENGINE_PATH}/scripts")
include(Porkholt)
set(PH_NAME "Electric")
set(PH_IOS_TARGET_IPAD "YES")
set(PH_LIVEPAPERS_FPS_STAGES "{30, 30, 30, 0}")

porkholt(Electric main.cpp)
