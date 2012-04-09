set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_OSX_SYSROOT iphoneos)
set(CMAKE_OSX_ARCHITECTURES "armv6 armv7")
set(CMAKE_EXE_LINKER_FLAGS
  "-llua -lz -lpng15 -framework AudioToolbox -framework AVFoundation -framework OpenAL -framework OpenGLES -framework Foundation -framework QuartzCore -framework UIKit"
  )
link_directories(${PH_EXTERNALS}/lib/darwin/ios)
include(${PH_ENGINE_PATH}/CMakeLib/Porkholt_IncludeDirs.cmake)
set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.porkholt.${PH_NAME}")

add_executable(${PH_NAME} MACOSX_BUNDLE ${PH_SOURCES} )
target_link_libraries(${PH_NAME} Porkholt_iOS )

set_target_properties(${PH_NAME} PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ${PH_IOS_CODE_SIGN_IDENTITY})
set_target_properties(${PH_NAME} PROPERTIES XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvmgcc42")
set_target_properties(${PH_NAME} PROPERTIES XCODE_ATTRIBUTE_GCC_THUMB_SUPPORT "NO")
set_target_properties(${PH_NAME} PROPERTIES XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS "YES")
set_target_properties(${PH_NAME} PROPERTIES XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES")
