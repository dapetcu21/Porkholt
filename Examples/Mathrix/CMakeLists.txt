cmake_minimum_required(VERSION 2.8)
project("Mathrix")

set(PH_ENGINE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../Engine")
set(CMAKE_MODULE_PATH "${PH_ENGINE_PATH}/scripts")
set(PH_NAME "Mathrix")
set(PH_IOS_TARGET_IPAD "YES")
set(PH_ORIENTATION "portrait")

include(Porkholt)
porkholt(Mathrix main.cpp)
