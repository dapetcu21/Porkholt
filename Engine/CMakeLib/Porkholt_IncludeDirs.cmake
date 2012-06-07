if (NOT DEFINED PH_ENGINE_HINCLUDED)
  set(PH_ENGINE_HINCLUDED "YES")
  include_directories(
    ${PH_ENGINE_PATH}/Core 
    ${PH_ENGINE_PATH}/Geometry
    ${PH_ENGINE_PATH}/Sound
    ${PH_ENGINE_PATH}/UI
    ${PH_ENGINE_PATH}/Network
    ${PH_ENGINE_PATH}/3D
    ${PH_EXTERNALS}/lua/src
    ${PH_EXTERNALS}/libpng
    ${PH_EXTERNALS}/zlib
    ${PH_EXTERNALS}/uremote/include
    )
endif()