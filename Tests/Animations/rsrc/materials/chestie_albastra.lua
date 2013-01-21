emissive = vec4(0.0, 0.0, 0.0, 0.0)
diffuse = vec4(1.0, 1.0, 1.0, 1.0)
specular = vec4(1.0, 1.0, 1.0, 1.0)
shininess = 10

material = {
  [0] = {
    shader = "front_pixel_shading[point_light][texture][normal_map][specular]",
    uniforms = {
      modelViewMatrix = vars.modelViewMatrix,
      projectionMatrix = vars.projectionMatrix,
      normalMatrix = vars.modelViewMatrix.inverse.transposed,
      ambientColor = emissive + vars.ambientColor * diffuse,
      shininess = shininess,
      diffuseColor = diffuse * vars.lightDiffuse * vars.lightIntensity,
      specularColor = specular * vars.lightSpecular * vars.lightIntensity,
      lightPosition = vars.lightPosition;
      tex = "earth",
      nMap = "earth.nmap",
      texR = vec4(0, 0, 1, 1),
      nMapR = vec4(0, 0, 1, 1),
    }
  }
}

