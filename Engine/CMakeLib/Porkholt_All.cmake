if(NOT DEFINED ${PH_PLATFORM})
	message("PH_PLATFORM is not defined. Defaulting to \"iOS\"")
	set(PH_PLATFORM iOS)
endif(NOT DEFINED ${PH_PLATFORM})
set(PH_SAME_PROJECT "NO" CACHE STRING "Compile the engine in the same project")
if(PH_SAME_PROJECT)
  include("${PH_ENGINE_PATH}/CMakeLists.txt")
else(PH_SAME_PROJECT)
  add_subdirectory(${PH_ENGINE_PATH} ${PH_ENGINE_PATH})
endif(PH_SAME_PROJECT)
include("${PH_ENGINE_PATH}/CMakeLib/App_${PH_PLATFORM}.cmake")
