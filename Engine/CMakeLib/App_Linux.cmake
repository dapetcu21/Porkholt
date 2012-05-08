set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_OSX_SYSROOT macosx)
set(CMAKE_EXE_LINKER_FLAGS
  " -llua -lz -lpng15 -framework OpenGL -lopengl -lglut"
  )
set(CMAKE_CXX_FLAGS_RELEASE "-Os")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")
  
link_directories(${PH_EXTERNALS}/lib/darwin/osx)
link_directories(${Porkholt_OSX_BINARY_DIR})

include(${PH_ENGINE_PATH}/CMakeLib/Porkholt_IncludeDirs.cmake)

add_executable(${PH_NAME} ${PH_SOURCES} ${PH_HEADERS})
target_link_libraries(${PH_NAME} Porkholt_Linux)

if(PH_USE_BOX2D)
  include("${PH_ENGINE_PATH}/CMakeLib/Box2D_Linux.cmake")
  target_link_libraries(${PH_NAME} Box2D_Linux)
endif(PH_USE_BOX2D)

set(RES_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rsrc)
set(RES_DEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/_OSX/rsrc)

add_custom_command(
  TARGET ${PH_NAME}
  POST_BUILD
  COMMAND ${PH_ENGINE_PATH}/CMakeLib/postprocess.sh ${RES_SRC_DIR} ${RES_DEST_DIR} "build-nodownscale" ${PH_EXTERNALS}
  )

set(APP_NAME \${TARGET_BUILD_DIR}/\${FULL_PRODUCT_NAME})
add_custom_command(
  TARGET ${PH_NAME}
  POST_BUILD
  COMMAND /bin/mkdir -p ${APP_NAME}/Contents/Resources && cp -a ${RES_DEST_DIR} ${APP_NAME}-rsrc
  )
