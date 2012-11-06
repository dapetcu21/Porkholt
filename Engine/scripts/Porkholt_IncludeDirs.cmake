if (NOT DEFINED PH_ENGINE_HINCLUDED)
  set(PH_ENGINE_HINCLUDED "YES")
  if (ANDROID_ABI)
      include_directories("${PH_EXTERNALS}/libogg/build-${ANDROID_ABI}/inc")
  endif()
  include_directories(
    ${PH_ENGINE_PATH}/include 
    ${PH_EXTERNALS}/luajit/src
    ${PH_EXTERNALS}/libpng
    ${PH_EXTERNALS}/zlib
    ${PH_EXTERNALS}/uremote/include
    ${PH_EXTERNALS}/libogg/include
    ${PH_EXTERNALS}/libvorbis/include
    )

    if (PH_PLATFORM STREQUAL OSX)
        set(PH_LIBS "darwin/osx")
    elseif (PH_PLATFORM STREQUAL iOS)
        set(PH_LIBS "darwin/ios")
    elseif (PH_PLATFORM STREQUAL X11)
        if (CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
            set(PH_LIBS "freebsd")
            include_directories(/usr/local/include)
        else()
            set(PH_LIBS "linux")
        endif()
    endif()
endif()
