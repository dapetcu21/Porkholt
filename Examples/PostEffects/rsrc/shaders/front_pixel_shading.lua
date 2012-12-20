attributeBindings = {
  posi = 0
}

if (directional_light or point_light) then
  attributeBindings.nrm = 3;
end

if (texture or normal_map) then
  attributeBindings.texCoord = 1;
end
