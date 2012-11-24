cmake_minimum_required(VERSION 2.8)
project(Demo)

set(PH_ENGINE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../Engine")
set(CMAKE_MODULE_PATH "${PH_ENGINE_PATH}/scripts")
set(PH_NAME "Demo")
set(PH_BEENO "${PH_ENGINE_PATH}/../Beeno/Game")
set(PH_IGOR "${PH_ENGINE_PATH}/../Igor/Game")

set(PH_SOURCES
  main.cpp
  ${PH_BEENO}/LevelSystem/PHCaptureView.cpp
  ${PH_BEENO}/LevelSystem/PHDialogView.cpp
  ${PH_BEENO}/LevelSystem/PHDistanceJoint.cpp
  ${PH_BEENO}/LevelSystem/PHGaugeView.cpp
  ${PH_BEENO}/LevelSystem/PHHeartView.cpp
  ${PH_BEENO}/LevelSystem/PHJoint.cpp
  ${PH_BEENO}/LevelSystem/PHLAnimation.cpp
  ${PH_BEENO}/LevelSystem/PHLAuxLayer.cpp
  ${PH_BEENO}/LevelSystem/PHLBomberBird.cpp
  ${PH_BEENO}/LevelSystem/PHLBull.cpp
  ${PH_BEENO}/LevelSystem/PHLCamera.cpp
  ${PH_BEENO}/LevelSystem/PHLEggBomb.cpp
  ${PH_BEENO}/LevelSystem/PHLGround.cpp
  ${PH_BEENO}/LevelSystem/PHLHPPowerup.cpp
  ${PH_BEENO}/LevelSystem/PHLLevelEnd.cpp
  ${PH_BEENO}/LevelSystem/PHLMob.cpp
  ${PH_BEENO}/LevelSystem/PHLNPC.cpp
  ${PH_BEENO}/LevelSystem/PHLObject.cpp
  ${PH_BEENO}/LevelSystem/PHLPit.cpp
  ${PH_BEENO}/LevelSystem/PHLPlatform.cpp
  ${PH_BEENO}/LevelSystem/PHLPlayer.cpp
  ${PH_BEENO}/LevelSystem/PHLPowerPowerup.cpp
  ${PH_BEENO}/LevelSystem/PHLPowerup.cpp
  ${PH_BEENO}/LevelSystem/PHLSensor.cpp
  ${PH_BEENO}/LevelSystem/PHLShieldPowerup.cpp
  ${PH_BEENO}/LevelSystem/PHLSign.cpp
  ${PH_BEENO}/LevelSystem/PHLevelController.cpp
  ${PH_BEENO}/LevelSystem/PHMotorJoint.cpp
  ${PH_BEENO}/LevelSystem/PHPlayerView.cpp
  ${PH_BEENO}/LevelSystem/PHPoofView.cpp
  ${PH_BEENO}/LevelSystem/PHPrismaticJoint.cpp
  ${PH_BEENO}/LevelSystem/PHRevoluteJoint.cpp
  ${PH_BEENO}/LevelSystem/PHScriptableTimer.cpp
  ${PH_BEENO}/LevelSystem/PHScripting.cpp
  ${PH_BEENO}/LevelSystem/PHShieldView.cpp
  ${PH_BEENO}/LevelSystem/PHWorld.cpp
  ${PH_BEENO}/UI/PHChapterController.cpp
  ${PH_BEENO}/UI/PHMenuController.cpp
  ${PH_BEENO}/UI/PHTitleScreen.cpp

  ${PH_BEENO}/LevelSystem/PHCaptureView.h
  ${PH_BEENO}/LevelSystem/PHDialog.h
  ${PH_BEENO}/LevelSystem/PHDialogView.h
  ${PH_BEENO}/LevelSystem/PHDistanceJoint.h
  ${PH_BEENO}/LevelSystem/PHGaugeView.h
  ${PH_BEENO}/LevelSystem/PHHeartView.h
  ${PH_BEENO}/LevelSystem/PHJoint.h
  ${PH_BEENO}/LevelSystem/PHLAnimation.h
  ${PH_BEENO}/LevelSystem/PHLAuxLayer.h
  ${PH_BEENO}/LevelSystem/PHLBomberBird.h
  ${PH_BEENO}/LevelSystem/PHLBull.h
  ${PH_BEENO}/LevelSystem/PHLCamera.h
  ${PH_BEENO}/LevelSystem/PHLEggBomb.h
  ${PH_BEENO}/LevelSystem/PHLGround.h
  ${PH_BEENO}/LevelSystem/PHLHPPowerup.h
  ${PH_BEENO}/LevelSystem/PHLLevelEnd.h
  ${PH_BEENO}/LevelSystem/PHLMob.h
  ${PH_BEENO}/LevelSystem/PHLNPC.h
  ${PH_BEENO}/LevelSystem/PHLObject.h
  ${PH_BEENO}/LevelSystem/PHLPit.h
  ${PH_BEENO}/LevelSystem/PHLPlatform.h
  ${PH_BEENO}/LevelSystem/PHLPlayer.h
  ${PH_BEENO}/LevelSystem/PHLPowerPowerup.h
  ${PH_BEENO}/LevelSystem/PHLPowerup.h
  ${PH_BEENO}/LevelSystem/PHLSensor.h
  ${PH_BEENO}/LevelSystem/PHLShieldPowerup.h
  ${PH_BEENO}/LevelSystem/PHLSign.h
  ${PH_BEENO}/LevelSystem/PHLevelController.h
  ${PH_BEENO}/LevelSystem/PHMotorJoint.h
  ${PH_BEENO}/LevelSystem/PHPlayerView.h
  ${PH_BEENO}/LevelSystem/PHPoofView.h
  ${PH_BEENO}/LevelSystem/PHPrismaticJoint.h
  ${PH_BEENO}/LevelSystem/PHRevoluteJoint.h
  ${PH_BEENO}/LevelSystem/PHScriptableTimer.h
  ${PH_BEENO}/LevelSystem/PHScripting.h
  ${PH_BEENO}/LevelSystem/PHShieldView.h
  ${PH_BEENO}/LevelSystem/PHWorld.h
  ${PH_BEENO}/UI/PHChapterController.h
  ${PH_BEENO}/UI/PHTitleScreen.h
  ${PH_BEENO}/UI/PHMenuController.h

  ${PH_IGOR}/Classes/IGLevelController.cpp
  ${PH_IGOR}/Classes/IGScripting.cpp
  ${PH_IGOR}/Classes/IGWorld.cpp
  ${PH_IGOR}/Classes/IGObject.cpp
  ${PH_IGOR}/Classes/IGProp.cpp
  ${PH_IGOR}/Classes/IGImageProp.cpp
  ${PH_IGOR}/Classes/IGInput.cpp
  ${PH_IGOR}/Classes/IGDampingProp.cpp
  ${PH_IGOR}/Classes/IGPlayer.cpp
  ${PH_IGOR}/Classes/IGWallCell.cpp
  ${PH_IGOR}/Classes/IGMob.cpp
  ${PH_IGOR}/Classes/IGBasicMob.cpp
  ${PH_IGOR}/Classes/IGBulletManager.cpp
  ${PH_IGOR}/Classes/IGScreenBounds.cpp
  ${PH_IGOR}/Classes/IGKinematic.cpp
  ${PH_IGOR}/Classes/IGWallManager.cpp
  ${PH_IGOR}/Classes/IGWallView.cpp
  ${PH_IGOR}/Classes/IGWallCurve.cpp
  ${PH_IGOR}/Classes/IGBouncyView.cpp
  ${PH_IGOR}/Classes/IGBouncyButton.cpp
  ${PH_IGOR}/Classes/IGBackgroundParticles.cpp
  ${PH_IGOR}/Classes/IGRipples.cpp
  ${PH_IGOR}/Classes/IGKnifeMob.cpp
  ${PH_IGOR}/Classes/IGStandardBullet.cpp
  ${PH_IGOR}/Classes/IGHomingBullet.cpp
  ${PH_IGOR}/Classes/IGDeadpool.cpp

  ${PH_IGOR}/Classes/IGLevelController.h
  ${PH_IGOR}/Classes/IGScripting.h
  ${PH_IGOR}/Classes/IGWorld.h
  ${PH_IGOR}/Classes/IGObject.h
  ${PH_IGOR}/Classes/IGProp.h
  ${PH_IGOR}/Classes/IGImageProp.h
  ${PH_IGOR}/Classes/IGInput.h
  ${PH_IGOR}/Classes/IGDampingProp.h
  ${PH_IGOR}/Classes/IGPlayer.h
  ${PH_IGOR}/Classes/IGMob.h
  ${PH_IGOR}/Classes/IGBasicMob.h
  ${PH_IGOR}/Classes/IGWallCell.h
  ${PH_IGOR}/Classes/IGBulletManager.h
  ${PH_IGOR}/Classes/IGScreenBounds.h
  ${PH_IGOR}/Classes/IGKinematic.h
  ${PH_IGOR}/Classes/IGWallManager.h
  ${PH_IGOR}/Classes/IGWallView.h
  ${PH_IGOR}/Classes/IGWallCurve.h
  ${PH_IGOR}/Classes/IGBouncyView.h
  ${PH_IGOR}/Classes/IGBouncyButton.h
  ${PH_IGOR}/Classes/IGBackgroundParticles.h
  ${PH_IGOR}/Classes/IGRipples.h
  ${PH_IGOR}/Classes/IGKnifeMob.h
  ${PH_IGOR}/Classes/IGBullet.h
  ${PH_IGOR}/Classes/IGStandardBullet.h
  ${PH_IGOR}/Classes/IGHomingBullet.h
  ${PH_IGOR}/Classes/IGDeadpool.h

  )

set(PH_USE_BOX2D "YES")
set(PH_IOS_TARGET_IPAD "YES")

include_directories(${PH_BEENO}
  ${PH_BEENO}/UI
  ${PH_BEENO}/LevelSystem
  ${PH_IGOR}/Classes
)

include(Porkholt)
porkholt(Demo ${PH_SOURCES})
