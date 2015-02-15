cmake_minimum_required(VERSION 2.8)
project(Porkholt)

set(PH_PLATFORM_DOC "Target platform. Can be one of X11 OSX iOS Android")
if (APPLE)
    set(PH_PLATFORM OSX CACHE STRING ${PH_PLATFORM_DOC})
else()
    set(PH_PLATFORM X11 CACHE STRING ${PH_PLATFORM_DOC})
endif()

set(PH_ENGINE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "Path to the Engine folder")
get_filename_component(PH_ENGINE_PATH "${PH_ENGINE_PATH}" REALPATH)
set(PH_EXTERNALS "${PH_ENGINE_PATH}/../Externals" CACHE PATH "Path to the Externals folder")
get_filename_component(PH_EXTERNALS "${PH_EXTERNALS}" REALPATH)

include(${PH_ENGINE_PATH}/scripts/Porkholt_Common.cmake)

if (PH_PLATFORM STREQUAL "Android")
    include(${PH_ENGINE_PATH}/scripts/Android_Fork.cmake)
elseif(PH_PLATFORM STREQUAL "iOS" AND NOT CMAKE_GENERATOR STREQUAL "Xcode")
    include(${PH_ENGINE_PATH}/scripts/iOS_Fork.cmake)
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
elseif(PH_PLATFORM STREQUAL "iOS" AND NOT CMAKE_GENERATOR STREQUAL "Xcode")
    if (PH_FORK STREQUAL "1")
        add_library(Porkholt ${PH_ENGINE_SRCS} ${PH_ENGINE_HEADERS})
        
        if(NOT DEFINED PH_IOS_DEPLOYMENT_TARGET)
          set(PH_IOS_DEPLOYMENT_TARGET "3.2")
        endif()
 
        set(DEPLOY_FLAGS "-miphoneos-version-min=${PH_IOS_DEPLOYMENT_TARGET}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${DEPLOY_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DEPLOY_FLAGS}")

        add_custom_target(External_Libs
            COMMAND ${PH_EXTERNALS}/make.sh
            WORKING_DIRECTORY ${PH_EXTERNALS}
            )
        add_dependencies(Porkholt External_Libs)
    else()
        invoke_make(Porkholt)
    endif()
else()
    add_library(Porkholt ${PH_ENGINE_SRCS} ${PH_ENGINE_HEADERS})

    if(PH_PLATFORM STREQUAL "OSX")
        if (NOT DEFINED PH_OSX_DEPLOYMENT_TARGET)
            set(PH_OSX_DEPLOYMENT_TARGET "")
        endif()
        if(NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET OR NOT CMAKE_OSX_DEPLOYMENT_TARGET)
            set(CMAKE_OSX_DEPLOYMENT_TARGET ${PH_OSX_DEPLOYMENT_TARGET} PARENT_SCOPE)
        endif()

        set_target_properties(Porkholt PROPERTIES XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS "YES")
        set_target_properties(Porkholt PROPERTIES XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES")
    endif()
    if(PH_PLATFORM STREQUAL "iOS")
        if(NOT DEFINED PH_IOS_DEPLOYMENT_TARGET)
          set(PH_IOS_DEPLOYMENT_TARGET "3.2")
        endif()
 
        set_target_properties(Porkholt PROPERTIES XCODE_ATTRIBUTE_GCC_THUMB_SUPPORT "NO")
        set_target_properties(Porkholt PROPERTIES XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS "YES")
        set_target_properties(Porkholt PROPERTIES XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES")
        set_target_properties(${PH_APP_TARGET} PROPERTIES XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "${PH_IOS_DEPLOYMENT_TARGET}")
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
