if(NOT DEFINED PH_PLATFORM)
	message("PH_PLATFORM is not defined. Defaulting to \"iOS\"")
	set(PH_PLATFORM iOS)
endif(NOT DEFINED PH_PLATFORM)

if(NOT DEFINED PH_SAME_PROJECT)
  set(PH_SAME_PROJECT "NO")
endif(NOT DEFINED PH_SAME_PROJECT)

if(NOT DEFINED PH_ENGINE_PATH)
  set(PH_ENGINE_PATH ".")
  message("PH_ENGINE_PATH is not defined. Defaulting to \".\"")
endif()

IF(NOT DEFINED PH_EXTERNALS)
  set(PH_EXTERNALS "${PH_ENGINE_PATH}/../Externals")
endif(NOT DEFINED PH_EXTERNALS)

if(PH_SAME_PROJECT)
  include("${PH_ENGINE_PATH}/CMakeLib/Porkholt_${PH_PLATFORM}.cmake")
else(PH_SAME_PROJECT)
  add_subdirectory(${PH_ENGINE_PATH} ${PH_ENGINE_PATH}/build)
endif(PH_SAME_PROJECT)

include("${PH_ENGINE_PATH}/CMakeLib/App_${PH_PLATFORM}.cmake")
