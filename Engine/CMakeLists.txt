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

include(${PH_ENGINE_PATH}/scripts/Porkholt_Common.cmake)

if (PH_PLATFORM STREQUAL "Android")
    include(${PH_ENGINE_PATH}/scripts/Android_Fork.cmake)
endif()

include(${PH_ENGINE_PATH}/scripts/Porkholt_Files.cmake)

if(PH_PLATFORM STREQUAL "Android")
    if (PH_FORK STREQUAL "1")
        link_directories(${PH_EXTERNALS}/lib/android/libs/${ANDROID_NDK_ABI_NAME})
        add_library(Porkholt SHARED ${PH_ENGINE_SRCS} ${PH_ENGINE_HEADERS})
        target_link_libraries(Porkholt openal luajit png15 ogg vorbis vorbisfile android log EGL)

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
