function(porkholt PH_APP_TARGET)
  set(PH_SOURCES ${ARGV})
  list(REMOVE_AT PH_SOURCES 0)
  if(NOT DEFINED PH_NAME OR NOT PH_NAME)
    set(PH_NAME ${PH_APP_TARGET})
  endif()
  
  if(NOT DEFINED PH_PLATFORM)
    if (APPLE)
      set(PH_PLATFORM OSX)
    else()
      set(PH_PLATFORM X11)
    endif()
    message("PH_PLATFORM is not defined. Defaulting to \"${PH_PLATFORM}\"")
  endif()
  message("PH_PLATFORM \"${PH_PLATFORM}\"")

  if(NOT DEFINED PH_ENGINE_PATH)
    set(PH_ENGINE_PATH ".")
    message("PH_ENGINE_PATH is not defined. Defaulting to \".\"")
  endif()

  IF(NOT DEFINED PH_EXTERNALS)
    set(PH_EXTERNALS "${PH_ENGINE_PATH}/../Externals")
  endif()

  add_subdirectory(${PH_ENGINE_PATH} ${CMAKE_CURRENT_BINARY_DIR}/engine-build)
  include(${PH_ENGINE_PATH}/CMakeLib/Porkholt_IncludeDirs.cmake)

  if(CMAKE_GENERATOR STREQUAL "Xcode")
      if    (PH_PLATFORM STREQUAL "OSX")
          set(CMAKE_OSX_SYSROOT macosx PARENT_SCOPE)
      elseif(PH_PLATFORM STREQUAL "iOS")
          set(CMAKE_OSX_SYSROOT iphoneos PARENT_SCOPE)
      endif()
  endif()

  if    (PH_PLATFORM STREQUAL "OSX")
      set(CMAKE_OSX_ARCHITECTURES i386;x86_64 PARENT_SCOPE)
  elseif(PH_PLATFORM STREQUAL "iOS")
      set(CMAKE_OSX_ARCHITECTURES armv6;armv7 PARENT_SCOPE)
  endif()
  
  if(PH_PLATFORM STREQUAL "OSX")
    add_executable(${PH_APP_TARGET} MACOSX_BUNDLE ${PH_SOURCES})
    set_target_properties(${PH_APP_TARGET} PROPERTIES LINK_FLAGS "-ObjC -framework Foundation -framework CoreVideo -framework AppKit")
  else()
    add_executable(${PH_APP_TARGET} ${PH_SOURCES})
  endif()


  function(to_list_spaces _LIST_NAME OUTPUT_VAR)
    set(NEW_LIST_SPACE)
    foreach(ITEM ${${_LIST_NAME}})
      set(NEW_LIST_SPACE "${NEW_LIST_SPACE} ${ITEM}")
    endforeach()
    string(STRIP ${NEW_LIST_SPACE} NEW_LIST_SPACE)
    set(${OUTPUT_VAR} "${NEW_LIST_SPACE}" PARENT_SCOPE)
  endfunction()

  set(CMAKE_CONFIGURATION_TYPES Debug Release PARENT_SCOPE)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os" PARENT_SCOPE)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG" PARENT_SCOPE)

  find_library(PH_OPENGL GL)
  if(PH_PLATFORM STREQUAL "X11")
    find_package(X11 REQUIRED)
    include_directories(${X11_INCLUDE_DIR})
#    to_list_spaces(X11_LIBRARIES X11_LIBS)
#    set_target_properties(${PH_APP_TARGET} PROPERTIES LINK_FLAGS "${X11_LIBS}")
#    link_directories(/usr/lib/x86_64-linux-gnu)
    find_library(PH_XRANDR Xrandr)
  endif()
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
  target_link_libraries(${PH_APP_TARGET}
    ${PH_EXTERNALS}/lib/${PH_LIBS}/liblua.a
    ${PH_EXTERNALS}/lib/${PH_LIBS}/libpng15.a
    ${PH_EXTERNALS}/lib/${PH_LIBS}/libz.a
    ${X11_LIBRARIES}
    ${PH_XRANDR}
    ${OPENGL_LIBRARIES}
    ${PH_OPENAL}
    ${CMAKE_THREAD_LIBS_INIT}
    )
  target_link_libraries(${PH_APP_TARGET} Porkholt)

  if(PH_USE_BOX2D)
    include("${PH_ENGINE_PATH}/CMakeLib/Box2D.cmake")
    target_link_libraries(${PH_APP_TARGET} Box2D)
  endif()

  if (PH_PLATFORM STREQUAL "OSX")
    if(NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET OR NOT CMAKE_OSX_DEPLOYMENT_TARGET)
      set(CMAKE_OSX_DEPLOYMENT_TARGET "10.6" PARENT_SCOPE)
    endif()
    if (NOT DEFINED PH_OSX_INFO_PLIST)
    	set(PH_OSX_INFO_PLIST "${PROJECT_SOURCE_DIR}/Info-OSX.plist")
    endif ()
    if (NOT EXISTS ${PH_OSX_INFO_PLIST})
    	set(PH_OSX_INFO_PLIST "${PH_ENGINE_PATH}/CMakeLib/Info-OSX.plist")
    endif ()
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

  if (PH_PLATFORM STREQUAL "OSX")
    if(CMAKE_GENERATOR STREQUAL "Xcode")
      set(APP_NAME \${TARGET_BUILD_DIR}/\${FULL_PRODUCT_NAME})
      add_custom_command(
        TARGET ${PH_APP_TARGET}
        POST_BUILD
        COMMAND ${PH_EXTERNALS}/lua/src/lua ${PH_ENGINE_PATH}/CMakeLib/postprocess.lua ${RES_SRC_DIR} ${APP_NAME}/Contents/Resources/rsrc "build-nodownscale" ${PH_EXTERNALS}
        )
      add_custom_command(
        TARGET ${PH_APP_TARGET}
        POST_BUILD
        COMMAND ${PH_ENGINE_PATH}/CMakeLib/copy_libraries.sh ${PH_EXTERNALS}/lib/darwin/osx ${APP_NAME}/Contents/lib
        )
    else()
      set(APP_NAME "${CMAKE_CURRENT_BINARY_DIR}/${PH_APP_TARGET}.app")
      add_custom_target(
        PostProcess_Resources
        COMMAND ${PH_EXTERNALS}/lua/src/lua ${PH_ENGINE_PATH}/CMakeLib/postprocess.lua ${RES_SRC_DIR} ${APP_NAME}/Contents/Resources/rsrc "build-nodownscale" ${PH_EXTERNALS}
        )
      add_dependencies(${PH_APP_TARGET} PostProcess_Resources)
      add_dependencies(PostProcess_Resources External_Libs)  
      add_custom_target(
        Copy_Libraries
        COMMAND ${PH_ENGINE_PATH}/CMakeLib/copy_libraries.sh ${PH_EXTERNALS}/lib/darwin/osx ${APP_NAME}/Contents/lib
        )
      add_dependencies(${PH_APP_TARGET} Copy_Libraries)
      add_dependencies(Copy_Libraries External_Libs)  
    endif()
  else()
    set(RES_DEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/${PH_APP_TARGET}-rsrc)
    add_custom_target(
      PostProcess_Resources
      COMMAND ${PH_EXTERNALS}/lua/src/lua ${PH_ENGINE_PATH}/CMakeLib/postprocess.lua ${RES_SRC_DIR} ${RES_DEST_DIR} "build-nodownscale" ${PH_EXTERNALS}
      )
    add_dependencies(${PH_APP_TARGET} PostProcess_Resources)
    add_dependencies(PostProcess_Resources External_Libs)
  endif()
endfunction()
