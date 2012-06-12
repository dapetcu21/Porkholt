set(PH_ENGINE_SRCS
  ${PH_ENGINE_PATH}/src/Core/PHAccelInterface.cpp
  ${PH_ENGINE_PATH}/src/Core/PHFontInitPool.cpp
  ${PH_ENGINE_PATH}/src/Core/PHLuaObject.cpp
  ${PH_ENGINE_PATH}/src/Core/PHAnimatedImage.cpp
  ${PH_ENGINE_PATH}/src/Core/PHGLProgramInitPool.cpp
  ${PH_ENGINE_PATH}/src/Core/PHMessage.cpp
  ${PH_ENGINE_PATH}/src/Core/PHAnimator.cpp
  ${PH_ENGINE_PATH}/src/Core/PHGLShader.cpp
  ${PH_ENGINE_PATH}/src/Core/PHMotion.cpp
  ${PH_ENGINE_PATH}/src/Core/PHAnimatorPool.cpp
  ${PH_ENGINE_PATH}/src/Core/PHGLShaderProgram.cpp
  ${PH_ENGINE_PATH}/src/Core/PHNavigationController.cpp
  ${PH_ENGINE_PATH}/src/Core/PHAuxLayerView.cpp
  ${PH_ENGINE_PATH}/src/Core/PHGLUniformStates.cpp
  ${PH_ENGINE_PATH}/src/Core/PHNormalImage.cpp
  ${PH_ENGINE_PATH}/src/Core/PHBezierPath.cpp
  ${PH_ENGINE_PATH}/src/Core/PHGLVertexArrayObject.cpp
  ${PH_ENGINE_PATH}/src/Core/PHParticleAnimator.cpp
  ${PH_ENGINE_PATH}/src/Core/PHCinematicActor.cpp
  ${PH_ENGINE_PATH}/src/Core/PHGLVertexBufferObject.cpp   
  ${PH_ENGINE_PATH}/src/Core/PHParticleView.cpp
  ${PH_ENGINE_PATH}/src/Core/PHCinematicAnimator.cpp
  ${PH_ENGINE_PATH}/src/Core/PHGameManager.cpp
  ${PH_ENGINE_PATH}/src/Core/PHCurve.cpp
  ${PH_ENGINE_PATH}/src/Core/PHImage.cpp
  ${PH_ENGINE_PATH}/src/Core/PHStripeCurve.cpp
  ${PH_ENGINE_PATH}/src/Core/PHDeferredView.cpp
  ${PH_ENGINE_PATH}/src/Core/PHImageAnimator.cpp
  ${PH_ENGINE_PATH}/src/Core/PHTextView.cpp
  ${PH_ENGINE_PATH}/src/Core/PHErrors.cpp
  ${PH_ENGINE_PATH}/src/Core/PHImageInitPool.cpp
  ${PH_ENGINE_PATH}/src/Core/PHThread.cpp
  ${PH_ENGINE_PATH}/src/Core/PHEvent.cpp
  ${PH_ENGINE_PATH}/src/Core/PHImageView.cpp
  ${PH_ENGINE_PATH}/src/Core/PHTime.cpp
  ${PH_ENGINE_PATH}/src/Core/PHEventHandler.cpp
  ${PH_ENGINE_PATH}/src/Core/PHKeyframeAnimator.cpp
  ${PH_ENGINE_PATH}/src/Core/PHTimer.cpp
  ${PH_ENGINE_PATH}/src/Core/PHKeyframeAnimatorGroup.cpp
  ${PH_ENGINE_PATH}/src/Core/PHTrailImageView.cpp
  ${PH_ENGINE_PATH}/src/IO/PHFileManager.cpp
  ${PH_ENGINE_PATH}/src/Core/PHLua.cpp
  ${PH_ENGINE_PATH}/src/Core/PHDrawable.cpp
  ${PH_ENGINE_PATH}/src/Core/PHView.cpp
  ${PH_ENGINE_PATH}/src/Core/PHFont.cpp
  ${PH_ENGINE_PATH}/src/Core/PHViewController.cpp
  ${PH_ENGINE_PATH}/src/Core/PHGLTexture.cpp
  ${PH_ENGINE_PATH}/src/IO/PHInode.cpp
  ${PH_ENGINE_PATH}/src/IO/PHRegularDirectory.cpp
  ${PH_ENGINE_PATH}/src/IO/PHRegularFile.cpp
  ${PH_ENGINE_PATH}/src/Geometry/PHColor.cpp
  ${PH_ENGINE_PATH}/src/Geometry/PHMatrix.cpp
  ${PH_ENGINE_PATH}/src/Geometry/PHRange.cpp
  ${PH_ENGINE_PATH}/src/Geometry/PHGeometry.cpp
  ${PH_ENGINE_PATH}/src/Geometry/PHPoint.cpp
  ${PH_ENGINE_PATH}/src/Geometry/PHRect.cpp
  ${PH_ENGINE_PATH}/src/Geometry/PHQuaternion.cpp
  ${PH_ENGINE_PATH}/src/UI/PHButtonView.cpp
  ${PH_ENGINE_PATH}/src/UI/PHTestViewController.cpp
  ${PH_ENGINE_PATH}/src/UI/PHTextControllerView.cpp
  ${PH_ENGINE_PATH}/src/UI/PHTestView.cpp
  ${PH_ENGINE_PATH}/src/UI/PHTextController.cpp
  ${PH_ENGINE_PATH}/src/Network/PHLuaConsole.cpp
  ${PH_ENGINE_PATH}/src/Network/PHRemote.cpp
  ${PH_ENGINE_PATH}/src/Network/PHTCPConsole.cpp
  ${PH_ENGINE_PATH}/src/Network/PHTCPServer.cpp
  ${PH_ENGINE_PATH}/src/Sound/PHSoundPool.cpp
  ${PH_ENGINE_PATH}/src/3D/PHMesh.cpp
  ${PH_ENGINE_PATH}/src/3D/PHSphereMesh.cpp
  ${PH_ENGINE_PATH}/src/3D/PHBody.cpp
  ${PH_ENGINE_PATH}/src/3D/PHMeshBody.cpp
  ${PH_ENGINE_PATH}/src/3D/PHProjectionChanger.cpp
  ${PH_EXTERNALS}/uremote/src/URClient.cpp
  ${PH_EXTERNALS}/uremote/src/URCommon.cpp
  ${PH_EXTERNALS}/uremote/src/URField.cpp
  ${PH_EXTERNALS}/uremote/src/URServer.cpp
  )

set(PH_ENGINE_HEADERS
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHAccelInterface.h
  ${PH_ENGINE_PATH}/include/Porkholt/IO/PHFileManager.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHImageInitPool.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHParticleAnimator.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHAnimatedImage.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHFont.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHImageView.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHParticleView.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHAnimator.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHFontInitPool.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHInvocation.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHRemote.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHAnimatorPool.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHGLLight.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHKeyframeAnimator.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHSemaphore.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHAuxLayerView.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHGLProgramInitPool.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHKeyframeAnimatorGroup.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHSerialization.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHBezierPath.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHGLShader.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHLua.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHWindowing.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHCinematicActor.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHGLShaderProgram.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHLuaObject.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHStripeCurve.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHCinematicAnimator.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHGLUniformStates.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHMain.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHTextView.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHCurve.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHGLVertexArrayObject.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHMessage.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHThread.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHDeferredView.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHGLVertexBufferObject.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHMotion.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHTime.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHErrors.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHGameManager.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHMutex.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHTimer.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHEvent.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHGenericCinematicAnimator.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHNavigationController.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHTrailImageView.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHEventHandler.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHImage.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHNormalImage.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHView.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHDrawable.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHImageAnimator.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHObject.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHViewController.h
  ${PH_ENGINE_PATH}/include/Porkholt/Core/PHGLTexture.h
  ${PH_ENGINE_PATH}/include/Porkholt/IO/PHInode.h
  ${PH_ENGINE_PATH}/include/Porkholt/IO/PHStream.h
  ${PH_ENGINE_PATH}/include/Porkholt/IO/PHFile.h
  ${PH_ENGINE_PATH}/include/Porkholt/IO/PHDirectory.h
  ${PH_ENGINE_PATH}/include/Porkholt/IO/PHRegularDirectory.h
  ${PH_ENGINE_PATH}/include/Porkholt/IO/PHRegularFile.h
  ${PH_ENGINE_PATH}/include/Porkholt/Geometry/PHColor.h
  ${PH_ENGINE_PATH}/include/Porkholt/Geometry/PHGeometry.h
  ${PH_ENGINE_PATH}/include/Porkholt/Geometry/PHMatrix.h
  ${PH_ENGINE_PATH}/include/Porkholt/Geometry/PHPoint.h
  ${PH_ENGINE_PATH}/include/Porkholt/Geometry/PHRange.h
  ${PH_ENGINE_PATH}/include/Porkholt/Geometry/PHRect.h
  ${PH_ENGINE_PATH}/include/Porkholt/Geometry/PHQuaternion.h
  ${PH_ENGINE_PATH}/include/Porkholt/UI/PHButtonView.h
  ${PH_ENGINE_PATH}/include/Porkholt/UI/PHTestView.h
  ${PH_ENGINE_PATH}/include/Porkholt/UI/PHTestViewController.h
  ${PH_ENGINE_PATH}/include/Porkholt/UI/PHTextController.h
  ${PH_ENGINE_PATH}/include/Porkholt/UI/PHTextControllerView.h
  ${PH_ENGINE_PATH}/include/Porkholt/Sound/PHMusicManager.h
  ${PH_ENGINE_PATH}/include/Porkholt/Sound/PHSound.h
  ${PH_ENGINE_PATH}/include/Porkholt/Sound/PHSoundManager.h
  ${PH_ENGINE_PATH}/include/Porkholt/Sound/PHSoundPool.h
  ${PH_ENGINE_PATH}/include/Porkholt/Network/PHLuaConsole.h
  ${PH_ENGINE_PATH}/include/Porkholt/Network/PHTCPConsole.h
  ${PH_ENGINE_PATH}/include/Porkholt/Network/PHTCPServer.h
  ${PH_ENGINE_PATH}/include/Porkholt/Sound/PHMusicManager.h
  ${PH_ENGINE_PATH}/include/Porkholt/Sound/PHSound.h
  ${PH_ENGINE_PATH}/include/Porkholt/Sound/PHSoundManager.h
  ${PH_ENGINE_PATH}/include/Porkholt/Sound/PHSoundPool.h
  ${PH_ENGINE_PATH}/include/Porkholt/3D/PHMesh.h
  ${PH_ENGINE_PATH}/include/Porkholt/3D/PHSphereMesh.h
  ${PH_ENGINE_PATH}/include/Porkholt/3D/PHBody.h
  ${PH_ENGINE_PATH}/include/Porkholt/3D/PHProjectionChanger.h
  ${PH_ENGINE_PATH}/include/Porkholt/3D/PHMeshBody.h
  ${PH_ENGINE_PATH}/include/Porkholt/3D/PHMaterial.h
  )

include(${PH_ENGINE_PATH}/CMakeLib/Porkholt_IncludeDirs.cmake)
