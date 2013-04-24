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
    ${PH_EXTERNALS}/math/math-vfp
    ${PH_EXTERNALS}/math/math-neon
    )

    if (PH_PLATFORM STREQUAL OSX)
        set(PH_LIBS "darwin/osx")
        include_directories(${PH_EXTERNALS}/unimotion)
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

    if (CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
        set(PH_BINS "freebsd")
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(PH_BINS "linux")
    elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
        set(PH_BINS "darwin/osx")
    endif()
endif()
