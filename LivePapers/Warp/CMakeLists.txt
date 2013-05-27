cmake_minimum_required(VERSION 2.8)
project(Warp)

set(PH_ENGINE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../Engine")
set(CMAKE_MODULE_PATH "${PH_ENGINE_PATH}/scripts")
include(Porkholt)
set(PH_NAME "Warp")
set(PH_BUNDLE_ID "org.porkholt.livepapers.warp")
set(PH_IOS_TARGET_IPAD "YES")
set(PH_LIVEPAPERS_FPS_STAGES "{30, 30, 30, 0}")
set(PH_LIVEPAPERS_HAS_SETTINGS "YES")
set(PH_DESCRIPTION "Configurable OpenGL Star Trek Warp drive live wallpaper")

set(PH_FILES main.cpp)
if(PH_LIVEPAPERS)
  set(PH_LPPREFS ~/Programare/LivePapers/common/LPPrefs)
  include_directories("${PH_LPPREFS}")
  #set(PH_FILES ${PH_FILES}
  #  ${PH_LPPREFS}/LPPrefsViewController.h
  #  ${PH_LPPREFS}/LPPrefsViewController.mm
  #  ${PH_LPPREFS}/LPPref.h
  #  ${PH_LPPREFS}/LPPref.mm
  #  ${PH_LPPREFS}/LPSliderPref.h
  #  ${PH_LPPREFS}/LPSliderPref.mm
  #  ${PH_LPPREFS}/LPSwitchPref.h
  #  ${PH_LPPREFS}/LPSwitchPref.mm
  #  ${PH_LPPREFS}/LPSegmentPref.h
  #  ${PH_LPPREFS}/LPSegmentPref.mm
  #  ${PH_LPPREFS}/LPDatePref.h
  #  ${PH_LPPREFS}/LPDatePref.mm
  #  ${PH_LPPREFS}/LPColorPref.h
  #  ${PH_LPPREFS}/LPColorPref.mm
  #  ${PH_LPPREFS}/LPButtonPref.h
  #  ${PH_LPPREFS}/LPButtonPref.mm
  #  WPrefsViewController.h
  #  WPrefsViewController.mm
  #  WSettingsLP.mm
  #  )
endif()

porkholt(Warp ${PH_FILES})
