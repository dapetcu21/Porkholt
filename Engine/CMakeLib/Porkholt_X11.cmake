if(NOT PH_SAME_PROJECT)
  project(Porkholt_X11)
endif()

set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLib/Porkholt_Common.cmake)

set(PH_ENGINE_SRCS ${PH_ENGINE_SRCS}
  ${PH_ENGINE_PATH}/src/Bindings/X11/PHWindowing.cpp
  ${PH_ENGINE_PATH}/src/Bindings/X11/PHX11.cpp
  )
  
set(PH_ENGINE_HEADERS ${PH_ENGINE_HEADERS}
  ${PH_ENGINE_PATH}/src/Bindings/X11/PHX11.h
  )

add_library(Porkholt_X11 ${PH_ENGINE_SRCS} ${PH_ENGINE_HEADERS})
add_custom_target( External_Libs
	COMMAND ${PH_EXTERNALS}/make_linux.sh
	WORKING_DIRECTORY ${PH_EXTERNALS}
	)
add_dependencies(Porkholt_X11 External_Libs)


