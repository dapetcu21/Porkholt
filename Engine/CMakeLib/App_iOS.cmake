set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_OSX_SYSROOT iphoneos)
set(CMAKE_OSX_ARCHITECTURES "armv6 armv7")
set(CMAKE_EXE_LINKER_FLAGS
  "-ObjC -lPorkholt_iOS -llua -lz -lpng15 -framework AudioToolbox -framework AVFoundation -framework OpenAL -framework OpenGLES -framework Foundation -framework QuartzCore -framework UIKit"
  )
  
link_directories(${PH_EXTERNALS}/lib/darwin/ios)
link_directories(${Porkholt_iOS_BINARY_DIR})

include(${PH_ENGINE_PATH}/CMakeLib/Porkholt_IncludeDirs.cmake)
set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.porkholt.${PH_NAME}")

add_executable(${PH_NAME} MACOSX_BUNDLE ${PH_SOURCES} )
add_dependencies(${PH_NAME} Porkholt_iOS)

if(NOT DEFINED PH_IOS_CODE_SIGN_IDENTITY)
  set(PH_IOS_CODE_SIGN_IDENTITY "iPhone Developer")
endif(NOT DEFINED PH_IOS_CODE_SIGN_IDENTITY)
if(NOT DEFINED PH_IOS_DEPLOYMENT_TARGET)
  set(PH_IOS_DEPLOYMENT_TARGET "3.2")
endif(NOT DEFINED PH_IOS_DEPLOYMENT_TARGET)
if(NOT DEFINED PH_IOS_TARGET_IPHONE)
  set(PH_IOS_TARGET_IPHONE "YES")
endif(NOT DEFINED PH_IOS_TARGET_IPHONE)
if(NOT DEFINED PH_IOS_TARGET_IPAD)
  set(PH_IOS_TARGET_IPAD "NO")
endif(NOT DEFINED PH_IOS_TARGET_IPAD)
if (PH_IOS_TARGET_IPHONE)
  if (PH_IOS_TARGET_IPAD)
    set(PH_IOS_DEV_FAMILY "1,2")
  else (PH_IOS_TARGET_IPAD)
    set(PH_IOS_DEV_FAMILY "1")
  endif (PH_IOS_TARGET_IPAD)
else (PH_IOS_TARGET_IPHONE)
  if (PH_IOS_TARGET_IPAD)
    set(PH_IOS_DEV_FAMILY "2")
  else (PH_IOS_TARGET_IPAD)
    set(PH_IOS_DEV_FAMILY "")
  endif (PH_IOS_TARGET_IPAD)
endif (PH_IOS_TARGET_IPHONE)

set_target_properties(${PH_NAME} PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ${PH_IOS_CODE_SIGN_IDENTITY})
set_target_properties(${PH_NAME} PROPERTIES XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvmgcc42")
set_target_properties(${PH_NAME} PROPERTIES XCODE_ATTRIBUTE_GCC_THUMB_SUPPORT "NO")
set_target_properties(${PH_NAME} PROPERTIES XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS "YES")
set_target_properties(${PH_NAME} PROPERTIES XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES")
set_target_properties(${PH_NAME} PROPERTIES XCODE_ATTRIBUTE_INFOPLIST_FILE ${PROJECT_SOURCE_DIR}/Info-iOS.plist )
set_target_properties(${PH_NAME} PROPERTIES XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "${PH_IOS_DEPLOYMENT_TARGET}")
set_target_properties(${PH_NAME} PROPERTIES XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "${PH_IOS_DEV_FAMILY}")


set(RES_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rsrc)
set(RES_DEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/_/rsrc)

add_custom_command(
  TARGET ${PH_NAME}
  POST_BUILD
  COMMAND ${PH_ENGINE_PATH}/CMakeLib/postprocess.sh ${RES_SRC_DIR} ${RES_DEST_DIR} "build" ${PH_EXTERNALS}
  )

set(APP_NAME \${TARGET_BUILD_DIR}/\${FULL_PRODUCT_NAME})
add_custom_command(
  TARGET ${PH_NAME}
  POST_BUILD
  COMMAND /Developer/Library/PrivateFrameworks/DevToolsCore.framework/Resources/pbxcp -exclude .DS_Store -exclude CVS -exclude .svn ${RES_DEST_DIR} ${APP_NAME}
  )