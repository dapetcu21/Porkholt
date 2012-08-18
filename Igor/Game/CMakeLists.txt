cmake_minimum_required(VERSION 2.8)
project(Igor)

set(PH_ENGINE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../Engine")
set(PH_NAME "Igor")
set(PH_SOURCES
  main.cpp
  Classes/PHGameController.cpp
  Classes/PHGameView.cpp
  Classes/PHPlayerView.cpp
  )
set(PH_HEADERS
  Classes/PHGameController.h
  Classes/PHGameView.h
  Classes/PHPlayerView.h
  )
include_directories(Classes)
set(PH_IOS_TARGET_IPAD "YES")

include(${PH_ENGINE_PATH}/scripts/Porkholt_All.cmake)
