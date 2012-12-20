-- Relevant globals:
--   current_src
--   current_dst
--   current_prefix

scaling_descriptors = {}

function scale_image(t)
  for i,v in ipairs(t) do
    scaling_descriptors[current_prefix..v] = t
    t[i] = nil
  end
end
