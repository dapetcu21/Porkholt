if(CMAKE_GENERATOR STREQUAL "Xcode")
    if(PH_PLATFORM STREQUAL "OSX")
        set(CMAKE_OSX_SYSROOT macosx CACHE INTERNAL "internal")
    elseif(PH_PLATFORM STREQUAL "iOS")
        set(CMAKE_OSX_SYSROOT iphoneos CACHE INTERNAL "internal")
        set(CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphoneos;-iphonesimulator" CACHE INTERNAL "internal")
    endif()
endif()

if(PH_PLATFORM STREQUAL "OSX")
    set(CMAKE_OSX_ARCHITECTURES i386;x86_64 CACHE INTERNAL "internal")
elseif(PH_PLATFORM STREQUAL "iOS")
    set(CMAKE_OSX_ARCHITECTURES armv7;armv7s CACHE INTERNAL "internal")
endif()
  
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE INTERNAL "internal")
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release" FORCE ) 
endif()


if(NOT PH_COMMON_CMAKE)
    set(PH_COMMON_CMAKE "YES" CACHE INTERNAL "internal")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os" CACHE INTERNAL "internal")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG" CACHE INTERNAL "internal")

    if(PH_PLATFORM STREQUAL "OSX")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Xarch_x86_64 -Wl,-pagezero_size,10000,-image_base,100000000" CACHE INTERNAL "internal")
    endif()

endif()

function(append_target_property TARGET PROPERTY ITEMS)
    get_target_property(__TARGET_PROP ${TARGET} ${PROPERTY})
    if (NOT __TARGET_PROP)
        unset(__TARGET_PROP)
    endif()
    set_target_properties(${TARGET} PROPERTIES ${PROPERTY} "${__TARGET_PROP} ${ITEMS}")
endfunction()
