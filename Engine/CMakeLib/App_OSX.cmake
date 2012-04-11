set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_OSX_SYSROOT macosx)
set(CMAKE_EXE_LINKER_FLAGS
  "-ObjC -lPorkholt_OSX -llua -lz -lpng15 -framework OpenGL -framework Foundation -framework CoreVideo -framework AppKit"
  )
set(CMAKE_CXX_FLAGS_RELEASE "-Os")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")
  
link_directories(${PH_EXTERNALS}/lib/darwin/osx)
link_directories(${Porkholt_OSX_BINARY_DIR})

include(${PH_ENGINE_PATH}/CMakeLib/Porkholt_IncludeDirs.cmake)

add_executable(${PH_NAME} MACOSX_BUNDLE ${PH_SOURCES} ${PH_HEADERS})
add_dependencies(${PH_NAME} Porkholt_OSX)

if(PH_USE_BOX2D)
  include("${PH_ENGINE_PATH}/CMakeLib/Box2D_OSX.cmake")
  link_directories(${Box2D_OSX_BINARY_DIR})
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lBox2D_OSX")
  add_dependencies(${PH_NAME} Box2D_OSX)
endif(PH_USE_BOX2D)

if(NOT DEFINED PH_OSX_DEPLOYMENT_TARGET)
  set(PH_OSX_DEPLOYMENT_TARGET "10.5")
endif(NOT DEFINED PH_OSX_DEPLOYMENT_TARGET)

if (NOT DEFINED PH_OSX_INFO_PLIST)
	set(PH_OSX_INFO_PLIST "${PROJECT_SOURCE_DIR}/Info-OSX.plist")
endif (NOT DEFINED PH_OSX_INFO_PLIST)
if (NOT EXISTS ${PH_OSX_INFO_PLIST})
	set(PH_OSX_INFO_PLIST "${PH_ENGINE_PATH}/CMakeLib/Info-OSX.plist")
endif (NOT EXISTS ${PH_OSX_INFO_PLIST})

set(CMAKE_OSX_DEPLOYMENT_TARGET "${PH_OSX_DEPLOYMENT_TARGET}")
set_target_properties(${PH_NAME} PROPERTIES XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS "YES")
set_target_properties(${PH_NAME} PROPERTIES XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES")
set_target_properties(${PH_NAME} PROPERTIES XCODE_ATTRIBUTE_INFOPLIST_FILE ${PH_OSX_INFO_PLIST} )
set_target_properties(${PH_NAME} PROPERTIES XCODE_ATTRIBUTE_GCC_C_LANGUAGE_STANDARD "c99")


set(RES_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rsrc)
set(RES_DEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/_/rsrc)

add_custom_command(
  TARGET ${PH_NAME}
  POST_BUILD
  COMMAND ${PH_ENGINE_PATH}/CMakeLib/postprocess.sh ${RES_SRC_DIR} ${RES_DEST_DIR} "build-nodownscale" ${PH_EXTERNALS}
  )

set(APP_NAME \${TARGET_BUILD_DIR}/\${FULL_PRODUCT_NAME})
add_custom_command(
  TARGET ${PH_NAME}
  POST_BUILD
  COMMAND /bin/mkdir -p ${APP_NAME}/Contents/Resources && /Developer/Library/PrivateFrameworks/DevToolsCore.framework/Resources/pbxcp -exclude .DS_Store -exclude CVS -exclude .svn ${RES_DEST_DIR} ${APP_NAME}/Contents/Resources
  )