cmake_minimum_required(VERSION 2.8)
project(Igor)

set(PH_ENGINE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../Engine")
set(CMAKE_MODULE_PATH "${PH_ENGINE_PATH}/scripts")
include(Porkholt)

set(PH_NAME "Igor")
set(PH_SOURCES
  main.cpp
  Classes/IGLevelController.cpp
  Classes/IGScripting.cpp
  Classes/IGWorld.cpp
  Classes/IGBackground.cpp
  )
set(PH_HEADERS
  Classes/IGLevelController.h
  Classes/IGScripting.h
  Classes/IGWorld.h
  Classes/IGBackground.h
  )
include_directories(Classes)
set(PH_IOS_TARGET_IPAD "YES")

porkholt(Igor ${PH_SOURCES} ${PH_HEADERS})
