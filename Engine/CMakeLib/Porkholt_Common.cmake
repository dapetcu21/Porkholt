set(PH_ENGINE_SRCS
  ${PH_ENGINE_PATH}/Core/PHAccelInterface.cpp
  ${PH_ENGINE_PATH}/Core/PHFontInitPool.cpp
  ${PH_ENGINE_PATH}/Core/PHLuaObject.cpp
  ${PH_ENGINE_PATH}/Core/PHAnimatedImage.cpp
  ${PH_ENGINE_PATH}/Core/PHGLProgramInitPool.cpp
  ${PH_ENGINE_PATH}/Core/PHMessage.cpp
  ${PH_ENGINE_PATH}/Core/PHAnimator.cpp
  ${PH_ENGINE_PATH}/Core/PHGLShader.cpp
  ${PH_ENGINE_PATH}/Core/PHMotion.cpp
  ${PH_ENGINE_PATH}/Core/PHAnimatorPool.cpp
  ${PH_ENGINE_PATH}/Core/PHGLShaderProgram.cpp
  ${PH_ENGINE_PATH}/Core/PHNavigationController.cpp
  ${PH_ENGINE_PATH}/Core/PHAuxLayerView.cpp
  ${PH_ENGINE_PATH}/Core/PHGLUniformStates.cpp
  ${PH_ENGINE_PATH}/Core/PHNormalImage.cpp
  ${PH_ENGINE_PATH}/Core/PHBezierPath.cpp
  ${PH_ENGINE_PATH}/Core/PHGLVertexArrayObject.cpp
  ${PH_ENGINE_PATH}/Core/PHParticleAnimator.cpp
  ${PH_ENGINE_PATH}/Core/PHCinematicActor.cpp
  ${PH_ENGINE_PATH}/Core/PHGLVertexBufferObject.cpp   
  ${PH_ENGINE_PATH}/Core/PHParticleView.cpp
  ${PH_ENGINE_PATH}/Core/PHCinematicAnimator.cpp
  ${PH_ENGINE_PATH}/Core/PHGameManager.cpp
  ${PH_ENGINE_PATH}/Core/PHCurve.cpp
  ${PH_ENGINE_PATH}/Core/PHImage.cpp
  ${PH_ENGINE_PATH}/Core/PHStripeCurve.cpp
  ${PH_ENGINE_PATH}/Core/PHDeferredView.cpp
  ${PH_ENGINE_PATH}/Core/PHImageAnimator.cpp
  ${PH_ENGINE_PATH}/Core/PHTextView.cpp
  ${PH_ENGINE_PATH}/Core/PHErrors.cpp
  ${PH_ENGINE_PATH}/Core/PHImageInitPool.cpp
  ${PH_ENGINE_PATH}/Core/PHThread.cpp
  ${PH_ENGINE_PATH}/Core/PHEvent.cpp
  ${PH_ENGINE_PATH}/Core/PHImageView.cpp
  ${PH_ENGINE_PATH}/Core/PHTime.cpp
  ${PH_ENGINE_PATH}/Core/PHEventHandler.cpp
  ${PH_ENGINE_PATH}/Core/PHKeyframeAnimator.cpp
  ${PH_ENGINE_PATH}/Core/PHTimer.cpp
  ${PH_ENGINE_PATH}/Core/PHEventQueue.cpp
  ${PH_ENGINE_PATH}/Core/PHKeyframeAnimatorGroup.cpp
  ${PH_ENGINE_PATH}/Core/PHTrailImageView.cpp
  ${PH_ENGINE_PATH}/Core/PHFileManager.cpp
  ${PH_ENGINE_PATH}/Core/PHLua.cpp
  ${PH_ENGINE_PATH}/Core/PHView.cpp
  ${PH_ENGINE_PATH}/Core/PHFont.cpp
  ${PH_ENGINE_PATH}/Core/PHViewController.cpp
  ${PH_ENGINE_PATH}/Geometry/PHColor.cpp
  ${PH_ENGINE_PATH}/Geometry/PHMatrix.cpp
  ${PH_ENGINE_PATH}/Geometry/PHRange.cpp
  ${PH_ENGINE_PATH}/Geometry/PHGeometry.cpp
  ${PH_ENGINE_PATH}/Geometry/PHPoint.cpp
  ${PH_ENGINE_PATH}/Geometry/PHRect.cpp
  ${PH_ENGINE_PATH}/UI/PHButtonView.cpp
  ${PH_ENGINE_PATH}/UI/PHTestViewController.cpp
  ${PH_ENGINE_PATH}/UI/PHTextControllerView.cpp
  ${PH_ENGINE_PATH}/UI/PHTestView.cpp
  ${PH_ENGINE_PATH}/UI/PHTextController.cpp
  ${PH_ENGINE_PATH}/Network/PHLuaConsole.cpp
  ${PH_ENGINE_PATH}/Network/PHRemote.cpp
  ${PH_ENGINE_PATH}/Network/PHTCPConsole.cpp
  ${PH_ENGINE_PATH}/Network/PHTCPServer.cpp
  ${PH_EXTERNALS}/uremote/src/URClient.cpp
  ${PH_EXTERNALS}/uremote/src/URCommon.cpp
  ${PH_EXTERNALS}/uremote/src/URField.cpp
  ${PH_EXTERNALS}/uremote/src/URServer.cpp
  ${PH_ENGINE_PATH}/Sound/PHSoundPool.cpp
  )

set(PH_ENGINE_HEADERS
  ${PH_ENGINE_PATH}/Core/PHAccelInterface.h
  ${PH_ENGINE_PATH}/Core/PHFileManager.h
  ${PH_ENGINE_PATH}/Core/PHImageInitPool.h
  ${PH_ENGINE_PATH}/Core/PHParticleAnimator.h
  ${PH_ENGINE_PATH}/Core/PHAnimatedImage.h
  ${PH_ENGINE_PATH}/Core/PHFont.h
  ${PH_ENGINE_PATH}/Core/PHImageView.h
  ${PH_ENGINE_PATH}/Core/PHParticleView.h
  ${PH_ENGINE_PATH}/Core/PHAnimator.h
  ${PH_ENGINE_PATH}/Core/PHFontInitPool.h
  ${PH_ENGINE_PATH}/Core/PHInvocation.h
  ${PH_ENGINE_PATH}/Core/PHRemote.h
  ${PH_ENGINE_PATH}/Core/PHAnimatorPool.h
  ${PH_ENGINE_PATH}/Core/PHGLLight.h
  ${PH_ENGINE_PATH}/Core/PHKeyframeAnimator.h
  ${PH_ENGINE_PATH}/Core/PHSemaphore.h
  ${PH_ENGINE_PATH}/Core/PHAuxLayerView.h
  ${PH_ENGINE_PATH}/Core/PHGLProgramInitPool.h
  ${PH_ENGINE_PATH}/Core/PHKeyframeAnimatorGroup.h
  ${PH_ENGINE_PATH}/Core/PHSerialization.h
  ${PH_ENGINE_PATH}/Core/PHBezierPath.h
  ${PH_ENGINE_PATH}/Core/PHGLShader.h
  ${PH_ENGINE_PATH}/Core/PHLua.h
  ${PH_ENGINE_PATH}/Core/PHStartGame.h
  ${PH_ENGINE_PATH}/Core/PHCinematicActor.h
  ${PH_ENGINE_PATH}/Core/PHGLShaderProgram.h
  ${PH_ENGINE_PATH}/Core/PHLuaObject.h
  ${PH_ENGINE_PATH}/Core/PHStripeCurve.h
  ${PH_ENGINE_PATH}/Core/PHCinematicAnimator.h
  ${PH_ENGINE_PATH}/Core/PHGLUniformStates.h
  ${PH_ENGINE_PATH}/Core/PHMain.h
  ${PH_ENGINE_PATH}/Core/PHTextView.h
  ${PH_ENGINE_PATH}/Core/PHCurve.h
  ${PH_ENGINE_PATH}/Core/PHGLVertexArrayObject.h
  ${PH_ENGINE_PATH}/Core/PHMessage.h
  ${PH_ENGINE_PATH}/Core/PHThread.h
  ${PH_ENGINE_PATH}/Core/PHDeferredView.h
  ${PH_ENGINE_PATH}/Core/PHGLVertexBufferObject.h
  ${PH_ENGINE_PATH}/Core/PHMotion.h
  ${PH_ENGINE_PATH}/Core/PHTime.h
  ${PH_ENGINE_PATH}/Core/PHErrors.h
  ${PH_ENGINE_PATH}/Core/PHGameManager.h
  ${PH_ENGINE_PATH}/Core/PHMutex.h
  ${PH_ENGINE_PATH}/Core/PHTimer.h
  ${PH_ENGINE_PATH}/Core/PHEvent.h
  ${PH_ENGINE_PATH}/Core/PHGenericCinematicAnimator.h
  ${PH_ENGINE_PATH}/Core/PHNavigationController.h
  ${PH_ENGINE_PATH}/Core/PHTrailImageView.h
  ${PH_ENGINE_PATH}/Core/PHEventHandler.h
  ${PH_ENGINE_PATH}/Core/PHImage.h
  ${PH_ENGINE_PATH}/Core/PHNormalImage.h
  ${PH_ENGINE_PATH}/Core/PHView.h
  ${PH_ENGINE_PATH}/Core/PHEventQueue.h
  ${PH_ENGINE_PATH}/Core/PHImageAnimator.h
  ${PH_ENGINE_PATH}/Core/PHObject.h
  ${PH_ENGINE_PATH}/Core/PHViewController.h
  ${PH_ENGINE_PATH}/Geometry/PHColor.h
  ${PH_ENGINE_PATH}/Geometry/PHGeometry.h
  ${PH_ENGINE_PATH}/Geometry/PHMatrix.h
  ${PH_ENGINE_PATH}/Geometry/PHPoint.h
  ${PH_ENGINE_PATH}/Geometry/PHRange.h
  ${PH_ENGINE_PATH}/Geometry/PHRect.h
  ${PH_ENGINE_PATH}/UI/PHButtonView.h
  ${PH_ENGINE_PATH}/UI/PHTestView.h
  ${PH_ENGINE_PATH}/UI/PHTestViewController.h
  ${PH_ENGINE_PATH}/UI/PHTextController.h
  ${PH_ENGINE_PATH}/UI/PHTextControllerView.h
  ${PH_ENGINE_PATH}/Sound/PHMusicManager.h
  ${PH_ENGINE_PATH}/Sound/PHSound.h
  ${PH_ENGINE_PATH}/Sound/PHSoundManager.h
  ${PH_ENGINE_PATH}/Sound/PHSoundPool.h
  ${PH_ENGINE_PATH}/Network/PHLuaConsole.h
  ${PH_ENGINE_PATH}/Network/PHTCPConsole.h
  ${PH_ENGINE_PATH}/Network/PHTCPServer.h
  ${PH_ENGINE_PATH}/Sound/PHMusicManager.h
  ${PH_ENGINE_PATH}/Sound/PHSound.h
  ${PH_ENGINE_PATH}/Sound/PHSoundManager.h
  ${PH_ENGINE_PATH}/Sound/PHSoundPool.h
  )

include(${PH_ENGINE_PATH}/CMakeLib/Porkholt_IncludeDirs.cmake)