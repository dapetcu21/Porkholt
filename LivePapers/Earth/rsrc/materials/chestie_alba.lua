material = {
  [0] = {
    shader = "sprites",
    uniforms = {
      modelViewProjectionMatrix = vars.projectionMatrix * vars.modelViewMatrix,
      color = vec4(1, 1, 1, 1),
      texture = "placeholder"
    }
  }
}
