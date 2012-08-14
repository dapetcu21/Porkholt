if(NOT DEFINED PH_PLATFORM)
  if (APPLE)
    set(PH_PLATFORM OSX)
  else()
    set(PH_PLATFORM X11)
  endif()
  message("PH_PLATFORM is not defined. Defaulting to \"${PH_PLATFORM}\"")
endif()

if(NOT DEFINED PH_ENGINE_PATH)
  set(PH_ENGINE_PATH ".")
  message("PH_ENGINE_PATH is not defined. Defaulting to \".\"")
endif()

IF(NOT DEFINED PH_EXTERNALS)
  set(PH_EXTERNALS "${PH_ENGINE_PATH}/../Externals")
endif()

add_subdirectory(${PH_ENGINE_PATH} ${CMAKE_CURRENT_BINARY_DIR}/engine-build)

include("${PH_ENGINE_PATH}/CMakeLib/App_${PH_PLATFORM}.cmake")
