if (NOT DEFINED PH_ENGINE_HINCLUDED)
  set(PH_ENGINE_HINCLUDED "YES")
  include_directories(
    ${PH_ENGINE_PATH}/include 
    ${PH_EXTERNALS}/lua/src
    ${PH_EXTERNALS}/libpng
    ${PH_EXTERNALS}/zlib
    ${PH_EXTERNALS}/uremote/include
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
