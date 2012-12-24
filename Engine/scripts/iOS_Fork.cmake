if (NOT PH_FORK STREQUAL "1")
    get_cmake_property(CACHE_VARS CACHE_VARIABLES)
    set(CMAKE_SUCKS2 "CMAKE_BUILD_TYPE")
    foreach(CACHE_VAR ${CACHE_VARS})
        get_property(CACHE_VAR_HELPSTRING CACHE ${CACHE_VAR} PROPERTY HELPSTRING)
        #message("${CACHE_VAR} ${${CACHE_VAR}} ${CACHE_VAR_HELPSTRING}")
        if (CACHE_VAR MATCHES "^PH.?" OR 
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

    function(exec)
        execute_process(${ARGV} RESULT_VARIABLE RESULT)
        if (RESULT)
            message(FATAL_ERROR "Command failed ${ARGV1}: exitcode ${RESULT}")
        endif()
    endfunction()

    set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/fork)
    exec(COMMAND mkdir -p ${OUTPUT_DIR})
    exec(COMMAND ${CMAKE_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_ARGS} -DCMAKE_TOOLCHAIN_FILE=${PH_ENGINE_PATH}/scripts/iOS.toolchain.cmake -DPH_FORK=1
         WORKING_DIRECTORY ${OUTPUT_DIR})

    function(invoke_make TARGET_NAME)
        if (CMAKE_GENERATOR STREQUAL "Unix Makefiles")
            set(PH_MAKE "$(MAKE)")
        else()
            set(PH_MAKE "make")
        endif()
        
        set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/fork)
        set(CMD "cd \"${OUTPUT_DIR}\" && ${PH_MAKE} ")
        add_custom_target(${TARGET_NAME} ALL bash -c ${CMD} VERBATIM )
    endfunction()

endif()
