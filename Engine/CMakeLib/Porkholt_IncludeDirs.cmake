if (NOT DEFINED PH_ENGINE_HINCLUDED)
  set(PH_ENGINE_HINCLUDED "YES")
  include_directories(
    ${PH_ENGINE_PATH}/include 
    ${PH_EXTERNALS}/lua/src
    ${PH_EXTERNALS}/libpng
    ${PH_EXTERNALS}/zlib
    ${PH_EXTERNALS}/uremote/include
    )
endif()
