if(NOT PH_SAME_PROJECT)
  project(Porkholt_Linux)
endif(NOT PH_SAME_PROJECT)

set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_OSX_SYSROOT macosx)
set(CMAKE_CXX_FLAGS_RELEASE "-Os")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")

include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLib/Porkholt_Common.cmake)

set(PH_ENGINE_SRCS ${PH_ENGINE_SRCS}
  ${PH_ENGINE_PATH}/Bindings/Linux/PHStartGame.cpp
  ${PH_ENGINE_PATH}/Sound/PHSound-Linux.cpp
  ${PH_ENGINE_PATH}/Sound/PHSoundManager-Linux.cpp
  ${PH_ENGINE_PATH}/Sound/PHMusicManager-Linux.cpp
  )
  
set(PH_ENGINE_HEADERS ${PH_ENGINE_HEADERS}
#  ${PH_ENGINE_PATH}/Bindings/OSX/PHAppDelegate.h
#  ${PH_ENGINE_PATH}/Bindings/OSX/PHGLView.h
#  ${PH_ENGINE_PATH}/Bindings/OSX/OpenGLTimer.h
#  ${PH_ENGINE_PATH}/Bindings/OSX/PHWindow.h
  )

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/Bindings/Linux
  )

add_library(Porkholt_Linux ${PH_ENGINE_SRCS} ${PH_ENGINE_HEADERS})
add_custom_target( External_Libs
	COMMAND ${PH_EXTERNALS}/make_linux.sh
	WORKING_DIRECTORY ${PH_EXTERNALS}
	)
add_dependencies(Porkholt_Linux External_Libs)

