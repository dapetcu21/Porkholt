include(${PH_ENGINE_PATH}/CMakeLib/Box2D_Common.cmake)

add_library(Box2D_iOS ${PH_B2D_SRCS} ${PH_B2D_HEADERS})
set_target_properties(Box2D_iOS PROPERTIES XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvmgcc42")
set_target_properties(Box2D_iOS PROPERTIES XCODE_ATTRIBUTE_GCC_THUMB_SUPPORT "NO")
set_target_properties(Box2D_iOS PROPERTIES XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES")
set_target_properties(Box2D_iOS PROPERTIES XCODE_ATTRIBUTE_GCC_C_LANGUAGE_STANDARD "c99")
