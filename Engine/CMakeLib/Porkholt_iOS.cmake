project(Porkholt_iOS)

set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_OSX_SYSROOT iphoneos)
set(CMAKE_OSX_ARCHITECTURES "armv6 armv7")

include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLib/Porkholt_Common.cmake)

set(SRCS ${SRCS}
  Bindings/iOS/EAGLView.mm
  Bindings/iOS/PorkholtAppDelegate.mm
  Bindings/iOS/PHStartGame.mm
  Bindings/iOS/PorkholtViewController.mm
  Bindings/iOS/PHTouchInterface.mm
  Sound/PHMusicManager-iOS.mm
  Sound/PHSound-iOS.mm
  Sound/PHSoundManager-iOS.mm
  Sound/PHSoundPool-iOS.cpp
  ${PH_EXTERNALS}/finch/Finch/FIBuffer.m
  ${PH_EXTERNALS}/finch/Finch/FISound.m
  ${PH_EXTERNALS}/finch/Finch/FIErrorReporter.m
  ${PH_EXTERNALS}/finch/Finch/FILogger.m
  ${PH_EXTERNALS}/finch/Finch/FISoundEngine.m
  ${PH_EXTERNALS}/finch/Finch/FIFactory.m
  ${PH_EXTERNALS}/finch/Finch/FIRevolverSound.m
  ${PH_EXTERNALS}/finch/Decoder/FIPCMDecoder.m
  ${PH_EXTERNALS}/finch/Decoder/FISoundSample.m
  )
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/Bindings/iOS
  ${CMAKE_CURRENT_SOURCE_DIR}/Geometry/math
  ${PH_EXTERNALS}/finch/Finch
  ${PH_EXTERNALS}/finch/Decoder
  )

add_library(Porkholt_iOS ${SRCS})
