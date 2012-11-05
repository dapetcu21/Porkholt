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
  Classes/IGObject.cpp
  Classes/IGProp.cpp
  Classes/IGImageProp.cpp
  Classes/IGInput.cpp
  Classes/IGDampingProp.cpp
  Classes/IGPlayer.cpp
  Classes/IGWallCell.cpp
  Classes/IGMob.cpp
  Classes/IGBasicMob.cpp
  Classes/IGBulletManager.cpp
  Classes/IGScreenBounds.cpp
  Classes/IGKinematic.cpp
  )
set(PH_HEADERS
  Classes/IGLevelController.h
  Classes/IGScripting.h
  Classes/IGWorld.h
  Classes/IGBackground.h
  Classes/IGObject.h
  Classes/IGProp.h
  Classes/IGImageProp.h
  Classes/IGInput.h
  Classes/IGDampingProp.h
  Classes/IGPlayer.h
  Classes/IGMob.h
  Classes/IGBasicMob.h
  Classes/IGWallCell.h
  Classes/IGBulletManager.h
  Classes/IGScreenBounds.h
  Classes/IGKinematic.h
  )
include_directories(Classes)
set(PH_IOS_TARGET_IPAD "YES")
set(PH_USE_BOX2D "YES")

porkholt(Igor ${PH_SOURCES} ${PH_HEADERS})
