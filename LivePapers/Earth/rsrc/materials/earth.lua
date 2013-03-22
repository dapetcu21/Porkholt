diffuse = vec4(1.0, 1.0, 1.0, 1.0)

material = {
  [0] = {
    shader = "earth",
    uniforms = {
      modelViewMatrix = vars.modelViewMatrix,
      projectionMatrix = vars.projectionMatrix,
      normalMatrix = vars.modelViewMatrix.inverse.transposed,
      diffuseColor = diffuse * vars.lightDiffuse * vars.lightIntensity,
      lightPosition = vars.lightPosition,
      tex = "earth",
      night = "night",
    }
  }
}

local uniforms = material[0].uniforms
local mat = material[0]

if options.normalMap then
  uniforms.nMap = "earth.nmap"
  mat.shader = mat.shader .. "[normal_map]" 
end

if options.specular then
  --uniforms.shininess = shininess
  mat.shader = mat.shader .. "[specular]"
end

if options.night then
  mat.shader = mat.shader .. "[night_map]"
end
