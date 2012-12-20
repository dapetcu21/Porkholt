#!/usr/bin/env lua

function exec(x)
  local file = io.popen(x)
  local s = file:read("*a")
  file:close()
  return s;
end

function strip_n(x)
  if (string.sub(x, -1) == '\n') then
    x = string.sub(x, 1, -2)
  end
  return x
end

map = {}
out = ""

package.path = package.path..";"..string.gsub(arg[0], "(/?)[^/]*%.lua$", "%1?.lua")
require("path")

for i,v in ipairs(arg) do
  v = path.getabsolute(v)
  v = strip_n(exec('dirname "'..v..'"'))
  if not map[v] then
    map[v] = true
    out = out..v..' '
  end
end

io.write(out)
