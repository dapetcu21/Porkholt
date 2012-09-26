function(porkholt PH_APP_TARGET)
  set(PH_SOURCES ${ARGV})
  list(REMOVE_AT PH_SOURCES 0)
  if(NOT DEFINED PH_NAME OR NOT PH_NAME)
    set(PH_NAME ${PH_APP_TARGET})
  endif()
  
  set(PH_PLATFORM_DOC "Target platform. Can be one of X11 OSX iOS Android")
  set(PH_ENGINE_PATH_DOC "Path to the Engine folder")
  set(PH_EXTERNALS_DOC "Path to the Externals folder")
  if(NOT DEFINED PH_PLATFORM)
    if (APPLE)
      set(PH_PLATFORM OSX CACHE STRING ${PH_PLATFORM_DOC})
    else()
      set(PH_PLATFORM X11 CACHE STRING ${PH_PLATFORM_DOC})
    endif()
    message("PH_PLATFORM is not defined. Defaulting to \"${PH_PLATFORM}\"")
  else()
    set(PH_PLATFORM ${PH_PLATFORM} CACHE STRING ${PH_PLATFORM_DOC})
  endif()

  if(NOT DEFINED PH_ENGINE_PATH)
    set(PH_ENGINE_PATH "." CACHE STRING ${PH_ENGINE_PATH_DOC})
    message("PH_ENGINE_PATH is not defined. Defaulting to \"${PH_ENGINE_PATH}\"")
  else()
    set(PH_ENGINE_PATH ${PH_ENGINE_PATH} CACHE STRING ${PH_ENGINE_PATH_DOC})
  endif()

  IF(NOT DEFINED PH_EXTERNALS)
    set(PH_EXTERNALS "${PH_ENGINE_PATH}/../Externals" CACHE STRING ${PH_EXTERNALS_DOC})
  else()
    set(PH_EXTERNALS ${PH_EXTERNALS} CACHE STRING ${PH_EXTERNALS_DOC})
  endif()
  
  set(LIBRARY_OUTPUT_PATH_ROOT ${CMAKE_CURRENT_BINARY_DIR} CACHE STRING "Don't change")
  if (NOT PH_FORK STREQUAL "1")
    add_subdirectory(${PH_ENGINE_PATH} ${CMAKE_CURRENT_BINARY_DIR}/engine-build)
  endif()

  include(${PH_ENGINE_PATH}/scripts/Porkholt_IncludeDirs.cmake)

  if(CMAKE_GENERATOR STREQUAL "Xcode")
      if(PH_PLATFORM STREQUAL "OSX")
        set(CMAKE_OSX_SYSROOT macosx PARENT_SCOPE)
      elseif(PH_PLATFORM STREQUAL "iOS")
        set(CMAKE_OSX_SYSROOT iphoneos PARENT_SCOPE)
        set(CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphoneos;-iphonesimulator" PARENT_SCOPE)
      endif()
  endif()

  if    (PH_PLATFORM STREQUAL "OSX")
      set(CMAKE_OSX_ARCHITECTURES i386;x86_64 PARENT_SCOPE)
  elseif(PH_PLATFORM STREQUAL "iOS")
      set(CMAKE_OSX_ARCHITECTURES armv6;armv7 PARENT_SCOPE)
  endif()
  
  set(CMAKE_CONFIGURATION_TYPES Debug Release PARENT_SCOPE)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os" PARENT_SCOPE)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG" PARENT_SCOPE)

  if (NOT DEFINED PH_MARKETING_VERSION)
    set(PH_MARKETING_VERSION 1.0)
  endif()
  if (NOT DEFINED PH_BUILD_NUMBER)
    set(PH_BUILD_NUMBER 1)
  endif()
  if (NOT DEFINED PH_NAME_IDENTIFIER)
    execute_process(COMMAND bash -c "echo -n \"${PH_APP_TARGET}\" | tr -d \"\\ \" "
                    OUTPUT_VARIABLE PH_NAME_IDENTIFIER)
  endif()
  if (NOT DEFINED PH_BUNDLE_ID)
    if (NOT DEFINED PH_COMPANY_ID)
      set(PH_COMPANY_ID com.mycompany)
    endif()
    set(PH_BUNDLE_ID ${PH_COMPANY_ID}.${PH_NAME_IDENTIFIER})
  endif()
  if (NOT DEFINED PH_COPYRIGHT)
    set(PH_COPYRIGHT "Copyright © 2012 __MyCompanyName__. All rights reserved.")
  endif()
  if (NOT DEFINED PH_ORIENTATION)
    set(PH_ORIENTATION "landscape")
  endif()

  if(PH_PLATFORM STREQUAL "Android")
    if (NOT PH_FORK STREQUAL "1")
        configure_file(${PH_ENGINE_PATH}/scripts/android_bootstrap.c ${CMAKE_CURRENT_BINARY_DIR}/bootstrap.c)
    endif()
    include(${PH_ENGINE_PATH}/scripts/Android_Fork.cmake)
  endif()

  if(PH_PLATFORM STREQUAL "X11")
    find_package(X11 REQUIRED)
    include_directories(${X11_INCLUDE_DIR})
    find_library(PH_XRANDR Xrandr)
  endif()
  if(NOT PH_PLATFORM STREQUAL "Android")
    if(NOT CMAKE_GENERATOR STREQUAL "Xcode")
      find_package(Threads REQUIRED)
    endif()
    if(NOT PH_PLATFORM STREQUAL "iOS")
      find_package(OpenGL REQUIRED)
      include_directories(${OPENGL_INCLUDE_DIRS})
      if(PH_PLATFORM STREQUAL "OSX")
        set(PH_OPENAL ${PH_EXTERNALS}/lib/${PH_LIBS}/libopenal.dylib)
      else()
        find_library(PH_OPENAL openal)
      endif()
    endif()
  endif()
  
  if(PH_PLATFORM STREQUAL "OSX")
    add_executable(${PH_APP_TARGET} MACOSX_BUNDLE ${PH_SOURCES})
    set_target_properties(${PH_APP_TARGET} PROPERTIES LINK_FLAGS "-ObjC -framework Foundation -framework CoreVideo -framework AppKit")
  elseif(PH_PLATFORM STREQUAL "iOS")
    add_executable(${PH_APP_TARGET} MACOSX_BUNDLE ${PH_SOURCES})
    set_target_properties(${PH_APP_TARGET} PROPERTIES LINK_FLAGS "-ObjC -framework AudioToolbox -framework AVFoundation -framework OpenAL -framework OpenGLES -framework Foundation -framework QuartzCore -framework UIKit")
  elseif(PH_PLATFORM STREQUAL "Android")
    if(PH_FORK STREQUAL "1")
      add_custom_target(
        ${PH_APP_TARGET}-CopyLibraries
        COMMAND /bin/bash -c "export PATH=\${PATH}${ANDROID_SDK}; mkdir -p ./libs/${ANDROID_NDK_ABI_NAME} && cp -r ${PH_EXTERNALS}/lib/android/libs/${ANDROID_NDK_ABI_NAME}/*.so ./libs/${ANDROID_NDK_ABI_NAME}"
        WORKING_DIRECTORY ${LIBRARY_OUTPUT_PATH_ROOT}
        VERBATIM
            )

      link_directories("${LIBRARY_OUTPUT_PATH_ROOT}/libs/${ANDROID_NDK_ABI_NAME}")
      add_library(${PH_APP_TARGET} SHARED ${PH_SOURCES})
      target_link_libraries(${PH_APP_TARGET} -lPorkholt -llua)
      add_dependencies(${PH_APP_TARGET} ${PH_APP_TARGET}-CopyLibraries)

      if(PH_USE_BOX2D)
          include("${PH_ENGINE_PATH}/scripts/Box2D.cmake")
          target_link_libraries(${PH_APP_TARGET} Box2D)
      endif()

      add_library(bootstrap SHARED ${CMAKE_CURRENT_BINARY_DIR}/../bootstrap.c)
      target_link_libraries(bootstrap log)

      if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        include(${PH_ENGINE_PATH}/scripts/Porkholt_Files.cmake)
        execute_process(COMMAND ${PH_ENGINE_PATH}/scripts/android_gdb_trim.lua 
            ${PH_SOURCES} 
            ${PH_ENGINE_SRCS} 
            ${PH_ENGINE_HEADERS}
            ${ANDROID_SYSROOT}/usr/include 
            ${ANDROID_NDK}/sources/cxx-stl/system
           OUTPUT_VARIABLE PH_GDB_DIRECTORY)
        set(PH_GDB_SOLIB_PATH ${LIBRARY_OUTPUT_PATH_ROOT}/libs/${ANDROID_NDK_ABI_NAME})
        configure_file(${PH_ENGINE_PATH}/scripts/android_gdb.setup ${LIBRARY_OUTPUT_PATH_ROOT}/libs/${ANDROID_NDK_ABI_NAME}/gdb.setup)
        configure_file(${ANDROID_NDK}/prebuilt/android-${ANDROID_ARCH_NAME}/gdbserver/gdbserver ${LIBRARY_OUTPUT_PATH_ROOT}/libs/${ANDROID_NDK_ABI_NAME}/gdbserver COPYONLY)
      endif()

    else()
      invoke_make(${PH_APP_TARGET}-lib)
      add_dependencies(${PH_APP_TARGET}-lib Porkholt) 
    endif()
  else()
    add_executable(${PH_APP_TARGET} ${PH_SOURCES})
  endif()
  
  if(NOT PH_PLATFORM STREQUAL "Android")
    target_link_libraries(${PH_APP_TARGET} Porkholt
      ${PH_EXTERNALS}/lib/${PH_LIBS}/liblua.a
      ${PH_EXTERNALS}/lib/${PH_LIBS}/libpng15.a
      ${PH_EXTERNALS}/lib/${PH_LIBS}/libz.a
      ${X11_LIBRARIES}
      ${PH_XRANDR}
      ${OPENGL_LIBRARIES}
      ${PH_OPENAL}
      ${CMAKE_THREAD_LIBS_INIT}
      )

    if(PH_USE_BOX2D)
      include("${PH_ENGINE_PATH}/scripts/Box2D.cmake")
      target_link_libraries(${PH_APP_TARGET} Box2D)
    endif()
  endif()

  if (PH_PLATFORM STREQUAL "iOS")
    if(NOT DEFINED PH_IOS_DEPLOYMENT_TARGET)
      set(PH_IOS_DEPLOYMENT_TARGET "3.2")
    endif()
    #if(NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET OR NOT CMAKE_OSX_DEPLOYMENT_TARGET)
    #  set(CMAKE_OSX_DEPLOYMENT_TARGET ${PH_IOS_DEPLOYMENT_TARGET} PARENT_SCOPE)
    #endif()
        
    if(NOT DEFINED PH_IOS_CODE_SIGN_IDENTITY)
      set(PH_IOS_CODE_SIGN_IDENTITY "iPhone Developer")
    endif()
    if(NOT DEFINED PH_IOS_TARGET_IPHONE)
      set(PH_IOS_TARGET_IPHONE "YES")
    endif()
    if(NOT DEFINED PH_IOS_TARGET_IPAD)
      set(PH_IOS_TARGET_IPAD "NO")
    endif()
    if(PH_IOS_TARGET_IPHONE)
      if(PH_IOS_TARGET_IPAD)
        set(PH_IOS_DEV_FAMILY "1,2")
      else()
        set(PH_IOS_DEV_FAMILY "1")
      endif()
    else()
      if(PH_IOS_TARGET_IPAD)
        set(PH_IOS_DEV_FAMILY "2")
      else()
        set(PH_IOS_DEV_FAMILY "")
      endif()
    endif()

    if (NOT DEFINED PH_IOS_INFO_PLIST)
      set(PH_IOS_INFO_PLIST "${PROJECT_SOURCE_DIR}/Info-iOS.plist")
    endif()
    if (NOT EXISTS ${PH_IOS_INFO_PLIST})
      set(PH_IOS_INFO_PLIST "${PH_ENGINE_PATH}/scripts/Info-iOS.plist")
    endif()

    if (CMAKE_GENERATOR STREQUAL "Xcode")
        set_target_properties(${PH_APP_TARGET} PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ${PH_IOS_CODE_SIGN_IDENTITY})
        set_target_properties(${PH_APP_TARGET} PROPERTIES XCODE_ATTRIBUTE_GCC_THUMB_SUPPORT "NO")
        set_target_properties(${PH_APP_TARGET} PROPERTIES XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS "YES")
        set_target_properties(${PH_APP_TARGET} PROPERTIES XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES")
        set(PH_IOS_INFO_PLIST_OUT "${CMAKE_CURRENT_BINARY_DIR}/Info-iOS.plist")
        configure_file(${PH_IOS_INFO_PLIST} ${PH_IOS_INFO_PLIST_OUT})
        set_target_properties(${PH_APP_TARGET} PROPERTIES XCODE_ATTRIBUTE_INFOPLIST_FILE ${PH_IOS_INFO_PLIST_OUT} )
        set_target_properties(${PH_APP_TARGET} PROPERTIES XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "${PH_IOS_DEPLOYMENT_TARGET}")
        set_target_properties(${PH_APP_TARGET} PROPERTIES XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "${PH_IOS_DEV_FAMILY}")
    else()
        set_target_properties(${PH_APP_TARGET} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PH_IOS_INFO_PLIST})
    endif()

  elseif (PH_PLATFORM STREQUAL "OSX")
    if (NOT DEFINED PH_OSX_DEPLOYMENT_TARGET)
      set(PH_OSX_DEPLOYMENT_TARGET "10.6")
    endif()
    if(NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET OR NOT CMAKE_OSX_DEPLOYMENT_TARGET)
      set(CMAKE_OSX_DEPLOYMENT_TARGET ${PH_OSX_DEPLOYMENT_TARGET} PARENT_SCOPE)
    endif()
    if (NOT DEFINED PH_OSX_INFO_PLIST)
    	set(PH_OSX_INFO_PLIST "${PROJECT_SOURCE_DIR}/Info-OSX.plist")
    endif ()
    if (NOT EXISTS ${PH_OSX_INFO_PLIST})
    	set(PH_OSX_INFO_PLIST "${PH_ENGINE_PATH}/scripts/Info-OSX.plist")
    endif ()

    if(CMAKE_GENERATOR STREQUAL "Xcode")
      set(PH_OSX_INFO_PLIST_OUT "${CMAKE_CURRENT_BINARY_DIR}/Info-OSX.plist")
      set_target_properties(${PH_APP_TARGET} PROPERTIES XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS "YES")
      set_target_properties(${PH_APP_TARGET} PROPERTIES XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES")
      configure_file(${PH_OSX_INFO_PLIST} ${PH_OSX_INFO_PLIST_OUT})
      set_target_properties(${PH_APP_TARGET} PROPERTIES XCODE_ATTRIBUTE_INFOPLIST_FILE ${PH_OSX_INFO_PLIST_OUT} )
    else()
      set_target_properties(${PH_APP_TARGET} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PH_OSX_INFO_PLIST})
    endif()
  endif()
  
  set(RES_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rsrc)
  if (PH_PLATFORM STREQUAL "OSX" OR PH_PLATFORM STREQUAL "iOS")
    if (PH_PLATFORM STREQUAL "OSX")
      set(PH_BUNDLE_PREFIX "Contents/Resources/")
      set(PH_BUILD_TYPE "build-nodownscale")
    else()
      set(PH_BUNDLE_PREFIX "")
      set(PH_BUILD_TYPE "build")
    endif()
    if(CMAKE_GENERATOR STREQUAL "Xcode")
      set(APP_NAME \${TARGET_BUILD_DIR}/\${FULL_PRODUCT_NAME})
      add_custom_command(
        TARGET ${PH_APP_TARGET}
        POST_BUILD
        COMMAND ${PH_EXTERNALS}/lua/src/lua ${PH_ENGINE_PATH}/scripts/postprocess.lua ${RES_SRC_DIR} ${APP_NAME}/${PH_BUNDLE_PREFIX}rsrc ${PH_BUILD_TYPE} ${PH_EXTERNALS}
        )
      if (PH_PLATFORM STREQUAL "OSX")
        add_custom_command(
          TARGET ${PH_APP_TARGET}
          POST_BUILD
          COMMAND ${PH_ENGINE_PATH}/scripts/copy_libraries.sh ${PH_EXTERNALS}/lib/darwin/osx ${APP_NAME}/Contents/lib
          )
      endif()
    else()
      set(APP_NAME "${CMAKE_CURRENT_BINARY_DIR}/${PH_APP_TARGET}.app")
      add_custom_target(
        PostProcess_Resources
        COMMAND ${PH_EXTERNALS}/lua/src/lua ${PH_ENGINE_PATH}/scripts/postprocess.lua ${RES_SRC_DIR} ${APP_NAME}/${PH_BUNDLE_PREFIX}rsrc ${PH_BUILD_TYPE} ${PH_EXTERNALS}
        )
      add_dependencies(${PH_APP_TARGET} PostProcess_Resources)
      add_dependencies(PostProcess_Resources External_Libs)  
      if (PH_PLATFORM STREQUAL "OSX")
        add_custom_target(
          Copy_Libraries
          COMMAND ${PH_ENGINE_PATH}/scripts/copy_libraries.sh ${PH_EXTERNALS}/lib/darwin/osx ${APP_NAME}/Contents/lib
          )
        add_dependencies(${PH_APP_TARGET} Copy_Libraries)
        add_dependencies(Copy_Libraries External_Libs)  
      endif()
    endif()
  elseif(PH_PLATFORM STREQUAL "X11")
    set(RES_DEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/${PH_APP_TARGET}-rsrc)
    add_custom_target(
      PostProcess_Resources
      COMMAND ${PH_EXTERNALS}/lua/src/lua ${PH_ENGINE_PATH}/scripts/postprocess.lua ${RES_SRC_DIR} ${RES_DEST_DIR} "build-nodownscale" ${PH_EXTERNALS}
      )
    add_dependencies(${PH_APP_TARGET} PostProcess_Resources)
    add_dependencies(PostProcess_Resources External_Libs)
  elseif(PH_PLATFORM STREQUAL "Android" AND NOT PH_FORK STREQUAL "1")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(PH_ANT_TARGET debug)
    else()
        set(PH_ANT_TARGET release)
    endif()
    configure_file(${PH_ENGINE_PATH}/scripts/AndroidManifest.xml ${CMAKE_CURRENT_BINARY_DIR}/AndroidManifest.xml)
    configure_file(${PH_ENGINE_PATH}/scripts/default.properties ${CMAKE_CURRENT_BINARY_DIR}/default.properties)

    if (EXISTS ANDROID_SDK)
      set(ANDROID_SDK ":${ANDROID_SDK}/tools")
    elseif(EXISTS "$ENV{ANDROID_SDK}")
      set(ANDROID_SDK ":$ENV{ANDROID_SDK}/tools")
    elseif(EXISTS "/opt/android-sdk")
    else()
      unset(ANDROID_SDK)
    endif()

    set(RES_DEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/res/raw/rsrc)
    add_custom_target(
        ${PH_APP_TARGET}-resources
        COMMAND ${PH_EXTERNALS}/lua/src/lua ${PH_ENGINE_PATH}/scripts/postprocess.lua ${RES_SRC_DIR} ${RES_DEST_DIR} "build" ${PH_EXTERNALS}
      )

    add_custom_target(
        ${PH_APP_TARGET}
        ALL /bin/bash -c "export PATH=\${PATH}${ANDROID_SDK}; mkdir -p src/org/porkholt/jniloader; cp ${PH_ENGINE_PATH}/scripts/JNILoader.java src/org/porkholt/jniloader/JNILoader.java && android update project -p . --target android-10 --name ${PH_APP_TARGET}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        VERBATIM
        )


    add_custom_target(
        ${PH_APP_TARGET}-package
        ALL /bin/bash -c "export PATH=\${PATH}${ANDROID_SDK}; ant ${PH_ANT_TARGET}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        VERBATIM
        )
 
    add_custom_target(
        ${PH_APP_TARGET}-install 
        COMMAND /bin/bash -c "export PATH=\${PATH}${ANDROID_SDK}; ant ${PH_ANT_TARGET} install"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        VERBATIM
        )

    add_custom_target(
        ${PH_APP_TARGET}-run
        COMMAND /bin/bash -c "export PATH=\${PATH}${ANDROID_SDK}; adb shell am start -a android.intent.action.MAIN -n ${PH_BUNDLE_ID}/android.app.NativeActivity"
        VERBATIM
        )

    if (NOT TARGET install)
        add_custom_target(install)
    endif()
    if (NOT TARGET run)
        add_custom_target(run)
    endif()
    if (NOT TARGET package)
        add_custom_target(package)
    endif()

    add_dependencies(${PH_APP_TARGET} ${PH_APP_TARGET}-lib)
    add_dependencies(${PH_APP_TARGET} ${PH_APP_TARGET}-resources)
    add_dependencies(${PH_APP_TARGET}-install ${PH_APP_TARGET})
    add_dependencies(${PH_APP_TARGET}-package ${PH_APP_TARGET})
    add_dependencies(${PH_APP_TARGET}-run ${PH_APP_TARGET}-install)
    add_dependencies(package ${PH_APP_TARGET}-package)
    add_dependencies(install ${PH_APP_TARGET}-install)
    add_dependencies(run ${PH_APP_TARGET}-run)

  endif()
endfunction()
