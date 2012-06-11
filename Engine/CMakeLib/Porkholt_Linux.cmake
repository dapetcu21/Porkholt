if(NOT PH_SAME_PROJECT)
  project(Porkholt_Linux)
endif()

set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLib/Porkholt_Common.cmake)

set(PH_ENGINE_SRCS ${PH_ENGINE_SRCS}
  ${PH_ENGINE_PATH}/src/Bindings/Linux/PHWindowing.cpp
  ${PH_ENGINE_PATH}/src/Bindings/Linux/PHX11.cpp
  ${PH_ENGINE_PATH}/src/Sound/PHSound-Linux.cpp
  ${PH_ENGINE_PATH}/src/Sound/PHSoundManager-Linux.cpp
  ${PH_ENGINE_PATH}/src/Sound/PHMusicManager-Linux.cpp
  )
  
set(PH_ENGINE_HEADERS ${PH_ENGINE_HEADERS}
  ${PH_ENGINE_PATH}/src/Bindings/Linux/PHX11.h
  )

add_library(Porkholt_Linux ${PH_ENGINE_SRCS} ${PH_ENGINE_HEADERS})
add_custom_target( External_Libs
	COMMAND ${PH_EXTERNALS}/make_linux.sh
	WORKING_DIRECTORY ${PH_EXTERNALS}
	)
add_dependencies(Porkholt_Linux External_Libs)


