if(NOT PH_SAME_PROJECT)
  project(Porkholt_iOS)
endif()

set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_OSX_SYSROOT iphoneos)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_OSX_ARCHITECTURES "armv6 armv7")

include(${PH_ENGINE_PATH}/CMakeLib/Porkholt_Common.cmake)

set(PH_ENGINE_SRCS ${PH_ENGINE_SRCS}
  ${PH_ENGINE_PATH}/src/Bindings/iOS/EAGLView.mm
  ${PH_ENGINE_PATH}/src/Bindings/iOS/PorkholtAppDelegate.mm
  ${PH_ENGINE_PATH}/src/Bindings/iOS/PHStartGame.mm
  ${PH_ENGINE_PATH}/src/Bindings/iOS/PorkholtViewController.mm
  ${PH_ENGINE_PATH}/src/Bindings/iOS/PHTouchInterface.mm
  )
set(PH_ENGINE_HEADERS ${PH_ENGINE_HEADERS}
  ${PH_ENGINE_PATH}/src/Bindings/iOS/EAGLView.h
  ${PH_ENGINE_PATH}/src/Bindings/iOS/PorkholtAppDelegate.h
  ${PH_ENGINE_PATH}/src/Bindings/iOS/PorkholtViewController.h
  ${PH_ENGINE_PATH}/src/Bindings/iOS/PHTouchInterface.h
  )
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Geometry/math
  )

add_library(Porkholt_iOS ${PH_ENGINE_SRCS} ${PH_ENGINE_HEADERS})
set_target_properties(Porkholt_iOS PROPERTIES XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvmgcc42")
set_target_properties(Porkholt_iOS PROPERTIES XCODE_ATTRIBUTE_GCC_THUMB_SUPPORT "NO")
set_target_properties(Porkholt_iOS PROPERTIES XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS "YES")
set_target_properties(Porkholt_iOS PROPERTIES XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES")
set_target_properties(Porkholt_iOS PROPERTIES XCODE_ATTRIBUTE_GCC_C_LANGUAGE_STANDARD "c99")

add_custom_command( TARGET Porkholt_iOS 
	PRE_BUILD
	COMMAND ${PH_EXTERNALS}/make_darwin.sh
	WORKING_DIRECTORY ${PH_EXTERNALS}
	)
