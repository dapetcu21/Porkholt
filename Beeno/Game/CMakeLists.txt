cmake_minimum_required(VERSION 2.8)
project(Beeno)

set(PH_ENGINE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../Engine")
set(CMAKE_MODULE_PATH "${PH_ENGINE_PATH}/CMakeLib")
include(Porkholt)
set(PH_NAME "Beeno Reloaded")
set(PH_SOURCES
  main.cpp
  LevelSystem/PHCaptureView.cpp
  LevelSystem/PHDialogView.cpp
  LevelSystem/PHDistanceJoint.cpp
  LevelSystem/PHGaugeView.cpp
  LevelSystem/PHHeartView.cpp
  LevelSystem/PHJoint.cpp
  LevelSystem/PHLAnimation.cpp
  LevelSystem/PHLAuxLayer.cpp
  LevelSystem/PHLBomberBird.cpp
  LevelSystem/PHLBull.cpp
  LevelSystem/PHLCamera.cpp
  LevelSystem/PHLEggBomb.cpp
  LevelSystem/PHLGround.cpp
  LevelSystem/PHLHPPowerup.cpp
  LevelSystem/PHLLevelEnd.cpp
  LevelSystem/PHLMob.cpp
  LevelSystem/PHLNPC.cpp
  LevelSystem/PHLObject.cpp
  LevelSystem/PHLPit.cpp
  LevelSystem/PHLPlatform.cpp
  LevelSystem/PHLPlayer.cpp
  LevelSystem/PHLPowerPowerup.cpp
  LevelSystem/PHLPowerup.cpp
  LevelSystem/PHLSensor.cpp
  LevelSystem/PHLShieldPowerup.cpp
  LevelSystem/PHLSign.cpp
  LevelSystem/PHLevelController.cpp
  LevelSystem/PHMotorJoint.cpp
  LevelSystem/PHPlayerView.cpp
  LevelSystem/PHPoofView.cpp
  LevelSystem/PHPrismaticJoint.cpp
  LevelSystem/PHRevoluteJoint.cpp
  LevelSystem/PHScriptableTimer.cpp
  LevelSystem/PHScripting.cpp
  LevelSystem/PHShieldView.cpp
  LevelSystem/PHWorld.cpp
  UI/PHChapterController.cpp
  UI/PHMenuController.cpp
  UI/PHTitleScreen.cpp
  )
set(PH_HEADERS
  LevelSystem/PHCaptureView.h
  LevelSystem/PHDialog.h
  LevelSystem/PHDialogView.h
  LevelSystem/PHDistanceJoint.h
  LevelSystem/PHGaugeView.h
  LevelSystem/PHHeartView.h
  LevelSystem/PHJoint.h
  LevelSystem/PHLAnimation.h
  LevelSystem/PHLAuxLayer.h
  LevelSystem/PHLBomberBird.h
  LevelSystem/PHLBull.h
  LevelSystem/PHLCamera.h
  LevelSystem/PHLEggBomb.h
  LevelSystem/PHLGround.h
  LevelSystem/PHLHPPowerup.h
  LevelSystem/PHLLevelEnd.h
  LevelSystem/PHLMob.h
  LevelSystem/PHLNPC.h
  LevelSystem/PHLObject.h
  LevelSystem/PHLPit.h
  LevelSystem/PHLPlatform.h
  LevelSystem/PHLPlayer.h
  LevelSystem/PHLPowerPowerup.h
  LevelSystem/PHLPowerup.h
  LevelSystem/PHLSensor.h
  LevelSystem/PHLShieldPowerup.h
  LevelSystem/PHLSign.h
  LevelSystem/PHLevelController.h
  LevelSystem/PHMotorJoint.h
  LevelSystem/PHPlayerView.h
  LevelSystem/PHPoofView.h
  LevelSystem/PHPrismaticJoint.h
  LevelSystem/PHRevoluteJoint.h
  LevelSystem/PHScriptableTimer.h
  LevelSystem/PHScripting.h
  LevelSystem/PHShieldView.h
  LevelSystem/PHWorld.h
  UI/PHChapterController.h
  UI/PHTitleScreen.h
  UI/PHMenuController.h
  )
include_directories(UI LevelSystem)
set(PH_IOS_TARGET_IPAD "YES")
set(PH_USE_BOX2D "YES")

porkholt(Beeno ${PH_SOURCES} ${PH_HEADERS})
