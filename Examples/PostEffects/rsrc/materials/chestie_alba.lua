material = {
  [0] = {
    shader = "sprites",
    uniforms = {
      modelViewProjectionMatrix = vars.projectionMatrix * vars.modelViewMatrix,
      color = vars.currentColor,
      texture = "placeholder"
    }
  }
}
