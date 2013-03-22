classes = {}
for i,v in ipairs(objects) do
  if v.class == "" then
    local bt = v.traces[1].backtrace
    for i = 2,#bt do
      local d1,d2,s = string.find(bt[i], "_ZN(%d*%a*)C")
      if s then
        v.class = s
      else
        break
      end
    end
  end

  local t = classes[v.class]
  if not t then
    t = {}
    classes[v.class] = t
  end
  table.insert(t, v)
end

