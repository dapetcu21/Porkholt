include(${PH_ENGINE_PATH}/scripts/Box2D_Common.cmake)

add_library(Box2D ${PH_B2D_SRCS} ${PH_B2D_HEADERS})
set_target_properties(Box2D PROPERTIES XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES")

if (PH_PLATFORM STREQUAL "iOS")
    set_target_properties(Box2D PROPERTIES XCODE_ATTRIBUTE_GCC_THUMB_SUPPORT "NO")
endif()

