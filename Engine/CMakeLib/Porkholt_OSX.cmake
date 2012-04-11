if(NOT PH_SAME_PROJECT)
  project(Porkholt_OSX)
endif(NOT PH_SAME_PROJECT)

set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_OSX_SYSROOT macosx)
set(CMAKE_CXX_FLAGS_RELEASE "-Os")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")

include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLib/Porkholt_Common.cmake)

set(PH_ENGINE_SRCS ${PH_ENGINE_SRCS}
  ${PH_ENGINE_PATH}/Bindings/OSX/PHStartGame.mm
  ${PH_ENGINE_PATH}/Bindings/OSX/PHAppDelegate.mm
  ${PH_ENGINE_PATH}/Bindings/OSX/PHGLView.mm
  ${PH_ENGINE_PATH}/Bindings/OSX/OpenGLTimer.mm
  ${PH_ENGINE_PATH}/Bindings/OSX/PHWindow.mm
  )
  
set(PH_ENGINE_HEADERS ${PH_ENGINE_HEADERS}
  ${PH_ENGINE_PATH}/Bindings/OSX/PHAppDelegate.h
  ${PH_ENGINE_PATH}/Bindings/OSX/PHGLView.h
  ${PH_ENGINE_PATH}/Bindings/OSX/OpenGLTimer.h
  ${PH_ENGINE_PATH}/Bindings/OSX/PHWindow.h
  )

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/Bindings/OSX
  )

add_library(Porkholt_OSX ${PH_ENGINE_SRCS} ${PH_ENGINE_HEADERS})


set_target_properties(Porkholt_OSX PROPERTIES XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS "YES")
set_target_properties(Porkholt_OSX PROPERTIES XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES")
set_target_properties(Porkholt_OSX PROPERTIES XCODE_ATTRIBUTE_GCC_C_LANGUAGE_STANDARD "c99")
