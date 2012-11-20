cmake_minimum_required(VERSION 2.8)
project(TexturePicker)

set(PH_ENGINE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../Engine")
set(CMAKE_MODULE_PATH "${PH_ENGINE_PATH}/scripts")
include(Porkholt)
set(PH_NAME "Texture Picker")
set(PH_IOS_TARGET_IPAD "YES")

porkholt(TexturePicker main.cpp)
