set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
  
link_directories(${PH_EXTERNALS}/lib/linux)
link_directories(${Porkholt_OSX_BINARY_DIR})

include(${PH_ENGINE_PATH}/CMakeLib/Porkholt_IncludeDirs.cmake)

add_executable(${PH_NAME} ${PH_SOURCES} ${PH_HEADERS})
find_library(PH_OPENGL GL)
find_library(PH_GLUT glut)
find_package(Threads REQUIRED)
target_link_libraries(${PH_NAME} Porkholt_Linux
  ${PH_EXTERNALS}/lib/linux/liblua.a
  ${PH_EXTERNALS}/lib/linux/libpng15.a
  ${PH_EXTERNALS}/lib/linux/libz.a
  ${PH_GLUT}
  ${PH_OPENGL}
  ${CMAKE_THREAD_LIBS_INIT}
  )

if(PH_USE_BOX2D)
  include("${PH_ENGINE_PATH}/CMakeLib/Box2D_Linux.cmake")
  target_link_libraries(${PH_NAME} Box2D_Linux)
endif(PH_USE_BOX2D)

set(RES_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rsrc)
set(RES_DEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/${PH_NAME}-rsrc)

add_custom_target(
  PostProcess_Resources
  COMMAND ${PH_ENGINE_PATH}/CMakeLib/postprocess.sh ${RES_SRC_DIR} ${RES_DEST_DIR} "build-nodownscale" ${PH_EXTERNALS}
  )
add_dependencies(${PH_NAME} PostProcess_Resources)
add_dependencies(PostProcess_Resources External_Libs)
