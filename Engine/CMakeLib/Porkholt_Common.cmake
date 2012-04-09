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
  )

include(${PH_ENGINE_PATH}/CMakeLib/Porkholt_IncludeDirs.cmake)