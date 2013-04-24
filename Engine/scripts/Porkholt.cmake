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
    if(APPLE)
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
  get_filename_component(PH_ENGINE_PATH "${PH_ENGINE_PATH}" REALPATH)

  IF(NOT DEFINED PH_EXTERNALS)
    set(PH_EXTERNALS "${PH_ENGINE_PATH}/../Externals" CACHE STRING ${PH_EXTERNALS_DOC})
  else()
    set(PH_EXTERNALS ${PH_EXTERNALS} CACHE STRING ${PH_EXTERNALS_DOC})
  endif()
  get_filename_component(PH_EXTERNALS "${PH_EXTERNALS}" REALPATH)

  include(${PH_ENGINE_PATH}/scripts/Porkholt_Common.cmake)
  
  set(LIBRARY_OUTPUT_PATH_ROOT ${CMAKE_CURRENT_BINARY_DIR} CACHE STRING "Don't change")
  if(NOT PH_FORK STREQUAL "1")
    set(PH_LIVEPAPERS_FPS_STAGES "${PH_LIVEPAPERS_FPS_STAGES}" CACHE STRING "Fps Stages")
    set(PH_APP_TARGET "${PH_APP_TARGET}" CACHE STRING "App Target")
    add_subdirectory(${PH_ENGINE_PATH} ${CMAKE_CURRENT_BINARY_DIR}/engine-build)
  endif()

  include(${PH_ENGINE_PATH}/scripts/Porkholt_IncludeDirs.cmake)
  
  if(NOT DEFINED PH_MARKETING_VERSION)
    set(PH_MARKETING_VERSION 1.0)
  endif()
  if(NOT DEFINED PH_BUILD_NUMBER)
    set(PH_BUILD_NUMBER 1)
  endif()
  if(NOT DEFINED PH_NAME_IDENTIFIER)
    execute_process(COMMAND bash -c "echo -n \"${PH_APP_TARGET}\" | tr -d \"\\ \" "
                    OUTPUT_VARIABLE PH_NAME_IDENTIFIER)
  endif()
  if(NOT DEFINED PH_BUNDLE_ID)
    if(NOT DEFINED PH_COMPANY_ID)
      set(PH_COMPANY_ID com.mycompany)
    endif()
    set(PH_BUNDLE_ID ${PH_COMPANY_ID}.${PH_NAME_IDENTIFIER})
  endif()
  if(NOT DEFINED PH_COPYRIGHT)
    set(PH_COPYRIGHT "Copyright © 2012 __MyCompanyName__. All rights reserved.")
  endif()
  if(NOT DEFINED PH_ORIENTATION)
    set(PH_ORIENTATION "landscape")
  endif()

  if(PH_PLATFORM STREQUAL "Android")
    if(NOT PH_FORK STREQUAL "1")
        configure_file(${PH_ENGINE_PATH}/scripts/android_bootstrap.c ${CMAKE_CURRENT_BINARY_DIR}/bootstrap.c)
    endif()
    include(${PH_ENGINE_PATH}/scripts/Android_Fork.cmake)
  endif()

  if(PH_PLATFORM STREQUAL "iOS" AND NOT CMAKE_GENERATOR STREQUAL "Xcode" )
    include(${PH_ENGINE_PATH}/scripts/iOS_Fork.cmake)
  endif()


  if(PH_PLATFORM STREQUAL "X11")
    find_package(X11 REQUIRED)
    include_directories(${X11_INCLUDE_DIR})
    find_library(PH_XRANDR Xrandr)
  endif()
  if(NOT PH_PLATFORM STREQUAL "Android")
    if(NOT PH_PLATFORM STREQUAL "iOS")
      if(NOT CMAKE_GENERATOR STREQUAL "Xcode")
        find_package(Threads REQUIRED)
      endif()
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
    set_target_properties(${PH_APP_TARGET} PROPERTIES LINK_FLAGS "-ObjC -framework Foundation -framework CoreVideo -framework AppKit -framework IOKit")
  elseif(PH_PLATFORM STREQUAL "iOS")
    set(PH_IOS_LINKER_FLAGS "-ObjC -framework AudioToolbox -framework AVFoundation -framework OpenAL -framework OpenGLES -framework Foundation -framework QuartzCore -framework UIKit")
    if(CMAKE_GENERATOR STREQUAL "Xcode")
      add_executable(${PH_APP_TARGET} MACOSX_BUNDLE ${PH_SOURCES})
      set_target_properties(${PH_APP_TARGET} PROPERTIES LINK_FLAGS "${PH_IOS_LINKER_FLAGS}")
    elseif(PH_FORK STREQUAL "1")
      if(PH_LIVEPAPERS)
        add_library(${PH_APP_TARGET} SHARED ${PH_SOURCES})
        set(PH_IOS_LINKER_FLAGS "${PH_IOS_LINKER_FLAGS} -framework GLKit")
        set(PH_STAGING "${CMAKE_CURRENT_BINARY_DIR}/../_")
        set_target_properties(${PH_APP_TARGET} PROPERTIES PREFIX "")
        set_target_properties(${PH_APP_TARGET} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${PH_STAGING}/Library/LivePapers/Plugins")
      else()
        add_executable(${PH_APP_TARGET} ${PH_SOURCES})
        set_target_properties(${PH_APP_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../${PH_APP_TARGET}.app")
      endif()
      set_target_properties(${PH_APP_TARGET} PROPERTIES LINK_FLAGS "${PH_IOS_LINKER_FLAGS}")
    else()
      set(PH_STAGING "${CMAKE_CURRENT_BINARY_DIR}/_")
      invoke_make(${PH_APP_TARGET})
      add_dependencies(${PH_APP_TARGET} Porkholt)
    endif()
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
      target_link_libraries(${PH_APP_TARGET} -lPorkholt -lluajit)
      add_dependencies(${PH_APP_TARGET} ${PH_APP_TARGET}-CopyLibraries)

      if(PH_USE_BOX2D)
          include("${PH_ENGINE_PATH}/scripts/Box2D.cmake")
          target_link_libraries(${PH_APP_TARGET} Box2D)
      endif()

      add_library(bootstrap SHARED ${CMAKE_CURRENT_BINARY_DIR}/../bootstrap.c)
      target_link_libraries(bootstrap log)

      if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        include(${PH_ENGINE_PATH}/scripts/Porkholt_Files.cmake)
        execute_process(COMMAND ${PH_EXTERNALS}/bin/${PH_BINS}/luajit ${PH_ENGINE_PATH}/scripts/android_gdb_trim.lua 
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
  
  if(NOT PH_PLATFORM STREQUAL "Android" AND ( NOT PH_PLATFORM STREQUAL "iOS" OR PH_FORK ) )
    if(PH_FORK)
      get_filename_component(PH__LIB_PATH "${CMAKE_CURRENT_BINARY_DIR}/../engine-build/fork/libPorkholt.a" REALPATH)
      target_link_libraries(${PH_APP_TARGET} "${PH__LIB_PATH}")
    else()
      target_link_libraries(${PH_APP_TARGET} Porkholt)
    endif()
    target_link_libraries(${PH_APP_TARGET} 
      ${PH_EXTERNALS}/lib/${PH_LIBS}/libluajit.a
      ${PH_EXTERNALS}/lib/${PH_LIBS}/libpng15.a
      ${PH_EXTERNALS}/lib/${PH_LIBS}/libz.a
      ${PH_EXTERNALS}/lib/${PH_LIBS}/libvorbisfile.a
      ${PH_EXTERNALS}/lib/${PH_LIBS}/libvorbis.a
      ${PH_EXTERNALS}/lib/${PH_LIBS}/libogg.a
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

  if(PH_PLATFORM STREQUAL "iOS")
    if(NOT DEFINED PH_IOS_DEPLOYMENT_TARGET)
      set(PH_IOS_DEPLOYMENT_TARGET "3.2")
    endif()
       
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

    if(NOT DEFINED PH_IOS_INFO_PLIST)
      if(PH_LIVEPAPERS)
        set(PH_IOS_INFO_PLIST "${PROJECT_SOURCE_DIR}/LivePapers.plist")
      else()
        set(PH_IOS_INFO_PLIST "${PROJECT_SOURCE_DIR}/Info-iOS.plist")
      endif()
    endif()
    if(NOT EXISTS ${PH_IOS_INFO_PLIST})
      if(PH_LIVEPAPERS)
        set(PH_IOS_INFO_PLIST "${PH_ENGINE_PATH}/scripts/LivePapers.plist")
      else()
        set(PH_IOS_INFO_PLIST "${PH_ENGINE_PATH}/scripts/Info-iOS.plist")
      endif()
    endif()

    set(PH_IOS_INFO_PLIST_OUT "${CMAKE_CURRENT_BINARY_DIR}/Info-iOS.plist")
    if(CMAKE_GENERATOR STREQUAL "Xcode")
      configure_file(${PH_IOS_INFO_PLIST} ${PH_IOS_INFO_PLIST_OUT})
      set_target_properties(${PH_APP_TARGET} PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ${PH_IOS_CODE_SIGN_IDENTITY})
      set_target_properties(${PH_APP_TARGET} PROPERTIES XCODE_ATTRIBUTE_GCC_THUMB_SUPPORT "NO")
      set_target_properties(${PH_APP_TARGET} PROPERTIES XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS "YES")
      set_target_properties(${PH_APP_TARGET} PROPERTIES XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES")
      set_target_properties(${PH_APP_TARGET} PROPERTIES XCODE_ATTRIBUTE_INFOPLIST_FILE ${PH_IOS_INFO_PLIST_OUT} )
      set_target_properties(${PH_APP_TARGET} PROPERTIES XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "${PH_IOS_DEPLOYMENT_TARGET}")
      set_target_properties(${PH_APP_TARGET} PROPERTIES XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "${PH_IOS_DEV_FAMILY}")
    elseif(PH_FORK STREQUAL "1")
      set_target_properties(${PH_APP_TARGET} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PH_IOS_INFO_PLIST})
      set(PH__DEPLOY_FLAGS "-miphoneos-version-min=${PH_IOS_DEPLOYMENT_TARGET}")

      append_target_property(${PH_APP_TARGET} COMPILE_FLAGS "${PH__DEPLOY_FLAGS}")
      append_target_property(${PH_APP_TARGET} LINK_FLAGS "${PH__DEPLOY_FLAGS}")
    else()
      if(PH_LIVEPAPERS)
        set(PH_IOS_INFO_PLIST_DEST "${PH_STAGING}/Library/LivePapers/Wallpapers/${PH_BUNDLE_ID}/Info.plist")
        configure_file("${PROJECT_SOURCE_DIR}/LivePapers-Default.png" "${PH_STAGING}/Library/LivePapers/Wallpapers/${PH_BUNDLE_ID}/Default.png" COPYONLY)
      else()
        set(PH_IOS_INFO_PLIST_DEST "${CMAKE_CURRENT_BINARY_DIR}/${PH_APP_TARGET}.app/Info.plist")
      endif()
      configure_file(${PH_IOS_INFO_PLIST} ${PH_IOS_INFO_PLIST_OUT})
      add_custom_target(InfoPlist COMMAND ${CMAKE_COMMAND} -E copy "${PH_IOS_INFO_PLIST_OUT}" "${PH_IOS_INFO_PLIST_DEST}")
      add_dependencies(${PH_APP_TARGET} InfoPlist)
    endif()

  elseif(PH_PLATFORM STREQUAL "OSX")
    if(NOT DEFINED PH_OSX_DEPLOYMENT_TARGET)
      set(PH_OSX_DEPLOYMENT_TARGET "10.6")
    endif()
    if(NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET OR NOT CMAKE_OSX_DEPLOYMENT_TARGET)
      set(CMAKE_OSX_DEPLOYMENT_TARGET ${PH_OSX_DEPLOYMENT_TARGET} PARENT_SCOPE)
    endif()
    if(NOT DEFINED PH_OSX_INFO_PLIST)
    	set(PH_OSX_INFO_PLIST "${PROJECT_SOURCE_DIR}/Info-OSX.plist")
    endif()
    if(NOT EXISTS ${PH_OSX_INFO_PLIST})
    	set(PH_OSX_INFO_PLIST "${PH_ENGINE_PATH}/scripts/Info-OSX.plist")
    endif()

    set(PH_OSX_INFO_PLIST_OUT "${CMAKE_CURRENT_BINARY_DIR}/Info-OSX.plist")
    configure_file(${PH_OSX_INFO_PLIST} ${PH_OSX_INFO_PLIST_OUT})
    if(CMAKE_GENERATOR STREQUAL "Xcode")
      set_target_properties(${PH_APP_TARGET} PROPERTIES XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS "YES")
      set_target_properties(${PH_APP_TARGET} PROPERTIES XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES")
      set_target_properties(${PH_APP_TARGET} PROPERTIES XCODE_ATTRIBUTE_INFOPLIST_FILE ${PH_OSX_INFO_PLIST_OUT} )
    else()
      set_target_properties(${PH_APP_TARGET} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PH_OSX_INFO_PLIST_OUT})
    endif()
  endif()

  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(PH_BUILD_TYPE "noluacompress")
  endif()
  
  set(RES_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rsrc)
  if(PH_PLATFORM STREQUAL "OSX" OR ( PH_PLATFORM STREQUAL "iOS" AND NOT PH_FORK ) )
    if(PH_PLATFORM STREQUAL "OSX")
      set(PH_BUNDLE_PREFIX "Contents/Resources/")
    else()
      set(PH_BUNDLE_PREFIX "")
    endif()
    if(CMAKE_GENERATOR STREQUAL "Xcode")
      set(APP_NAME \${TARGET_BUILD_DIR}/\${FULL_PRODUCT_NAME})
      add_custom_command(
        TARGET ${PH_APP_TARGET}
        POST_BUILD
        COMMAND ${PH_EXTERNALS}/bin/${PH_BINS}/luajit ${PH_ENGINE_PATH}/scripts/postprocess.lua ${RES_SRC_DIR} ${APP_NAME}/${PH_BUNDLE_PREFIX}rsrc ${PH_EXTERNALS} ${PH_PLATFORM} ${PH_BUILD_TYPE}
        )
      if(PH_PLATFORM STREQUAL "OSX")
        add_custom_command(
          TARGET ${PH_APP_TARGET}
          POST_BUILD
          COMMAND ${PH_ENGINE_PATH}/scripts/copy_libraries.sh ${PH_EXTERNALS}/lib/darwin/osx ${APP_NAME}/Contents/lib
          )
      endif()
    else()
      if(PH_LIVEPAPERS)
        set(APP_NAME "${PH_STAGING}/Library/LivePapers/Plugins")
        set(PH_BUNDLE_PREFIX "${PH_APP_TARGET}-")
      else()
        set(APP_NAME "${CMAKE_CURRENT_BINARY_DIR}/${PH_APP_TARGET}.app")
      endif()
      add_custom_target(
        PostProcess_Resources
        COMMAND ${PH_EXTERNALS}/bin/${PH_BINS}/luajit ${PH_ENGINE_PATH}/scripts/postprocess.lua ${RES_SRC_DIR} ${APP_NAME}/${PH_BUNDLE_PREFIX}rsrc ${PH_EXTERNALS} ${PH_PLATFORM} ${PH_BUILD_TYPE}
        )
      add_dependencies(${PH_APP_TARGET} PostProcess_Resources)
      add_dependencies(PostProcess_Resources External_Libs)  
      if(PH_PLATFORM STREQUAL "OSX")
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
      COMMAND ${PH_EXTERNALS}/bin/${PH_BINS}/luajit ${PH_ENGINE_PATH}/scripts/postprocess.lua ${RES_SRC_DIR} ${RES_DEST_DIR} ${PH_EXTERNALS} ${PH_PLATFORM} ${PH_BUILD_TYPE}
      )
    add_dependencies(${PH_APP_TARGET} PostProcess_Resources)
    add_dependencies(PostProcess_Resources External_Libs)
  elseif(PH_PLATFORM STREQUAL "Android" AND NOT PH_FORK STREQUAL "1")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(PH_ANT_TARGET debug)
    else()
        set(PH_ANT_TARGET release)
    endif()
    configure_file(${PH_ENGINE_PATH}/scripts/AndroidManifest.xml ${CMAKE_CURRENT_BINARY_DIR}/AndroidManifest.xml)
    configure_file(${PH_ENGINE_PATH}/scripts/default.properties ${CMAKE_CURRENT_BINARY_DIR}/default.properties)

    if(EXISTS ANDROID_SDK)
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
        COMMAND ${PH_EXTERNALS}/bin/${PH_BINS}/luajit ${PH_ENGINE_PATH}/scripts/postprocess.lua ${RES_SRC_DIR} ${RES_DEST_DIR} ${PH_EXTERNALS} ${PH_PLATFORM} ${PH_BUILD_TYPE}
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

    add_custom_target(
        ${PH_APP_TARGET}-package-only
        ALL /bin/bash -c "export PATH=\${PATH}${ANDROID_SDK}; ant ${PH_ANT_TARGET}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        VERBATIM
        )
 
    add_custom_target(
        ${PH_APP_TARGET}-install-only
        COMMAND /bin/bash -c "export PATH=\${PATH}${ANDROID_SDK}; ant ${PH_ANT_TARGET} install"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        VERBATIM
        )

    add_custom_target(
        ${PH_APP_TARGET}-run-only
        COMMAND /bin/bash -c "export PATH=\${PATH}${ANDROID_SDK}; adb shell am start -a android.intent.action.MAIN -n ${PH_BUNDLE_ID}/android.app.NativeActivity"
        VERBATIM
        )

    if(NOT TARGET install)
        add_custom_target(install)
    endif()
    if(NOT TARGET run)
        add_custom_target(run)
    endif()
    if(NOT TARGET package)
        add_custom_target(package)
    endif()

    if(NOT TARGET install-only)
        add_custom_target(install-only)
    endif()
    if(NOT TARGET run-only)
        add_custom_target(run-only)
    endif()
    if(NOT TARGET package-only)
        add_custom_target(package-only)
    endif()

    add_dependencies(${PH_APP_TARGET} ${PH_APP_TARGET}-lib)
    add_dependencies(${PH_APP_TARGET} ${PH_APP_TARGET}-resources)
    add_dependencies(${PH_APP_TARGET}-install ${PH_APP_TARGET})
    add_dependencies(${PH_APP_TARGET}-package ${PH_APP_TARGET})
    add_dependencies(${PH_APP_TARGET}-run ${PH_APP_TARGET}-install)
    add_dependencies(package ${PH_APP_TARGET}-package)
    add_dependencies(package-only ${PH_APP_TARGET}-package-only)
    add_dependencies(install ${PH_APP_TARGET}-install)
    add_dependencies(install-only ${PH_APP_TARGET}-install-only)
    add_dependencies(run ${PH_APP_TARGET}-run)
    add_dependencies(run-only ${PH_APP_TARGET}-run-only)

  endif()

  if(PH_PLATFORM STREQUAL "iOS" AND NOT CMAKE_GENERATOR STREQUAL "Xcode" AND NOT PH_FORK)
    if(PH_LIVEPAPERS)
      set(PH__OUTPUT_BINARY "${PH_STAGING}/Library/LivePapers/Plugins/${PH_APP_TARGET}.dylib")
    else()
      set(PH__OUTPUT_BINARY "${CMAKE_CURRENT_BINARY_DIR}/${PH_APP_TARGET}.app/${PH_APP_TARGET}")
    endif()
    add_custom_target(${PH_APP_TARGET}-sign ALL codesign -f -s "${PH_IOS_CODE_SIGN_IDENTITY}" "${PH__OUTPUT_BINARY}" VERBATIM)
    add_dependencies(${PH_APP_TARGET}-sign ${PH_APP_TARGET})

    if(PH_LIVEPAPERS)
      add_custom_target(${PH_APP_TARGET}-install bash -c "rsync -a --exclude=*~ --exclude=.DS_Store --exclude=*.swp --exclude=*.swo \"${PH_STAGING}/\" root@$PH_DEVICE_IP:/ && ( ssh root@$PH_DEVICE_IP killall LivePapers || exit 0 )" VERBATIM)
    else()
      add_custom_target(${PH_APP_TARGET}-install bash -c "rsync -a --delete --delete-excluded --exclude=*~ --exclude=.DS_Store --exclude=*.swp --exclude=*.swo ${CMAKE_CURRENT_BINARY_DIR}/${PH_APP_TARGET}.app root@$PH_DEVICE_IP:/Applications" VERBATIM)
    endif()
    add_dependencies(${PH_APP_TARGET}-install ${PH_APP_TARGET}-sign)

    add_custom_target(${PH_APP_TARGET}-debug "${PH_ENGINE_PATH}/scripts/ios-debug/run-gdb.sh" "${PH__OUTPUT_BINARY}" "/Applications/${PH_APP_TARGET}.app/${PH_APP_TARGET}" "${CMAKE_CURRENT_BINARY_DIR}/${PH_APP_TARGET}.dSYM" VERBATIM)

    if(PH_LIVEPAPERS)
      set(PH_CONTROL_FILE "${CMAKE_CURRENT_SOURCE_DIR}/control")
      if (NOT EXISTS "${PH_CONTROL_FILE}")
        set(PH_CONTROL_FILE "${PH_ENGINE_PATH}/scripts/control")
      endif()
      configure_file("${PH_CONTROL_FILE}" "${PH_STAGING}/DEBIAN/control")
      add_custom_target(${PH_APP_TARGET}-package bash -c "dpkg-deb -b \"${PH_STAGING}\" \"${CMAKE_CURRENT_BINARY_DIR}/${PH_BUNDLE_ID}.deb\"" VERBATIM)
    else()
      add_custom_target(${PH_APP_TARGET}-package bash -c "echo Not implemented yet" VERBATIM)
    endif()
    add_dependencies(${PH_APP_TARGET}-package ${PH_APP_TARGET}-sign)
    
    if(NOT TARGET install)
      add_custom_target(install)
    endif()
    if(NOT TARGET package)
      add_custom_target(package)
    endif()
    if(NOT TARGET debug)
      add_custom_target(debug)
    endif()
    if(NOT TARGET codesign)
      add_custom_target(codesign)
    endif()

    add_dependencies(install ${PH_APP_TARGET}-install)
    add_dependencies(package ${PH_APP_TARGET}-package)
    add_dependencies(codesign ${PH_APP_TARGET}-codesign)
    add_dependencies(debug ${PH_APP_TARGET}-debug)
  endif()

endfunction()
