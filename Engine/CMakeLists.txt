cmake_minimum_required(VERSION 2.8)
project(Porkholt)

set(PH_PLATFORM_DOC "Target platform. Can be one of X11 OSX iOS Android")
if (APPLE)
    set(PH_PLATFORM OSX CACHE STRING ${PH_PLATFORM_DOC})
else()
    set(PH_PLATFORM X11 CACHE STRING ${PH_PLATFORM_DOC})
endif()

set(PH_ENGINE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "Path to the Engine folder")
set(PH_EXTERNALS "${PH_ENGINE_PATH}/../Externals" CACHE PATH "Path to the Externals folder")

set(CMAKE_CONFIGURATION_TYPES Debug Release)
if(CMAKE_GENERATOR STREQUAL "Xcode")
    if    (PH_PLATFORM STREQUAL "OSX")
        set(CMAKE_OSX_SYSROOT macosx)
    elseif(PH_PLATFORM STREQUAL "iOS")
        set(CMAKE_OSX_SYSROOT iphoneos)
        set(CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphoneos;-iphonesimulator")
    endif()
endif()

if    (PH_PLATFORM STREQUAL "OSX")
    set(CMAKE_OSX_ARCHITECTURES i386;x86_64)
elseif(PH_PLATFORM STREQUAL "iOS")
    set(CMAKE_OSX_ARCHITECTURES armv6;armv7)
endif()
set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

if (PH_PLATFORM STREQUAL "Android")
include(${CMAKE_CURRENT_SOURCE_DIR}/scripts/Android_Fork.cmake)
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/scripts/Porkholt_Files.cmake)

if(PH_PLATFORM STREQUAL "Android")
    set(PH_ENGINE_SRCS ${PH_ENGINE_SRCS}
      )
    set(PH_ENGINE_HEADERS ${PH_ENGINE_HEADERS}
      )

    include_directories(${PH_EXTERNALS}/openal-soft/include)
endif()

if(PH_PLATFORM STREQUAL "X11")
    set(PH_ENGINE_SRCS ${PH_ENGINE_SRCS}
      ${PH_ENGINE_PATH}/src/Bindings/X11/PHWindowing.cpp
      ${PH_ENGINE_PATH}/src/Bindings/X11/PHX11.cpp
      )
      
    set(PH_ENGINE_HEADERS ${PH_ENGINE_HEADERS}
      ${PH_ENGINE_PATH}/src/Bindings/X11/PHX11.h
      )
endif()

if(PH_PLATFORM STREQUAL "OSX")
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

endif()

if(PH_PLATFORM STREQUAL "iOS")
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
endif()

if(PH_PLATFORM STREQUAL "Android")
    if (PH_FORK STREQUAL "1")
        link_directories(${PH_EXTERNALS}/lib/android/libs/${ANDROID_NDK_ABI_NAME})
        add_library(Porkholt SHARED ${PH_ENGINE_SRCS} ${PH_ENGINE_HEADERS})
        target_link_libraries(Porkholt openal)

        add_custom_target(External_Libs
            COMMAND ${PH_EXTERNALS}/make_android.sh "${ANDROID_NDK}" "${ANDROID_ABI}" "${ANDROID_NDK_ABI_NAME}"
            WORKING_DIRECTORY ${PH_EXTERNALS}
            VERBATIM
            )
        add_dependencies(Porkholt External_Libs)
    else()
        invoke_make(Porkholt)
    endif()
else()
    add_library(Porkholt ${PH_ENGINE_SRCS} ${PH_ENGINE_HEADERS})

    if(PH_PLATFORM STREQUAL "OSX")
        set_target_properties(Porkholt PROPERTIES XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS "YES")
        set_target_properties(Porkholt PROPERTIES XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES")
    endif()
    if(PH_PLATFORM STREQUAL "iOS")
        set_target_properties(Porkholt PROPERTIES XCODE_ATTRIBUTE_GCC_THUMB_SUPPORT "NO")
        set_target_properties(Porkholt PROPERTIES XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS "YES")
        set_target_properties(Porkholt PROPERTIES XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES")
    endif()

    if(CMAKE_GENERATOR STREQUAL "Xcode")
        add_custom_command( TARGET Porkholt
            PRE_BUILD
            COMMAND ${PH_EXTERNALS}/make.sh
            WORKING_DIRECTORY ${PH_EXTERNALS}
            )
    else()
        add_custom_target(External_Libs
            COMMAND ${PH_EXTERNALS}/make.sh
            WORKING_DIRECTORY ${PH_EXTERNALS}
            )
        add_dependencies(Porkholt External_Libs)
    endif()

    endif()
