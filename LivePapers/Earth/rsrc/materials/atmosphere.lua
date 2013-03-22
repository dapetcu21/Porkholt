material = {
  [0] = {
    shader = "atmosphere",
    uniforms = {
      modelViewMatrix = vars.modelViewMatrix,
      projectionMatrix = vars.projectionMatrix,
      normalMatrix = vars.modelViewMatrix.inverse.transposed,
      lightPosition = vars.lightPosition;
    }
  }
}

