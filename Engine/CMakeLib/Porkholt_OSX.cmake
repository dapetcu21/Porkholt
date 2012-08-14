
if(NOT PH_SAME_PROJECT)
  project(Porkholt_OSX)
endif()

set(CMAKE_CONFIGURATION_TYPES Debug Release)
if(CMAKE_GENERATOR STREQUAL "Xcode")
	set(CMAKE_OSX_SYSROOT macosx)
endif()

set(CMAKE_OSX_ARCHITECTURES i386;x86_64)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLib/Porkholt_Common.cmake)

set(PH_ENGINE_SRCS ${PH_ENGINE_SRCS}
  ${PH_ENGINE_PATH}/src/Bindings/OSX/PHWindowing.mm
  ${PH_ENGINE_PATH}/src/Bindings/OSX/PHAppDelegate.mm
  ${PH_ENGINE_PATH}/src/Bindings/OSX/PHGLView.mm
  ${PH_ENGINE_PATH}/src/Bindings/OSX/PHWindow.mm
  )
  
set(PH_ENGINE_HEADERS ${PH_ENGINE_HEADERS}
  ${PH_ENGINE_PATH}/include/Porkholt/Bindings/OSX/PHAppDelegate.h
  ${PH_ENGINE_PATH}/include/Porkholt/Bindings/OSX/PHGLView.h
  ${PH_ENGINE_PATH}/include/Porkholt/Bindings/OSX/PHWindow.h
  )

add_library(Porkholt_OSX ${PH_ENGINE_SRCS} ${PH_ENGINE_HEADERS})

set_target_properties(Porkholt_OSX PROPERTIES XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS "YES")
set_target_properties(Porkholt_OSX PROPERTIES XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES")
set_target_properties(Porkholt_OSX PROPERTIES XCODE_ATTRIBUTE_GCC_C_LANGUAGE_STANDARD "c99")

add_custom_target( External_Libs
	COMMAND ${PH_EXTERNALS}/make_darwin.sh
	WORKING_DIRECTORY ${PH_EXTERNALS}
	)
add_dependencies(Porkholt_OSX External_Libs)

include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLib/cotire.cmake)
cotire(Porkholt_OSX)
