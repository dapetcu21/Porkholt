if (NOT PH_FORK STREQUAL "1")
    get_cmake_property(CACHE_VARS CACHE_VARIABLES)
    set(CMAKE_SUCKS1 "LIBRARY_OUTPUT_PATH_ROOT")
    set(CMAKE_SUCKS2 "CMAKE_BUILD_TYPE")
    foreach(CACHE_VAR ${CACHE_VARS})
        get_property(CACHE_VAR_HELPSTRING CACHE ${CACHE_VAR} PROPERTY HELPSTRING)
        #message("${CACHE_VAR} ${${CACHE_VAR}} ${CACHE_VAR_HELPSTRING}")
        if (CACHE_VAR MATCHES "^PH.?" OR 
            CACHE_VAR MATCHES "ANDROID.?" OR 
            CACHE_VAR STREQUAL CMAKE_SUCKS1 OR 
            CACHE_VAR STREQUAL CMAKE_SUCKS2 OR 
            CACHE_VAR_HELPSTRING STREQUAL "No help, variable specified on the command line.")

            get_property(CACHE_VAR_TYPE CACHE ${CACHE_VAR} PROPERTY TYPE)
            if(CACHE_VAR_TYPE STREQUAL "UNINITIALIZED")
                set(CACHE_VAR_TYPE)
            else()
                set(CACHE_VAR_TYPE :${CACHE_VAR_TYPE})
            endif()
            list(APPEND CMAKE_ARGS "-D${CACHE_VAR}${CACHE_VAR_TYPE}=${${CACHE_VAR}}")
        endif()
    endforeach()

    if(NOT ANDROID_ARCHS)
        if (CMAKE_BUILD_TYPE STREQUAL "Release")
            set(ANDROID_ARCHS armeabi;armeabi-v7a;x86;mips)
        else()
            set(ANDROID_ARCHS armeabi-v7a)
        endif()
    endif()
    
    if (NOT ANDROID_NDK AND NOT EXISTS "$ENV{ANDROID_NDK}" AND EXISTS "/opt/android-ndk")
        list(APPEND CMAKE_ARGS "-DANDROID_NDK=/opt/android-ndk")
    elseif (NOT ANDROID_STANDALONE_TOOLCHAIN AND NOT EXISTS "$ENV{ANDROID_STANDALONE_TOOLCHAIN}" AND EXISTS "/opt/android-toolchain")
        list(APPEND CMAKE_ARGS "-DANDROID_STANDALONE_TOOLCHAIN=/opt/android-toolchain")
    endif()

    function(exec)
        execute_process(${ARGV} RESULT_VARIABLE RESULT)
        if (RESULT)
            message(FATAL_ERROR "Command failed ${ARGV1}: exitcode ${RESULT}")
        endif()
    endfunction()

    foreach(ARCH ${ANDROID_ARCHS})
        set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/${ARCH})
        exec(COMMAND mkdir -p ${OUTPUT_DIR})
        exec(COMMAND ${CMAKE_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_ARGS} -DANDROID_DYNAMIC_STL=TRUE -DANDROID_ABI=${ARCH} -DCMAKE_TOOLCHAIN_FILE=${PH_ENGINE_PATH}/scripts/android.toolchain.cmake -DPH_FORK=1
                        WORKING_DIRECTORY ${OUTPUT_DIR})
    endforeach()

    function(invoke_make TARGET_NAME)
        set(CMD "echo Compiling for Android architectures: \\\"${ANDROID_ARCHS}\\\"")
        foreach(ARCH ${ANDROID_ARCHS})
            set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/${ARCH})
            set(CMD "${CMD} && cd \"${OUTPUT_DIR}\" && make ")
        endforeach()
        add_custom_target(${TARGET_NAME} ALL bash -c ${CMD} VERBATIM )
    endfunction()

endif()
