cmake_minimum_required(VERSION 2.8)
project(Earth)

set(PH_COMPANY_ID "org.porkholt")
set(PH_ENGINE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../Engine")
set(CMAKE_MODULE_PATH "${PH_ENGINE_PATH}/scripts")
set(PH_NAME "Earth")
set(PH_IOS_TARGET_IPAD "YES")
set(PH_LIVEPAPERS_FPS_STAGES "{60, 0}")

set(PH_FILES main.cpp ESettings.h ESettings.cpp)
if(PH_LIVEPAPERS)
  set(PH_LPPREFS ~/Programare/LivePapers/common/LPPrefs)
  include_directories("${PH_LPPREFS}")
  set(PH_FILES ${PH_FILES}
    ${PH_LPPREFS}/LPPrefsViewController.h
    ${PH_LPPREFS}/LPPrefsViewController.mm
    ${PH_LPPREFS}/LPPref.h
    ${PH_LPPREFS}/LPPref.mm
    ${PH_LPPREFS}/LPSliderPref.h
    ${PH_LPPREFS}/LPSliderPref.mm
    ${PH_LPPREFS}/LPSwitchPref.h
    ${PH_LPPREFS}/LPSwitchPref.mm
    ${PH_LPPREFS}/LPSegmentPref.h
    ${PH_LPPREFS}/LPSegmentPref.mm
    ${PH_LPPREFS}/LPDatePref.h
    ${PH_LPPREFS}/LPDatePref.mm
    ${PH_LPPREFS}/LPColorPref.h
    ${PH_LPPREFS}/LPColorPref.mm
    EPrefsViewController.h
    EPrefsViewController.mm
    ESettingsLP.mm
    )
endif()


include(Porkholt)
porkholt(Earth ${PH_FILES})
