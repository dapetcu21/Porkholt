set(CMAKE_CONFIGURATION_TYPES Debug Release)
if(CMAKE_GENERATOR STREQUAL "Xcode")
	set(CMAKE_OSX_SYSROOT macosx)
endif()
set(CMAKE_EXE_LINKER_FLAGS
  "-ObjC -llua -lz -lpng15 -framework OpenGL -framework Foundation -framework CoreVideo -framework AppKit"
  )

set(CMAKE_OSX_ARCHITECTURES i386;x86_64)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

link_directories(${PH_EXTERNALS}/lib/darwin/osx)
link_directories(${Porkholt_OSX_BINARY_DIR})

include(${PH_ENGINE_PATH}/CMakeLib/Porkholt_IncludeDirs.cmake)

add_executable(${PH_NAME} MACOSX_BUNDLE ${PH_SOURCES} ${PH_HEADERS})
if(CMAKE_GENERATOR STREQUAL "Xcode")
    add_dependencies(${PH_NAME} Porkholt_OSX)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lPorkholt_OSX")
else()
    target_link_libraries(${PH_NAME} Porkholt_OSX)
endif()

find_package(OpenAL REQUIRED)
include_directories(${OPENAL_INCLUDE_DIR})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OPENAL_LIBRARY}/OpenAL")

if(PH_USE_BOX2D)
  include("${PH_ENGINE_PATH}/CMakeLib/Box2D_OSX.cmake")
  if(CMAKE_GENERATOR STREQUAL "Xcode")
    link_directories(${Box2D_OSX_BINARY_DIR})
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lBox2D_OSX")
    add_dependencies(${PH_NAME} Box2D_OSX)
  else()
    target_link_libraries(${PH_NAME} Box2D_OSX)
  endif()
endif()

if(NOT DEFINED PH_OSX_DEPLOYMENT_TARGET)
  set(PH_OSX_DEPLOYMENT_TARGET "10.6")
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
  execute_process(COMMAND bash -c "echo -n \"${PH_NAME}\" | tr -d \"\\ \" "
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

set(CMAKE_OSX_DEPLOYMENT_TARGET "${PH_OSX_DEPLOYMENT_TARGET}")
set_target_properties(${PH_NAME} PROPERTIES XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS "YES")
set_target_properties(${PH_NAME} PROPERTIES XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES")
if(CMAKE_GENERATOR STREQUAL "Xcode")
  set(PH_OSX_INFO_PLIST_OUT "${CMAKE_CURRENT_BINARY_DIR}/Info-OSX.plist")
  configure_file(${PH_OSX_INFO_PLIST} ${PH_OSX_INFO_PLIST_OUT})
  set_target_properties(${PH_NAME} PROPERTIES XCODE_ATTRIBUTE_INFOPLIST_FILE ${PH_OSX_INFO_PLIST_OUT} )
else()
  set_target_properties(${PH_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PH_OSX_INFO_PLIST})
endif()
set_target_properties(${PH_NAME} PROPERTIES XCODE_ATTRIBUTE_GCC_C_LANGUAGE_STANDARD "c99")

set(RES_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rsrc)

if(CMAKE_GENERATOR STREQUAL "Xcode")
  set(APP_NAME \${TARGET_BUILD_DIR}/\${FULL_PRODUCT_NAME})
  add_custom_command(
    TARGET ${PH_NAME}
    POST_BUILD
    COMMAND ${PH_EXTERNALS}/lua/src/lua ${PH_ENGINE_PATH}/CMakeLib/postprocess.lua ${RES_SRC_DIR} ${APP_NAME}/Contents/Resources/rsrc "build-nodownscale" ${PH_EXTERNALS}
    )
else()
  set(APP_NAME "${CMAKE_CURRENT_BINARY_DIR}/${PH_NAME}.app")
  add_custom_target(
    PostProcess_Resources
    COMMAND ${PH_EXTERNALS}/lua/src/lua ${PH_ENGINE_PATH}/CMakeLib/postprocess.lua ${RES_SRC_DIR} ${APP_NAME}/Contents/Resources/rsrc "build-nodownscale" ${PH_EXTERNALS}
    )
  add_dependencies(${PH_NAME} PostProcess_Resources)
  add_dependencies(PostProcess_Resources External_Libs)  
endif()
